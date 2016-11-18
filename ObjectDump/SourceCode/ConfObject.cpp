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
* @file ConfObject.cpp 
*/

#include "DefineGeneral.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "OutputModule.h"

RegisterObject::RegisterObject ()
{
  address = -1;
  data = -1;
}

ConfObject::ConfObject ()
{
  LinkType = -1;
  LinkNumber = -1;
  ConetNode = -1;
  VMEBaseAddress = -1;
  DSR4_Frequency = -1;
  output_file_format = -1;
  gnuplot = NULL;
  header_yes_no = -1;
  record_length = -1;
  test_pattern = -1;
  desmod = -1;
  external_trigger_acquisition_mode = -1;
  fast_trigger_acquisition_mode = -1;
  enable_fast_trigger_digitizing = -1;
  max_num_events_BLT = -1;
  decimation_factor = -1;
  post_trigger = -1;
  rising_falling = -1;
  use_interrupt = -1;
  nim_ttl = -1;

  Address_register = -1;
  Mask_register = -1;
  Data_register = -1;

  enable_input = -1;
  dc_offset = -1;
  trigger_threshold = -1;

  channel_enable_mask = -1;
  group_enable_mask = -1;
  self_trigger_enable_mask = -1;
  self_trigger_enable_mask_mode = -1;

  //It could be useful to set a default value for the gnuplot path
  gnuplot = (char *)malloc(strlen("gnuplot") +1);
  strcpy (gnuplot, "gnuplot");
}


ChannelObject::ChannelObject ()
{
  set = -1;
  numChannel = -1;
  enable_input = -1;
  dc_offset = -1;
  trigger_threshold = -1;
  channel_trigger = -1;
}


GroupObject::GroupObject ()
{
  set = -1;
  numGroup = -1;
  enable_input = -1;
  dc_offset = -1;
  trigger_threshold = -1;
  group_trg_enable_mask = -1;
}


FastObject::FastObject ()
{
  set = -1;
  numFast = -1;
  dc_offset = -1;
  trigger_threshold = -1;
}


void
ChannelObject::PrintChannel ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];
  bzero (stringa, STANDARDBUFFERLIMIT);

  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "numchannel\t\t\t%d\nset\t\t\t\t%d\nenable_input\t\t\t%d\ndc_offset\t\t\t%d\ntrigger_threshold\t\t%d\nchannel_trigger\t\t\t%d\n\n",
	    numChannel, set, enable_input, dc_offset, trigger_threshold,
	    channel_trigger);

  output_module->Output(stringa);
}


void
GroupObject::PrintGroup ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];
  bzero (stringa, STANDARDBUFFERLIMIT);

  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "numgroup\t\t\t%d\nset\t\t\t\t%d\nenable_input\t\t\t%d\ndc_offset\t\t\t%d\ntrigger_threshold\t\t%d\ngroup_trg_enable_mask\t\t%d\n\n",
	    numGroup, set, enable_input, dc_offset, trigger_threshold,
	    group_trg_enable_mask);

  output_module->Output(stringa);
}


void
FastObject::PrintFast ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];
  bzero (stringa, STANDARDBUFFERLIMIT);

  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "numFast\t\t\t\t%d\nset\t\t\t\t%d\ndc_offset\t\t\t%d\ntrigger_threshold\t\t%d\n\n",
	    numFast, set, dc_offset, trigger_threshold);

  output_module->Output(stringa);
}


void
ConfObject::PrintAllHuman ()
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];
  bzero (stringa, STANDARDBUFFERLIMIT);

  int i;

  const char *output_file_format_string;
  if (output_file_format == 0)
    output_file_format_string = "BINARY";
  else if (output_file_format == 1)
    output_file_format_string = "ASCII";
  else
    output_file_format_string = "UNSET";

  const char *header_yes_no_string;
  if (header_yes_no == 1)
    header_yes_no_string = "YES";
  else if (header_yes_no == 0)
    header_yes_no_string = "NO";
  else
    header_yes_no_string = "UNSET";

  const char *des_mod_string;
  if (desmod == 1)
    des_mod_string = "YES";
  else if (desmod == 0)
    des_mod_string = "NO";
  else
    des_mod_string = "UNSET";

  const char *external_trigger_acquisition_mode_string;
  if (external_trigger_acquisition_mode == 0)
    external_trigger_acquisition_mode_string = "ACQUISITION_ONLY";
  else if (external_trigger_acquisition_mode == 1)
    external_trigger_acquisition_mode_string = "ACQUISITION_AND_TRGOUT";
  else if (external_trigger_acquisition_mode == 2)
    external_trigger_acquisition_mode_string = "DISABLED";
  else
    external_trigger_acquisition_mode_string = "UNSET";

  const char *fast_trigger_acquisition_mode_string;
  if (fast_trigger_acquisition_mode == 0)
    fast_trigger_acquisition_mode_string = "ACQUISITION_ONLY";
  else if (fast_trigger_acquisition_mode == 2)
    fast_trigger_acquisition_mode_string = "DISABLED";
  else
    fast_trigger_acquisition_mode_string = "UNSET";

  const char *enable_fast_trigger_digitizing_string;
  if (enable_fast_trigger_digitizing == 1)
    enable_fast_trigger_digitizing_string = "YES";
  else if (enable_fast_trigger_digitizing == 0)
    enable_fast_trigger_digitizing_string = "NO";
  else
    enable_fast_trigger_digitizing_string = "UNSET";

  const char *rising_falling_string;
  if (rising_falling == 1)
    rising_falling_string = "FALLING";
  else if (rising_falling == 0)
    rising_falling_string = "RISING";
  else
    rising_falling_string = "UNSET";

  const char *use_interrupt_string;
  if (use_interrupt == 1)
    use_interrupt_string = "YES";
  else if (use_interrupt == 0)
    use_interrupt_string = "NO";
  else
    use_interrupt_string = "UNSET";

  const char *nim_ttl_string;
  if (nim_ttl == 1)
    nim_ttl_string = "TTL";
  else if (nim_ttl == 0)
    nim_ttl_string = "NIM";
  else
    nim_ttl_string = "UNSET";

  const char *enable_input_string;
  if (enable_input == 1)
    enable_input_string = "YES";
  else if (enable_input == 0)
    enable_input_string = "NO";
  else
    enable_input_string = "UNSET";

  const char *self_trigger_acquisition_mode_string;
  if (self_trigger_enable_mask_mode == 0)
    self_trigger_acquisition_mode_string = "ACQUISITION_ONLY";
  else if (self_trigger_enable_mask_mode == 1)
    self_trigger_acquisition_mode_string = "ACQUISITION_AND_TRGOUT";
  else if (self_trigger_enable_mask_mode == 2)
    self_trigger_acquisition_mode_string = "DISABLED";
  else
    self_trigger_acquisition_mode_string = "UNSET";


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "LinkType\t\t\t\t\t%d\nLinkNumber\t\t\t\t\t%d\nConetNode\t\t\t\t\t%d\nVMEBaseAddress\t\t\t\t\t%d\n",
	    LinkType, LinkNumber, ConetNode, VMEBaseAddress);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "DSR4_Frequency\t\t\t\t\t%d\nOutputFileFormat\t\t\t\t%s\ngnuplot\t\t\t\t\t\t%s\nheader_yes_no\t\t\t\t\t%s\nrecord_length\t\t\t\t\t%d\n",
	    DSR4_Frequency, output_file_format_string, gnuplot,
	    header_yes_no_string, record_length);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "DESMod\t\t\t\t\t\t%s\nexternal_trigger_acquisition_mode\t\t%s\nfast_trigger_acquisition_mode\t\t\t%s\n",
	    des_mod_string, external_trigger_acquisition_mode_string,
	    fast_trigger_acquisition_mode_string);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "enable_fast_trigger_digitizing\t\t\t%s\nmax_num_events_BLT\t\t\t\t%d\ndecimation_factor\t\t\t\t%d\n",
	    enable_fast_trigger_digitizing_string, max_num_events_BLT,
	    decimation_factor);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "post_trigger\t\t\t\t\t%d\nrising_falling\t\t\t\t\t%s\nuse_interrupt\t\t\t\t\t%s\nnim_ttl\t\t\t\t\t\t%s\n",
	    post_trigger, rising_falling_string, use_interrupt_string,
	    nim_ttl_string);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "REGISTER ADDRESS MASK DATA\t\t\t%d %d %d\n", Address_register,
	    Data_register, Mask_register);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "enable_input, dc_offset, trigger_threshold\t%s, %d, %d \n\n",
	    enable_input_string, dc_offset, trigger_threshold);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "channel enable mask: %x\n",
	    channel_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "group enable mask: %x\n\n",
	    group_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "self trigger enable mask: %x\n\n",
	    self_trigger_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "self trigger enable mask mode: %s\n\n",
	    self_trigger_acquisition_mode_string);
  output_module->Output(stringa);


  for (i = 0; i < MAXCHANNELOBJECT; i++)
    if (channels[i].set != -1)
      channels[i].PrintChannel ();
  for (i = 0; i < MAXGROUPOBJECT; i++)
    if (groups[i].set != -1)
      groups[i].PrintGroup ();
  for (i = 0; i < MAXFASTOBJECT; i++)
    if (fasts[i].set != -1)
      fasts[i].PrintFast ();

}


void
ConfObject::PrintAll ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];

  int i;

  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "LinkType\t\t\t\t\t%d\nLinkNumber\t\t\t\t\t%d\nConetNode\t\t\t\t\t%d\nVMEBaseAddress\t\t\t\t\t%d\n",
	    LinkType, LinkNumber, ConetNode, VMEBaseAddress);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "DSR4_Frequency\t\t\t\t\t%d\nOutputFileFormat\t\t\t\t%d\ngnuplot\t\t\t\t\t\t%s\nheader_yes_no\t\t\t\t\t%d\nrecord_length\t\t\t\t\t%d\n",
	    DSR4_Frequency, output_file_format, gnuplot, header_yes_no,
	    record_length);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "DESMod\t\t\t\t\t\t%d\nexternal_trigger_acquisition_mode\t\t%d\nfast_trigger_acquisition_mode\t\t\t%d\n",
	    desmod, external_trigger_acquisition_mode,
	    fast_trigger_acquisition_mode);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "enable_fast_trigger_digitizing\t\t\t%d\nmax_num_events_BLT\t\t\t\t%d\ndecimation_factor\t\t\t\t%d\n",
	    enable_fast_trigger_digitizing, max_num_events_BLT,
	    decimation_factor);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "post_trigger\t\t\t\t\t%d\nrising_falling\t\t\t\t\t%d\nuse_interrupt\t\t\t\t\t%d\nnim_ttl\t\t\t\t\t\t%d\n",
	    post_trigger, rising_falling, use_interrupt, nim_ttl);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "REGISTER ADDRESS MASK DATA\t\t\t%d %d %d\n", Address_register,
	    Data_register, Mask_register);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "enable_input, dc_offset, trigger_threshold\t%d, %d, %d\n\n",
	    enable_input, dc_offset, trigger_threshold);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "channel enable mask: %x\n",
	    channel_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "group enable mask: %x\n\n",
	    group_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "self trigger enable mask: %x\n\n",
	    self_trigger_enable_mask);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT,
	    "self trigger enable mask mode: %x\n\n",
	    self_trigger_enable_mask_mode);
  output_module->Output(stringa);



  for (i = 0; i < MAXCHANNELOBJECT; i++)
    if (channels[i].set != -1)
      channels[i].PrintChannel ();
  for (i = 0; i < MAXGROUPOBJECT; i++)
    if (groups[i].set != -1)
      groups[i].PrintGroup ();
  for (i = 0; i < MAXFASTOBJECT; i++)
    if (fasts[i].set != -1)
      fasts[i].PrintFast ();
}
