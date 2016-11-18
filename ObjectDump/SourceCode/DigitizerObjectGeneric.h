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
* @file DigitizerObjectGeneric.h 
*/

/**
* @brief The DigitizerObjectGeneric class extends the DigitizerObject class whith methods for setting the digitizer using "internal_config" attribute.
* @details DigitizerObject class provides methods for setting the digitizer specifying the settings in their parameters. 
* The methods of DigitizerObjectGeneric class are parameterless because they use settings provides by internal_config attribute.
* @author Daniele Berto
*/

#ifndef DIGIZEROBJECTGENERIC_H
#define DIGIZEROBJECTGENERIC_H

#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>

class DigitizerObjectGeneric:public DigitizerObject
{
public:

  /**
   * @brief The DigitizerObjectGenericSetRecordLength method sets the size of the acquisition window.
   * @return int
   */
  int DigitizerObjectGenericSetRecordLength ();

  /**
   * @brief The DigitizerObjectGenericSetMaxNumEventsBLT method sets the maximum number of events that can be trasferred in a readout cycle.
   * @return int
   */
  int DigitizerObjectGenericSetMaxNumEventsBLT ();

  /**
   * @brief The DigitizerObjectGenericSetEnableMask method sets the groups or the channels (it depends on the model family) able to receive input in accordance with 
   * the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetEnableMask ();

  /**
   * @brief The DigitizerObjectGenericSetFastTriggerDigitizing method sets the TRn input channel (only for the x742 family) in accordance with 
   * the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetFastTriggerDigitizing ();

  /**
   * @brief The DigitizerObjectGenericSetChannelSelfTrigger method sets the groups or the channels (it depends on the model family) self trigger.
   * @return int
   */
  int DigitizerObjectGenericSetSelfTrigger ();

  /**
   * @brief The DigitizerObjectGenericSetChannelSelfTriggerThreshold method sets the self trigger threshold of the groups or of the channels (it depends on the model family).
   * @return int
   */
  int DigitizerObjectGenericSetChannelSelfTriggerThreshold ();


  /**
   * @brief The DigitizerObjectGenericSetEnableMask method sets the DCOffset of the groups or of the channels (it depends on the model family).
   * @return int
   */
  int DigitizerObjectGenericSetDCOffset ();

  /**
   * @brief The DigitizerObjectWriteRegister method writes a digitizer register in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericWriteRegister ();

  /**
   * @brief The DigitizerObjectGenericReadRegister method reads a digitizer register in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericReadRegister ();

  /**
   * @brief The DigitizerObjectGenericGetDRS4SamplingFrequency method gets the sampling frequency of the digitizer.
   * @return int
   */
  int DigitizerObjectGenericGetDRS4SamplingFrequency ();

  /**
   * @brief The DigitizerObjectGenericSetDRS4SamplingFrequency method sets the sampling frequency of the digitizer.
   * @return int
   */
  int DigitizerObjectGenericSetDRS4SamplingFrequency ();

  /**
   * @brief The DigitizerObjectGenericSetChannelDCOffset method sets the DCOffset of the channels in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetChannelDCOffset ();

  /**
   * @brief The DigitizerObjectGenericSetGroupDCOffset method sets the DCOffset of the groups in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetGroupDCOffset ();

  /**
   * @brief The DigitizerObjectGenericSetTriggerPolarity method sets the trigger polarity of a specified channel in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetTriggerPolarity ();

  /**
   * @brief The DigitizerObjectGenericSetPostTriggerSize method sets the position of the trigger within the acquisition window in accordance with 
   * the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetPostTriggerSize ();

  /**
   * @brief The DigitizerObjectGenericSetIOLevel method sets the I/0 level in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetIOLevel ();

  /**
   * @brief The DigitizerObjectGenericSetAcquisitionMode method sets the data acquisition mode in accordance with the config file settings.
   * @return int
   */
  int DigitizerObjectGenericSetAcquisitionMode ();

  /**
   * @brief The DigitizerObjectGenericSetExtTriggerInputMode method sets how an external trigger has to be used.
   * @return int
   */
  int DigitizerObjectGenericSetExtTriggerInputMode ();

  /**
   * @brief The DigitizerObjectGenericSetSWTriggerMode method sets how a software trigger has to be used.
   * @return int
   */
  int DigitizerObjectGenericSetSWTriggerMode ();

  /**
   * @brief The DigitizerObjectGenericSetDecimationFactor method sets the decimation factor. This method produces effects only for 740 series.
   * @return int
   */
  int DigitizerObjectGenericSetDecimationFactor ();

  /**
   * @brief The DigitizerObjectGenericSetDesMode method enables the dual edge sampling (DES) mode (for 731 and 751 series only).
   * @return int
   */
  int DigitizerObjectGenericSetDesMode ();

  /**
   * @brief The DigitizerObjectGenericSetTestPattern method sets the waveform test bit for debugging.
   * @return int
   */
  int DigitizerObjectGenericSetTestPattern ();

  /**
   * @brief The DigitizerObjectGenericSetAllInformations method sets the digitizer with the settings marked with "all" in the configuration file.
   * @return int
   */
  int DigitizerObjectGenericSetAllInformations ();

  /**
   * @brief The DigitizerObjectSetAutomaticCorrectionX742 method sets the digitizer DRS4 correction parameters stored in the board.
   * @return int
   */
  int DigitizerObjectSetAutomaticCorrectionX742 ();

  /**
   * @brief The DigitizerObjectGeneric constructor sets to 0 the set_board_info attribute and gets an instance of the LogFile singleton.
   */
  DigitizerObjectGeneric ();
};

#endif
