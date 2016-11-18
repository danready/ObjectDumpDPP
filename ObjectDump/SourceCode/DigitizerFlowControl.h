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
* @file DigitizerFlowControl.h 
*/


/**
* @brief The DigitizerFlowControl class controls the flow of execution of the program.
*
* @author Daniele Berto
*/

#ifndef DIGITIZERFLOWCONTROL_H
#define DIGITIZERFLOWCONTROL_H

class DigitizerFlowControl
{
public:

  /**
   * @brief The input_flow_cond condition variable is used to notify to the object that an input command is ready to be fetched.
   * The DigitizerFlowControl object waits on it until an Input object or a CommunicationObject wakes it up with the pthread_cond_signal function.
   */
  static pthread_cond_t input_flow_cond;

  /**
   * @brief The input_flow_mutex is used with the input_flox_cond to guarantee the consistency of the operations.
   */
  static pthread_mutex_t input_flow_mutex;

  /**
   * @brief The mioinput variable is used to get the input from the stdin.
   */
  Input* mioinput; //Modifica Input

  /**
   * @brief The mioTCP pointer is used to get the input from the clients via TCP.
   */
  CommunicationObject mioTCP;

  /**
   * @brief The digitizer_flow_control_application_setup variable is used to configure the application.
   */
  ApplicationSetup *digitizer_flow_control_application_setup;

   /**
   * @brief The DigitizerFlowControl constructor sets the digitizer_flow_control_application_setup variable.
   */
    DigitizerFlowControl ();

  /**
   * @brief The DigitizerFlowControlStart method begins the main control cycle.
   * @return void
   */
  void DigitizerFlowControlStart ();

  /**
   * @brief The ParseCommand method prints the meaning of the command received via TCP.
   * @param recvline is the command to interpret.
   * @return void
   */
  const char *ParseCommand (int recvline);

  /**
   * @brief The help method lists the commands available.
   * @return void
   */
  void Help ();

  /**
   * @brief The reg_matcher method compares the string str with the regex pattern.
   * @return bool
   */
  bool reg_matches (const char *str, const char *pattern);
};

#endif
