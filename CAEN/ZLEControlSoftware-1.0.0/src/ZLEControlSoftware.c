/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/

#define ZLECS_Release        "1.0"
#define ZLECS_Release_Date   "February 2014" 

#include <stdio.h>
#include <CAENDigitizerType.h>
#include <CAENDigitizer.h>

#include "ZLEControlSoftware.h"
#include "ZLECSplot.h"

#ifdef LINUX
#define Sleep(t) usleep((t)*1000)
#endif

int h2dcnt=0;

void saveTimeTags(FILE *fp, char *buffer, int bufferSize);

/* Error messages */
typedef enum  {
	ERR_NONE= 0,
	ERR_CONF_FILE_NOT_FOUND,
	ERR_DGZ_OPEN,
	ERR_BOARD_INFO_READ,
	ERR_INVALID_BOARD_TYPE,
	ERR_DGZ_PROGRAM,
	ERR_MALLOC,
	ERR_RESTART,
	ERR_INTERRUPT,
	ERR_READOUT,
	ERR_EVENT_BUILD,
	ERR_HISTO_MALLOC,
	ERR_UNHANDLED_BOARD,
	ERR_OUTFILE_WRITE,

	ERR_DUMMY_LAST,
} ERROR_CODES;
static char ErrMsg[ERR_DUMMY_LAST][100] = {
	"No Error",                                         /* ERR_NONE */
	"Configuration File not found",                     /* ERR_CONF_FILE_NOT_FOUND */
	"Can't open the digitizer",                         /* ERR_DGZ_OPEN */
	"Can't read the Board Info",                        /* ERR_BOARD_INFO_READ */
	"Can't run DPPrunner for this digitizer",           /* ERR_INVALID_BOARD_TYPE */
	"Can't program the digitizer",                      /* ERR_DGZ_PROGRAM */
	"Can't allocate the memory for the readout buffer", /* ERR_MALLOC */
	"Restarting Error",                                 /* ERR_RESTART */
	"Interrupt Error",                                  /* ERR_INTERRUPT */
	"Readout Error",                                    /* ERR_READOUT */
	"Event Build Error",                                /* ERR_EVENT_BUILD */
	"Can't allocate the memory fro the histograms",     /* ERR_HISTO_MALLOC */
	"Unhandled board type",                             /* ERR_UNHANDLED_BOARD */
	"Output file write error",                          /* ERR_OUTFILE_WRITE */

};


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/* ###########################################################################
*  Global Variables
*  ########################################################################### */
ZLECSConfig_t  ZLEcsCFile;


/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*
*   \return  time in msec
*/
static long get_time()
{
	long time_ms;
#ifdef WIN32
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
	struct timeval t1;
	struct timezone tz;
	gettimeofday(&t1, &tz);
	time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
	return time_ms;
}

int WriteOutputFiles(int ch, CAEN_DGTZ_751_ZLE_Waveforms_t *Waveform, CAEN_DGTZ_751_ZLE_Event_t Event)
{
	int  j, ns;

	int Size = Waveform->Ns;
	if (Size <= 0) {
		return 0;
	}
	if ((ZLEcsCFile.SingleWrite[ch]) || (ZLEcsCFile.ContinuousWrite )) {
		// Ascii file format
		if (!ZLEcsCFile.fout[ch]) {
			char fname[100];
			sprintf(fname, "wave%d.txt", ch);
			if ((ZLEcsCFile.fout[ch] = fopen(fname, "w")) == NULL)
				return -1;
		}
		// Write the Channel Header
		fprintf(ZLEcsCFile.fout[ch], "Record Length: %d\n", Size);
		fprintf(ZLEcsCFile.fout[ch], "Channel: %d\n", ch);
		fprintf(ZLEcsCFile.fout[ch], "Event Baseline: %d\n", Event.baseline);
		fprintf(ZLEcsCFile.fout[ch], "Trigger Time Stamp: %u\n\n", Event.timeTag);
		for(j=0; j<Size; j++) {
			fprintf(ZLEcsCFile.fout[ch], "%04d\t%01d\n", Waveform->Trace1[j],Waveform->Discarded[j]);
		}
		if (ZLEcsCFile.SingleWrite[ch]) {
			fclose(ZLEcsCFile.fout[ch]);
			ZLEcsCFile.fout[ch]= NULL;
			ZLEcsCFile.SingleWrite[ch] = 0;
		}
	}
	return 0;

}

/*! \fn      int ProgramDigitizer(int handle, DPPrunnerConfig_t ZLEcsCFile)
*   \brief   configure the digitizer according to the parameters read from
*            the cofiguration file and saved in the ZLEcsCFile data structure
*
*   \param   handle   Digitizer handle
*   \param   ZLEcsCFile:   DPPrunnerConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int ProgramDigitizer(int handle)
{
	int i, ret = 0;
	uint32_t d32;

	/* reset the digitizer */
	ret |= CAEN_DGTZ_Reset(handle);

	ret |= CAEN_DGTZ_SetRecordLength(handle, ZLEcsCFile.recordLength);	
	ret |= CAEN_DGTZ_SetIOLevel(handle, ZLEcsCFile.FPIOtype);
	ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, ZLEcsCFile.NumEvents);
	ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
	ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, ZLEcsCFile.ExtTriggerMode);
	ret |= CAEN_DGTZ_SetChannelEnableMask(handle, ZLEcsCFile.EnableMask);
	//ret = 0;



	ZLEcsCFile.tSampl = 1;
	CAEN_DGTZ_SetZLEParameters(handle,ZLEcsCFile.EnableMask,&(ZLEcsCFile.ZLEParams));
	for(i=0; i<ZLEcsCFile.Nch; i++) {
		if (ZLEcsCFile.EnableMask & (1<<i)) {
			ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, ZLEcsCFile.DCoffset[i]);
		}
	}

	//Control Register
	// Sequential access and Individual trigger enabled	
	d32 = ( ((ZLEcsCFile.TestWave & 0x1)<<3)| (0x1<<4));
	ret |= CAEN_DGTZ_WriteRegister(handle, 0x8004, d32);
	ret |= CAEN_DGTZ_WriteRegister(handle, 0x8008, 0x100);

	//Pre-Trigger Register
	ret |= CAEN_DGTZ_WriteRegister(handle, 0x8038, (ZLEcsCFile.ZLEParams.preTrgg & 0xFF));

	/* execute generic write commands */
	for(i=0; i<ZLEcsCFile.GWn; i++)
		ret |= CAEN_DGTZ_WriteRegister(handle, ZLEcsCFile.GWaddr[i], ZLEcsCFile.GWdata[i]);


	if (ret) printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
	return 0;
}



int ZLECS_PlotWave(WDPlot_t *PlotVar, CAEN_DGTZ_751_ZLE_Waveforms_t *Waveforms, int16_t baseline)
{
	int i;

	if (IsPlotterBusy())
		return 0;


	if ( ZLEcsCFile.SetPlotOptions ) {
		PlotVar->Xscale = ZLEcsCFile.tSampl / (float)1000.0; 
		strcpy(PlotVar->Xlabel, "us");
		strcpy(PlotVar->Ylabel, "ADC counts");

		sprintf(PlotVar->Title, "%s%d", "Waveforms Ch", ZLEcsCFile.chPlot);
		PlotVar->Yautoscale = 0;
		PlotVar->Ymin = 0;
		PlotVar->Ymax = 1024;
		PlotVar->Xautoscale = 1;

		for (i=0; i<2; i++) {
			PlotVar->Gain[i] = 1;
			PlotVar->Offset[i] = 0;
		}
		PlotVar->Gain[2] = 100;
		PlotVar->Offset[2] = baseline;
		SetPlotOptions();
		ZLEcsCFile.SetPlotOptions = 0;
	}
	// Input
	PlotVar->TraceEnable[0] = ZLEcsCFile.TraceEnable[0];
	if (PlotVar->TraceEnable[0]) {
		PlotVar->DataType[0] = PLOT_DATA_UINT16;
		PlotVar->TraceSize[0] = Waveforms->Ns;
		sprintf(PlotVar->TraceName[0], "(1) %s", "Input Signal");
		memcpy(PlotVar->TraceData[0], Waveforms->Trace1, Waveforms->Ns * sizeof(*Waveforms->Trace1));
	}
	PlotVar->TraceEnable[1] = ZLEcsCFile.TraceEnable[1];
	if (PlotVar->TraceEnable[1]) {
		PlotVar->DataType[1] = PLOT_DATA_UINT16;
		PlotVar->TraceSize[1] = Waveforms->Ns;
		sprintf(PlotVar->TraceName[1], "(2) %s", "Baseline Signal");
		for (i=0; i<Waveforms->Ns; i++) {
			uint16_t *p;
			p = (uint16_t*) (PlotVar->TraceData[1]);
			p[i] = baseline;  
		}
	}
	// Good Samples
	PlotVar->TraceEnable[2] = ZLEcsCFile.TraceEnable[2];
	if (PlotVar->TraceEnable[2]) {
		PlotVar->DataType[2] = PLOT_DATA_UINT8;
		PlotVar->TraceSize[2] = Waveforms->Ns;
		sprintf(PlotVar->TraceName[2], "(3) %s", "Discarded Samples");                    
		memcpy(PlotVar->TraceData[2], Waveforms->Discarded, Waveforms->Ns * sizeof(*Waveforms->Discarded));
	}

	PlotVar->NumTraces = 3;

	ZLEcsCFile.SinglePlot = 0;
	if (PlotWaveforms() < 0) {
		printf("Plot Error.\n");
		ZLEcsCFile.ContinuousPlot = 0;
		return -1;
	}
	//getch();
	return 0;
}






/*! \fn      void CheckKeyboardCommands(int handle)
*   \brief   check if there is a key pressed and execute the relevant command
*
*   \param   handle:  the handle of the board
*/

void CheckKeyboardCommands(int handle)
{
	int c = 0, ch, i, run=0, ris=0;
	char title[500], cmd[500];

	if(!kbhit()) return;

	c = getch();
	switch(c) {
	case 'q' :
		ZLEcsCFile.Quit = 1;
		break;
	case 'w' :
		for (i=0;i<MAX_CH;i++) ZLEcsCFile.SingleWrite[i] = 1;
		break;
	case 'c' : 
		printf("\nPress 0-7 to select the channel to plot\n");
		ch = getch()-'0';
		if( !( ZLEcsCFile.EnableMask & (1 << ch))) {
			printf("Channel %d not enabled for acquisition\n", ch);
		} else {
			printf("Plot Channel %d\n", ch);
			ZLEcsCFile.chPlot = ch;
		}
		ZLEcsCFile.SetPlotOptions = 1;
		break;
	case '1' :
		if (!ZLEcsCFile.TraceEnable[1] && !ZLEcsCFile.TraceEnable[2]) {
			printf("This is the only trace enabled! \n");
			break;
		}
		ZLEcsCFile.TraceEnable[0] = ZLEcsCFile.TraceEnable[0] ^ 1;
		break;
	case '2' :
		if (!ZLEcsCFile.TraceEnable[0] && !ZLEcsCFile.TraceEnable[2]) {
			printf("This is the only trace enabled! \n");
			break;
		}
		ZLEcsCFile.TraceEnable[1] = ZLEcsCFile.TraceEnable[1] ^ 1;
		break;
	case '3' :
		if (!ZLEcsCFile.TraceEnable[0] && !ZLEcsCFile.TraceEnable[1]) {
			printf("This is the only trace enabled! \n");
			break;
		}
		ZLEcsCFile.SetPlotOptions = 1;
		ZLEcsCFile.TraceEnable[2] = ZLEcsCFile.TraceEnable[2] ^ 1;
		break;
	case 'r' :
		for(i=0; i<ZLEcsCFile.Nch; i++){
			if (!(ZLEcsCFile.EnableMask & (1<<i)))
				continue;
		}
		break;
	case 't' :
		if (!ZLEcsCFile.ContinuousTrigger) {
			CAEN_DGTZ_SendSWtrigger(handle);
			printf("Single Software Trigger issued\n");
		}
		break;
	case 'T' :
		ZLEcsCFile.ContinuousTrigger ^= 1;
		if (ZLEcsCFile.ContinuousTrigger)  printf("Continuous trigger is enabled\n");
		else                         printf("Continuous trigger is disabled\n");
		break;
	case 'P' :
		ZLEcsCFile.ContinuousPlot ^= 1;
		break;
	case 'p' :
		ZLEcsCFile.SinglePlot = 1;
		break;
	case 'o' :
		ZLEcsCFile.PlotType = PLOT_WAVEFORMS;
		ZLEcsCFile.SetPlotOptions = 1;
		break;
	case 'W' :
		ZLEcsCFile.ContinuousWrite ^= 1;
		if (ZLEcsCFile.ContinuousWrite)  printf("Continuous writing is enabled\n");
		else                       printf("Continuous writing is disabled\n");
		break;
	case 's' :
		if (ZLEcsCFile.AcqRun == 0) {
			ZLEcsCFile.SetPlotOptions = 1;
			ris=0;
			ris=CAEN_DGTZ_SWStartAcquisition(handle);
			if (ris==0) {
				printf("Acquisition started\n");
				ZLEcsCFile.AcqRun = 1;
			}
			else
				ris=ris;
		} else {
			ris=0;
			ris=CAEN_DGTZ_SWStopAcquisition(handle);
			if (ris == 0){
				printf("Acquisition stopped\n");
				ZLEcsCFile.AcqRun = 0;
			}
			else
				ris=ris;
		}
		break;
	case ' ' :  //F1
		printf("\nBindkey help\n");
		printf("[q]   Quit\n");
		printf("[s]   Start/Stop acquisition\n");
		printf("[t]   Send a software trigger (single shot)\n");
		printf("[T]   Enable/Disable continuous software trigger\n");
		printf("[w]   Write one event to output file\n");
		printf("[W]   Enable/Disable continuous writing to output file\n");
		printf("[p]   Plot one event\n");
		printf("[P]   Enable/Disable continuous plot\n");
		printf("[1-3] Enable/Disable one trace on the plot\n");
		printf("[c]   Select channel to plot\n");
		printf("[spacebar]  This help\n\n");
		printf("Press a key to continue\n");
		getch();
		break;
	default :   break;
	}
}

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
	CAEN_DGTZ_ErrorCode ret;
	int  handle;
	int fRet=0;
	int plotThis=0;
	int trggMissCnt=0, plsRjctdCnt=0, PURcnt=0;
	ERROR_CODES ErrCode= ERR_NONE;
	int i, ch=0, ev, Nb=0, dataWidthByte=2;
	int numCNT=0,qCh= -1;
	uint32_t AllocatedSize, BufferSize=0, BufferSizeOLD=0;
	char *buffer = NULL;
	char ConfigFileName[100];
	int MajorNumber;
	int NumEvents[8];
	uint64_t CurrentTime, PrevRateTime, ElapsedTime, Plot2dTime, PlotFromFileTime=0;
	CAEN_DGTZ_751_ZLE_Event_t              *Events[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	CAEN_DGTZ_751_ZLE_Waveforms_t          *Waveforms=NULL;
	CAEN_DGTZ_BoardInfo_t           BoardInfo;
	WDPlot_t                        *PlotVar=NULL;
	FILE *f_ini, *fin=NULL;


	printf("\n");
	printf("**************************************************************\n");
	printf("                      ZLE ControlSoftware %s\n", ZLECS_Release);
	printf("**************************************************************\n");

	/* *************************************************************************************** */
	/* Open and parse configuration file , then init parameters                                */
	/* *************************************************************************************** */
	if (argc > 1)
		strcpy(ConfigFileName, argv[1]);
	else
		strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
	printf("Opening Configuration File %s\n", ConfigFileName);
	f_ini = fopen(ConfigFileName, "r");
	ErrCode = ERR_CONF_FILE_NOT_FOUND;  if (f_ini == NULL ) goto QuitProgram;
	ParseConfigFile(f_ini, &ZLEcsCFile);
	fclose(f_ini);

	ZLEcsCFile.ContinuousPlot = 0; 
	ZLEcsCFile.SetPlotOptions = 1;

	ZLEcsCFile.TraceEnable[0] = 1;
	ZLEcsCFile.TraceEnable[1] = 0;
	ZLEcsCFile.TraceEnable[2] = 0;

	ZLEcsCFile.tSampl = 1;

	/* *************************************************************************************** */
	/* Open the digitizer and read the board information                                       */
	/* *************************************************************************************** */
	ret = CAEN_DGTZ_OpenDigitizer(ZLEcsCFile.LinkType, ZLEcsCFile.LinkNum, ZLEcsCFile.ConetNode, ZLEcsCFile.BaseAddress, &handle);
	ErrCode = ERR_DGZ_OPEN; if (ret) goto QuitProgram;    
	ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
	ErrCode = ERR_BOARD_INFO_READ; if (ret) goto QuitProgram;
	printf("\nConnected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
	printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
	printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
	// Check firmware rivision (DPP firmwares cannot be used with DPPrunner */
	sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber); //HACK

	// mask the channels not available for this model
	ZLEcsCFile.Nch = ((BoardInfo.FormFactor == CAEN_DGTZ_VME64_FORM_FACTOR) || (BoardInfo.FormFactor == CAEN_DGTZ_VME64X_FORM_FACTOR)) ? 8 : 4; // KACK gestire caso con 2 ch.
	ZLEcsCFile.EnableMask &= ((1<<ZLEcsCFile.Nch)-1);


	/* *************************************************************************************** */
	/* program the digitizer                                                                   */
	/* *************************************************************************************** */
	ret = ProgramDigitizer(handle);
	ErrCode = ERR_DGZ_PROGRAM; if (ret) goto QuitProgram;


	// XXX HACK : abilita trigger non overlapped
	//    ret = CAEN_DGTZ_WriteRegister(handle, 0x8004, 0x2);
	// ERRORE: faceva esattamente l'opposto: BIT1: 0= trg overlap disabled, 1= trg overlap enabled

	/* *************************************************************************************** */
	/* allocate memory for the readout buffer and the event data                               */ 
	/* *************************************************************************************** */
	/* WARNING: The mallocs must be done after the digitizer programming */
	/* Allocate memory for the readout buffer */
	ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &AllocatedSize);

	/* Allocate memory for the events */
	ret |= CAEN_DGTZ_MallocZLEEvents(handle, Events, &AllocatedSize); 

	/* Allocate memory for the waveforms */
	ret |= CAEN_DGTZ_MallocZLEWaveforms(handle, &Waveforms, &AllocatedSize); 

	/* Allocate memory for the historams */
	for(i=0; i<ZLEcsCFile.Nch; i++){
		ZLEcsCFile.TrgCnt[i] = 0; //Initializing
		if (!(ZLEcsCFile.EnableMask & (1<<i)))
			continue;
	}


	ErrCode = ERR_MALLOC; if (ret) goto QuitProgram;

	/* *************************************************************************************** */
	/* open the plotter                                                                        */
	/* *************************************************************************************** */
	//    PlotVar = OpenPlotter(ZLEcsCFile.GnuPlotPath, 6, max((ZLEcsCFile.recordLength<<3), HISTO_CHANNELS));    
	PlotVar = OpenPlotter(ZLEcsCFile.GnuPlotPath, 3, max((ZLEcsCFile.recordLength*2), MAX_NUMSAMPLES));    
	if (PlotVar == NULL) {
		printf("Can't open the plotter\n");
		ZLEcsCFile.ContinuousPlot = 0;
	}

	printf("[s] start/stop the acquisition, [q] quit, [space key] help\n");
	PrevRateTime = get_time();
	Plot2dTime = PrevRateTime;
	/* *************************************************************************************** */
	/* Readout Loop                                                                            */
	/* *************************************************************************************** */
	while(!ZLEcsCFile.Quit) {


		/* Check for keyboard commands (key pressed) */
		CheckKeyboardCommands(handle);
		//     if (!ZLEcsCFile.AcqRun) {
		//ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
		//if (BufferSize == 0){
		//	Sleep(100);
		//	continue;
		//}
		//else
		//	BufferSize=BufferSize;
		//     }
		if (!ZLEcsCFile.AcqRun) {
			Sleep(100);
			continue;
		}			

		/* Calculate throughput and trigger rate (every second) */
		CurrentTime = get_time();
		ElapsedTime = CurrentTime - PrevRateTime;
		if (ElapsedTime > 1000) {
			if (Nb == 0)
				printf("No data...\n");
			else
				printf("Readout Rate=%.2f MB/s  TrgRate=%.2fKhz\n", (float)Nb/((float)ElapsedTime*1048.576f), ((float)numCNT)/((float)ElapsedTime));
			Nb = 0;
			numCNT = 0;
			qCh = -1;
			PrevRateTime = CurrentTime;
		}

		/* if enabled, send a software trigger */
		if (ZLEcsCFile.ContinuousTrigger)  
			CAEN_DGTZ_SendSWtrigger(handle);

		BufferSizeOLD=BufferSize;
		/* Read data from the board */
		ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
		if (ret) {
			ErrCode = ERR_READOUT;
			goto QuitProgram;
		}

		// Scrittura dei Timetag globali degli eventi su File x debug
#if 0	
		saveTimeTags(fp, buffer, BufferSize);
		fflush(fp);
#endif

		if((BufferSize%695)>4) 
			BufferSize=BufferSize;

		if (BufferSize > 0) {
			Nb += BufferSize;
			ret = CAEN_DGTZ_GetZLEEvents(handle, buffer, BufferSize, Events, NumEvents);
			if (ret) {
				ErrCode = ERR_EVENT_BUILD;
				goto QuitProgram;
			}
		} else {
			continue;
		}

		/* Analyze event data for each channel */
		for(ch=0; ch<ZLEcsCFile.Nch; ch++) {
			if (!(ZLEcsCFile.EnableMask & (1<<ch)))
				continue;	

			//getch();
			/* scan events */
			for(ev=0; ev<NumEvents[ch]; ev++) {
				int PlotWave;
				double TimeStamp;
				uint64_t TT;  // Pulse Time tag in us
				if (qCh == -1) qCh =ch;
				//				if (qCh == ch) numCNT += NumEvents[ev];
				//				if (qCh == ch) numCNT += NumEvents[ch];
				if (qCh == ch) numCNT++;
				ZLEcsCFile.TrgCnt[ch]++;
				/* Get Time Tag */
				TT = Events[ch][ev].timeTag;


				if (TT < ZLEcsCFile.PrevTime[ch]) 
					ZLEcsCFile.ExtendedTT[ch]++;
				TimeStamp = (double)(((ZLEcsCFile.ExtendedTT[ch] << 32) | TT) * ZLEcsCFile.tSampl);

				ZLEcsCFile.PrevTime[ch] = (uint32_t)TT;


				/* Get Short and Long gate Charges (energy) */

				if(fRet == -1){
					trggMissCnt ++; //pulse is too small;threshold is bigger w.r.t. the pulse height
					continue;
				}

				if(fRet == -2){
					plsRjctdCnt ++;	// la finestra comincia a cavallo di un impulso
					//plotThis=1;
					continue;
				}

				if(fRet == -3){
					PURcnt ++;		// PiluUP event
					//plotThis=1;
					continue;
				}

				//plotThis=1;

				PlotWave = (ZLEcsCFile.PlotType == PLOT_WAVEFORMS) && (ch == ZLEcsCFile.chPlot) && (ZLEcsCFile.ContinuousPlot || ZLEcsCFile.SinglePlot || plotThis);

				/* Plot and/or Save Waveforms */
				if (PlotWave || ZLEcsCFile.ContinuousWrite || ZLEcsCFile.SingleWrite[ch]) {

					CAEN_DGTZ_DecodeZLEWaveforms(handle, &(Events[ch][ev]), Waveforms);

					if (PlotWave) {
						ZLECS_PlotWave(PlotVar, Waveforms, Events[ch][ev].baseline);
						ZLEcsCFile.SinglePlot = 0;
					}

					if ((ZLEcsCFile.ContinuousWrite) || (ZLEcsCFile.SingleWrite)) {
						WriteOutputFiles(ch, Waveforms,Events[ch][ev]);
					}                    
				}

			}

		}

	}
	ErrCode = ERR_NONE;

QuitProgram:
	if (ErrCode) {
		printf("\a%s\n", ErrMsg[ErrCode]);
#ifdef WIN32
		getch();
		printf("Press a key to quit\n");
#endif
	}

	/* stop the acquisition */
	CAEN_DGTZ_SWStopAcquisition(handle);
	/* close the plotter */
	if (PlotVar)
		ClosePlotter();
	/* free */
	CAEN_DGTZ_FreeReadoutBuffer(&buffer);
	CAEN_DGTZ_FreeZLEEvents(handle,Events);
	CAEN_DGTZ_FreeZLEWaveforms(handle,Waveforms);
	/* close digitizer */
	CAEN_DGTZ_CloseDigitizer(handle);

	for(i=0; i<ZLEcsCFile.Nch; i++){
		if (!(ZLEcsCFile.EnableMask & (1<<i)))
			continue;
	}		
	return 0;
}

void saveTimeTags(FILE *fp, char *buffer, int bufferSize) {

	int i;
	static uint64_t ttt_old= 0, ttt = 0;
	double deltaTTT = 0.0;
	uint32_t *p;
	uint32_t nw;
	static uint64_t deltaN, deltaN1 = 0;

	if (bufferSize) {

		if (bufferSize == 16)
			printf("Empty Event\n");

		p= (uint32_t *)buffer;

		for(i = 0; i < bufferSize/4; i+=nw) {
			nw       = p[i]   & 0x0FFFFFFF;
			ttt      = p[i+3] & 0x3FFFFFFF;
			deltaN   = (ttt-ttt_old);
			deltaTTT = deltaN*8e-9;
			ttt_old  = ttt;
			fprintf(fp, "%f\t%d\n", deltaTTT, deltaN-deltaN1);
			deltaN1  = deltaN;
		}
	}
}
