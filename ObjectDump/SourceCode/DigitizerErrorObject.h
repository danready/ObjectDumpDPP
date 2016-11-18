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
* @file DigitizerErrorObject.h 
*/

#ifndef DIGITIZERERROROBJECT_H
#define DIGITIZERERROROBJECT_H

#include <stdio.h>
#include <CAENDigitizer.h>

/**
* @brief The DigitizerErrorObject class contains two methods that print the meaning of the CAEN_DGTZ_ErrorCode.
* @author Daniele Berto
*/

class DigitizerErrorObject
{
public:

/**
* @brief The DigitizerErrorObjectDebugging method prints the meaning of the CAEN_DGTZ_ErrorCode. It is called by other methods using __FILE__ preprocessing variable as second 
* paramater, __func__ preprocessing variable as third parameter and __LINE__ preprocessing variable as fourth parameter.
* @param ritorno is the CAEN_DGTZ_ErrorCode being interpreted.
* @param file is the file which call the method: it should be the __FILE__  preprocessing variable.
* @param func is the function where the method is called: it should be the __func__ preprocessing variable.
* @param line is the line where the method is called: it should be the __LINE__  preprocessing variable.
* @return void=
*/
  void DigitizerErrorObjectDebugging (CAEN_DGTZ_ErrorCode ritorno,
				      const char *file, const char *func,
				      int line);
/**
* @brief The DigitizerErrorObjectPrintError method prints the meaning of the CAEN_DGTZ_ErrorCode. 
* @param ritorno is the CAEN_DGTZ_ErrorCode being interpreted.
* @return void
*/
  void DigitizerErrorObjectPrintError (CAEN_DGTZ_ErrorCode ritorno);


/**
* @brief The DigitizerErrorObjectDebuggingLog method prints the meaning of the CAEN_DGTZ_ErrorCode in the logfile. 
* @param ritorno is the CAEN_DGTZ_ErrorCode being interpreted.
* @param file is the path of the file in which the functions that generated the error code where located.
* @param func is the name of the function in which the functions that generated the error code where located.
* @param line is the line number of the functions that generated the error code.
* @param log_file is the file pointer being written with the interpretation of the CAEN_DGTZ_ErrorCode.
* @return void
*/
  int DigitizerErrorObjectDebuggingLog (CAEN_DGTZ_ErrorCode ritorno,
					const char *file, const char *func,
					int line, FILE * log_file);
};

#endif
