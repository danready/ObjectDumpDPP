// GNU General Public License Agreement
// Copyright (C) 2015-2016 Daniele Berto daniele.fratello@gmail.com
//
// objectDump is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

/**
* @file DigitizerStateMachine.cpp 
*/

#include "DefineGeneral.h"
#include "X742DecodeRoutines.h"
#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "Analizzatore.h"
#include "DigitizerObjectGeneric.h"
#include "RawData.h"
#include "DigitizerStateMachine.h"
#include "OutputModule.h"
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timex.h>


using namespace std;

void
DigitizerStateMachine::Produttore ()
{
	ApplicationSetup *application_setup;
	application_setup = ApplicationSetup::Instance ();

	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	//Codice per testare il tempo.


	struct timespec start, stop;
	unsigned long accum;
	unsigned long parz_clock;  
	unsigned long parz=1000000000L;

	double accum2;


	unique_lock<mutex> ReservedPreprocessingInputAreaHandle(ReservedPreprocessingInputArea, defer_lock);
	unique_lock<mutex> ReservedVisualizationInputAreaHandle(ReservedVisualizationInputArea, defer_lock);

	int i;
	RawData tmp;
	tmp.RawDataSet (digitizer);

	//~ //Codice per testare il tempo.
	//~ clock_gettime(CLOCK_REALTIME, &start);

	while (go_general)
	{
		//~ //Codice per testare il tempo.
		//~ clock_gettime(CLOCK_REALTIME, &stop);

		//~ parz_clock = ((unsigned long)stop.tv_sec - (unsigned long)start.tv_sec) - 1 ;
		//~ if (parz_clock>=0) {accum=parz_clock*1000000000L+((parz-start.tv_nsec)+stop.tv_nsec);}
		//~ else accum = stop.tv_sec - start.tv_sec;
		//~ //printf( "%lf\n", accum );
		//~ accum2=(double)accum;
		//~ accum2=accum2/1000000L;
		//~ if(tmp.bsize>0)
		//~ fprintf(stderr, "\t%.6lf\n", accum2);

		//~ clock_gettime(CLOCK_REALTIME, &start);

		CAEN_DGTZ_IRQWait(digitizer.handle, 200);
		//~ fprintf(stderr, "Sono qui!!!\n");
		tmp.RawDataRead ();

		if (tmp.bsize > 0)
		{
			///////////////////////////////////////////////////////////////////////
			if (go_raw_data && go_general)
			{
				if(num_events_written<num_events_to_write || num_events_to_write == 0)
				{
					tmp.RawDataWriteOnFile (application_setup->
						ApplicationSetupGetDataFilePunt (),
						application_setup->
						ApplicationSetupGetDataFileSizePunt ());
						
					num_events_written++;
					
					if (num_events_to_write != 0 && num_events_written==num_events_to_write)
						output_module->Output("Warning: you have reached the maximum number of events indicated by rawstart\n");	
				} //if(num_events_written<num_events_to_write || num_events_to_write == 0)
			} //(go_raw_data && go_general)
			///////////////////////////////////////////////////////////////////////



			///////////////////////////////////////////////////////////////////////
			ReservedPreprocessingInputAreaHandle.lock();
			//fprintf(stderr, "num_mex_preprocessing:%d\n", num_mex_preprocessing);
			if (go_preprocessing && go_general)
			{
				if (num_mex_preprocessing < PREPROCESSINGQUEUE)
				{
					circular_buffer_preprocessing[coda_preprocessing] = tmp;

					if (coda_preprocessing == PREPROCESSINGQUEUE - 1)
						coda_preprocessing = 0;
					else
						coda_preprocessing = coda_preprocessing + 1;
						
					num_mex_preprocessing++;
					Acquisition_Cond1.notify_one();
				}
			}   // go_preprocessing
			ReservedPreprocessingInputAreaHandle.unlock();
			///////////////////////////////////////////////////////////////////////



			///////////////////////////////////////////////////////////////////////
			ReservedVisualizationInputAreaHandle.lock();
			//fprintf(stderr, "num_mex_visualization:%d\n", num_mex_visualization);
			if (go_visualization && go_general)
			{
				if (num_mex_visualization < VISUALIZATIONQUEUE)
				{
					circular_buffer_visualization[coda_visualization] = tmp;

					if (coda_visualization == VISUALIZATIONQUEUE - 1)
						coda_visualization = 0;
					else
						coda_visualization = coda_visualization + 1;
						
					num_mex_visualization++;
					Acquisition_Cond2.notify_one();
				}
			}   //go_visualization
			ReservedVisualizationInputAreaHandle.unlock();
			///////////////////////////////////////////////////////////////////////
		}   //if (tmp.bsize > 0)
	}   // go_general

	//segnalo la fine dell'acquisizione a tutti i threads
	for (i = 0; i < 10; i++)
	{
		Acquisition_Cond2.notify_one();
		Acquisition_Cond1.notify_one();
		pre_cond.notify_one();
		vis_cond.notify_one();
	}
	//fprintf (stderr, "Esco dal Reader Thread\n");

}

void 
DigitizerStateMachine::Preprocessing ()
{

	int i, j;
	uint32_t numEvents;
	int conta = 0;
	char *evtptr;
	CAEN_DGTZ_EventInfo_t eventInfo;
	RawData tmp;
	tmp.RawDataSet (digitizer);

	unique_lock<mutex> ReservedPreprocessingInputAreaHandle(ReservedPreprocessingInputArea, defer_lock);

	while (go_general)
	{

		if (go_preprocessing && go_general)
		{


			ReservedPreprocessingInputAreaHandle.lock();

			if (num_mex_preprocessing == 0)
			{
				Acquisition_Cond1.wait(ReservedPreprocessingInputAreaHandle);
			}

			if (go_general != 0 && go_preprocessing != 0)
			{

				tmp = circular_buffer_preprocessing[testa_preprocessing];

				ReservedPreprocessingInputAreaHandle.unlock();


				//fprintf(stderr, "Decodifico\n");
				tmp.RawDataDecode ();


				ReservedPreprocessingInputAreaHandle.lock();

				if (testa_preprocessing == PREPROCESSINGQUEUE - 1)
					testa_preprocessing = 0;
				else
					testa_preprocessing = testa_preprocessing + 1;

				num_mex_preprocessing--; 
			
			}

			ReservedPreprocessingInputAreaHandle.unlock();

		}
		else	
		{
			ReservedPreprocessingInputAreaHandle.lock();
			pre_cond.wait(ReservedPreprocessingInputAreaHandle);
			ReservedPreprocessingInputAreaHandle.unlock();
		}
	}   // go_general
}


void 
DigitizerStateMachine::Visualization ()
{
	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	RawData tmp;
	tmp.RawDataSet (digitizer);

	FILE *gnuplot;

	char stringa[STANDARDBUFFERLIMIT];
	bzero (stringa, STANDARDBUFFERLIMIT);
	snprintf(stringa, STANDARDBUFFERLIMIT, "%s -persist 2>/dev/null", digitizer.internal_config.gnuplot);

	gnuplot = NULL;
	gnuplot = popen (stringa, "w");

	unique_lock<mutex> ReservedVisualizationInputAreaHandle(ReservedVisualizationInputArea, defer_lock);

	//Il codice seguente serve per gestire il caso in cui l'utente non abbia installato gnuplot o abbia specificato male il comando
	//per lanciarlo nel configuration file. Occorre gestire questa situazione perche' si rischia, altrimenti, l'arresto del programma senza nessun
	//messaggio di errore rendendo il debugging difficile.
	int gnuplot_error_code;
	gnuplot_error_code = pclose(gnuplot);

	if (gnuplot_error_code != 0) 
	{
		output_module->Output("Error, you have not specify gnuplot command correctly in the configuration file\n");
	}

	gnuplot = NULL;
	gnuplot = popen (stringa, "w");

	while (go_general)
	{

		if (go_visualization && go_general)
		{

			ReservedVisualizationInputAreaHandle.lock();

			if (num_mex_visualization == 0)
				Acquisition_Cond2.wait(ReservedVisualizationInputAreaHandle);
			if (go_general != 0 && go_visualization != 0)
			{
				tmp = circular_buffer_visualization[testa_visualization];

				ReservedVisualizationInputAreaHandle.unlock();


				//fprintf(stderr, "plotto\n");
				if (tmp.bsize > 0 && gnuplot_error_code == 0)
					tmp.RawDataPlot (".plot_data.txt", gnuplot);

				ReservedVisualizationInputAreaHandle.lock();

				if (testa_visualization == VISUALIZATIONQUEUE - 1)
					testa_visualization = 0;
				else
					testa_visualization = testa_visualization + 1;

				num_mex_visualization--; 
			}

			ReservedVisualizationInputAreaHandle.unlock();
		}
		else    
		{
			ReservedVisualizationInputAreaHandle.lock();
			vis_cond.wait(ReservedVisualizationInputAreaHandle);
			ReservedVisualizationInputAreaHandle.unlock();          
		}

	}  // go_general

	pclose (gnuplot);
}

DigitizerStateMachine::DigitizerStateMachine ()
{
  imset = 0;
  imstarted = 0;
  go_general = 0;
  go_raw_data = 0;
  go_preprocessing = 0;
  go_visualization = 0;
  num_events_to_write = 0;
  num_events_written = 0;
  
  
  //~ ///////////////NTP!!!
  //~ int sockfd;
  //~ struct sockaddr_in servaddr, cliaddr;
  //~ char recvline[STANDARDBUFFERLIMIT];
  //~ sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  //~ bzero (&servaddr, sizeof (servaddr));
  //~ servaddr.sin_family = AF_INET;
  //~ servaddr.sin_addr.s_addr = inet_addr ("192.84.144.161");
  //~ //servaddr.sin_addr.s_addr = inet_addr ("192.168.6.2");  
  //~ servaddr.sin_port = htons (123);
//~ 
  //~ fprintf(stderr, "Welcome to NTP Server\n");
//~ 
  //~ connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
  //~ perror(" ");
  //~ RawData::NTPSocket = sockfd;
  //~ ///////////////NTP!!!  
  
}

void
DigitizerStateMachine::DigitizerStateMachineInit (const char *conf_file)
{
  digitizer.DigitizerObjectSetConfigStructureInit (conf_file);
  //Se la variabile imset e' impostata a 0 e viene eseguito il metodo DigitizerStateMachine::DigitizerStateMachineStartReading, 
  //viene stampato un messaggio di errore.
  imset = 1;

  digitizer.DigitizerObjectOpen ();
  digitizer.DigitizerObjectReset ();
  digitizer.DigitizerObjectGetInfo ();
  digitizer.PrintBoardInfo ();
}

///Il metodo DigitizerStateMachineSetup e' fondamentale perche' esegue tutte le funzioni per impostare il digitizer.
void
DigitizerStateMachine::DigitizerStateMachineSetup (const char *conf_file)
{
	if (imstarted == 0)
	{	
		int data;
		digitizer.DigitizerObjectReset ();
		digitizer.DigitizerObjectSetConfigStructureSetup (conf_file);
		digitizer.DigitizerObjectGenericSetRecordLength ();
		digitizer.DigitizerObjectGenericSetMaxNumEventsBLT ();
		digitizer.DigitizerObjectSetAcquisitionMode (CAEN_DGTZ_SW_CONTROLLED);
		digitizer.DigitizerObjectGenericSetExtTriggerInputMode ();
		digitizer.DigitizerObjectGenericSetEnableMask ();
		digitizer.DigitizerObjectGenericSetDRS4SamplingFrequency ();
		digitizer.DigitizerObjectGenericSetPostTriggerSize ();
		digitizer.DigitizerObjectGenericSetChannelSelfTriggerThreshold ();
		digitizer.DigitizerObjectGenericSetIOLevel ();
		digitizer.DigitizerObjectGenericSetDCOffset ();
		digitizer.DigitizerObjectGenericSetSelfTrigger ();
		digitizer.DigitizerObjectGenericSetFastTriggerDigitizing ();
		digitizer.DigitizerObjectGenericSetDecimationFactor ();
		digitizer.DigitizerObjectGenericSetDesMode ();
		digitizer.DigitizerObjectGenericSetTestPattern ();
		digitizer.DigitizerObjectGenericSetAllInformations ();
		////////////
		digitizer.DigitizerObjectSetAutomaticCorrectionX742 ();
		////////////

		//write register
		for(int i = 0; i < MAXREGISTERS; i++)
		{
			if(digitizer.internal_config.registers[i].address == -1)
				break;
			digitizer.DigitizerObjectWriteRegister(digitizer.internal_config.registers[i].address,
												 digitizer.internal_config.registers[i].data);
		}


		CAEN_DGTZ_SetInterruptConfig( digitizer.handle, CAEN_DGTZ_ENABLE,
					  1,0xAAAA ,1, CAEN_DGTZ_IRQ_MODE_ROAK);

	}
	else
	{
		OutputModule *output_module;
		output_module = OutputModule::Instance ();	
		output_module->Output("DigitizerStateMachine: Error, you have stop acquisition using 'stop' command before setupping the dgitizer. \n");
	}
}

///Il metodo DigitizerStateMachineRawDataInit fa riferimento ai parametri RAWDATAQUEUE, PREPROCESSINGQUEUE e VISUALIZATIONQUEUE definiti in DefineGeneral.h.
///Si consiglia di non superare il valore di 100 per non intasare le risorse del calcolatore.
void
DigitizerStateMachine::DigitizerStateMachineRawDataInit ()
{
	int i = 0;

	num_mex_preprocessing = 0;
	coda_preprocessing = 0;
	testa_preprocessing = 0;

	num_mex_visualization = 0;
	coda_visualization = 0;
	testa_visualization = 0;

	for (i = 0; i < PREPROCESSINGQUEUE; i++)
	{
		circular_buffer_preprocessing[i].RawDataDel ();
		circular_buffer_preprocessing[i].RawDataSet (digitizer);
	}

	for (i = 0; i < VISUALIZATIONQUEUE; i++)
	{
		circular_buffer_visualization[i].RawDataDel ();
		circular_buffer_visualization[i].RawDataSet (digitizer);
	}

	go_general = 1;

	visualization_thread = new thread (&DigitizerStateMachine::Visualization, this);

	preprocessing_thread = new thread (&DigitizerStateMachine::Preprocessing, this);

	produttore_thread = new thread (&DigitizerStateMachine::Produttore, this);

	////////Detaching threads
	visualization_thread->detach();

	preprocessing_thread->detach();

	produttore_thread->detach();
	////////Detaching threads
}


void
DigitizerStateMachine::DigitizerStateMachineStartReading ()
{
	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	if (imset == 0)
	{
		output_module->Output("DigitizerStateMachine: Error, you have not set the object, use DigitizerStateMachineInit(const char *conf_file) method.\n");
	}

	if (imstarted == 0)
	{
		imstarted = 1;
		digitizer.DigitizerObjectSWStartAcquisition ();
		DigitizerStateMachine::DigitizerStateMachineRawDataInit ();
		output_module->Output("DigitizerStateMachine: Inizio la lettura dei dati\n");
	}
	else
	{
		output_module->Output("DigitizerStateMachine: Error, you have already insert start command \n");
	}
}


void
DigitizerStateMachine::DigitizerStateMachineQuit ()
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();
  output_module->Output("DigitizerStateMachine: Esco dalla sessione di acquisizione\n");
  DigitizerStateMachine::DigitizerStateMachineStopReading ();
  digitizer.DigitizerObjectClose ();

}

void
DigitizerStateMachine::DigitizerStateMachineStopReading ()
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Interrompo la lettura dei dati\n");

  imstarted = 0;
  go_general = 0;


  //DigitizerStateMachine::DigitizerStateStopPreprocessing();
  Acquisition_Cond1.notify_one();


  //DigitizerStateMachine::DigitizerStateStopVisualization();
  Acquisition_Cond2.notify_one();

  vis_cond.notify_one();
  pre_cond.notify_one();

  digitizer.DigitizerObjectSWStopAcquisition ();

}

void
DigitizerStateMachine::DigitizerStateMachineSendSWTrigger ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: invio il software trigger\n");

  CAEN_DGTZ_SendSWtrigger (digitizer.handle);
}

void
DigitizerStateMachine::DigitizerStateStartPreprocessing ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Starting Preprocessing thread\n");

  go_preprocessing = 1;
  pre_cond.notify_one();
}

void
DigitizerStateMachine::DigitizerStateStartVisualization ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("Starting Visualization thread\n");

  go_visualization = 1;
  vis_cond.notify_one();
}

void
DigitizerStateMachine::DigitizerStateStopPreprocessing ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Stopping Preprocessing thread\n");

  go_preprocessing = 0;
}

void
DigitizerStateMachine::DigitizerStateStartRawDataWriting ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Starting RawData thread\n");

  num_events_written = 0;

  go_raw_data = 1;

}

void
DigitizerStateMachine::DigitizerStateStopRawDataWriting ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Stopping RawData thread\n");

  go_raw_data = 0;

  num_events_written = 0;

  num_events_to_write = 0;

}

void
DigitizerStateMachine::DigitizerStateStopVisualization ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  output_module->Output("DigitizerStateMachine: Stopping Visualization thread\n");

  go_visualization = 0;
}

void
DigitizerStateMachine::DigitizerStateMachinePrintStatus ()
{
	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	char stringa[STANDARDBUFFERLIMIT];
	bzero(stringa, STANDARDBUFFERLIMIT);


	if (num_events_to_write != 0)
	{
		snprintf(stringa, STANDARDBUFFERLIMIT, "Numbers of events to write: %d\nNumbers of events already written: %d\n", num_events_to_write, num_events_written);
		output_module->Output(stringa);
	}
	else 
	{
		snprintf(stringa, STANDARDBUFFERLIMIT, "Numbers of events to write: infinite\nNumbers of events already written: %d\n", num_events_written);
		output_module->Output(stringa);
	}

	if (go_general)
	{
		output_module->Output("Data acquisition thread ON\n");
	}
	else
	{
		output_module->Output("Data acquisition thread OFF\n");
	}

	if (go_preprocessing)
	{
		output_module->Output("Preprocessing thread ON\n");
	}
	else
	{
		output_module->Output("Preprocessing thread OFF\n");
	}

	if (go_raw_data)
	{
		output_module->Output("Raw data thread ON\n");
	}
	else
	{
		output_module->Output("Raw data thread OFF\n");
	}

	if (go_visualization)
	{
		output_module->Output("Visualization thread ON\n");
	}
	else
	{
		output_module->Output("Visualization thread OFF\n");
	}
}
