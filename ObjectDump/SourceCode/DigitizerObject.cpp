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
* @file DigitizerObject.cpp
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include "ConfObject.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "Analizzatore.h"
#include "ApplicationSetup.h"
#include "OutputModule.h"
#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>

DigitizerObject::DigitizerObject (const char *config_file)
{
	if (AnalizzaInit (&internal_config, config_file))
		logfile->
		LogFileWriteString
		("Warning: can't create DigitizerObject correctly. Configuration file does't exists\n");
	if (AnalizzaSetup (&internal_config, config_file))
		logfile->
		LogFileWriteString
		("Warning: can't create DigitizerObject correctly. Configuration file does't exists\n");
	set_board_info = 0;
	logfile = LogFile::Instance ();
}

DigitizerObject::DigitizerObject (ConfObject config)
{
	internal_config = config;
	set_board_info = 0;
	logfile = LogFile::Instance ();
}

void
DigitizerObject::
DigitizerObjectSetConfigStructureConfObject (ConfObject config)
{
	internal_config = config;
	logfile = LogFile::Instance ();
}

DigitizerObject::DigitizerObject ()
{
	set_board_info = 0;
	logfile = LogFile::Instance ();
}

void
DigitizerObject::
DigitizerObjectSetConfigStructureInit (const char *config_file)
{
	if (AnalizzaInit (&internal_config, config_file))
		logfile->
		LogFileWriteString
		("Warning: can't create DigitizerObject correctly. Configuration file does't exists\n");
}

void
DigitizerObject::
DigitizerObjectSetConfigStructureSetup (const char *config_file)
{
	if (AnalizzaSetup (&internal_config, config_file))
		logfile->
		LogFileWriteString
		("Warning: can't create DigitizerObject correctly. Configuration file does't exists\n");
}

int
DigitizerObject::DigitizerObjectOpen ()
{
	char stringa[STANDARDBUFFERLIMIT];
	bzero (stringa, STANDARDBUFFERLIMIT);

	CAEN_DGTZ_ConnectionType Connection;

	if (internal_config.LinkType == 0)
		Connection = CAEN_DGTZ_USB;
	else if (internal_config.LinkType == 1)
		Connection = CAEN_DGTZ_OpticalLink;
	else
	{
		snprintf (stringa, STANDARDBUFFERLIMIT,
			"%s %d Configuration structure invalid: LinkType %d field inconsistent\n",
			__FILE__, __LINE__, internal_config.LinkType);
		logfile->LogFileWriteString (stringa);
		return 1;
	}

	ret =
	CAEN_DGTZ_OpenDigitizer (Connection, internal_config.LinkNumber,
	internal_config.ConetNode,
	internal_config.VMEBaseAddress, &handle);
	//ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
	logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	//ret_error.DigitizerErrorObjectPrintError (ret);

	return 0;
}

int
DigitizerObject::DigitizerObjectReset ()
{
	ret = CAEN_DGTZ_Reset (handle);
	//ret_error.digitizer_error_object_debugging(ret, __FILE__, __LINE__);
	//ret_error.DigitizerErrorObjectPrintError (ret);
	//ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
	logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	return 0;
}

int
DigitizerObject::DigitizerObjectGetInfo ()
{
	ret = CAEN_DGTZ_GetInfo (handle, &BoardInfo);
	//ret_error.digitizer_error_object_debugging(ret, __FILE__, __LINE__);
	//ret_error.DigitizerErrorObjectPrintError (ret);
	//ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
	logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	if (ret == CAEN_DGTZ_Success)
		set_board_info = 1;
	return 0;
}

int
DigitizerObject::GetFamilyCode(int * FamilyCode)
{

	char stringa[STANDARDBUFFERLIMIT];

	if (set_board_info == 0)
	{
		bzero (stringa, STANDARDBUFFERLIMIT);
		snprintf (stringa, STANDARDBUFFERLIMIT,
			"File: %s, Function %s, Line: %d, Error: can't print board info. Use DigitizerObject::DigitizerObjectgetinfo first. Perhaps you have not open the digitizer?\n",
			__FILE__, __func__, __LINE__);
		logfile->LogFileWriteString (stringa);
		return 1;
	}

	*FamilyCode = BoardInfo.FamilyCode;

}

int
DigitizerObject::GetFormFactorCode(int * FormFactor)
{

	char stringa[STANDARDBUFFERLIMIT];

	if (set_board_info == 0)
	{
		bzero (stringa, STANDARDBUFFERLIMIT);
		snprintf (stringa, STANDARDBUFFERLIMIT,
			"File: %s, Function %s, Line: %d, Error: can't print board info. Use DigitizerObject::DigitizerObjectgetinfo first. Perhaps you have not open the digitizer?\n",
			__FILE__, __func__, __LINE__);
		logfile->LogFileWriteString (stringa);
		return 1;
	}

	*FormFactor = BoardInfo.FormFactor;

}

void
DigitizerObject::PrintBoardInfo ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char stringa[STANDARDBUFFERLIMIT];

  if (set_board_info == 0)
  {
    bzero (stringa, STANDARDBUFFERLIMIT);
    snprintf (stringa, STANDARDBUFFERLIMIT,
	  "File: %s, Function %s, Line: %d, Error: can't print board info. Use DigitizerObject::DigitizerObjectgetinfo first. Perhaps you have not open the digitizer?\n",
	  __FILE__, __func__, __LINE__);
    logfile->LogFileWriteString (stringa);
    return;
  }

  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "Board model: %s\n",
	    BoardInfo.ModelName);
  output_module->Output(stringa);

  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "Model: %d\n", BoardInfo.Model);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "Channels: %d\n",
	    BoardInfo.Channels);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "Form factor: %d\n",
	    BoardInfo.FormFactor);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "FamilyCode: %d\n",
	    BoardInfo.FamilyCode);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "ROC_FirmwareRel: %s\n",
	    BoardInfo.ROC_FirmwareRel);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "AMC_FirmwareRel: %s\n",
	    BoardInfo.AMC_FirmwareRel);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "SerialNumber: %d\n",
	    BoardInfo.SerialNumber);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "PCB_Revision: %d\n",
	    BoardInfo.PCB_Revision);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "ADC_NBits: %d\n",
	    BoardInfo.ADC_NBits);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "SAMCorrectionDataLoaded: %d\n",
	    BoardInfo.SAMCorrectionDataLoaded);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "CommHandle: %d\n",
	    BoardInfo.CommHandle);
  output_module->Output(stringa);


  bzero (stringa, STANDARDBUFFERLIMIT);
  snprintf (stringa, STANDARDBUFFERLIMIT, "License: %s\n", BoardInfo.License);
  output_module->Output(stringa);


}

int
DigitizerObject::DigitizerObjectSetRecordLength (int recordlength)
{

  ret = CAEN_DGTZ_SetRecordLength (handle, recordlength);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetGroupEnableMask (int enablemask)
{
  ret = CAEN_DGTZ_SetGroupEnableMask (handle, enablemask);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;
}

int
DigitizerObject::DigitizerObjectWriteRegister (int registry, int data)
{

  ret = CAEN_DGTZ_WriteRegister (handle, registry, data);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectReadRegister (int registry, int *data)
{

  ret =
    CAEN_DGTZ_ReadRegister (handle, (uint32_t) registry, (uint32_t *) data);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectGetDRS4SamplingFrequency
  (CAEN_DGTZ_DRS4Frequency_t * frequenza)
{

  ret = CAEN_DGTZ_GetDRS4SamplingFrequency (handle, frequenza);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetDRS4SamplingFrequency
  (CAEN_DGTZ_DRS4Frequency_t frequenza)
{

  ret = CAEN_DGTZ_SetDRS4SamplingFrequency (handle, frequenza);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetChannelDCOffset (int channel, int dc_mask)
{

  ret = CAEN_DGTZ_SetChannelDCOffset (handle, channel, dc_mask);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetGroupDCOffset (int group, int dc_mask)
{

  ret = CAEN_DGTZ_SetGroupDCOffset (handle, group, dc_mask);
  ret_error.DigitizerErrorObjectPrintError (ret);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetMaxNumEventsBLT (int MaxNumEventsBLT)
{

  ret = CAEN_DGTZ_SetMaxNumEventsBLT (handle, MaxNumEventsBLT);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetAcquisitionMode (CAEN_DGTZ_AcqMode_t
						    AcqMode)
{

  ret = CAEN_DGTZ_SetAcquisitionMode (handle, AcqMode);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectClose ()
{

  ret = CAEN_DGTZ_CloseDigitizer (handle);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetExtTriggerInputMode
  (CAEN_DGTZ_TriggerMode_t TriggerMode)
{

  ret = CAEN_DGTZ_SetExtTriggerInputMode (handle, TriggerMode);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSetSWTriggerMode (CAEN_DGTZ_TriggerMode_t
						  TriggerMode)
{

  ret = CAEN_DGTZ_SetSWTriggerMode (handle, TriggerMode);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSWStartAcquisition ()
{

  ret = CAEN_DGTZ_SWStartAcquisition (handle);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}

int
DigitizerObject::DigitizerObjectSWStopAcquisition ()
{

  ret = CAEN_DGTZ_SWStopAcquisition (handle);
  //ret_error.DigitizerErrorObjectPrintError (ret);
  //ret_error.DigitizerErrorObjectDebugging(ret, __FILE__, __func__, __LINE__);
  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
  return 0;

}
