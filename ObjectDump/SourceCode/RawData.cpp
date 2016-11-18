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
* @file RawData.cpp 
*/

#include "DefineGeneral.h"
#include "X742DecodeRoutines.h"
#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "Analizzatore.h"
#include "DigitizerObjectGeneric.h"
#include "RawData.h"
#include "DigitizerStateMachine.h"
#include "OutputModule.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CAENDigitizer.h>


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

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4
#define PREC -6
#define JAN_1970  0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

//~ int RawData::NTPSocket;

RawData & RawData::operator= (const RawData & p)
{
  int i;
  if (this != &p)
    {
      free (buffer);
      buffer = NULL;
      buffer = (char *) malloc (size);
      for (i = 0; i < p.bsize; i++)
		buffer[i] = p.buffer[i];
      imset = p.imset;
      handle = p.handle;
      bsize = p.bsize;
      size = p.size;
      ret = p.ret;
      eventInfo = p.eventInfo;
      ret_error = p.ret_error;
      Mode = p.Mode;
      file = p.file;
    }
  return *this;
}


RawData::RawData ()
{
  buffer = NULL;
  imset = 0;
}


void
RawData::RawDataSet (DigitizerObjectGeneric digitizer_arg)
{
  digitizer = digitizer_arg;
  
  handle = digitizer_arg.handle;
  ret = CAEN_DGTZ_MallocReadoutBuffer (handle, &buffer, (uint32_t *) & size);
  //ret_error.digitizer_error_object_print_error (ret);
}


void
RawData::RawDataDel ()
{
  CAEN_DGTZ_FreeReadoutBuffer (&buffer);
  //free (buffer);
  buffer = NULL;
  //ret_error.digitizer_error_object_print_error (ret);
}


void
RawData::RawDataWriteOnFile (const char *file_arg)
{
	
  if (bsize > 0)
    {
      FILE *file_size;
      char *file_size_path;
      file_size_path = (char *) malloc (strlen (file_arg) + 3);
      strcpy (file_size_path, file_arg);
      strcat (file_size_path, "sz");
      file = fopen (file_arg, "a");
      file_size = fopen (file_size_path, "a");
      if (file != NULL && file_size != NULL)
	  {
	    fprintf (file_size, "%d\n", bsize);
	    fwrite (buffer, bsize, 1, file);
	  
	    fclose (file);
	    fclose (file_size);
	  }   //if (file != NULL && file_size != NULL)
    }   //if (bsize>0)
}


void
RawData::RawDataWriteOnFile (FILE * file, FILE * file_size)
{
  if (file != NULL && file_size != NULL)
  {

//~ //////////////////////////////////////NTP!!!
	//~ memset(data, 0, sizeof(data));
	//~ memset(packet, 0, sizeof(packet));
	//~ data[0] = htonl (
		//~ ( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		//~ ( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
	//~ data[1] = htonl(1<<16);  // Root Delay (seconds) 
	//~ data[2] = htonl(1<<16);  // Root Dispersion (seconds) 
	//~ //gettimeofday(&now,NULL);
	//~ //data[10] = htonl(now.tv_sec + JAN_1970); // Transmit Timestamp coarse 
	//~ //data[11] = htonl(NTPFRAC(now.tv_usec));  // Transmit Timestamp fine
	//~ send(NTPSocket,data,48,0);
//~ 
//~ 
//~ 
//~ 
	//~ //syslog(LOG_DAEMON | LOG_DEBUG, "Received packet from server ...");
	  //~ recv(NTPSocket, packet, sizeof(packet), 0);	
	//~ 
	//~ tv.tv_sec  = ntohl(((uint32_t *)packet)[10]) - JAN_1970;
	//~ tv.tv_usec = USEC(ntohl(((uint32_t *)packet)[11]));
	//~ 
	//~ //fprintf(stderr,"Secondi: %u\n", (uint32_t)tv.tv_sec);
	//~ 
	//~ //fprintf(stderr,"Microsecondi: %u\n", (uint32_t)tv.tv_usec);
		//~ 
	  //~ fprintf (file_size,"%u\t", (uint32_t)tv.tv_sec);
	  //~ fprintf(file_size,"%u\t", (uint32_t)tv.tv_usec);
	  //~ 
	  //~ //fprintf(stderr,"sizeof packet: %d\n", sizeof(packet));
	  //~ //fwrite(packet, sizeof(packet), 1, file_size);
//~ //////////////////////////////////////NTP!!!



	  
    fprintf (file_size, "%d\n", bsize);
    fwrite (buffer, bsize, 1, file);
  }
}


void
RawData::RawDataWriteDecodeEventOnPlotFile (const char *file_arg)
{

	FILE *file_private_punt;
	file_private_punt = fopen (file_arg, "w");

	ApplicationSetup *application_setup;
	application_setup = ApplicationSetup::Instance ();

	if (file_private_punt != NULL)
	{
		CAEN_DGTZ_UINT8_EVENT_t *Evt8 = NULL;
		CAEN_DGTZ_UINT16_EVENT_t *Evt16 = NULL;
		CAEN_DGTZ_X742_EVENT_t *Evt742 = NULL;
		CAEN_DGTZ_X743_EVENT_t *Evt743= NULL;
		int max_channels = 0;
		int max_groups = 0;

		int FamilyCode = digitizer.BoardInfo.FamilyCode;

		int FormFactor = digitizer.BoardInfo.FormFactor;

		int i, j, k, o;
		uint32_t numEvents;
		int conta = 0;
		char *evtptr;

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
			case CAEN_DGTZ_XX743_FAMILY_CODE:
			switch(FormFactor) 
			{
				case CAEN_DGTZ_VME64_FORM_FACTOR:
				case CAEN_DGTZ_VME64X_FORM_FACTOR:
				max_channels = 16;
				max_groups = 8;
				break;
				case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
				case CAEN_DGTZ_NIM_FORM_FACTOR:
				max_channels = 8;
				max_groups = 4;
				break;
			}
			break;
			case CAEN_DGTZ_XX742_FAMILY_CODE:
			switch(FormFactor) 
			{
				case CAEN_DGTZ_VME64_FORM_FACTOR:
				case CAEN_DGTZ_VME64X_FORM_FACTOR:
				max_groups = 4;
				break;
				case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
				case CAEN_DGTZ_NIM_FORM_FACTOR:
				max_groups = 2;
				break;
			}
			break;
			//default:
			//assert("This program cannot be used with this digitizer family\n");
		}

		////////////////////////////////////Decoding event//////////////////////////////////
		ret = CAEN_DGTZ_GetNumEvents (handle, buffer, bsize, &numEvents);
		conta += numEvents;

		////////////////////Se c'e' almeno un evento//////////////////
		for (i = 0; (unsigned int) i < numEvents; i++)
		{
			ret = CAEN_DGTZ_GetEventInfo (handle, buffer, bsize, i, &eventInfo,
			&evtptr);

			///////////////////Decoding valido se la Board non e' della famiglia 742 ne' della famiglia 732///////////////
			if (FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE && FamilyCode != CAEN_DGTZ_XX743_FAMILY_CODE)
			{

				if (FamilyCode == CAEN_DGTZ_XX721_FAMILY_CODE || FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)
				{
					ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt8);
					if (application_setup->channel_visualized<max_channels)
					{
						for (j=0; j<Evt8->ChSize[application_setup->channel_visualized]; j++)
						{
							fprintf(file_private_punt, "%d\n",
							(int) Evt8->DataChannel[application_setup->channel_visualized][j]);
						}
					}
					CAEN_DGTZ_FreeEvent(handle, (void **) &Evt8);
				}
				else
				{
					ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt16);
					if (application_setup->channel_visualized<max_channels)
					{
						for (j=0; j<Evt16->ChSize[application_setup->channel_visualized]; j++)
						{
							fprintf(file_private_punt, "%d\n",
							(int) Evt16->DataChannel[application_setup->channel_visualized][j]);
						}
					}
					CAEN_DGTZ_FreeEvent(handle, (void **) &Evt16);
				}
			}

			///////////////////Se la Board e' della famiglia 742///////////////
			else if (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
			{
				CAEN_DGTZ_AllocateEvent(handle, (void **)&Evt742);
				CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt742);

				if ((application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE) < MAX_X742_GROUP_SIZE)
				{
					if (Evt742->GrPresent[application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE] == 1)
					{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "####GROUP: %d\n", o);
						//OUTPUT(stringa);
						//if (application_setup->channel_visualized % MAX_X742_CHANNEL_SIZE < MAX_X742_CHANNEL_SIZE)
						//{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "####CHANNEL: %d\n", j);
						//OUTPUT(stringa);                                           
						for (k = 0; k < Evt742->DataGroup[application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE].
						ChSize[application_setup->channel_visualized % MAX_X742_CHANNEL_SIZE] ; k++)
						{
							fprintf(file_private_punt, "%d\n",
							(int) Evt742->DataGroup[application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE].
							DataChannel[application_setup->channel_visualized % MAX_X742_CHANNEL_SIZE][k]);
						} //for (k = 0; k < Evt->DataGroup[i].ChSize[j] ; k++)
						//} //if (application_setup->channel_visualizedMAX_X742_CHANNEL_SIZE)
					} //if (Evt742->GrPresent[application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE] == 1)
				} //if ((application_setup->channel_visualized / MAX_X742_CHANNEL_SIZE) < MAX_X742_GROUP_SIZE)
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt742);
			} // END OF ELSE IF (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)

			///////////////////Se la Board e' della famiglia 743///////////////
			else if (FamilyCode == CAEN_DGTZ_XX743_FAMILY_CODE)
			{
				CAEN_DGTZ_AllocateEvent (handle, (void**)&Evt743);

				//Decodifico l'evento
				ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt743);

				if ((application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP) < max_groups)
				{
					if (Evt743->GrPresent[application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP] == 1)
					{
						for (k = 0; k < Evt743->DataGroup[application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP].ChSize ; k++)
						{
							fprintf(file_private_punt, "%d\n",
							(int) Evt743->DataGroup[application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP].
							DataChannel[application_setup->channel_visualized % MAX_X743_CHANNELS_X_GROUP][k]);
						} //for (k = 0; k < Evt->DataGroup[...].ChSize ; k++)
					} //if (Evt743->GrPresent[application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP] == 1)
				} //if ((application_setup->channel_visualized / MAX_X743_CHANNELS_X_GROUP) < max_groups)

				//Libero le risorse occupate da Evt743
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt743);

			}

			//fprintf(stderr, "Un segnale: %d\n", (int) Evt->DataChannel[0][0]);

		}	//for (i = 0; (unsigned int) i < numEvents; i++)

		fclose (file_private_punt);
	}   // if (file_private_punt != NULL)
}



void
RawData::RawDataPlot (const char *file_arg, FILE * gnuplot)
{

  uint32_t numEvents = 0;

  ret = CAEN_DGTZ_GetNumEvents (handle, buffer, bsize, &numEvents);

  if (numEvents > 0)
  {
      RawData::RawDataWriteDecodeEventOnPlotFile (file_arg);
      fprintf (gnuplot, "plot '%s' with line\n", file_arg);
      // This line is fundamental in order to send data to the gnuplot program in real-time.
      fflush (gnuplot);
      sleep (1);
  }

}



void
RawData::RawDataRead ()
{
  ret =
    CAEN_DGTZ_ReadData (handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
			buffer, (uint32_t *) & bsize);
  //ret_error.digitizer_error_object_print_error (ret);
}



void
RawData::RawDataDecode ()
{

	CAEN_DGTZ_UINT8_EVENT_t *Evt8 = NULL;
	CAEN_DGTZ_UINT16_EVENT_t *Evt16 = NULL;
	CAEN_DGTZ_X742_EVENT_t *Evt742 = NULL;
	CAEN_DGTZ_X743_EVENT_t *Evt743= NULL;

	char stringa[STANDARDBUFFERLIMIT];
	bzero (stringa, STANDARDBUFFERLIMIT);

	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	int max_channels = 0;
	int max_groups = 0;

	////////////////////////////Getting useful information////////////////////////////

	int FamilyCode = digitizer.BoardInfo.FamilyCode;

	int FormFactor = digitizer.BoardInfo.FormFactor;

	////////////////////////////Getting useful information////////////////////////////

	int i, j, k, o;
	uint32_t numEvents;
	int conta = 0;
	char *evtptr;

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
		case CAEN_DGTZ_XX743_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			max_channels = 16;
			max_groups = 8;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			max_channels = 8;
			max_groups = 4;
			break;
		}
		break;
		case CAEN_DGTZ_XX742_FAMILY_CODE:
		switch(FormFactor) 
		{
			case CAEN_DGTZ_VME64_FORM_FACTOR:
			case CAEN_DGTZ_VME64X_FORM_FACTOR:
			max_groups = 4;
			break;
			case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
			case CAEN_DGTZ_NIM_FORM_FACTOR:
			max_groups = 2;
			break;
		}
		break;
		//default:
		//assert("This program cannot be used with this digitizer family\n");
	}


	////////////////////////////////////Decoding event//////////////////////////////////
	ret = CAEN_DGTZ_GetNumEvents (handle, buffer, bsize, &numEvents);
	conta += numEvents;

	////////////////////Se c'e' almeno un evento//////////////////
	for (i = 0; (unsigned int) i < numEvents; i++)
	{
		ret = CAEN_DGTZ_GetEventInfo (handle, buffer, bsize, i, &eventInfo,
		&evtptr);

		///////////////////Decoding valido se la Board non e' della famiglia 742  ne' della famiglia XX743///////////////
		if (FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE && FamilyCode != CAEN_DGTZ_XX743_FAMILY_CODE)
		{

			if (FamilyCode == CAEN_DGTZ_XX721_FAMILY_CODE || FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)
			{
				ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt8);
				for (o=0; o<max_channels; o++)
				{
					for (j=0; j<Evt8->ChSize[o]; j++)
					{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "Valore del digitizer: %d\n",
						//(int) Evt8->DataChannel[o][j]);
						//output_module->Output(stringa);
					}
				}
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt8);
			}
			else
			{
				ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt16);
				for (o=0; o<max_channels; o++)
				{
					for (j=0; j<Evt16->ChSize[o]; j++)
					{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "Valore del digitizer: %d\n",
						//(int) Evt16->DataChannel[o][j]);
						//output_module->Output(stringa);
					}
				}
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt16);
			}
		}
		///////////////////Se la Board e' della famiglia 742///////////////
		else if (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
		{
			CAEN_DGTZ_AllocateEvent(handle, (void **)&Evt742);
			CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt742);
			for (o=0; o<max_groups; o++)
			{
				if (Evt742->GrPresent[o] == 1)
				{
					//bzero(stringa, STANDARDBUFFERLIMIT);
					//snprintf(stringa, STANDARDBUFFERLIMIT, "####GROUP: %d\n", o);
					//OUTPUT(stringa);
					for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
					{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "####CHANNEL: %d\n", j);
						//OUTPUT(stringa);                                           
						for (k = 0; k < Evt742->DataGroup[o].ChSize[j] ; k++)
						{

							//bzero(stringa, STANDARDBUFFERLIMIT);
							//snprintf(stringa, STANDARDBUFFERLIMIT, "Valore del digitizer: %d\n",
							//(int) Evt742->DataGroup[o].DataChannel[j][k]);
							//OUTPUT(stringa);
						} //for (k = 0; k < Evt->DataGroup[i].ChSize[j] ; k++)
					} //for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
				} //if (Evt->GrPresent[o] == 1)
			} //for (o=0; o<MAX_X742_GROUP_SIZE; o++)
			CAEN_DGTZ_FreeEvent(handle, (void **) &Evt742);
		}  // END OF ELSE IF (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
		//Se il digitizer e' della famiglia XX743
		else if (FamilyCode == CAEN_DGTZ_XX743_FAMILY_CODE)
		{
			CAEN_DGTZ_AllocateEvent (handle, (void**)&Evt743);

			//Decodifico l'evento
			ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt743);

			//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale di ogni gruppo
			for (o=0; o<max_groups; o++)
			{
				if (Evt743->GrPresent[o] == 1)
				{
					//bzero(stringa, STANDARDBUFFERLIMIT);
					//snprintf(stringa, STANDARDBUFFERLIMIT, "####GROUP: %d\n", o);
					//OUTPUT(stringa);
					for (j=0; j<MAX_X743_CHANNELS_X_GROUP; j++)
					{
						//bzero(stringa, STANDARDBUFFERLIMIT);
						//snprintf(stringa, STANDARDBUFFERLIMIT, "####CHANNEL: %d\n", j);
						//OUTPUT(stringa);                                     
						for (k = 0; k < Evt743->DataGroup[o].ChSize ; k++)
						{
							//bzero(stringa, STANDARDBUFFERLIMIT);
							//snprintf(stringa, STANDARDBUFFERLIMIT, "Valore del digitizer: %d\n",
							//(int) Evt743->DataGroup[o].DataChannel[j][k]);
							//OUTPUT(stringa);
						} //for (k = 0; k < Evt->DataGroup[i].ChSize ; k++)
					} //for (j=0; j<MAX_X743_CHANNEL_SIZE; j++)
				} //if (Evt->GrPresent[i] == 1)
			} //for (i=0; i<MAX_X743_GROUP_SIZE; i++)

			//Libero le risorse occupate da Evt743
			CAEN_DGTZ_FreeEvent(handle, (void **) &Evt743);

		}

		//fprintf(stderr, "Un segnale: %d\n", (int) Evt->DataChannel[0][0]);

	}	//for (i = 0; (unsigned int) i < numEvents; i++)
}
