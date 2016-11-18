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
* @file ConfObject.h 
*/

#ifndef CONFOBJECT_H
#define CONFOBJECT_H

#include "DefineGeneral.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* @brief The RegisterObject class holds address and data of a register.
* @details It is used to define an array of registers (to be set by the user using the configuration file) in the ConfObject.
* @author Bruno Zerbo
*/
class RegisterObject
{
public:

  /**
   * @brief The address of the register.
   */
  int address;

  /**
   * @brief The data of the register.
   */
  int data;

  /**
   * @brief The RegisterObject() constructor sets all the values of the RegisterObject attributes to -1.
   */
    RegisterObject ();
};

/**
* @brief The ChannelObject class picks up all the settings taken from the configuration file regarding one channel of the digitizer.
* @details The number of the channel is indicated by numChannel variable. 
* If the channel has not been set, the set variable is set to -1 like the other unsetted variables.
* @author Daniele Berto
*/

class ChannelObject
{
public:

  /**
   * @brief The set variable indicates if a channel has been set or not.
   */
  int set;

  /**
   * @brief The numChannel variable indicates the number of the channel.
   */
  int numChannel;

  /**
   * @brief The enable_input variable indicates if a channel can receive input or not.
   */
  int enable_input;

  /**
   * @brief The variable dc_offset indicates the dc_offset being applicated to the channel.
   */
  int dc_offset;

  /**
   * @brief The variable trigger_threshold contains the threshold for the channel auto trigger.
   */
  int trigger_threshold;

  /**
   * @brief The variable channel_trigger contains the channel auto trigger settings.
   * @details Values: 0 = ACQUISITION_ONLY, 1 = ACQUISITION_AND_TRGOUT, 2 = DISABLED, other=UNSET.
   */
  int channel_trigger;

  /**
   * @brief The ChannelObject() constructor sets all the values of the channel_object attributes to -1.
   */
    ChannelObject ();

  /**
   * @brief The PrintChannel() method prints all the channel_object attributes.
   * @return void
   */
  void PrintChannel ();
};



/**
* @brief The GroupObject class picks up all the settings taken from the configuration file regarding one group of the digitizer.
* @details The number of the group is indicated by numGroup variable. 
* If the group has not been set, the set variable is set to -1 like the other variables.
* @author Daniele Berto
*/

class GroupObject
{
public:

  /**
   * @brief The set variable indicates if a group has been set or not.
   */
  int set;

  /**
   * @brief The numGroup variable indicates the number of the group.
   */
  int numGroup;

  /**
   * @brief The enable_input variable indicates if a group can receive input or not.
   */
  int enable_input;

   /**
   * @brief The variable dc_offset indicates the dc_offset being applicated to the group.
   */
  int dc_offset;

  /**
   * @brief The variable trigger_threshold contains the threshold for the group auto trigger.
   */
  int trigger_threshold;

  /**
   * @brief this option is used only for the Models x740. 
   * @details These models have the channels grouped 8 by 8; 
   * one group of 8 channels has a common trigger that is generated as the OR of the self trigger of the channels 
   * in the group that are enabled by this mask.
   */
  int group_trg_enable_mask;

  /**
   * @brief The GroupObject() constructor sets all the values of the group_object attributes to -1.
   */
    GroupObject ();

  /**
   * @brief The PrintGroup() method prints all the group_object attributes.
   * @return void
   */
  void PrintGroup ();
};


/**
* @brief The FastObject class picks up all the settings taken from the configuration file regarding a TRn input channel of the digitizer (only for x742 series).
* @details The number of the channel is indicated by numFast variable. 
* If the channel has not been set, the set variable is set to -1 like the other variables.
* @author Daniele Berto
*/

class FastObject
{
public:

  /**
   * @brief The set variable indicates if a TRn has been set or not.
   */
  int set;

  /**
   * @brief The numFast variable indicates the number of the TRn.
   */
  int numFast;

   /**
   * @brief The variable dc_offset indicates the dc_offset being applicated to the TRn.
   */
  int dc_offset;

  /**
   * @brief The variable trigger_threshold contains the threshold for the TRn auto trigger.
   */
  int trigger_threshold;

  /**
   * @brief The FastObject() constructor sets all the values of the fast_object attributes to -1.
   */
    FastObject ();

  /**
   * @brief The PrintFast() method prints all the fast_object attributes.
   * @return void
   */
  void PrintFast ();
};


/**
* @brief The ConfObject class picks up all the settings taken from the configuration file.
* @details A ConfObject is required by AnalizzaInit and AnalizzaSetup functions (see analizzatore.h file).
* This class is useful to sum all the setting informations in only one place.
* @author Daniele Berto
*/

class ConfObject
{
public:

  /**
   * @brief The LinkType variable indicates the physical communication channel. 
   * @details 0 indicates the USB, 1 indicates the Optical Link
   */
  int LinkType;

  /**
   * @brief In case of USB, the link numbers are assigned by the PC when you connect the cable to the device. 
   * @details For other details please consult CAENDigitizer library documentation.
   */
  int LinkNumber;

  /**
   * @brief The CONET node identifies which device in the Daisy chain is being addressed. In case of USB, ConetNode must be 0.
   */
  int ConetNode;

  /**
   * @brief The VME Base Address of the board (rotary switches setting) expressed as a 32-bit number.
   * @details This argument is used only for the VME models accessed through the VME bus and must be 0 in all other cases.
   */
  int VMEBaseAddress;

  /**
   * @brief The DSR4_Frequency indicates the sampling frequency (for X742 boards only).
   * @details Values: 0 = 5GHz, 1 = 2.5GHz, 2 = 1GHz.
   */
  int DSR4_Frequency;

  /**
   * @brief The output_file_format variable indicates the format of the output file.
   * @details Values: 0 = BINARY, 1 = ASCII.
   */
  int output_file_format;

  /**
   * @brief The gnuplot variable indicates the path to the gnuplot program.
   */
  char *gnuplot;

  /**
   * @brief The header_yes_no variable indicates if the output file has to have a header.
   * @details Values: 0 = NO, 1 = YES, other=UNSET.
   */
  int header_yes_no;

  /**
   * @brief The record_length variable indicates the size of the acquisition window, that is the number of samples that belong to it.
   */
  int record_length;

  /**
   * @brief The test_pattern variable indicates if the board has to produce a test wave.
   * @details Values: 0 = NO, 1 = YES, other=UNSET.
   */
  int test_pattern;

  /**
   * @brief The desmod variable indicates if the board has to enable the double edge sampling mode (only for the models 731 and 751).
   * @details Values: 0 = NO, 1 = YES, other=UNSET.
   */
  int desmod;

  /**
   * @brief The external_trigger_acquisition_mode variable indicates the operating mode of the external trigger.
   * @details Values: 0 = ACQUISITION_ONLY, 1 = ACQUISITION_AND_TRGOUT, 2 = DISABLED, other=UNSET.
   */
  int external_trigger_acquisition_mode;

  /**
   * @brief The fast_trigger_acquisition_mode variable indicates the operating mode of the TRn.
   * @details Values: 0 = ACQUISITION_ONLY, 1 = ACQUISITION_AND_TRGOUT, 2 = DISABLED, other=UNSET.
   */
  int fast_trigger_acquisition_mode;

  /**
   * @brief The enable_fast_trigger_digitizing variable (only x742 model) enables/disables the presence of the TRn signal in the data readout.
   * @details Values: 0 = NO, 1 = YES, other=UNSET.
   */
  int enable_fast_trigger_digitizing;

  /**
   * @brief The max_num_events_BLT variable indicates the maximum number of events to read out in one Block Transfer.
   */
  int max_num_events_BLT;

  /**
   * @brief The decimation factor variable (only for 740 model) changes the decimation factor for the acquisition.
   * @details Options are 1 2 4 8 16 32 64 128.  
   */
  int decimation_factor;

  /**
   * @brief post trigger size in percent of the whole acquisition window.
   * @details Options: 0 to 100. On models 742 there is a delay of about 35nsec on signal Fast Trigger TR; the post trigger is added to
   * this delay.   
   */
  int post_trigger;

  /**
   * @brief rising_falling variable decides whether the trigger occurs on the rising or falling edge of the signal.
   * @details Values: 1 = FALLING, 0 = RISING, other=UNSET. 
   */
  int rising_falling;

  /**
   * @brief The variable use_interrupt represents the number of events that must be ready for the readout when the IRQ is asserted.
   */
  int use_interrupt;

  /**
   * @brief The variable nim_ttl indicates the type of the front panel I/O LEMO connectors.
   * @details Values: 1 = TTL, 0 = NIM, other = UNSET
   */
  int nim_ttl;

  /**
   * @brief The variable Address_register contains the address of the register which has to be written at the end of the application setup.
   */
  int Address_register;

  /**
   * @brief The variable Mask_register contains the bitmask to be used for data masking.
   */
  int Mask_register;

  /**
   * @brief The variable Data_register represents the value being written.
   */
  int Data_register;

  /**
   * @brief The variable enable_input indicates if the channels can receive input or not.
   */
  int enable_input;

  /**
   * @brief The variable dc_offset indicates the dc_offset being applicated to all channels.
   */
  int dc_offset;

  /**
   * @brief The variable trigger_threshold contains the threshold for the channel auto trigger.
   */
  int trigger_threshold;

  /**
   * @brief The registers[MAXREGISTERS] array contains the information settings for each register.
   */
  RegisterObject registers[MAXREGISTERS];

  /**
   * @brief The channels[MAXCHANNELOBJECT] array contains the information settings for each channel.
   */
  ChannelObject channels[MAXCHANNELOBJECT];

  /**
   * @brief The groups[MAXGROUPOBJECT] array contains the information settings for each group.
   */
  GroupObject groups[MAXGROUPOBJECT];

  /**
   * @brief The fasts[MAXFASTOBJECT] array contains the information settings for each TRn.
   */
  FastObject fasts[MAXFASTOBJECT];

  /**
   * @brief The channel_enable_mask variable contains the mask for enabling the input from the channels.
   */
  int channel_enable_mask;

  /**
   * @brief The group_enable_mask variable contains the mask for enabling the input from the groups.
   */
  int group_enable_mask;

  /**
   * @brief The trigger_enable_mask variable contains the mask for enabling the self trigger of the channel or of the groups (it depends on the model family).
   */
  int self_trigger_enable_mask;

  /**
   * @brief The trigger_enable_mask variable contains the mode for enabling the self trigger of the channel or of the groups (it depends on the model family).
   */
  int self_trigger_enable_mask_mode;

  /**
   * @brief The ConfObject constructor sets all the values of the conf_object attributes to -1.
   */
    ConfObject ();

  /**
   * @brief The PrintAll () method prints all the conf_object attributes.
   * @return void
   */
  void PrintAll ();

  /**
   * @brief The PrintAllHuman () method prints all the conf_object attributes and interprets their means. Ex: 0=NO, 1=YES, other=UNSET.
   * @return void
   */
  void PrintAllHuman ();
};

#endif
