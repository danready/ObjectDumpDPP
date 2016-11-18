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
******************************************************************************/

#ifndef _ZLEControlSoftware_H_
#define _ZLEControlSoftware_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#ifdef WIN32

#include <time.h>
#include <sys/timeb.h>
#include <conio.h>
#include <process.h>

#define getch _getch     /* redefine POSIX 'deprecated' */
#define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
#include <unistd.h>
#include <stdint.h>   /* C99 compliant compilers: uint64_t */
#include <ctype.h>    /* toupper() */
#include <sys/time.h>
#endif

#ifdef LINUX
#define DEFAULT_CONFIG_FILE  "/etc/ZLEControlSoftware/ZLEControlSoftwareConfig.txt"
#define GNUPLOT_DEFAULT_PATH ""
#else
#define DEFAULT_CONFIG_FILE  "ZLEControlSoftwareConfig.txt"  /* local directory */
#define GNUPLOT_DEFAULT_PATH ""
#endif

#define OUTFILENAME "wave"  /* The actual file name is wave_n.txt, where n is the channel */
#define MAX_CH  8           /* max. number of channels */

#define MAX_GW  1000        /* max. number of generic write commads */

#define PLOT_REFRESH_TIME 1000
#define DIGITAL_TRACE_HEIGHT 100

#define PLOT_WAVEFORMS   0
#define PLOT_EHISTO      1
#define PLOT_HISTO2D     2

#define OUTPUTFILE_ENABLE_WAVEFORMS   0

#define MAX_NUMSAMPLES   (1<<23)

/*###########################################################################
Typedefs
###########################################################################*/

typedef struct {

	// Parameter for the communication
	int LinkType;
	int LinkNum;
	int ConetNode;
	uint32_t BaseAddress;

	// Parameter for the digitizer
	int tSampl;
	int Nch;
	int NumEvents;  // MAX_NUM_EVENTS_BLT
	int recordLength;
	CAEN_DGTZ_IOLevel_t FPIOtype;
	int AcqMode;
	int TestWave;

	int DCoffset[MAX_CH];
	CAEN_DGTZ_751_ZLE_Params_t ZLEParams;

	int buffSize;
	int dataWidthByte;

	uint8_t EnableMask;
	CAEN_DGTZ_TriggerMode_t ExtTriggerMode;

	// Generic write accesses to the registers
	int GWn;
	uint32_t GWaddr[MAX_GW];
	uint32_t GWdata[MAX_GW];

	// Gnuplot 
	char GnuPlotPath[1000];

	// Variables for the acquisition and run control
	int Quit;
	int AcqRun;

	int ContinuousTrigger;
	int ContinuousWrite;
	int SingleWrite[MAX_CH];
	int OFenable;
	FILE *fout[MAX_CH];

	// Variables for the plots
	int SetPlotOptions;
	int PlotType;
	int ContinuousPlot;
	int SinglePlot;
	int chPlot;
	int TraceEnable[3];

	// Data buffers and variables for the analysis
	uint32_t PrevTime[MAX_CH];
	uint64_t Ecnt[MAX_CH], Tcnt[MAX_CH], TrgCnt[MAX_CH], PURcnt[MAX_CH];  // counters
	uint64_t ExtendedTT[MAX_CH];

} ZLECSConfig_t;

/* Function prototypes */
int ParseConfigFile(FILE *f_ini, ZLECSConfig_t *ZLEcsCFile);


#endif /* _ZLEControlSoftware__H */
