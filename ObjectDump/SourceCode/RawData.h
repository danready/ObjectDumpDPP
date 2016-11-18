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
* @file RawData.h 
*/

/**
* @brief The RawData class manages the readout from the digitizer.
*
* @author Daniele Berto
*/

#ifndef RAWDATA_H
#define RAWDATA_H

#include <assert.h>
#include <stdio.h>
#include <CAENDigitizer.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timex.h>

class RawData
{

public:

  /**
   * @brief The ret variable contains the error code returned by CAEN_DGTZ library functions.
   */
  CAEN_DGTZ_ErrorCode ret;

  /**
   * @brief The eventInfo variable contains the information about an event.
   */
  CAEN_DGTZ_EventInfo_t eventInfo;

  /**
   * @brief The ret_error object is used to print the meaning of CAEN_DGTZ_ErrorCode.
   */
  DigitizerErrorObject ret_error;

  /**
   * @brief The handle variable represents the device handler of the digitizer.
   */
  int handle;

  /**
   * @brief The size variable represents the dimension of the readout buffer.
   * @details The dimension is determined by CAEN_DGTZ_MallocReadoutBuffer function.
   */
  int size;

  /**
   * @brief The Mode variable represents the mode of readout.
   */
  CAEN_DGTZ_ReadMode_t Mode;

  /**
   * @brief The imset variable indicates if the object has been set or not.
   */
  int imset;

  /**
   * @brief The file pointer is used to write the raw data to the hard disk.
   */
  FILE *file;

  /**
   * @brief The RawData constructor set "imset" to 0.
   */
    RawData ();

  /**
   * @brief The bsize variable represents the dimension of the data read from the digitizer.
   */
  int bsize;

  /**
   * @brief The buffer pointer contains the data read from the digitizer.
   */
  char *buffer;

  /**
   * @brief The digitizer object is fundamental to get the informations to decode correctly the events read from the digitizer.
   */
  DigitizerObjectGeneric digitizer;


  /**
   * @brief The RawDataSet method copies the parameter handler_raw to the private attribute handler. 
   * It also allocates the buffer public attribute using MallocReadoutBuffer function.
   * @return void
   */
  void RawDataSet (DigitizerObjectGeneric digitizer_arg);

  /**
   * @brief The RawDataDel method deallocates buffer attribute. 
   * @details The method uses the free function from the stdlib, not the CAEN_DGTZ_FreeReadoutBuffer function.
   * @return void
   */
  void RawDataDel ();

  /**
   * @brief The RawDataRead method reads data from the digitizer and puts it in the buffer variable. 
   * @details The method uses CAEN_DGTZ_ReadData function from CAENDigitizer library.
   * @return void
   */
  void RawDataRead ();

  /**
   * @brief The RawWriteOnFile method writes the events read from the digitizer into the hard disk. 
   * @details The method doesn't decode the events.
   * @param file_arg is the path of file being written with the raw data.
   * @return void
   */
  void RawDataWriteOnFile (const char *file_arg);

  /**
   * @brief The RawWriteOnFile method writes the events read from the digitizer into the hard disk. 
   * @details The method doesn't decode the events.
   * @param file is the pointer to the file being written with the raw data.
   * @param file_size is the pointer to the file being written with the raw data size.
   * @return void
   */
  void RawDataWriteOnFile (FILE * file, FILE * file_size);

  /**
   * @brief The RawDataWriteDecodeEventOnPlotFile method decodes the events stored in the buffer attribute and writes the in the file specified by file_arg. 
   * @details The method is called by the RawDataPlot method.
   * @param file_arg is the path of file being written with the raw data.
   * @return void
   */
  void RawDataWriteDecodeEventOnPlotFile (const char *file_arg);

  /**
   * @brief The RawDataPlot method plots the decodified events stored in the file specified by file_arg.
   * @details The method calls RawDataWriteDecodeEventOnPlotFile method.
   * The code line "fflush(gnuplot)" is fundamental in order to send data to the gnuplot program in real-time.
   * @param file_arg is the path of the file being written with the raw data.
   * @param gnuplot is the pipe communicating with the gnuplot program.
   * @return void
   */
  void RawDataPlot (const char *file_arg, FILE * gnuplot);

  /**
   * @brief The RawDataDecode method decodes the events stored in the buffer attributes.
   * @return void
   */
  void RawDataDecode ();

  /**
   * @brief This overloading is necessary in order to perform a deep copy of the object. 
   * @details Now the assigment operator copies all the values of a RawData object, including the buffer attribute (not only the pointer of memory area).
   * @param p is the RawData being copied.
   * @return RawData &
   */
    RawData & operator= (const RawData & p);
    
    //~ static int NTPSocket;
    
    uint32_t data[12];

	uint32_t packet[12];
    
	struct timeval now;
	
	struct timeval tv;
};

#endif
