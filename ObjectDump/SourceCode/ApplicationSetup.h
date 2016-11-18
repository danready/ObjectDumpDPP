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
* @file ApplicationSetup.h
* @author Daniele Berto
*/

/**
* @brief The ApplicationSetup class gets the application setup from the main parameters.
* @details Il singleton ApplicationSetup permette di salvare e rendere disponibili in tutte le parti del programma le informazioni necessarie per il funzionamento dell'applicazione.
* Esse sono inserite dall'utente lanciando il programma o inserendo l'apposito input ad applicazione avviata. Dunque, ApplicationSetup ricava le informazioni da argc e argv, cioe'
* dagli argomenti del main. Occorre che la funzione main chiami il metodo ApplicationSetupSet(argc, argv) per riempire ApplicationSetup. Gli argomenti del main vengono analizzati
* in modo classico, utilizzando la funzione "getopt".
* @author Daniele Berto
*/

#ifndef APPLICATIONSETUP_H
#define APPLICATIONSETUP_H

#include <thread>
#include <mutex>

using std::mutex;

class ApplicationSetup
{
private:

  /**
   * @brief The application_setup_pInstance reference is used to implement the singleton design pattern.
   */
  static ApplicationSetup *application_setup_pInstance;

  /**
   * @brief The ApplicationSetup constructor sets imset variable to zero.
   */
    ApplicationSetup ();
public:

  /**
   * @brief The channel_visualized variable indicates the channel being visualized. The variable is modified by the command "vistart [channelnumber]".
   */
  int channel_visualized;

  /**
   * @brief Instance() method is used to implement the singleton design pattern: it returns a reference to ApplicationSetup.
   */
  static ApplicationSetup *Instance ();

  /**
   * @brief The application_setup_conf_object variable is used for application settings.
   */
  ConfObject application_setup_conf_object;

  /**
   * @brief The application_setup_conf_file variable is used to open the configuration file.
   */
  FILE *application_setup_conf_file;

  /**
   * @brief The argc variable is used for counting the number of the main arguments.
   */
  int argc;

  /**
   * @brief The imset variable indicates if the object has been set.
   */
  int imset;

  /**
   * @brief The argv variable contains the main arguments.
   */
  char **argv;

  /**
   * @brief The ApplicationSetupDataFileModify method modifies the path of the data file and open it.
   */
  int ApplicationSetupDataFileModify (const char
				      *application_setup_data_file_path_arg);

  /**
   * @brief The ApplicationSetupGetDataFilePunt method modifies copies in application_setup_data_file_punt_arg the private attrivute application_setup_data_file_punt.
   */
  FILE *ApplicationSetupGetDataFilePunt ();

  /**
   * @brief The ApplicationSetupGetDataFileSziePunt method modifies copies in application_setup_data_file_punt_arg the private attrivute application_setup_data_file_size_punt.
   */
  FILE *ApplicationSetupGetDataFileSizePunt ();

  /**
   * @brief The application_setup_conf_file_path variable contains the path of the configuration file.
   */
  const char *application_setup_conf_file_path;

  /**
   * @brief The application_setup_log_file_path variable contains the path of the log file.
   */
  const char *application_setup_log_file_path;

  /**
   * @brief The application_setup_data_file_path variable contains the path of the rawdata file.
   */
  const char *application_setup_data_file_path;

  /**
   * @brief The application_setup_data_file_size_path variable contains the path of the file contains the rawdata sizes.
   */
  const char *application_setup_data_file_size_path;

  /**
   * @brief The application_setup_input_mode variable contains the input mode (as it is inserted with -m flag).
   */
  const char *application_setup_input_mode;

  /**
   * @brief The application_setup_data_file_punt variable contains a pointer to the rawdata file.
   */
  FILE *application_setup_data_file_punt;

  /**
   * @brief The application_setup_data_file_size_punt variable contains a pointer to the file contains the rawdata sizes.
   */
  FILE *application_setup_data_file_size_punt;

  /**
   * @brief The input_mode variable contains the input mode chosed by the user.
   */
  int input_mode;

  /**
   * @brief The ArgumentsParsing() method extracts informations from argc and argv.
   */
  void ArgumentsParsing ();

  /**
   * @brief The ApplicationSetupSet method picks up settings informations from the main arguments using ArgumentsParsing() method.
   * @details The caller has to pass argc and argv of the main function.
   * @param argc is the number of the main arguments
   * @param argv are the main arguments.
   * @return void
   */
  void ApplicationSetupSet (int argc, char **argv);

  /**
   * @brief The FetchInputMode method picks up the input mode from argv and puts it in input_mode variable.
   * @details This method is called by application_setup_set method.
   * @return void
   */
  void FetchInputMode (const char *application_setup_input_mode);
  
  /**
  * @brief The mtx_constructor mutex avoids race conditions in the object construction.
  */
  static mutex mtx_constructor;
  
  /**
  * @brief The mtx_output mutex avoids race conditions inside the methods of the object.
  */    
  mutex mtx_output;    
  
};

#endif
