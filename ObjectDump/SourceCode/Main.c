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
* @mainpage objectDump application
*
* objectDump is an useful support to data acquisition with CAEN digitizers.
* The application has been tested with a x742 board.
* objectDump is widely modular so extending it is easy. The user can adapt the application to his purpose.
*
* @author Daniele Berto
*/

/**
* @file Main.c
*
* @brief This file contains the program main
*
**/

#include "X742DecodeRoutines.h"
#include "TcpUser.h"
#include "ConfObject.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "DigitizerObjectGeneric.h"
#include "RawData.h"
#include "DigitizerStateMachine.h"
#include "Analizzatore.h"
#include "ApplicationSetup.h"
#include "Input.h"
#include "CommunicationObject.h"
#include "DigitizerFlowControl.h"
#include <CAENDigitizer.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define ONLY_TCP 1

   /**
   * @brief The main function of the application.
   * @param argc is the number of the main arguments
   * @param argv are the main arguments
   * @return int
   **/
int
main (int argc, char **argv)
{

  //Se l'utente sceglie di avviare il programma in modalita' ONLY_TCP (flag -m tcp), occorre eseguire il programma in modalita' demone.
  //Di default il valore della variabile e' zero cosi' l'espressione process_id == 0 e' verificata anche senza assegnare a process_id il risultato 
  //della system call fork() che viene chiamata solo se il programma viene eseguito con il flag -m tcp (cioe' l'unica modalita' di accettazione dell'input e di
  //produzione dell'output e' quella tramite tcp).
  pid_t process_id = 0;

  //L'oggetto di tipo ApplicationSetup serve per salvare le impostazioni che l'utente ha inserito tramite flags nella stringa di esecuzione del programma.
  //ApplicationSetup e' un singleton, quindi e' necessario ottenerne un riferimento con il metodo Instance().
  ApplicationSetup *main_application_setup;
  main_application_setup = ApplicationSetup::Instance ();

  //Il metodo ApplicationSetupSet copia nello scope di ApplicationSetup gli argomenti del main argc e argv, li analizza e salva le informazioni ottenute.
  //Gli altri oggetti del programma potranno accedere alle informazioni ottenute ottenendo un'istanza di ApplicationSetup.
  main_application_setup->ApplicationSetupSet (argc, argv);

  //Se l'utente ha avviato il programma con il flag -m tcp, esso deve essere eseguito in modalita' demone.
  if (main_application_setup->input_mode == ONLY_TCP)
    {
      process_id = fork ();
    }

  //L'espressione risulta vera se e' letta dal processo figlio o da processo padre eseguito senza il flag -m tcp
  if (process_id == 0)
    {
      //Creo e avvio l'oggetto digitizer_flow_control_main di tipo DigitizerFlowControl: l'oggetto in questione gestisce l'intero flusso di esecuzione del programma.
      DigitizerFlowControl digitizer_flow_control_main;
      digitizer_flow_control_main.DigitizerFlowControlStart ();
    }

  remove(".plot_data.txt");

}
