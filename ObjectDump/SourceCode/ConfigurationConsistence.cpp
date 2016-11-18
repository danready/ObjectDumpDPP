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
* @file ConfigurationConsistence.cpp
* @author Daniele Berto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CAENDigitizer.h>
#include "DefineGeneral.h"
#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "Analizzatore.h"
#include "ConfigurationConsistence.h"
#include "OutputModule.h"



int
ConfigurationConsistence::
ConfigurationConsistenceConfFileInit (const char *conf_file)
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;


  if (AnalizzaInitPrint (&conf_object_private, conf_file))
    {
      fprintf (stderr, "%s: Errore: il file di configurazione non esiste\n",
	       __func__);
      ret_flag = 1;
	output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.LinkType == -1)
    {
      fprintf (stderr,
	       "%s: Error: you have not specify LinkType in OPEN parameter\n",
	       __func__);
      ret_flag++;
	output_module->Output("Errore: il file di configurazione non esiste\n");
    }

  if (conf_object_private.LinkNumber == -1)
    {
      fprintf (stderr,
	       "%s: Error: you have not specify LinkNumber in OPEN parameter\n",
	       __func__);
      ret_flag++;
      output_module->Output("Error: you have not specify LinkNumber in OPEN parameter\n");
    }

  if (conf_object_private.ConetNode == -1)
    {
      fprintf (stderr,
	       "%s: Error: you have not specify ConetNode in OPEN parameter\n",
	       __func__);
      ret_flag++;
      output_module->Output("Error: you have not specify ConetNode in OPEN parameter\n");
    }

  if (conf_object_private.VMEBaseAddress == -1)
    {
      fprintf (stderr,
	       "%s: Error: you have not specify VMEBaseAddress in OPEN parameter\n",
	       __func__);
      ret_flag++;
      output_module->Output("Error: you have not specify VMEBaseAddress in OPEN parameter\n");
    }


  if (ret_flag == 4)
    {
      fprintf (stderr,
	       "%s: Ops, have you forgotten to insert OPEN attribute in the configuration file?\n",
	       __func__);
      output_module->Output("Ops, have you forgotten to insert OPEN attribute in the configuration file?\n");
    }

  return ret_flag;

}  //int ConfigurationConsistence::ConfigurationConsistenceConfFileInit (const char * conf_file)



int
ConfigurationConsistence::
ConfigurationConsistenceConfFileInitNoPrint (const char *conf_file)
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;


  if (AnalizzaInit (&conf_object_private, conf_file))
    {
      ret_flag = 1;
	 output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.LinkType == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify LinkType in OPEN parameter\n");
    }

  if (conf_object_private.LinkNumber == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify LinkNumber in OPEN parameter\n");
    }

  if (conf_object_private.ConetNode == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify ConetNode in OPEN parameter\n");
    }

  if (conf_object_private.VMEBaseAddress == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify VMEBaseAddress in OPEN parameter\n");
    }


  if (ret_flag == 4)
    {
      output_module->Output("Ops, have you forgotten to insert OPEN attribute in the configuration file?\n");
    }

  return ret_flag;

}



int
ConfigurationConsistence::ConfigurationConsistenceConfFileSetupEssentialWithInitCheck (const char *conf_file)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;
  ret_flag =
    ConfigurationConsistence::
    ConfigurationConsistenceConfFileInit (conf_file);

  if (AnalizzaSetupPrint (&conf_object_private, conf_file))
    {
      ret_flag++;
      output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.record_length == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify RECORD_LENGTH\n");
    }

  if (conf_object_private.max_num_events_BLT == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify MAX_NUM_EVENTS_BLT\n");
    }

  return ret_flag;

}



int
ConfigurationConsistence:: ConfigurationConsistenceConfFileSetupEssentialWithInitCheckNoPrint (const char *conf_file)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;
  ret_flag =
    ConfigurationConsistence::
    ConfigurationConsistenceConfFileInitNoPrint (conf_file);

  if (AnalizzaSetup (&conf_object_private, conf_file))
    {
      ret_flag++;
      output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.record_length == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify RECORD_LENGTH\n");
    }

  if (conf_object_private.max_num_events_BLT == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify MAX_NUM_EVENTS_BLT\n");
    }

  return ret_flag;

}


int
ConfigurationConsistence::ConfigurationConsistenceConfFileSetupEssentialWithoutInitCheck (const char *conf_file)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;

  if (AnalizzaSetupPrint (&conf_object_private, conf_file))
    {
      ret_flag++;
      output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.record_length == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify RECORD_LENGTH\n");
    }

  if (conf_object_private.max_num_events_BLT == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify MAX_NUM_EVENTS_BLT\n");
    }

  return ret_flag;

}



int
ConfigurationConsistence:: ConfigurationConsistenceConfFileSetupEssentialWithoutInitCheckNoPrint (const char *conf_file)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  int ret_flag = 0;
  ConfObject conf_object_private;

  if (AnalizzaSetup (&conf_object_private, conf_file))
    {
      ret_flag++;
      output_module->Output("Errore: il file di configurazione non esiste\n");
      return ret_flag;
    }

  if (conf_object_private.record_length == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify RECORD_LENGTH\n");
    }

  if (conf_object_private.max_num_events_BLT == -1)
    {
      ret_flag++;
      output_module->Output("Error: you have not specify MAX_NUM_EVENTS_BLT\n");
    }

  return ret_flag;

}
