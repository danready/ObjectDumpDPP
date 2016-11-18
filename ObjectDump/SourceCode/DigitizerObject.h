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
* @file DigitizerObject.h
* @author Daniele Berto
*/

/**
* @brief The DigitizerObject class envelops CAEN_DGTZ functions from CAENDigitizer library.
* @author Daniele Berto
*/

#ifndef DIGITIZEROBJECT_H
#define DIGITIZEROBJECT_H

#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>

class DigitizerObject
{

public:

  /**
   * @brief The ret variable contains the error code returned by CAEN_DGTZ library functions.
   */
  CAEN_DGTZ_ErrorCode ret;

  /**
   * @brief The ret_error object is used to print the meaning of CAEN_DGTZ_ErrorCode.
   */
  DigitizerErrorObject ret_error;

  /**
   * @brief The internal_config object is used to store the setting of the digitizer read from the configuration file.
   */
  ConfObject internal_config;

  /**
   * @brief The set_board_info variable indicates if the board informations have been already picked or not.
   */
  int set_board_info;

  /**
   * @brief The handle variable contains the digitizer handler device.
   */
  int handle;

  /**
   * @brief The BoardInfo variable contains the informations about the board.
   */
  CAEN_DGTZ_BoardInfo_t BoardInfo;

  /**
   * @brief The DigitizerObject default constructor sets to 0 the variables set_board_info, digitizer_open and set_internal_config.
   */
    DigitizerObject ();

  /**
   * @brief The DigitizerObject (const char *) constructor sets to 0 the variables set_board_info, digitizer_open and to 1 set_internal_config.
   * The method gets the settings from the configuration file specified in the config_file parameter using AnalizzaInit and AnalizzaSetup functions
   * from AnalizzatoreLessicale.flex file. These settings are copied in the internal_config attribute.
   * @param config_file is the path of the configuration file.
   */
    DigitizerObject (const char *config_file);

  /**
   * @brief The DigitizerObject (ConfObject) constructor sets to 0 the variables set_board_info, digitizer_open and to 1 set_internal_config.
   * The method gets the settings from the config parameter and stores them in the internal_config attribute.
   * @param config is the ConfObject being copied in the internal_config attribute.
   */
    DigitizerObject (ConfObject config);

  /**
   * @brief The DigitizerObjectSetConfigStructureConfObject (ConfObject) method copies the ConfObject parameter in the internal_config attribute.
   * @param config is the ConfObject being copied in the internal_config attribute.
   * @return void
   */
  void DigitizerObjectSetConfigStructureConfObject (ConfObject config);

  /**
   * @brief The DigitizerObjectSetConfigStructureInit (const char *) method scans the file in the path indicates by the parameter using AnalizzaInit and AnalizzaSetup functions
   * from AnalizzatoreLessicale.flex file and stores the retrieved informations in the internal_config attribute.
   * @param config_file is the path of the configuration file.
   * @return void
   */
  void DigitizerObjectSetConfigStructureInit (const char *config_file);

  /**
   * @brief The DigitizerObjectSetConfigStructureSetup (const char *) method scans the file in the path indicates by the parameter using AnalizzaInit and AnalizzaSetup functions
   * from AnalizzatoreLessicale.flex file and stores the retrieved informations in the internal_config attribute.
   * @param config_file is the path of the configuration file.
   * @return void
   */
  void DigitizerObjectSetConfigStructureSetup (const char *config_file);

  /**
   * @brief The DigitizerObjectOpen method opens the digitizer using the settings stored in the internal_config attribute.
   * @return int
   */
  int DigitizerObjectOpen ();

  /**
   * @brief The DigitizerObjectOpen method closes the digitizer.
   * @return int
   */
  int DigitizerObjectClose ();

  /**
   * @brief The DigitizerObjectReset method resets the digitizer.
   * @return int
   */
  int DigitizerObjectReset ();

  /**
   * @brief The DigitizerObjectGetInfo method gets factory informations from the digitizer and puts them in the BoardInfo attribute.
   * @return int
   */
  int DigitizerObjectGetInfo ();

  /**
   * @brief The GetBoardInfo method returns factory informations from the digitizer.
   * @return CAEN_DGTZ_BoardInfo_t
   */
  CAEN_DGTZ_BoardInfo_t GetBoardInfo ();

  /**
   * @brief The GetFamilyCode method puts in the parameter FamilyCode the family code of the digitizer.
   */
  int GetFamilyCode(int * FamilyCode);

  /**
   * @brief The GetFormFactorCode method puts in the parameter FormFactor the form factor code of the digitizer.
   */
  int GetFormFactorCode(int * FormFactor);

  /**
   * @brief The PrintBoardInfo method prints to the stdout the factory informations stored in the BoardInfo attribute.
   * @return void
   */
  void PrintBoardInfo ();

  /**
   * @brief The DigitizerObjectSetRecordLength method sets the size of the acquisition window.
   * @param recordlength is the size of the acquisition window.
   * @return int
   */
  int DigitizerObjectSetRecordLength (int recordlength);

  /**
   * @brief The DigitizerObjectSetGroupEnableMask method sets the groups enabled to receive input in accordance with the parameter.
   * @param enablemask is the mask that indicates which groups will receive input. 
   * @return int
   */
  int DigitizerObjectSetGroupEnableMask (int enablemask);

  /**
   * @brief The DigitizerObjectWriteRegister method writes a digitizer register in accordance with the parameters.
   * @param registry is the address of the registry being written.
   * @param data are the informations being written into the regitry indicated by registry parameter.
   * @return int
   */
  int DigitizerObjectWriteRegister (int registry, int data);

  /**
   * @brief The DigitizerObjectReadRegister method reads a digitizer register in accordance with the parameters.
   * @param registry is the address of the registry being read.
   * @param data are the informations being read from the regitry indicated by registry parameter.
   * @return int
   */
  int DigitizerObjectReadRegister (int registry, int *data);

  /**
   * @brief The DigitizerObjectGetDRS4SamplingFrequency method gets the sampling frequency of the digitizer.
   * @param frequenza is where the frequency information will be stored.
   * @return int
   */
  int DigitizerObjectGetDRS4SamplingFrequency (CAEN_DGTZ_DRS4Frequency_t *
					       frequenza);

  /**
   * @brief The DigitizerObjectSetDRS4SamplingFrequency method sets the sampling frequency of the digitizer.
   * @param frequenza is the frequency being setted.
   * @return int
   */
  int DigitizerObjectSetDRS4SamplingFrequency (CAEN_DGTZ_DRS4Frequency_t
					       frequenza);

  /**
   * @brief The DigitizerObjectSetChannelDCOffset method sets the DCOffset of the channels in accordance with the parameters.
   * @param channel_mask indicates the channels being influenced by the DCOffset.
   * @param dc_mask is the DAC value.
   * @return int
   */
  int DigitizerObjectSetChannelDCOffset (int channel_mask, int dc_mask);

  /**
   * @brief The DigitizerObjectSetGroupDCOffset method sets the DCOffset of the groups in accordance with the parameters.
   * @param group_mask indicates the groups being influenced by the DCOffset.
   * @param dc_mask is the DAC value.
   * @return int
   */
  int DigitizerObjectSetGroupDCOffset (int group_mask, int dc_mask);

  /**
   * @brief The DigitizerObjectSetMaxNumEventsBLT method sets the maximum number of events that can be trasferred in a readout cycle.
   * @param MaxNumEventsBLT is the maximum number of events that can be trasferred in a readout cycle.
   * @return int
   */
  int DigitizerObjectSetMaxNumEventsBLT (int MaxNumEventsBLT);

  /**
   * @brief The DigitizerObjectSetAcquisitionMode method sets the data acquisition mode in accordance with the parameter.
   * @param AcqMode is the data acquisition mode being setted.
   * @return int
   */
  int DigitizerObjectSetAcquisitionMode (CAEN_DGTZ_AcqMode_t AcqMode);

  /**
   * @brief The DigitizerObjectSetExtTriggerInputMode method sets how an external trigger has to be used.
   * @param TriggerMode indicates how an external trigger has to be used.
   * @return int
   */
  int DigitizerObjectSetExtTriggerInputMode (CAEN_DGTZ_TriggerMode_t
					     TriggerMode);

  /**
   * @brief The DigitizerObjectSetSWTriggerMode method sets how a software trigger has to be used.
   * @param TriggerMode indicates how a software trigger has to be used.
   * @return int
   */
  int DigitizerObjectSetSWTriggerMode (CAEN_DGTZ_TriggerMode_t TriggerMode);

  /**
   * @brief The DigitizerObjectSWStartAcquisition method starts the data acquisition.
   * @return int
   */
  int DigitizerObjectSWStartAcquisition ();

  /**
   * @brief The DigitizerObjectSWStartAcquisition method stops the data acquisition.
   * @return int
   */
  int DigitizerObjectSWStopAcquisition ();

  /**
   * @brief The logfile reference is used to write on logfile. 
   */
  LogFile *logfile;
};

#endif
