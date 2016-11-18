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
* @file DigitizerStateMachine.h 
*/

/**
* @brief The DigitizerStateMachine class manages application states.
*
* @author Daniele Berto
*/

#ifndef DIGITIZERSTATEMACHINE_H
#define DIGITIZERSTATEMACHINE_H

#include "DefineGeneral.h"
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <CAENDigitizer.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class DigitizerStateMachine
{

public:

  /**
   * @brief The num_mex_preprocessing variable represents the number of messages stored in the circular_buffer_preprocessing[PREPROCESSINGQUEUE] attribute.
   */
  int num_mex_preprocessing;

  /**
   * @brief The num_mex_visualization variable represents the number of messages stored in the circular_buffer_visualization[VISUALIZATIONQUEUE] attribute.
   */
  int num_mex_visualization;

  /**
   * @brief The testa_preprocessing variable is the pointer to the head of the circular_buffer_preprocessing[PREPROCESSINGQUEUE] attribute.
   */
  int testa_preprocessing;

  /**
   * @brief The testa_visualization variable is the pointer to the head of the circular_buffer_visualization[VISUALIZATIONQUEUE] attribute.
   */
  int testa_visualization;

  /**
   * @brief The coda_preprocessing variable is the pointer to the tail of the circular_buffer_preprocessing[PREPROCESSINGQUEUE] attribute.
   */
  int coda_preprocessing;

  /**
   * @brief The coda_visualization variable is the pointer to the tail of the circular_buffer_visualization[VISUALIZATIONQUEUE] attribute.
   */
  int coda_visualization;

  /**
   * @brief The go_general variable controls the main cycle of the threads.
   */
  int go_general;

  /**
   * @brief The go_preprocessing variable controls the main cycle of the threads.
   */
  int go_preprocessing;

  /**
   * @brief The go_raw_data variable controls the main cycle of the threads.
   */
  int go_raw_data;

  /**
   * @brief The go_visualization variable controls the main cycle of the threads.
   */
  int go_visualization;

  /**
   * @brief The num_events_to_write variable contains the maximum number of events being written by the RawData thread.
   */
  int num_events_to_write;

  /**
   * @brief The num_events_written variable contains the number of the events already saved on disk.
   */
  int num_events_written;


  /**
   * @brief The digitizer variable represents the digitizer.
   */
  DigitizerObjectGeneric digitizer;

  /**
   * @brief The circular_buffer_preprocessing[PREPROCESSINGQUEUE] variable is used to temporary store the data read from the digitizer.
   * @details The data are put in the circular_buffer_preprocessing[PREPROCESSINGQUEUE] by Produttore thread and they are picked up by Preprocessing thread.
   */
  RawData circular_buffer_preprocessing[PREPROCESSINGQUEUE];

  /**
   * @brief The circular_buffer_visualization[VISUALIZATIONQUEUE] variable is used to temporary store the data read from the digitizer.
   * @details The data are put in the circular_buffer_visualization[VISUALIZATIONQUEUE] by Produttore thread and they are picked up by Visualization thread.
   */
  RawData circular_buffer_visualization[VISUALIZATIONQUEUE];

  /**
   * @brief The DigitizerStateMachineSetup method sets the digitizer with the settings specified in the configuration file.
   * @param conf_file is the path of the configuration file
   * @return void
   */
  void DigitizerStateMachineSetup (const char *conf_file);

  /**
   * @brief The DigitizerStateMachineStartReading method begins the data acquisition from the digitizer.
   * @return void
   */
  void DigitizerStateMachineStartReading ();

  /**
   * @brief The DigitizerStateMachinePrintStatus method prints some informations about the threads of the object.
   * @return void
   */
  void DigitizerStateMachinePrintStatus ();

  /**
   * @brief The DigitizerStateMachineStopReading method stops the data acquisition from the digitizer.
   * @return void
   */
  void DigitizerStateMachineStopReading ();

  /**
   * @brief The DigitizerStateMachineRawDataInit method creates the Produttore and the Consumatore threads.
   * It also: initializes the mutexes and the cond variables; sets to 0 num_mex, testa_raw_data, coda_raw_data and sets to 1 the go_general variable.
   * @return void
   */
  void DigitizerStateMachineRawDataInit ();

  /**
   * @brief The DigitizerStateMachineQuit method closes the digitizer.
   * @return void
   */
  void DigitizerStateMachineQuit ();

  /**
   * @brief The DigitizerStateMachineSendSWTrigger method sends one software trigger to the digitizer.
   * @return void
   */
  void DigitizerStateMachineSendSWTrigger ();

  /**
   * @brief The DigitizerStartPreprocessing method starts the preprocessing actions.
   * @return void
   */
  void DigitizerStateStartPreprocessing ();

  /**
   * @brief The DigitizerStartVisualization method starts the visualization actions.
   * @return void
   */
  void DigitizerStateStartVisualization ();

  /**
   * @brief The DigitizerStopPreprocessing method starts the preprocessing actions.
   * @return void
   */
  void DigitizerStateStopPreprocessing ();

  /**
   * @brief The DigitizerStartRawDataWriting method starts the visualization actions.
   * @return void
   */
  void DigitizerStateStartRawDataWriting ();

  /**
   * @brief The DigitizerStopRawDataWriting method starts the preprocessing actions.
   * @return void
   */
  void DigitizerStateStopRawDataWriting ();

  /**
   * @brief The DigitizerStopVisualization method starts the visualization actions.
   * @return void
   */
  void DigitizerStateStopVisualization ();

  /**
   * @brief The Produttore thread reads data from the digitizer and puts them in the buffers.
   * @return void
   */
  void Produttore ();

  /**
   * @brief The Preprocessing thread picks data from the circular_buffer_preprocessing[PREPROCESSINGQUEUE] and performs the preprocessing data actions.
   * @return void
   */
  void Preprocessing ();

  /**
   * @brief The Visualization thread picks data from the circular_buffer_visualization[VISUALIZATIONQUEUE] and visualizes them using the gnuplot program.
   * @return void
   */
  void Visualization ();

  /**
   * @brief The thread id of the Produttore thread.
   */
  thread * produttore_thread;

  /**
   * @brief The thread id of the Preprocessing thread.
   */
  thread * preprocessing_thread;

  /**
   * @brief The thread id of the Visualization thread.
   */
  thread * visualization_thread;

  /**
   * @brief A mutex used to implement producer-consumer model.
   */
  mutex ReservedPreprocessingInputArea;

  /**
   * @brief A mutex used to implement producer-consumer model.
   */
  mutex ReservedVisualizationInputArea;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable Acquisition_Cond1;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable Acquisition_Cond2;

  /**
   * @brief When the preprocessing thread is not active, it is suspended in the pre_cond condition variable.
   */
  condition_variable pre_cond;

  /**
   * @brief When the visualization thread is not active, it is suspended in the vis_cond condition variable.
   */
  condition_variable vis_cond;

  /**
   * @brief The DigitizerStateMachineInit(const char *) configures the digitizer.
   * @return void. 
   * @param conf_file is the path of the configuration file.
   */
  void DigitizerStateMachineInit (const char *conf_file);

   /**
   * @brief The imstarted variable indicates if the user has already used "start" command.
   */
  int imstarted;

   /**
   * @brief The imset variable indicates if the user has already init the object whith DigitizerStateMachineInit(const char *conf_file) method.
   */
  int imset;

  /**
   * @brief The DigitizerStateMachine(const char *) constructor sets go_general variable to zero.
   * @details The constructor is private in order to implement singleton design pattern.
   */
    DigitizerStateMachine ();
};

#endif
