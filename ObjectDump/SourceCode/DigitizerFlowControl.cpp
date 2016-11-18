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
* @file DigitizerFlowControl.cpp 
*/


#include "DefineGeneral.h"
#include "DefineCommands.h"
#include "TcpUser.h"
#include "ConfObject.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "DigitizerObjectGeneric.h"
#include "RawData.h"
#include "DigitizerStateMachine.h"
#include "ApplicationSetup.h"
#include "Input.h"
#include "CommunicationObject.h"
#include "DigitizerFlowControl.h"
#include "ConfigurationConsistence.h"
#include "AnalizzatoreUtils.h"
#include "Analizzatore.h"
#include "OutputModule.h"
#include <CAENDigitizer.h>
#include <stdio.h>
#include <regex.h>
#include <stdbool.h>

pthread_cond_t DigitizerFlowControl::input_flow_cond;

pthread_mutex_t DigitizerFlowControl::input_flow_mutex;

// L'oggetto di tipo DigitizerFlowControl viene inizializzato ottenendo un riferimento all'oggetto ApplicationSetup.
DigitizerFlowControl::DigitizerFlowControl ()
{
	pthread_mutex_init (&input_flow_mutex, NULL);
	pthread_cond_init (&input_flow_cond, NULL);
	digitizer_flow_control_application_setup = ApplicationSetup::Instance ();
}


void
DigitizerFlowControl::DigitizerFlowControlStart ()
{

	//Viene ottenuto il riferimento ad OutputModule.
	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	//E' ottenuto un riferimento al singleton LogFile. LogFile permette di stampare (in modo verboso) il risultato delle funzioni della libreria CAENDigitizer nel file di log
	//indicato dal flag -l [logfilepath]. Se l'utente non ne indica uno, viene usato quello di default. Il contenuto del log file puo' essere visualizzato digitando il comando "more".
	//Questo permette di controllare rapidamente se gli altri comandi che interagiscono col digitizer hanno avuto successo. 
	LogFile *logfile;
	logfile = LogFile::Instance ();

	//Variabili di supporto
	char *my_punt;
	char *conf_file_string;
	char *data_file_string;
	char *log_file_string;
	int i;
	int Address = 0;
	int Data = 0;
	char support_string[STANDARDBUFFERLIMIT];
	bzero (support_string, STANDARDBUFFERLIMIT);
	int support_flag1 = 0;

	//Impostazione del log file. Il metodo LogFileSet apre il log file indicato dal flag -l [logfilepath] o quello di default.
	logfile->LogFileSet (digitizer_flow_control_application_setup->
	application_setup_log_file_path);

	//L'oggetto di tipo ConfigurationConsistence serve per controllare se nel file di configurazione sono presenti errori gravi (come l'omissione dell'attributo OPEN).
	//Questo tipo di controllo e' possibile farlo con il comando "check".
	ConfigurationConsistence my_consistence;

	//Se il metodo ConfigurationConsistenceConfFileSetupEssentialWithInitCheckNoPrint torna un valore diverso da zero vuol dire che ha trovato un errore nel file di configurazione.
	if (my_consistence.ConfigurationConsistenceConfFileSetupEssentialWithInitCheckNoPrint (digitizer_flow_control_application_setup->application_setup_conf_file_path))
	{
		fprintf (stderr, "%s: Some error detected in %s config file\n",
		__func__,
		digitizer_flow_control_application_setup->application_setup_conf_file_path);
	}

	//DigitizerStateMachine e' il tipo di oggetto che comunica col digitizer. E', insieme a DigitizerFlowControl, il tipo di oggetto piu' delicato perche' lancia e gestisce
	//il thread che comunica col digitizer e quelli che trattano i dati letti.
	DigitizerStateMachine miodigitizer;

	//buffer contiene il comando fetchato da stdin o da tcp.
	char *buffer;
	buffer = (char *) malloc (STANDARDBUFFERLIMIT);
	TcpUser command_received_by_user;

	//A seconda della modalita' di interazione scelta dall'utente (-m [all|user|tcp]) occorre inizializzare gli oggetti per fetchare l'input.

	//Se l'utente ha lanciato il programma con il flag -m all, vuol dire che l'input puo' essere inviato sia da stdin sia da tcp.
	if (digitizer_flow_control_application_setup->input_mode == ALL)
	{

		//Inizializzo CommunicationObject che raccoglie l'input via TCP.
		mioinput = new Input(); //modifica Input
		mioTCP.CommunicationObjectInit ();

	}	//if (digitizer_flow_control_application_setup->input_mode == ALL)

	//Se l'utente ha lanciato il programma con il flag -m user, vuol dire che l'input puo' essere inviato solo da stdin.
	else if (digitizer_flow_control_application_setup->input_mode == ONLYUSER)
	{
		mioinput = new Input(); //modifica Input
	}   //if (digitizer_flow_control_application_setup->input_mode == ONLYUSER)

	//Se l'utente ha lanciato il programma con il flag -m tcp, vuol dire che l'input puo' essere inviato solo via TCP.
	else if (digitizer_flow_control_application_setup->input_mode == ONLYTCP)
	{
	//Inizializzo CommunicationObject che raccoglie l'input via TCP.
		mioTCP.CommunicationObjectInit ();
	}	//if (digitizer_flow_control_application_setup->input_mode == ONLYTCP)

	//Se il programma e' stato eseguito in una modalita' diversa da ONLYTCP, stampo la stringa di benvenuto.
	//I controlli vengono fatti dai singleton StdOut e TcpUserArray.
	output_module->Output("objectDump, press help to get the available command list\n");

	//Inizia il ciclo di fetch-decode-execute-writeback del programma.
	while (1)
	{

		//Se il programma e' stato lanciato in una modalita' diversa da quella ONLYTCP e' effettuato un flush del stdout per stampare l'eventuale output bufferizzato.
		//Per evitare la bufferizzazione e' anche possibile stampare su stderr invece che su stdout, ma risulterebbe essere un utilizzo non appropriato di stderr.
		if (digitizer_flow_control_application_setup->input_mode != ONLYTCP)
		{
			fflush (stdout);
		}

		//Settiamo a zero buffer: in esso viene salvato il contenuto dell'input ottenuto dagli oggetti di tipo CommunicationObject (per il TCP) e Input (per l'stdout).
		bzero (buffer, STANDARDBUFFERLIMIT);

		//Azioni da intraprende per fetchare l'input se il programma e' stato lanciato sia in modalita' tcp che in modalita' user
		if (digitizer_flow_control_application_setup->input_mode == ALL)
		{

			//Chiedo l'input da stdin. Se non c'e' il programma prosegue e in buffer e' salvata la stringa vuota.
			mioinput->GetInput (buffer);

			//Se e' stato ricevuto un input dall'stdin, e' stampata una stringa che lo riproduce.
			if (strlen (buffer) != 0)
			{

				///////Armo output_module///////
				output_module->OutputModuleStdoutOn();
				output_module->OutputModuleSockidOff();
				///////Armo output_module///////

			}

			//Chiedo l'input inviato da TCP. 
			command_received_by_user = mioTCP.GetCommand ();

			//Se e' stato ricevuto un input via tcp.
			if (command_received_by_user.command_sent_by_user != 0)
			{

				//E' stampata una stringa che scrive l'input ricevuto e il sockid di chi l'ha inviato. 
				//Questo avviene solo se il programma e' lanciato in modalita' -m all: se e' lanciato in modalita' -m tcp, esso e' eseguito in modalita' demone.
				//Di conseguenza, in quest'ultimo caso non stampa niente a video.
				//~ fprintf (stdout, "Command sent by user %d: %d which means %s\n",
				//~ command_received_by_user.user_sockid,
				//~ command_received_by_user.command_sent_by_user,
				//~ DigitizerFlowControl::
				//~ ParseCommand (command_received_by_user.
				//~ command_sent_by_user));

				///////Armo output_module///////
				output_module->OutputModuleStdoutOff();
				output_module->OutputModuleSockidOn(command_received_by_user.user_sockid);
				///////Armo output_module///////

			}
		}  //if (digitizer_flow_control_application_setup->input_mode == ALL)

		//Azioni da intraprende per fetchare l'input se il programma e' stato lanciato in modalita' user.
		else if (digitizer_flow_control_application_setup->input_mode ==
		ONLYUSER)
		{

			//Chiedo l'input dall'stdin. Se non c'e' il programma prosegue e in buffer e' salvata la stringa vuota.
			mioinput->GetInput (buffer);

			//Se e' stato ricevuto un input dall'stdin, e' stampata una stringa che lo riproduce.
			if (strlen (buffer) != 0)
			{
				///////Armo output_module///////
				output_module->OutputModuleStdoutOn();
				output_module->OutputModuleSockidOff();
				///////Armo output_module///////
			}
		}  //if (digitizer_flow_control_application_setup->input_mode == ONLYUSER)

		//Azioni da intraprende per fetchare l'input se il programma e' stato lanciato in modalita' tcp.
		else if (digitizer_flow_control_application_setup->input_mode ==
		ONLYTCP)
		{

			//Chiedo l'input inviato da TCP.
			command_received_by_user = mioTCP.GetCommand ();

			//Se e' stato ricevuto un input via TCP.
			if (command_received_by_user.command_sent_by_user != 0)
			{

				///////Armo output_module///////
				output_module->OutputModuleStdoutOff();
				output_module->OutputModuleSockidOn(command_received_by_user.user_sockid);
				///////Armo output_module///////

			}

		}  //if (digitizer_flow_control_application_setup->input_mode == ONLYTCP)

		pthread_mutex_lock (&input_flow_mutex);

		//Se e' stato ricevuto un input via tcp o via stdin.
		if (strlen (buffer) != 0
		|| command_received_by_user.command_sent_by_user != 0)
		{
			//Se e' stato ricevuto il comando di exit o di quit
			if (DigitizerFlowControl::reg_matches (buffer, "^[Ee][Xx][iI][tT][ \t]*$") || DigitizerFlowControl::reg_matches (buffer, "^[qQ][uU][iI][tT]$") || command_received_by_user.command_sent_by_user == QUIT)	//11==quit
			{
				/////////Armo output_module/////////
				output_module->OutputModuleStdoutOn();
				/////////Armo output_module/////////

				//Spengo CommunicationObject
				if (digitizer_flow_control_application_setup->input_mode !=
				ONLYUSER)
					mioTCP.Finish ();

				//Spengo Input.
				if (digitizer_flow_control_application_setup->input_mode !=
				ONLYTCP)
					mioinput->Finish ();

				//Spengo DigitizerStateMachine.
				miodigitizer.DigitizerStateMachineQuit ();
				return;
			}

			//Se e' stato ricevuto il comando init.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[iI][Nn][Ii][Tt][ \t]*$")
			|| command_received_by_user.command_sent_by_user == INIT)
			{
				//Controllo se il file di configurazione contiene l'attributo OPEN e se e' ben settato. Il NoPrint del nome del metodo indica che non viene stampato a video
				//il parametro OPEN del file di configurazione.
				if (!my_consistence.
				ConfigurationConsistenceConfFileInitNoPrint
				(digitizer_flow_control_application_setup->
				application_setup_conf_file_path))
				{
					//Se non ci sono errori nell'attributo OPEN del file di configurazione, il digitizer viene aperto.
					output_module->Output("Digitizer initialization...\n");

					logfile->LogFileMessageOn();

					miodigitizer.
					DigitizerStateMachineInit
					(digitizer_flow_control_application_setup->
					application_setup_conf_file_path);

					logfile->LogFileMessageOff();
				}
				else
				{
					//Altrimenti non viene intrapresa nessuna azione
					output_module->Output("Some error occured, I cannot execute init command\n");
				}
			}

			//Se e' stato ricevuto il comando setup.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Ss][Ee][Tt][Uu][Pp][ \t]*$")
			|| command_received_by_user.command_sent_by_user == SETUP)
			{
				//Controllo se il file di configurazione contiene i parametri fondamentali per settare il digitizer.
				if (!my_consistence.
				ConfigurationConsistenceConfFileSetupEssentialWithInitCheckNoPrint
				(digitizer_flow_control_application_setup->
				application_setup_conf_file_path))
				{
					//Se non ci sono errori nell'attributo OPEN del file di configurazione, il digitizer viene impostato.
					output_module->Output("Digitizer setupping...\n");

					logfile->LogFileMessageOn();

					miodigitizer.
					DigitizerStateMachineSetup
					(digitizer_flow_control_application_setup->
					application_setup_conf_file_path);

					logfile->LogFileMessageOff();
				}
			}

			//Se e' stato ricevuto il comando start.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[sS][tT][aA][rR][tT][ \t]*$")
			|| command_received_by_user.command_sent_by_user == START)
			{
				//Il programma inizia ad acquisire dati dal digitizer. Di default nessun thread di trattamento dei dati e' attivo.
				output_module->Output("Data acquisition starting...\n");

				miodigitizer.DigitizerStateMachineStartReading ();

			}

			//Se e' stato ricevuto il comando stop.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[sS][tT][oO][pP][ \t]*$")
			|| command_received_by_user.command_sent_by_user == STOP)
			{
				//Il programma termina l'acquisizione dei dati dal digitizer.                           
				output_module->Output("Stopping data acquisition...\n");

				miodigitizer.DigitizerStateMachineStopReading ();

			}

			//Se e' stato ricevuto il comando prestart.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[pP][rR][eE][sS][tT][aA][rR][tT][ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			PRESTART)
			{
				//Il programma inizia il preprocessamento dei dati raccolti.
				output_module->Output("Starting Thread Preprocessing...\n");
				miodigitizer.DigitizerStateStartPreprocessing ();
			}

			//Se e' stato ricevuto il comando prestop.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[pP][Rr][Ee][Ss][Tt][Oo][Pp][ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			PRESTOP)
			{
				//Il programma termina il preprocessamento dei dati raccolti.
				output_module->Output("Stopping Thread Preprocessing...\n");
				miodigitizer.DigitizerStateStopPreprocessing ();
			}

			//Se e' stato ricevuto il comando rawstart.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[rR][aA][wW][sS][tT][aA][rR][tT]([ \t]+[0-9]{1,8}){0,1}[ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			RAWSTART)
			{
				if (command_received_by_user.command_sent_by_user == RAWSTART)
					miodigitizer.num_events_to_write = atoi(command_received_by_user.first_parameter); 
				else
					miodigitizer.num_events_to_write = FindIntegerValue(buffer);
				//Il programma inizia la scrittura su disco dei dati raccolti.
				output_module->Output("Starting Thread RawDataWriting...\n");
				miodigitizer.DigitizerStateStartRawDataWriting ();
			}

			//Se e' stato ricevuto il comando rawstop.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Rr][Aa][Ww][Ss][Tt][Oo][Pp][ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			RAWSTOP)
			{
				//Il programma termina la scrittura su disco dei dati raccolti.
				output_module->Output("Stopping Thread RawDataWriting...\n");
				miodigitizer.DigitizerStateStopRawDataWriting ();
			}

			//Se e' stato ricevuto il comando vistart.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Vv][Ii][Ss][Tt][Aa][Rr][Tt][ \t]+[0-9]{1,3}[ \t]*$"))
			{
				digitizer_flow_control_application_setup -> channel_visualized = FindIntegerValue(buffer);
				//Il programma inizia la visualizzazione dei dati raccolti.
				output_module->Output("Starting Thread Visualization...\n");
				miodigitizer.DigitizerStateStartVisualization ();
			}

			//Se e' stato ricevuto il comando vistop.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Vv][Ii][Ss][Tt][Oo][Pp][ \t]*$"))
			{
				//Il programma termina la visualizzazione dei dati raccolti.
				output_module->Output("Stopping Thread Visualization...\n");
				miodigitizer.DigitizerStateStopVisualization ();
			}

			//Se e' stato ricevuto il comando send.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Ss][Ee][Nn][Dd][ \t]*$")
			|| command_received_by_user.command_sent_by_user == SEND)
			{
				//Il programma invia un software trigger
				output_module->Output("Sending software trigger...\n");
				miodigitizer.DigitizerStateMachineSendSWTrigger ();
			}

			//Se e' stato ricevuto il comando close
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[cC][lL][oO][sS][eE][ \t]*$")
			|| command_received_by_user.command_sent_by_user == CLOSE)
			{
				//Il programma chiude il digitizer
				output_module->Output("Closing Digitizer...\n");
				miodigitizer.DigitizerStateMachineQuit ();
			}

			//Se e' stato ricevuto il comando print.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[pP][rR][iI][nN][tT][ \t]*$")
			|| command_received_by_user.command_sent_by_user == PRINT)
			{
				//Il programma stampa in formato comprensibile le informazioni contenuto nell'oggetto di tipo ConfObject che viene usato per configurare il
				//digitizer. Le informazioni contenuto nell'oggetto sono prese dal file di configurazione con i comandi init (attributo open) e setup (tutti gli
				//altri).
				output_module->Output("Printing ConfObject parameters in human readable format...\n");
				miodigitizer.digitizer.internal_config.PrintAllHuman ();
			}

			//Se e' stato ricevuto il comando print files.
			else if (DigitizerFlowControl::
			reg_matches (buffer,
			"^[pP][rR][iI][nN][tT][ \t]+[Ff][Ii][Ll][Ee][Ss][ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			PRINTFILES)
			{
				//Viene stampato il path dei file di configurazione, di log e di salvataggio dei rawdata.

				//Path del file di configurazione.
				if (digitizer_flow_control_application_setup->
				application_setup_conf_file_path != NULL)
				{
					output_module->Output(digitizer_flow_control_application_setup->
					application_setup_conf_file_path);
					output_module->Output("\n");
				}

				//Path del file di salvataggio dei rawdata.
				if (digitizer_flow_control_application_setup->
				application_setup_data_file_path != NULL)
				{
					output_module->Output(digitizer_flow_control_application_setup->
					application_setup_data_file_path);
					output_module->Output("\n");
				}

				//Path del file di log.
				if (digitizer_flow_control_application_setup->
				application_setup_log_file_path != NULL)
				{
					output_module->Output(digitizer_flow_control_application_setup->
					application_setup_log_file_path);
					output_module->Output("\n");
				}
			}

			//Se e' stato ricevuto il comando help (solo da stdin perche' i clients connessi via tcp hanno il loro comando help).
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[hH][eE][lL][pP][ \t]*$"))
			{
				//Viene stampata la lista dei comandi disponibili.
				printf ("Listing commands available...\n");
				DigitizerFlowControl::Help ();
			}

			//Se e' stato ricevuto il comando -f [pathdellogfile]
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[-][Ff][ ].+$")
			|| command_received_by_user.command_sent_by_user ==
			CHANGECONF)
			{
				//Se il comando e' stato inviato da tcp, viene ricavato il path del file di configurazione dalla stringa contenuta in buffer.
				if (command_received_by_user.command_sent_by_user == CHANGECONF)
				{
					output_module->Output("Changing conf file...\n");
					free ((void *) digitizer_flow_control_application_setup->
					application_setup_conf_file_path);
					digitizer_flow_control_application_setup->
					application_setup_conf_file_path = NULL;
					digitizer_flow_control_application_setup->
					application_setup_conf_file_path =
						(char *)
						malloc (strlen (command_received_by_user.first_parameter)
						+ 1);
					strcpy ((char *) digitizer_flow_control_application_setup->
						application_setup_conf_file_path,
						command_received_by_user.first_parameter);
				}
				else
				{
					//Se il comando e' stato inviato da stdin, viene ricavato il path del file di configurazione dalla stringa contenuta in buffer. 
					//In questo caso la stringa e' un po' diversa rispetto a quella del caso precedente: per questo e' necessario distinguere le procedure 
					//di ottenimento della stringa.
					my_punt = FindPointer (buffer);
					output_module->Output("Changing conf file...\n");
					free ((void *) digitizer_flow_control_application_setup->
					application_setup_conf_file_path);
					digitizer_flow_control_application_setup->
						application_setup_conf_file_path = NULL;
					digitizer_flow_control_application_setup->
						application_setup_conf_file_path =
						(char *) malloc (strlen (my_punt) + 1);
					strcpy ((char *) digitizer_flow_control_application_setup->
						application_setup_conf_file_path, my_punt);
				}
			}

			//Se e' stato ricevuto il comando -f [pathdelrawdatafile]
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[-][Dd][ ].+$")
			|| command_received_by_user.command_sent_by_user ==
			CHANGEDATA)
			{
				//WARNING: modificare il path del data file non e' banale perche' si richia l'inconsistenza delle operazioni se l'acquisizione e' attiva.
				//Qui si e' preferito interrompere l'acquisizione, modificare il path del data file e riprendere l'acquisizione.
				//Se il comando e' stato inviato da tcp, viene ricavato il path del file di salvataggio dei rawdata dalla stringa contenuta in buffer.
				if (command_received_by_user.command_sent_by_user == CHANGEDATA)
				{
					output_module->Output("Changing data file...\n");
					if (miodigitizer.go_raw_data == 1)
					{
						support_flag1 = 1;
						miodigitizer.DigitizerStateStopRawDataWriting ();
					}
					digitizer_flow_control_application_setup->
					ApplicationSetupDataFileModify (command_received_by_user.
					first_parameter);
					if (support_flag1 == 1)
					{
						support_flag1 = 0;
						miodigitizer.DigitizerStateStartRawDataWriting ();
					}
				}
				else
				{
					//Se il comando e' stato inviato da stdin, viene ricavato il path del file di salvataggio dei rawdata dalla stringa contenuta in buffer.
					//In questo caso la stringa e' un po' diversa rispetto a quella del caso precedente: per questo e' necessario distinguere le procedure 
					//di ricavo della stringa.
					my_punt = FindPointer (buffer);
					output_module->Output("Changing data file...\n");
					if (miodigitizer.go_raw_data == 1)
					{
						support_flag1 = 1;
						miodigitizer.DigitizerStateStopRawDataWriting ();
					}
					digitizer_flow_control_application_setup->
					ApplicationSetupDataFileModify (my_punt);
					if (support_flag1 == 1)
					{
						support_flag1 = 0;
						miodigitizer.DigitizerStateStartRawDataWriting ();
					}
				}
			}

			//Se e' stato ricevuto il comando -l [pathdellogfile]
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[-][Ll][ ].+$")
			|| command_received_by_user.command_sent_by_user ==
			CHANGELOG)
			{
				//Se il comando e' stato inviato da tcp, viene ricavato il path del log file dalla stringa contenuta in buffer.
				if (command_received_by_user.command_sent_by_user == CHANGELOG)
				{
					output_module->Output("Changing log file...\n");
					free ((void *) digitizer_flow_control_application_setup->
						application_setup_log_file_path);
					digitizer_flow_control_application_setup->
						application_setup_log_file_path = NULL;
					digitizer_flow_control_application_setup->
						application_setup_log_file_path =
						(char *)
						malloc (strlen (command_received_by_user.first_parameter)
						+ 1);
					strcpy ((char *) digitizer_flow_control_application_setup->
						application_setup_log_file_path,
						command_received_by_user.first_parameter);
					logfile->
						LogFileSet (digitizer_flow_control_application_setup->
						application_setup_log_file_path);
				}
				else
				{
					//Se il comando e' stato inviato da stdin, viene ricavato il path del log file dalla stringa contenuta in buffer.
					//In questo caso la stringa e' un po' diversa rispetto a quella del caso precedente: per questo e' necessario distinguere le procedure 
					//di ricavo della stringa.
					my_punt = FindPointer (buffer);
					output_module->Output("Changing log file...\n");
					free ((void *) digitizer_flow_control_application_setup->
						application_setup_log_file_path);
					digitizer_flow_control_application_setup->
						application_setup_log_file_path = NULL;
					digitizer_flow_control_application_setup->
						application_setup_log_file_path =
						(char *) malloc (strlen (my_punt) + 1);
					strcpy ((char *) digitizer_flow_control_application_setup->
						application_setup_log_file_path, my_punt);
					logfile->
						LogFileSet (digitizer_flow_control_application_setup->
						application_setup_log_file_path);
				}
			}

			//Se e' stato inviato il comando check.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Cc][Hh][Ee][Cc][Kk][ \t]*$")
			|| command_received_by_user.command_sent_by_user == CHECK)
			{
				//Viene controllato se il file di configurazione contiene manca dei parametri fondamentali per impostare il digitizer.
				output_module->Output("Checking configuration file...\n");
				my_consistence.
					ConfigurationConsistenceConfFileSetupEssentialWithInitCheck
					(digitizer_flow_control_application_setup->
					application_setup_conf_file_path);
			}

			//Se e' stato inviato il comando chkconf.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Cc][Hh][Kk][Cc][Oo][Nn][Ff][ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			CHKCONF)
			{
				//Viene stampato il contenuto del file di configurazione: come contenuto si intendono le stringhe riconosciute dallo scanner generato in flex.
				output_module->Output("These are the settings read from the configuration file...\n");
				AnalizzaPrint (digitizer_flow_control_application_setup->
				application_setup_conf_file_path);
			}

			//Se e' stato inviato il comando write register [address] [data].
			else if (DigitizerFlowControl::
			reg_matches (buffer,
			"^[Ww][Rr][Ii][Tt][Ee][ \t]+[Rr][Ee][Gg][Ii][Ss][Tt][Ee][Rr][ \t]+(0x[0-9a-fA-F]{1,16})[ \t]+(0x[0-9a-fA-F]{1,16})[ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			WRITEREGISTER)
			{

				Data = -1;

				//Se il comando e' stato inviato da tcp, vengono ricavati i dati dalla stringa inviata.                 
				if (command_received_by_user.command_sent_by_user ==
				WRITEREGISTER)
				{
					Address =
					strtoul (command_received_by_user.first_parameter, NULL,
						16);
					my_punt =
					FindPointer (command_received_by_user.first_parameter);
					Data = strtoul (my_punt, NULL, 16);
					bzero (support_string, STANDARDBUFFERLIMIT);
					snprintf (support_string, STANDARDBUFFERLIMIT,
						"Writing 0x%x in the 0x%x register\n", Data,
						Address);
					output_module->Output(support_string);
					miodigitizer.digitizer.
						DigitizerObjectWriteRegister (Address, Data);

					//Viene stampato il contenuto del registro modificato per poter rendersi subito conto se la modifica e' avvenuto con successo.
					Data = -1;
					miodigitizer.digitizer.
						DigitizerObjectReadRegister (Address, &Data);
					bzero (support_string, STANDARDBUFFERLIMIT);
					snprintf (support_string, STANDARDBUFFERLIMIT,
						"The data in the 0x%x register are: 0x%x\n",
						Address, Data);
					output_module->Output(support_string);
				}
				else
				{
					//Se il comando e' stato inviato da stdin, vengono ricavati i dati dalla stringa inviata: la stringa e' leggermente diversa rispetto a quella del
					//caso precedente quindi e' stato necessario distinguere i due casi.
					my_punt = FindPointer (buffer);
					my_punt = FindPointer (my_punt);
					Address = strtoul (my_punt, NULL, 16);
					my_punt = FindPointer (my_punt);
					Data = strtoul (my_punt, NULL, 16);
					bzero (support_string, STANDARDBUFFERLIMIT);
					snprintf (support_string, STANDARDBUFFERLIMIT,
						"Writing 0x%x in the 0x%x register\n", Data,
						Address);
					output_module->Output(support_string);
					miodigitizer.digitizer.
						DigitizerObjectWriteRegister (Address, Data);
					//Viene stampato il contenuto del registro modificato per poter rendersi subito conto se la modifica e' avvenuto con successo.
					Data = -1;
					miodigitizer.digitizer.
						DigitizerObjectReadRegister (Address, &Data);
					bzero (support_string, STANDARDBUFFERLIMIT);
					snprintf (support_string, STANDARDBUFFERLIMIT,
						"The data in the 0x%x register are: 0x%x\n",
						Address, Data);
					output_module->Output(support_string);
				}
			}

			//Se e' stato inviato il comando read register [address] [data].
			else if (DigitizerFlowControl::
			reg_matches (buffer,
			"^[Rr][Ee][Aa][Dd][ \t]+[Rr][Ee][Gg][Ii][Ss][Tt][Ee][Rr][ \t]+(0x[0-9a-fA-F]{1,16})[ \t]*$")
			|| command_received_by_user.command_sent_by_user ==
			READREGISTER)
			{

				Data = -1;

				//Se l'input e' stato ricevuto via tcp.
				if (command_received_by_user.command_sent_by_user ==
				READREGISTER)
				{
					//Ricavo dalla stringa contenuta in buffer l'indirizzo del registro da leggere.                         
					Address =
					strtoul (command_received_by_user.first_parameter, NULL,
						16);
					miodigitizer.digitizer.
					DigitizerObjectReadRegister (Address, &Data);
					bzero (support_string, STANDARDBUFFERLIMIT);
						snprintf (support_string, STANDARDBUFFERLIMIT,
						"I've found this data in the 0x%x register: 0x%x\n",
						Address, Data);
					output_module->Output(support_string);
				}
				else
				{
					//Se l'input e' stato ricevuto via stdin, ricavo dalla stringa contenuta in buffer l'indirizzo del registro da leggere. 
					my_punt = FindPointer (buffer);
					my_punt = FindPointer (my_punt);
					Address = strtoul (my_punt, NULL, 16);
					miodigitizer.digitizer.
						DigitizerObjectReadRegister (Address, &Data);
					bzero (support_string, STANDARDBUFFERLIMIT);
					snprintf (support_string, STANDARDBUFFERLIMIT,
						"I've found this data in the 0x%x register: 0x%x\n",
						Address, Data);
					output_module->Output(support_string);
				}
			}

			//Se e' stato inviato il comando "more".
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Mm][Oo][Rr][Ee][ \t]*$")
			|| command_received_by_user.command_sent_by_user == MORE)
			{
				//Viene letto il contenuto del log file.
				logfile->LogFileRead ();
			}

			//Se e' stato ricevuto il comando status.
			else if (DigitizerFlowControl::
			reg_matches (buffer, "^[Ss][Tt][Aa][Tt][Uu][Ss][ \t]*$")
			|| command_received_by_user.command_sent_by_user == STATUS)
			{
				//Viene stampato lo stato (ON/OFF) del thread che acquisisce i dati e di quelli che operano sui dati
				miodigitizer.DigitizerStateMachinePrintStatus ();
			}

			//Se il comando non e' stato riconosciuto.
			else
			{
				output_module->StdOutInsert ("Unrecognized command\n");
				output_module->StdOutPrint ();
			}

		}
		else
			pthread_cond_wait (&input_flow_cond, &input_flow_mutex);
		// strlen (buffer) != 0 || command_received_by_user.command_sent_by_user != 0
		pthread_mutex_unlock (&input_flow_mutex);
	}   // while 1

}   //void DigitizerFlowControl::DigitizerFlowControlStart ()

//Funzione per confrontare una stringa con un'espressione regolare.
bool DigitizerFlowControl::reg_matches (const char *str, const char *pattern)
{
  regex_t
    re;
  int
    ret;

  if (regcomp (&re, pattern, REG_EXTENDED) != 0)
    return false;

  ret = regexec (&re, str, (size_t) 0, NULL, 0);
  regfree (&re);

  if (ret == 0)
    return true;

  return false;
}

//Funzione di aiuto.
void
DigitizerFlowControl::Help ()
{
  printf ("Available command list:\n");
  printf ("init: open the digitizer\n");
  printf ("setup: setup the digitizer\n");
  printf ("start: start the data acquisition\n");
  printf ("stop: stop the data acquisition\n");
  printf ("prestart: start the preprocessing thread\n");
  printf ("prestop: stop the preprocessing thread\n");
  printf ("vistart [channelnumber]: start the visualization thread\n");
  printf ("vistop: stop the visualization thread\n");
  printf ("rawstart [num_events]: start the raw data writing thread saving at most the events indicated by num_events\n");
  printf ("rawstop: stop the raw data writing thread\n");
  printf ("close: close the digitizer\n");
  printf ("send: send a software trigger\n");
  printf ("help\n");
  printf ("check: check the correctness of the configuration file\n");
  printf ("chkconf: print the content of the configuration file\n");
  printf ("write register 0x[register] 0x[data]\n");
  printf ("read register 0x[register]\n");
  printf ("-f [conf file path]: change the configuration file path\n");
  printf ("-d [data file path]: change the data file path\n");
  printf ("-l [log file path]: change the log file path\n");
  printf ("print: print the internal configuration object used to configure the digitizer\n");
  printf ("print files: print the path of the configuration file, of the data file and of the log file\n");
  printf ("status: print the status of the threads acquisition, preprocessing, raw data and visualization\n");
  printf ("more: display the content of the logfile\n");
  printf ("exit: quit program\n");
  printf ("quit: quit program\n");
}

//Funzione per stampare il significato dei comandi inviati via tcp.
const char *
DigitizerFlowControl::ParseCommand (int comando_inviato_da_tcp)
{
  switch (comando_inviato_da_tcp)
    {

    case INIT:

      return "init";

    case SETUP:

      return "setup";

    case PRESTART:

      return "prestart";

    case PRESTOP:

      return "prestop";

    case VISTART:

      return "vistart";

    case VISTOP:

      return "vistop";

    case START:

      return "start";

    case STOP:

      return "stop";

    case SEND:

      return "send";

    case CLOSE:

      return "close";

    case QUIT:

      return "quit";

    case RAWSTART:

      return "rawstart";

    case RAWSTOP:

      return "rawstop";

    case PRINT:

      return "print";

    case CHECK:

      return "check";

    case CHKCONF:

      return "chkconf";

    case MORE:

      return "more";

    case CHANGECONF:

      return "-f [conf_file_path]";

    case CHANGEDATA:

      return "-d [data_file_path]";

    case CHANGELOG:

      return "-l [log_file_path]";

    case WRITEREGISTER:

      return "write register";

    case READREGISTER:

      return "read register";

    case PRINTFILES:

      return "print files";

    case STATUS:

      return "status";

    default:
      return "Unrecognized command";
    }
}
