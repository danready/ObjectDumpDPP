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
* @file DigitizerObjectGeneric.cpp 
*/

#include "X742CorrectionRoutines.h"
#include "ConfObject.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "DigitizerObjectGeneric.h"
#include "Analizzatore.h"
#include "DefineGeneral.h"
#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>


DigitizerObjectGeneric::DigitizerObjectGeneric ()
{
	set_board_info = 0;
	logfile = LogFile::Instance ();
}


int 
DigitizerObjectGeneric::DigitizerObjectGenericSetAllInformations ()
{
	int max_channels = 0;
	int max_groups = 0;

	int FamilyCode = BoardInfo.FamilyCode;

	int FormFactor = BoardInfo.FormFactor;

	int tmp;

	int i = 0;

	switch(FamilyCode) 
	{
		case CAEN_DGTZ_XX724_FAMILY_CODE:
		case CAEN_DGTZ_XX781_FAMILY_CODE:
		case CAEN_DGTZ_XX720_FAMILY_CODE:
		case CAEN_DGTZ_XX721_FAMILY_CODE:
		case CAEN_DGTZ_XX751_FAMILY_CODE:
		case CAEN_DGTZ_XX761_FAMILY_CODE:
		case CAEN_DGTZ_XX731_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			max_channels = 8;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			max_channels = 4;
			break;
		}
		break;
		case CAEN_DGTZ_XX730_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			max_channels = 16;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			max_channels = 8;
			break;
		}
		break;
		case CAEN_DGTZ_XX740_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			max_channels = 64;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			max_channels = 32;
			break;
		}
		break;
		case CAEN_DGTZ_XX742_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			//max_channels = 36; ---> sbagliato!!!
			max_groups = 4;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			//max_channels = 16; ---> sbagliato!!!
			max_groups = 2;
			break;
		}
		break;
		default:
		assert("This program cannot be used with this digitizer family\n");
	}

	if (FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
	{

		tmp = max_channels/8;

		if (internal_config.dc_offset != -1)
		{
			for (i=0; i<tmp; i++)
			{
				DigitizerObject::DigitizerObjectSetGroupDCOffset (i, internal_config.dc_offset);
			}
		}
		if (internal_config.trigger_threshold != -1)
		{
			for (i=0; i<tmp; i++)
			{
				ret = CAEN_DGTZ_SetGroupTriggerThreshold (handle, i, internal_config.trigger_threshold);
				logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			}
		}

	}
	else
	{

		if (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
		max_channels = max_groups*8;

		if (internal_config.dc_offset != -1)
		{
			for (i=0; i<max_channels; i++)
			{
				DigitizerObject::DigitizerObjectSetChannelDCOffset (i, internal_config.dc_offset);
			}
		}
		if (internal_config.trigger_threshold != -1)
		{
			for (i=0; i<max_channels; i++)
			{
				ret = CAEN_DGTZ_SetChannelTriggerThreshold (handle, i, internal_config.trigger_threshold);
				logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			}
		}
	}

}


int 
DigitizerObjectGeneric::DigitizerObjectGenericSetDecimationFactor ()
{
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE && internal_config.decimation_factor != -1)
	{ 
		ret = CAEN_DGTZ_SetDecimationFactor (handle, internal_config.decimation_factor);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		return 0;
	}
}


int 
DigitizerObjectGeneric::DigitizerObjectGenericSetDesMode ()
{
    if (((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)) && internal_config.desmod != -1) 
    {
		CAEN_DGTZ_EnaDis_t desmodtype;
		if (internal_config.desmod == 1) 
		{
			desmodtype = CAEN_DGTZ_ENABLE;
		}
		else
		{
			desmodtype = CAEN_DGTZ_DISABLE;		
		}

		ret = CAEN_DGTZ_SetDESMode(handle, desmodtype);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		return 0;

    }
}


int 
DigitizerObjectGeneric::DigitizerObjectGenericSetTestPattern ()
{
	if (internal_config.test_pattern == 1)
	{
		ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1<<3);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		return 0;
	}
}


int
DigitizerObjectGeneric::DigitizerObjectGenericSetRecordLength ()
{
	ret = CAEN_DGTZ_SetRecordLength (handle, internal_config.record_length);
	logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	return 0;
}


int
DigitizerObjectGeneric::DigitizerObjectGenericSetMaxNumEventsBLT ()
{
	ret =
		CAEN_DGTZ_SetMaxNumEventsBLT (handle, internal_config.max_num_events_BLT);
	logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	return 0;
}


int
DigitizerObjectGeneric::DigitizerObjectGenericSetEnableMask ()
{
	//Occorre scrivere le informazioni direttamente sui registri perche' le funzioni di libreria non funzionano
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE
	|| BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
	{
		if (internal_config.group_enable_mask != -1)
		{
			DigitizerObjectWriteRegister (0x8120,
					internal_config.group_enable_mask);
		}
	}
	else
	{
		if (internal_config.channel_enable_mask != -1)
		{
			DigitizerObjectWriteRegister (0x8120,
					internal_config.channel_enable_mask);
		}
	}
}  //int DigitizerObjectGeneric::DigitizerObjectGenericSetEnableMask ()



int
DigitizerObjectGeneric::DigitizerObjectGenericSetExtTriggerInputMode ()
{

	switch (internal_config.external_trigger_acquisition_mode)
	{
		case 0:
		ret =
		CAEN_DGTZ_SetExtTriggerInputMode (handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
		  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;

		case 1:
		ret =
		CAEN_DGTZ_SetExtTriggerInputMode (handle,
		  CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
		  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;

		case 2:
		ret =
		CAEN_DGTZ_SetExtTriggerInputMode (handle, CAEN_DGTZ_TRGMODE_DISABLED);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;
	}

}  //int DigitizerObjectGeneric::DigitizerObjectGenericSetExtTriggerInputMode ()



int
DigitizerObjectGeneric::DigitizerObjectGenericSetDRS4SamplingFrequency ()
{
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
	{ 
		switch (internal_config.DSR4_Frequency)
		{
			case 0:
			ret =
			CAEN_DGTZ_SetDRS4SamplingFrequency (handle, CAEN_DGTZ_DRS4_5GHz);
			  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;

			case 1:
			ret =
			CAEN_DGTZ_SetDRS4SamplingFrequency (handle,
			CAEN_DGTZ_DRS4_2_5GHz);
			  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;

			case 2:
			ret =
			CAEN_DGTZ_SetDRS4SamplingFrequency (handle, CAEN_DGTZ_DRS4_1GHz);
			  logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;
		}   //switch(internal_config.DSR4_Frequency)
	}   //if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
}  //int DigitizerObjectGeneric::DigitizerObjectGenericSetDRS4SamplingFrequency ()

int 
DigitizerObjectGeneric::DigitizerObjectSetAutomaticCorrectionX742 ()
{ 
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
	{   
		char stringa[1000];
		bzero(stringa,1000);
		sprintf(stringa, "%s", "./RawData/Corrections");
		CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];
		CAEN_DGTZ_DRS4Frequency_t frequenza;
		DigitizerObjectGeneric::DigitizerObjectGetDRS4SamplingFrequency(&frequenza);
		ret =CAEN_DGTZ_LoadDRS4CorrectionData(handle, frequenza);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		ret = CAEN_DGTZ_EnableDRS4Correction(handle);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		ret = CAEN_DGTZ_GetCorrectionTables(handle, frequenza, (void*)X742Tables);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		SaveCorrectionTables(stringa, 15, X742Tables);
	}
}

int
DigitizerObjectGeneric::DigitizerObjectGenericSetPostTriggerSize ()
{
	if (internal_config.post_trigger != -1)
	{
		ret =
		CAEN_DGTZ_SetPostTriggerSize (handle, internal_config.post_trigger);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
	}
}   //int DigitizerObjectGeneric::DigitizerObjectGenericSetPostTriggerSize ()


int
DigitizerObjectGeneric::DigitizerObjectGenericSetIOLevel ()
{

	switch (internal_config.nim_ttl)
	{
		case 0:			//NIM
		ret = CAEN_DGTZ_SetIOLevel (handle, CAEN_DGTZ_IOLevel_NIM);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;

		case 1:			//TTL
		ret = CAEN_DGTZ_SetIOLevel (handle, CAEN_DGTZ_IOLevel_TTL);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;
	}

}   // int DigitizerObjectGeneric::DigitizerObjectGenericSetIOLevel ()



int
DigitizerObjectGeneric::DigitizerObjectGenericSetDCOffset ()
{

	int i;

	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
	{
		for (i = 0; i < MAXGROUPOBJECT; i++)
		{
			if (internal_config.groups[i].set != -1
			&& internal_config.groups[i].dc_offset != -1)
			{
				DigitizerObject::DigitizerObjectSetGroupDCOffset
				(internal_config.groups[i].numGroup,
				internal_config.groups[i].dc_offset);
			}
		}   //for (i=0;i<MAXGROUPOBJECT;i++)
	}
	else
	{
		for (i = 0; i < MAXCHANNELOBJECT; i++)
		{
			if (internal_config.channels[i].set != -1
			&& internal_config.channels[i].dc_offset != -1)
			{
				DigitizerObject::DigitizerObjectSetChannelDCOffset
				(internal_config.channels[i].numChannel,
				internal_config.channels[i].dc_offset);
			}
		}   //for (i=0;i<MAXCHANNELOBJECT;i++)
	}

}   //int DigitizerObjectGeneric::DigitizerObjectGenericSetDCOffset ()



int
DigitizerObjectGeneric::DigitizerObjectGenericSetSelfTrigger ()
{

	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
	{

		switch (internal_config.self_trigger_enable_mask_mode)
		{
			case 0:
			ret =
			CAEN_DGTZ_SetGroupSelfTrigger (handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY,
				internal_config.
				self_trigger_enable_mask);
				logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;

			case 1:
			ret =
			CAEN_DGTZ_SetGroupSelfTrigger (handle,
				CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT,
				internal_config.
				self_trigger_enable_mask);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;

			case 2:
			ret =
			CAEN_DGTZ_SetGroupSelfTrigger (handle, CAEN_DGTZ_TRGMODE_DISABLED,
				internal_config.
				self_trigger_enable_mask);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			break;
		}

	}
	else
	{

		switch (internal_config.self_trigger_enable_mask_mode)
		{
		case 0:
		ret =
		CAEN_DGTZ_SetChannelSelfTrigger (handle,
			CAEN_DGTZ_TRGMODE_ACQ_ONLY,
			internal_config.
			self_trigger_enable_mask);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;

		case 1:
		ret =
		CAEN_DGTZ_SetChannelSelfTrigger (handle,
			CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT,
			internal_config.
			self_trigger_enable_mask);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;

		case 2:
		ret =
		CAEN_DGTZ_SetChannelSelfTrigger (handle,
			CAEN_DGTZ_TRGMODE_DISABLED,
			internal_config.
			self_trigger_enable_mask);
		logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		break;
		}

	}

}   //int DigitizerObjectGeneric::DigitizerObjectGenericSetSelfTrigger ()




int
DigitizerObjectGeneric::
DigitizerObjectGenericSetChannelSelfTriggerThreshold ()
{

	int i;

	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
	{
		for (i = 0; i < MAXGROUPOBJECT; i++)
		{
			if (internal_config.groups[i].set != -1
			&& internal_config.groups[i].trigger_threshold != -1)
			{
				ret = CAEN_DGTZ_SetGroupTriggerThreshold
				(handle, internal_config.groups[i].numGroup,
				internal_config.groups[i].trigger_threshold);
				logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			}
		}   //for (i=0; i<MAXGROUPOBJECT; i++)
	}
	else
	{
		for (i = 0; i < MAXCHANNELOBJECT; i++)
		{
			if (internal_config.channels[i].set != -1
			&& internal_config.channels[i].trigger_threshold != -1)
			{
				ret = CAEN_DGTZ_SetChannelTriggerThreshold
				(handle, internal_config.channels[i].numChannel,
				internal_config.channels[i].trigger_threshold);
				logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
			}
		}   //for (i=0;i<MAXCHANNELOBJECT;i++)
	}   //else        

}   //int DigitizerObjectGeneric::DigitizerObjectGenericSetChannelSelfTriggerThreshold ()



int
DigitizerObjectGeneric::DigitizerObjectGenericSetFastTriggerDigitizing ()
{

	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
	{

		if (internal_config.enable_fast_trigger_digitizing == 1)
		{
			ret = CAEN_DGTZ_SetFastTriggerDigitizing (handle, CAEN_DGTZ_ENABLE);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.enable_fast_trigger_digitizing == 0)
		{
			ret =
			CAEN_DGTZ_SetFastTriggerDigitizing (handle, CAEN_DGTZ_DISABLE);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fast_trigger_acquisition_mode == 0)
		{
			ret =
			CAEN_DGTZ_SetFastTriggerMode (handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fast_trigger_acquisition_mode == 2)
		{
			ret =
			CAEN_DGTZ_SetFastTriggerMode (handle, CAEN_DGTZ_TRGMODE_DISABLED);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fasts[0].set != -1
		&& internal_config.fasts[0].dc_offset != -1)
		{
			ret =
			CAEN_DGTZ_SetGroupFastTriggerDCOffset (handle,
			internal_config.fasts[0].
			numFast,
			internal_config.fasts[0].
			dc_offset);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fasts[1].set != -1
		&& internal_config.fasts[1].dc_offset != -1)
		{
			ret =
			CAEN_DGTZ_SetGroupFastTriggerDCOffset (handle,
			internal_config.fasts[1].
			numFast,
			internal_config.fasts[1].
			dc_offset);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fasts[0].set != -1
		&& internal_config.fasts[0].trigger_threshold != -1)
		{
			ret =
			CAEN_DGTZ_SetGroupFastTriggerThreshold (handle,
			internal_config.fasts[0].
			numFast,
			internal_config.fasts[0].
			trigger_threshold);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

		if (internal_config.fasts[1].set != -1
		&& internal_config.fasts[1].trigger_threshold != -1)
		{
			ret = CAEN_DGTZ_SetGroupFastTriggerThreshold
			(handle, internal_config.fasts[1].numFast,
			internal_config.fasts[1].trigger_threshold);
			logfile->LogFileWrite (ret, __FILE__, __func__, __LINE__);
		}

	}   //if (internal_config.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE){

}   //int DigitizerObjectGeneric::DigitizerObjectGenericSetFastTriggerDigitizing ()
