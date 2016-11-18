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
* @file ApplicationSetup.cpp
* @details Il singleton ApplicationSetup permette di salvare e rendere disponibili in tutte le parti del programma le informazioni necessarie per il funzionamento dell'applicazione.
* Esse sono inserite dall'utente lanciando il programma o inserendo l'apposito input ad applicazione avviata. Dunque, ApplicationSetup ricava le informazioni da argc e argv, cioe'
* dagli argomenti del main. Occorre che la funzione main chiami il metodo ApplicationSetupSet(argc, argv) per riempire ApplicationSetup. Gli argomenti del main vengono analizzati
* in modo classico, utilizzando la funzione "getopt".
* @author Daniele Berto
*/

#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "OutputModule.h"
#include "DefineGeneral.h"
#include <CAENDigitizer.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <thread>
#include <mutex>

using std::mutex;

/*
Implementazione standard dei metodi per implementare il singleton design pattern
*/
ApplicationSetup *
  ApplicationSetup::application_setup_pInstance = NULL;

mutex ApplicationSetup::mtx_constructor;

ApplicationSetup *
ApplicationSetup::Instance ()
{
	mtx_constructor.lock();

	if (!application_setup_pInstance)   // Only allow one instance of class to be generated.
		application_setup_pInstance = new ApplicationSetup ();

	mtx_constructor.unlock();

	return application_setup_pInstance;
}


///Il costruttore (privato) del singleton riempie le varibili contenenti i path dei files usati dal programma e la modalita' di input con valori di default.
ApplicationSetup::ApplicationSetup ()
{
	channel_visualized = 0;

	imset = 0;

	application_setup_log_file_path = (const char *)malloc ((strlen ("./LogFile") + 1));

	strcpy ((char *) application_setup_log_file_path,"./LogFile");

	application_setup_data_file_path =(const char *) malloc ((strlen ("./RawData/data.txt") + 1));
	strcpy ((char *) application_setup_data_file_path, "./RawData/data.txt");

	application_setup_conf_file_path = (const char *)malloc ((strlen ("./ConfigurationFile") + 1));
	strcpy ((char *) application_setup_conf_file_path, "./ConfigurationFile");

	application_setup_input_mode = (const char *) malloc ((strlen ("default") + 1));
	strcpy ((char *) application_setup_input_mode, "default");

	application_setup_data_file_size_path = (const char *) malloc (strlen (application_setup_data_file_path) + 3);
	strcpy ((char *) application_setup_data_file_size_path, application_setup_data_file_path);
	strcat ((char *) application_setup_data_file_size_path, "sz");

	application_setup_data_file_punt = fopen (application_setup_data_file_path, "a");
	application_setup_data_file_size_punt = fopen (application_setup_data_file_size_path, "a");
}


///La funzione ApplicationSetupSet salva nelle varibili argc e argv gli argomenti del main.
void
ApplicationSetup::ApplicationSetupSet (int argc_arg, char **argv_arg)
{

	int i;
	argc = argc_arg;
	argv = (char **) malloc (argc * sizeof (char *));
	for (i = 0; i < argc; i++)
	{
		argv[i] = (char *) malloc (sizeof (char) * (strlen (argv_arg[i]) + 1));
		strcpy (argv[i], argv_arg[i]);
	}
	ApplicationSetup::ArgumentsParsing ();
	ApplicationSetup::FetchInputMode (application_setup_input_mode);

}


int
ApplicationSetup:: ApplicationSetupDataFileModify (const char *application_setup_data_file_path_arg)
{
	mtx_output.lock();	

	free ((void *) application_setup_data_file_path);
	application_setup_data_file_path = NULL;
	application_setup_data_file_path = (char *) malloc (strlen (application_setup_data_file_path_arg) + 1);
	strcpy ((char *) application_setup_data_file_path, application_setup_data_file_path_arg);

	if (application_setup_data_file_punt != NULL)
		fclose (application_setup_data_file_punt);

	application_setup_data_file_punt = fopen (application_setup_data_file_path, "a");

	free ((void *) application_setup_data_file_size_path);
	application_setup_data_file_size_path = NULL;
	application_setup_data_file_size_path = (char *) malloc (strlen (application_setup_data_file_path_arg) + 3);
	strcpy ((char *) application_setup_data_file_size_path, application_setup_data_file_path_arg);
	strcat ((char *) application_setup_data_file_size_path, "sz");

	if (application_setup_data_file_size_punt != NULL)
		fclose (application_setup_data_file_size_punt);

	application_setup_data_file_size_punt = fopen (application_setup_data_file_size_path, "a");

	mtx_output.unlock();  
}


FILE *
ApplicationSetup::ApplicationSetupGetDataFilePunt ()
{
	return application_setup_data_file_punt;
}


FILE *
ApplicationSetup::ApplicationSetupGetDataFileSizePunt ()
{
	return application_setup_data_file_size_punt;
}


void
ApplicationSetup::FetchInputMode (const char *application_setup_input_mode)
{

	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	if (!strcmp ("user", application_setup_input_mode))
	{
		input_mode = 0;
		output_module->Output("User command mode activated\n");
	}

	else if (!strcmp ("tcp", application_setup_input_mode))
	{
		input_mode = 1;
		output_module->Output("Tcp command mode activated\n");
	}

	else if (!strcmp ("all", application_setup_input_mode))
	{
		input_mode = 2;
		output_module->Output("Tcp and User command mode activated\n");
	}

	else
	{
		output_module->Output("Tcp and User command mode activated\n");
		input_mode = 2;
	}
}


void
ApplicationSetup::ArgumentsParsing ()
{

	int c = 0;

	opterr = 0;

	while ((c = getopt (argc, argv, "f:m:d:l:")) != -1)
	switch (c)
	{
		case 'f':
		application_setup_conf_file_path =
		(char *) malloc (strlen (optarg) + 1);
		strcpy ((char *) application_setup_conf_file_path, optarg);
		break;
		
		case 'm':
		application_setup_input_mode = (char *) malloc (strlen (optarg) + 1);
		strcpy ((char *) application_setup_input_mode, optarg);
		break;
		
		case 'd':
		ApplicationSetup::ApplicationSetupDataFileModify (optarg);
		break;
		
		case 'l':
		application_setup_log_file_path =
		(char *) malloc (strlen (optarg) + 1);
		strcpy ((char *) application_setup_log_file_path, optarg);
		break;
		
		case '?':
		if (optopt == 'f' || optopt == 'm' || optopt == 'd' || optopt == 'l')
			fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
			fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		else
			fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
		break;
	}
}
