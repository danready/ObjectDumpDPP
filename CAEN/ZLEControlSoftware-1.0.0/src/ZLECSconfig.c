/******************************************************************************
* 
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the 
* software, documentation and results solely at his own risk.
* -----------------------------------------------------------------------------
* WDconfig contains the functions for reading the configuration file and 
* setting the parameters in the ZLEcsCFile structure
******************************************************************************/


#include <CAENDigitizer.h>
#include "ZLEControlSoftware.h"


/*! \fn      int ParseConfigFile(FILE *f_ini, ZLECSConfig_t *ZLEcsCFile) 
*   \brief   Read the configuration file and set the ZLE Control Software paremeters
*            
*   \param   f_ini        Pointer to the config file
*   \param   ZLEcsCFile:   Pointer to the ZLEControlSoftwareConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int ParseConfigFile(FILE *f_ini, ZLECSConfig_t *ZLEcsCFile) 
{
	char str[1000], str1[1000];
	int i, ch=-1, val, Off=0;

	/* Default settings */
	memset(ZLEcsCFile, 0, sizeof(*ZLEcsCFile));

	ZLEcsCFile->chPlot = 0;
	ZLEcsCFile->NumEvents = 200;
	ZLEcsCFile->EnableMask = 0xFF;
	ZLEcsCFile->FPIOtype = 0;

	ZLEcsCFile->AcqMode = 0;
	ZLEcsCFile->TestWave = 0;   
	ZLEcsCFile->ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
	strcpy(ZLEcsCFile->GnuPlotPath, GNUPLOT_DEFAULT_PATH);

	//	ZLEcsCFile->DataFromFile = 0;

	for(i=0; i<MAX_CH; i++) {  
		ZLEcsCFile->DCoffset[i] = 0;
		ZLEcsCFile->ZLEParams.ZleUppThr[i]	= 20;
		ZLEcsCFile->ZLEParams.ZleUndThr[i]	= 20;
		ZLEcsCFile->ZLEParams.NSampBck[i]	= 4;
		ZLEcsCFile->ZLEParams.NSampAhe[i]	= 4;
		ZLEcsCFile->ZLEParams.selNumSampBsl[i] = 2;
		ZLEcsCFile->ZLEParams.bslThrshld[i]= 5;
		ZLEcsCFile->ZLEParams.bslTimeOut[i]= 80;
	}
	ZLEcsCFile->ZLEParams.preTrgg	= 12;

	ZLEcsCFile->recordLength	= 16;

	/* read config file and assign parameters */
	while(!feof(f_ini)) {
		int read;
		// read a word from the file
		read = fscanf(f_ini, "%s", str);
		if( !read || (read == EOF) || !strlen(str))
			continue;
		// skip comments
		if(str[0] == '#') {
			fgets(str, 1000, f_ini);
			continue;
		}

		if (strcmp(str, "@ON")==0) {
			Off = 0;
			continue;
		}
		if (strcmp(str, "@OFF")==0)
			Off = 1;
		if (Off)
			continue;


		// Section (COMMON or individual channel)
		if (str[0] == '[') {
			if (strstr(str, "COMMON")) {
				ch = -1;
			} else {
				sscanf(str+1, "%d", &val);
				if (val < 0 || val >= MAX_CH) {
					printf("%s: Invalid channel number\n", str);
				} else {
					ch = val;
				}
			}
			continue;
		}

		// OPEN: read the details of physical path to the digitizer
		if (strstr(str, "OPEN")!=NULL) {
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "USB")==0)
				ZLEcsCFile->LinkType = CAEN_DGTZ_USB;
			else if (strcmp(str1, "PCI")==0)
				ZLEcsCFile->LinkType = CAEN_DGTZ_PCI_OpticalLink;
			else {
				printf("%s %s: Invalid connection type\n", str, str1);
				return -1; 
			}
			fscanf(f_ini, "%d", &ZLEcsCFile->LinkNum);
			if (ZLEcsCFile->LinkType == CAEN_DGTZ_USB)
				ZLEcsCFile->ConetNode = 0;
			else
				fscanf(f_ini, "%d", &ZLEcsCFile->ConetNode);
			fscanf(f_ini, "%x", &ZLEcsCFile->BaseAddress);
			continue;
		}

		// Generic VME Write (address offset + data, both exadecimal)
		if ((strstr(str, "WRITE_REGISTER")!=NULL) && (ZLEcsCFile->GWn < MAX_GW)) {
			fscanf(f_ini, "%x", (int *)&ZLEcsCFile->GWaddr[ZLEcsCFile->GWn]);
			fscanf(f_ini, "%x", (int *)&ZLEcsCFile->GWdata[ZLEcsCFile->GWn]);
			ZLEcsCFile->GWn++;
			continue;
		}

		// Front Panel LEMO I/O level (NIM, TTL)
		if (strstr(str, "FPIO_LEVEL")!=NULL) {
			ZLEcsCFile->FPIOtype = 0;
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "TTL")==0)
				ZLEcsCFile->FPIOtype = 1;
			else if (strcmp(str1, "NIM")!=0)
				printf("%s: invalid option\n", str);
			continue;
		}

		// TEST WAVEFORM
		if (strstr(str, "TEST_WAVEFORM")!=NULL) {
			fscanf(f_ini, "%s", str1);
			ZLEcsCFile->TestWave = 0;
			if (strcmp(str1, "ENABLED")==0)
				ZLEcsCFile->TestWave = 1;
			else if (strcmp(str1, "DISABLED")!=0)
				printf("%s: invalid option\n", str);
			continue;
		}

		// Acquisition Record Length (number of samples)
		if (strstr(str, "RECORD_LENGTH")!=NULL) {
			//fscanf(f_ini, "%d", &ZLEcsCFile->recordLength);
			fscanf(f_ini, "%d", &val);
			ZLEcsCFile->recordLength= val;
			continue;
		}

		// External Trigger (DISABLED, ACQUISITION_ONLY, ACQUISITION_AND_TRGOUT)
		if (strstr(str, "EXTERNAL_TRIGGER")!=NULL) {
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "DISABLED")==0)
				ZLEcsCFile->ExtTriggerMode = CAEN_DGTZ_TRGMODE_DISABLED;
			else if (strcmp(str1, "ACQUISITION_ONLY")==0)
				ZLEcsCFile->ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
			else if (strcmp(str1, "ACQUISITION_AND_TRGOUT")==0)
				ZLEcsCFile->ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;
			else
				printf("%s: Invalid Parameter\n", str);
			continue;
		}

		// Max. number of events for a block transfer (0 to 1023)
		if (strstr(str, "MAX_NUM_EVENTS_BLT")!=NULL) {
			fscanf(f_ini, "%d", &ZLEcsCFile->NumEvents);
			continue;
		}

		// GNUplot path
		if (strstr(str, "GNUPLOT_PATH")!=NULL) {
			fscanf(f_ini, "%s", ZLEcsCFile->GnuPlotPath);
			continue;
		}

		// ZLE_upper_thr 
		if (strstr(str, "ZLE_UPP_THRESHOLD")!=NULL) {
			fscanf(f_ini, "%d", &val);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.ZleUppThr[i] = val;
			else
				ZLEcsCFile->ZLEParams.ZleUppThr[ch] = val;
			continue;
		}

		// ZLE_under_thr
		if (strstr(str, "ZLE_UND_THRESHOLD")!=NULL) {
			fscanf(f_ini, "%d", &val);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.ZleUndThr[i] = val;
			else
				ZLEcsCFile->ZLEParams.ZleUndThr[ch] = val;
			continue;
		}

		// Nsampl_back
		if (strstr(str, "ZLE_NSAMP_BACK")!=NULL) {
			fscanf(f_ini, "%d", &val);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.NSampBck[i] = val;
			else
				ZLEcsCFile->ZLEParams.NSampBck[ch] = val;
			continue;
		}

		// Nsampl_ahead
		if (strstr(str, "ZLE_NSAMP_AHEAD")!=NULL) {
			fscanf(f_ini, "%d", &val);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.NSampAhe[i] = val;
			else
				ZLEcsCFile->ZLEParams.NSampAhe[ch] = val;
			continue;
		}

		// Pre Trigger
		if (strstr(str, "PRE_TRIGGER")!=NULL) {
			int fval;
			fscanf(f_ini, "%d", &fval);
			ZLEcsCFile->ZLEParams.preTrgg = fval;
			continue;
		}

		// Baseline Threshold
		if (strstr(str, "BSL_THRESHOLD")!=NULL) {
			int fval;
			fscanf(f_ini, "%d", &fval);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.bslThrshld[i] = fval ;
			else
				ZLEcsCFile->ZLEParams.bslThrshld[ch] = val;
			continue;
		}

		// Baseline Time Out
		if (strstr(str, "BSL_TIMEOUT")!=NULL) {
			int fval;
			fscanf(f_ini, "%d", &fval);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.bslTimeOut[i] = fval ;
			else
				ZLEcsCFile->ZLEParams.bslTimeOut[ch] = fval ;
			continue;
		}

		// Number of Samples for Baseline Calculation
		if (strstr(str, "SEL_NSBL")!=NULL) {
			int fval;
			fscanf(f_ini, "%d", &fval);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->ZLEParams.selNumSampBsl[i] = fval ;
			else
				ZLEcsCFile->ZLEParams.selNumSampBsl[ch] = fval ;
			continue;
		}

		// DC offset (percent of the dynamic range, -50 to 50)
		if (strstr(str, "DC_OFFSET")!=NULL) {
			float dc;
			fscanf(f_ini, "%f", &dc);
			val = (int)((dc+50) * 65535 / 100);
			if (ch == -1)
				for(i=0; i<MAX_CH; i++)
					ZLEcsCFile->DCoffset[i] = val;
			else
				ZLEcsCFile->DCoffset[ch] = val;
			continue;
		}

		// Channel Enable (YES/NO)
		if (strstr(str, "ENABLE_INPUT")!=NULL) {
			fscanf(f_ini, "%s", str1);
			if (strcmp(str1, "YES")==0) {
				if (ch == -1)
					ZLEcsCFile->EnableMask = 0xFF;
				else
					ZLEcsCFile->EnableMask |= (1 << ch);
				continue;
			} else if (strcmp(str1, "NO")==0) {
				if (ch == -1)
					ZLEcsCFile->EnableMask = 0x00;
				else
					ZLEcsCFile->EnableMask &= ~(1 << ch);
				continue;
			} else {
				printf("%s: invalid option\n", str);
			}
			continue;
		}

		printf("%s: invalid setting\n", str);
	}
	return 0;
}
