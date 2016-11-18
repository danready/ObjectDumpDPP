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
* @file DigitizerErrorObject.cpp 
*/

#include "DigitizerErrorObject.h"
#include "DefineGeneral.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CAENDigitizer.h>
#include <sys/time.h>
#include <time.h>

void
DigitizerErrorObject::DigitizerErrorObjectDebugging (CAEN_DGTZ_ErrorCode
						     ritorno,
						     const char *file,
						     const char *func,
						     int line)
{
  switch (ritorno)
    {

    case (CAEN_DGTZ_Success):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Operation completed successful\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_CommError):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Communication error\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_GenericError):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Unspecified error\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidParam):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Invalid parameter\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidLinkType):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Invalid Link Type\n",
	       file, func, line, ritorno);
      break;

    case (-5):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode:%d Invalid device handler\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_MaxDevicesError):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Maximum number of devices exceeded\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_BadBoardType):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Operation non allowed on this type of board\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_BadInterruptLev):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The interrupt level is not allowed\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_BadEventNumber):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The event number is bad\n",
	       file, func, line, ritorno);
      break;

    case (-10):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode:%d Unable to read the regitry\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_ChannelBusy):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The channel number is invalid\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_FPIOModeInvalid):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Invalid FPIO mode\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_WrongAcqMode):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Wrong acquisition mode\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_FunctionNotAllowed):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d This function is not allowed for this module\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_Timeout):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Communication Timeout\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidBuffer):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The buffer is invalid\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_EventNotFound):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The event is not found\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidEvent):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The event is invalid\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_OutOfMemory):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Out of memory\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_CalibrationError):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Unable to calibrate the board\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_DigitizerNotFound):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Unable to open the digitizer\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_DigitizerAlreadyOpen):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The Digitizer is already open\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_DigitizerNotReady):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The Digitizer is not ready to operate\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InterruptNotConfigured):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The Digitizer has not the IRQ configured\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_DigitizerMemoryCorrupted):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The Digitizer flash memory is corrupted\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_DPPFirmwareNotSupported):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The digitizer DPP firmware is not supported in this lib version\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidLicense):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d Invalid Firmware License\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidDigitizerStatus):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The digitizer is found in a corrupted status\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_UnsupportedTrace):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The given trace is not supported by the digitizer\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_InvalidProbe):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The given probe is not supported for the given digitizer's trace\n",
	       file, func, line, ritorno);
      break;

    case (CAEN_DGTZ_NotYetImplemented):
      fprintf (stderr,
	       "File: %s Function: %s Line: %d ErrorCode: %d The function is not yet implemented\n",
	       file, func, line, ritorno);
      break;
    }

}

void
DigitizerErrorObject::DigitizerErrorObjectPrintError
  (CAEN_DGTZ_ErrorCode ritorno)
{
  switch (ritorno)
    {

    case (CAEN_DGTZ_Success):
      fprintf (stderr, "ErrorCode: %d Operation completed successful\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_CommError):
      fprintf (stderr, "ErrorCode: %d Communication error\n", ritorno);
      break;

    case (CAEN_DGTZ_GenericError):
      fprintf (stderr, "ErrorCode: %d Unspecified error\n", ritorno);
      break;

    case (CAEN_DGTZ_InvalidParam):
      fprintf (stderr, "ErrorCode: %d Invalid parameter\n", ritorno);
      break;

    case (CAEN_DGTZ_InvalidLinkType):
      fprintf (stderr, "ErrorCode: %d Invalid Link Type\n", ritorno);
      break;

    case (-5):
      fprintf (stderr, "ErrorCode:%d Invalid device handler\n", ritorno);
      break;

    case (CAEN_DGTZ_MaxDevicesError):
      fprintf (stderr, "ErrorCode: %d Maximum number of devices exceeded\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_BadBoardType):
      fprintf (stderr,
	       "ErrorCode: %d Operation non allowed on this type of board\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_BadInterruptLev):
      fprintf (stderr, "ErrorCode: %d The interrupt level is not allowed\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_BadEventNumber):
      fprintf (stderr, "ErrorCode: %d The event number is bad\n", ritorno);
      break;

    case (-10):
      fprintf (stderr, "ErrorCode:%d Unable to read the regitry\n", ritorno);
      break;

    case (CAEN_DGTZ_ChannelBusy):
      fprintf (stderr, "ErrorCode: %d The channel number is invalid\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_FPIOModeInvalid):
      fprintf (stderr, "ErrorCode: %d Invalid FPIO mode\n", ritorno);
      break;

    case (CAEN_DGTZ_WrongAcqMode):
      fprintf (stderr, "ErrorCode: %d Wrong acquisition mode\n", ritorno);
      break;

    case (CAEN_DGTZ_FunctionNotAllowed):
      fprintf (stderr,
	       "ErrorCode: %d This function is not allowed for this module\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_Timeout):
      fprintf (stderr, "ErrorCode: %d Communication Timeout\n", ritorno);
      break;

    case (CAEN_DGTZ_InvalidBuffer):
      fprintf (stderr, "ErrorCode: %d The buffer is invalid\n", ritorno);
      break;

    case (CAEN_DGTZ_EventNotFound):
      fprintf (stderr, "ErrorCode: %d The event is not found\n", ritorno);
      break;

    case (CAEN_DGTZ_InvalidEvent):
      fprintf (stderr, "ErrorCode: %d The event is invalid\n", ritorno);
      break;

    case (CAEN_DGTZ_OutOfMemory):
      fprintf (stderr, "ErrorCode: %d Out of memory\n", ritorno);
      break;

    case (CAEN_DGTZ_CalibrationError):
      fprintf (stderr, "ErrorCode: %d Unable to calibrate the board\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_DigitizerNotFound):
      fprintf (stderr, "ErrorCode: %d Unable to open the digitizer\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_DigitizerAlreadyOpen):
      fprintf (stderr, "ErrorCode: %d The Digitizer is already open\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_DigitizerNotReady):
      fprintf (stderr,
	       "ErrorCode: %d The Digitizer is not ready to operate\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_InterruptNotConfigured):
      fprintf (stderr,
	       "ErrorCode: %d The Digitizer has not the IRQ configured\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_DigitizerMemoryCorrupted):
      fprintf (stderr,
	       "ErrorCode: %d The Digitizer flash memory is corrupted\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_DPPFirmwareNotSupported):
      fprintf (stderr,
	       "ErrorCode: %d The digitizer DPP firmware is not supported in this lib version\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_InvalidLicense):
      fprintf (stderr, "ErrorCode: %d Invalid Firmware License\n", ritorno);
      break;

    case (CAEN_DGTZ_InvalidDigitizerStatus):
      fprintf (stderr,
	       "ErrorCode: %d The digitizer is found in a corrupted status\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_UnsupportedTrace):
      fprintf (stderr,
	       "ErrorCode: %d The given trace is not supported by the digitizer\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_InvalidProbe):
      fprintf (stderr,
	       "ErrorCode: %d The given probe is not supported for the given digitizer's trace\n",
	       ritorno);
      break;

    case (CAEN_DGTZ_NotYetImplemented):
      fprintf (stderr, "ErrorCode: %d The function is not yet implemented\n",
	       ritorno);
      break;
    }
}


int
DigitizerErrorObject::DigitizerErrorObjectDebuggingLog (CAEN_DGTZ_ErrorCode
							ritorno,
							const char *file,
							const char *func,
							int line,
							FILE * log_file)
{

  char ts[100];
  time_t t;
  t = time (NULL);
  ctime_r (&t, ts);
  ts[strlen (ts) - 1] = '\0';

  switch (ritorno)
    {

    case (CAEN_DGTZ_Success):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Operation completed successfully\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 0;
      break;

    case (CAEN_DGTZ_CommError):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Communication error\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_GenericError):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Unspecified error\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidParam):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Invalid parameter\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidLinkType):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Invalid Link Type\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (-5):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode:%d Invalid device handler\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_MaxDevicesError):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Maximum number of devices exceeded\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_BadBoardType):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Operation non allowed on this type of board\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_BadInterruptLev):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The interrupt level is not allowed\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_BadEventNumber):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The event number is bad\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (-10):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode:%d Unable to read the regitry\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_ChannelBusy):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The channel number is invalid\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_FPIOModeInvalid):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Invalid FPIO mode\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_WrongAcqMode):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Wrong acquisition mode\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_FunctionNotAllowed):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d This function is not allowed for this module\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_Timeout):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Communication Timeout\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidBuffer):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The buffer is invalid\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_EventNotFound):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The event is not found\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidEvent):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The event is invalid\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_OutOfMemory):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Out of memory\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_CalibrationError):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Unable to calibrate the board\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_DigitizerNotFound):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Unable to open the digitizer\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_DigitizerAlreadyOpen):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The Digitizer is already open\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_DigitizerNotReady):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The Digitizer is not ready to operate\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InterruptNotConfigured):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The Digitizer has not the IRQ configured\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_DigitizerMemoryCorrupted):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The Digitizer flash memory is corrupted\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_DPPFirmwareNotSupported):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The digitizer DPP firmware is not supported in this lib version\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidLicense):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d Invalid Firmware License\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidDigitizerStatus):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The digitizer is found in a corrupted status\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_UnsupportedTrace):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The given trace is not supported by the digitizer\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_InvalidProbe):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The given probe is not supported for the given digitizer's trace\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;

    case (CAEN_DGTZ_NotYetImplemented):
      fprintf (log_file,
	       "%s File: %s Function: %s Line: %d ErrorCode: %d The function is not yet implemented\n",
	       ts, file, func, line, ritorno);
      fflush (log_file);
      return 1;
      break;
    }

}
