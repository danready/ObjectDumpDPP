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
*@file OfflineReadout.cpp
*@brief Il seguente programma consente di decodificare un insieme di dati non decodificati acquisiti da un digitizer CAEN con il programma objectDump.
*Il programma deve essere lanciato con la stringa: exexutablepath -r rawdatapath -s rawdatasize -o decodeeventpath. Ogni opzione e' necessaria.
*A causa della politica di gestione delle librerie di decoding dell'azienda CAEN, per usare il programma decode e' necessario interfacciarsi col digitizer.
*Per far questo, occorre configurare correttamente il file "DigitizerConfig.conf": in esso e' presente l'attributo OPEN che funziona come quello del file di
*configurazione di objectDump.
*/

#include "OfflineReadout.h"
#include "x742.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <CAENDigitizer.h>

/* Le tre funzioni seguenti servono per ricavare le informazioni dall'attributo OPEN contenuto nel file "DigitizerConfig.conf".
*/

///Confronta la stringa puntata da str con il pattern indicato da pattern. Restituisci true se e' possibile trovare pattern in str, false al contrario.
bool
reg_matches (const char *str, const char *pattern)
{
	regex_t re;
	int ret;

	if (regcomp (&re, pattern, REG_EXTENDED) != 0)
		return false;

	ret = regexec (&re, str, (size_t) 0, NULL, 0);
	regfree (&re);

	if (ret == 0)
		return true;

	return false;
}

///Restituisce un puntatore alla seconda parola della stringa puntata da yytext
char *
FindPointer (char *yytext)
{
	int i = 0;
	char *punt;
	while (yytext[i] != ' ' && yytext[i] != '\t')
	i++;
	while (yytext[i] == ' ' || yytext[i] == '\t')
	i++;
	punt = yytext + i;
	return punt;
}

///Restituisce il valore del primo intero presente in yytext.
int
FindIntegerValue (char *yytext)
{
	char *punt;
	int integer_value;
	punt = FindPointer (yytext);
	integer_value = atoi (punt);
	return integer_value;
}


int main (int argc, char *argv[])
{

	//Puntatore al file "DigitizerConfig.conf".
	FILE * configuration_file_pointer;

	//Variabili di supporto per analizzare gli argomenti con cui e' lanciato il programma
	int c = 0;
	opterr = 0;
	char stringa[1000];
	bzero(stringa, 1000);

	//Puntatori ai paths dei files con cui e' lanciato il programma
	const char *raw_data_path;
	const char *size_raw_data_path;
	const char *decode_event_path;

	//Numero di argomenti con dui e' lanciato il programma
	int arguments = 0;

	//Variabili di supporto per analizzare il file "DigitizerConfig.conf"
	char *punt;
	CAEN_DGTZ_ConnectionType Type;
	int LinkNumber = -1;
	int NodeNumber = -1;
	int BaseAddress = -1;
	CAEN_DGTZ_ErrorCode ret;

	//Puntatori ai paths dei files con cui e' lanciato il programma
	FILE *datirozzi;
	FILE *sizedatirozzi;
	FILE *eventodecodificato;

	//Variabili per decodificare i dati contenuti nel file indicato dopo il flag -f
	CAEN_DGTZ_X742_EVENT_t *Evt742= NULL;
	CAEN_DGTZ_X743_EVENT_t *Evt743= NULL;
	CAEN_DGTZ_UINT8_EVENT_t *Evt8= NULL;
	CAEN_DGTZ_UINT16_EVENT_t *Evt16= NULL;
	CAEN_DGTZ_EventInfo_t myeventInfo;
	CAEN_DGTZ_BoardInfo_t BoardInfo;
	char *myevtptr;
	int handle;
	int number_of_lines;
	uint32_t MyNumEvents;
	int *buffersize;
	char *buffer;
	int dato_letto;
	int i, j, k, o, l;
	CAEN_DGTZ_EventInfo_t eventInfo;
	uint32_t numEvents;
	int conta = 0;
	char *evtptr;
	char ch;
	unsigned int TimeTag;
	int max_channels = 0;
	int max_groups = 0;

	//Ricavo informazioni dagli argomenti del main
	while ((c = getopt (argc, argv, "r:s:o:")) != -1)
		switch (c)
		{
			case 'r':
				raw_data_path = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)raw_data_path, optarg);
				arguments++;
				break;
			case 's':
				size_raw_data_path = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)size_raw_data_path, optarg);
				arguments++;
				break;
			case 'o':
				decode_event_path = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)decode_event_path, optarg);
				arguments++;
				break;
			case '?':
				if (optopt == 'r' || optopt == 's' || optopt == 'o')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
				break;
		}

	//Tutte le opzioni sono necessarie: se gli argomenti del main non sono tre il programma fallisce.
	if (arguments != 3) 
	{
		fprintf(stderr, "Usage: exexutablepath -r rawdatapath -s rawdatasize -o decodeeventpath\n");
		fprintf(stderr, "Example: ./decode -r data.txt -s data.txtsz -o events.txt\n");		      
		return 1;
	}

	//Apro il file di configurazione DigitizerConfig.conf
	configuration_file_pointer = fopen("DigitizerConfig.conf", "r");

	//Se non e' presente il file di configurazione il programma fallisce
	if (configuration_file_pointer == NULL) 
	{
		fprintf(stderr, "Error, I cannot open DigitizerConfig.conf\n");
		exit(1);
	}

	//Ricavo dal file "DigitizerConfig.conf" stringa contenente l'attributo OPEN.
	fgets(stringa, 1000, configuration_file_pointer);

	//fprintf(stderr, "Stringa: %s\n", stringa);

	//Cerco l'attributo OPEN dentro la stringa presa dal file "DigitizerConfig.conf". 
	//Se l'attributo viene trovato correttamente, ne ricavo le informazioni necessarie ad aprire il digitizer.
	//Se l'attributo OPEN non viene trovato, il programma fallisce.
	if (reg_matches (stringa, ".*[Oo][Pp][Ee][Nn][ \t](([Uu][Ss][Bb])|([Pp][Cc][Ii]))[ \t][0-9]{0,1}[ \t]+[0-9]{0,1}[ \t]+(0|0x[0-9a-fA-F]{8})[ \t]*.*$"))
	{
		punt = FindPointer (stringa);
		if (reg_matches (punt, "[Uu][Ss][Bb]"))
		{
			Type = CAEN_DGTZ_USB; //fprintf(stderr, "USB\n");
		}
		else if (reg_matches (punt, "[Pp][Cc][Ii]"))
		{
			Type = CAEN_DGTZ_OpticalLink; //fprintf(stderr, "PCI\n");
		}
		else
		{
			fprintf(stderr, "Error, you have not specified connetion type\n"); 
			exit(1);
		}
		LinkNumber = FindIntegerValue (punt);
		//fprintf(stderr, "LinkNumber: %d\n", LinkNumber);
		punt = FindPointer (punt);
		NodeNumber = FindIntegerValue (punt);
		//fprintf(stderr, "NodeNumber: %d\n", NodeNumber);
		punt = FindPointer (punt);
		punt = FindPointer (punt);
		BaseAddress = strtoul (punt, NULL, 16);
		//fprintf(stderr, "BaseAddress: %d\n", BaseAddress);
	}
	else 
	{
		fprintf(stderr, "I can't find digitizer configuration in digitizer_config.conf file \n");
		exit(1);
	}

	//Controllo che l'attributo OPEN contenga le informazioni necessarie ad aprire il digitizer.
	if (LinkNumber == -1)
	{
		fprintf(stderr, "Error, you have not specified LinkNumber\n");
		exit(1);
	}
	else if (NodeNumber == -1)
	{
		fprintf(stderr, "Error, you have not specified NodeNumber\n");
		exit(1);
	}
	else if (BaseAddress == -1)
	{
		fprintf(stderr, "Error, you have not specified BaseAddress\n");
		exit(1);
	}

	//Apro il digitizer con le informazioni ricavate, se non e' possibile aprire il digitizer il programma termina.
	ret = CAEN_DGTZ_OpenDigitizer (Type, LinkNumber, NodeNumber, BaseAddress, &handle);
	if (ret != CAEN_DGTZ_Success)  
	{
		fprintf(stderr, "Error: cannot open the digitizer\n");
		exit(1);
	}

	//Apro il file contenente i dati da decodificare, quello contenente le dimensioni dei dati da modificare e quello che conterra' i risultati della decodifica.
	datirozzi=fopen(raw_data_path,"r");
	sizedatirozzi=fopen(size_raw_data_path,"r");
	eventodecodificato=fopen(decode_event_path,"w");
	if (datirozzi == NULL || sizedatirozzi == NULL || eventodecodificato == NULL)
	{
		fprintf(stderr, "I can't open specified files\n");
		exit(1);
	}

	/*
	In questa sezione del programma ricavo dal digitizer le informazioni necessarie ad eseguirne una corretta decodifica.
	*/
	ret = CAEN_DGTZ_GetInfo (handle, &BoardInfo);
	if (ret != CAEN_DGTZ_Success)
	{
		fprintf(stderr, "Error: cannot get board info\n");
		exit(1);
	}

	int FamilyCode = BoardInfo.FamilyCode;
	int FormFactor = BoardInfo.FormFactor;

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

	// Conto il numero di righe contenute nel file contenente le dimensioni dei dati da decodificare:
	// tale numero corrisponde al numero di data burst che verranno decodificati. Quindi, il numero di burst 
	// da decodificare e' indicato da number_of_lines.
	do 
	{
		ch = fgetc(sizedatirozzi);
		if(ch == '\n')
		number_of_lines++;
	} while (ch != EOF);

	// Alloco l'array buffersize che conterra' le dimensioni dei dati da decodificare.
	buffersize=(int *)malloc(number_of_lines*sizeof(int));

	fseek(sizedatirozzi, 0, SEEK_SET);

	// Riempio buffersize con le dimensioni ricavate dal file contenente le dimensioni dei dati da decodificare
	for (i=0;i<number_of_lines;i++)
		fscanf(sizedatirozzi,"%d", &buffersize[i]);

	//Per ogni burst di dati
	for (l=0;l<number_of_lines;l++)
	{
		fprintf(stderr, "Decoding burst %d\n", l);
		fprintf(eventodecodificato, "\n\n################SERIE DI EVENTI NUMERO: %d################\n\n", l);

		//Leggo dal file contenente i dati da decodificare il primo burst di dati.
		buffer=(char *)malloc(buffersize[l]);
		fread(buffer, buffersize[l], 1, datirozzi);

		//Ricavo il numero di eventi presenti nel primo burst di dati.
		ret = CAEN_DGTZ_GetNumEvents (handle, buffer, buffersize[l], &numEvents);
		conta += numEvents;

		//Se c'e' almeno un evento, procedo con la decodifica
		for (i = 0; (unsigned int) i < numEvents; i++)
		{ 
			//Ricavo informazioni sull'evento.
			ret = CAEN_DGTZ_GetEventInfo (handle, buffer, buffersize[l], i, &eventInfo,
			&evtptr);

			//Se il digitizer non e' della famiglia XX742 ne' della famiglia XX743
			if (FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE && FamilyCode != CAEN_DGTZ_XX743_FAMILY_CODE)
			{
				//Distinguo il caso in cui il digitizer sia a 8 nbits (occorre farlo perche' l'evento deve essere 
				//decodificato con un tipo di dato diverso.
				if (FamilyCode == CAEN_DGTZ_XX721_FAMILY_CODE || FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)
				{
					//Decodifico l'evento
					ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt8);

					//Stampo alcune informazioni riguardanti l'evento decodificato.
					fprintf(eventodecodificato, "BoardID: %d\n", eventInfo.BoardId);
					fprintf(eventodecodificato, "Event Number: %d\n", eventInfo.EventCounter);
					fprintf(eventodecodificato, "Trigger Time Stamp: %u\n", eventInfo.TriggerTimeTag);

					//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale
					for (o=0; o<max_channels; o++)
					{
						fprintf(eventodecodificato, "\n\n################CHANNEL: %d################\n", o);
						for (j=0; j<Evt8->ChSize[o]; j++)
						{
							fprintf(eventodecodificato, "%d\n",
							(int) Evt8->DataChannel[o][j]);
						}
					}
					//Libero le risorse occupate da Evt8
					CAEN_DGTZ_FreeEvent(handle, (void **) &Evt8);
				}
				else
				{
					//Decodifico l'evento
					ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt16);

					//Stampo alcune informazioni riguardanti l'evento decodificato.
					fprintf(eventodecodificato, "BoardID: %d\n", eventInfo.BoardId);
					fprintf(eventodecodificato, "Event Number: %d\n", eventInfo.EventCounter);
					fprintf(eventodecodificato, "Trigger Time Stamp: %u\n", eventInfo.TriggerTimeTag);

					//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale
					for (o=0; o<max_channels; o++)
					{
						fprintf(eventodecodificato, "\n\n################CHANNEL: %d################\n", o);
						for (j=0; j<Evt16->ChSize[o]; j++)
						{
							fprintf(eventodecodificato, "%d\n",
							(int) Evt16->DataChannel[o][j]);
						}
					}

					//Libero le risorse occupate da Evt16
					CAEN_DGTZ_FreeEvent(handle, (void **) &Evt16);
				}
			}
			//Se il digitizer e' della famiglia XX742
			else if (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
			{
				//Decodifico l'evento
				X742_DecodeEvent (evtptr, (void **)&Evt742);

				//Stampo alcune informazioni riguardanti l'evento decodificato.
				fprintf(eventodecodificato, "BoardID: %d\n", eventInfo.BoardId);
				fprintf(eventodecodificato, "Event Number: %d\n", eventInfo.EventCounter);
				fprintf(eventodecodificato, "Trigger Time Stamp: %u\n", eventInfo.TriggerTimeTag);

				//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale di ogni gruppo
				for (o=0; o<max_groups; o++)
				{
					if (Evt742->GrPresent[o] == 1)
					{
						fprintf(eventodecodificato, "\n\n################GROUP: %d################\n\n", o);
						for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
						{
							fprintf(eventodecodificato, "\n\n################CHANNEL: %d################\n", j);                                    
							for (k = 0; k < Evt742->DataGroup[o].ChSize[j] ; k++)
							{
								fprintf(eventodecodificato, "%d\n",
								(int) Evt742->DataGroup[o].DataChannel[j][k]);
							} //for (k = 0; k < Evt->DataGroup[i].ChSize[j] ; k++)
						} //for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
					} //if (Evt->GrPresent[i] == 1)
				} //for (i=0; i<MAX_X742_GROUP_SIZE; i++)

				//Libero le risorse occupate da Evt742
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt742);
			} // END OF ELSE IF (FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)
			//Se il digitizer e' della famiglia XX743
			else if (FamilyCode == CAEN_DGTZ_XX743_FAMILY_CODE)
			{
				CAEN_DGTZ_AllocateEvent (handle, (void**)&Evt743);

				//Decodifico l'evento
				ret = CAEN_DGTZ_DecodeEvent (handle, evtptr, (void **)&Evt743);

				//Stampo alcune informazioni riguardanti l'evento decodificato.
				fprintf(eventodecodificato, "BoardID: %d\n", eventInfo.BoardId);
				fprintf(eventodecodificato, "Event Number: %d\n", eventInfo.EventCounter);
				fprintf(eventodecodificato, "Trigger Time Stamp: %u\n", eventInfo.TriggerTimeTag);

				//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale di ogni gruppo
				for (o=0; o<max_groups; o++)
				{
					if (Evt743->GrPresent[o] == 1)
					{
						fprintf(eventodecodificato, "\n\n################GROUP: %d################\n\n", o);
						for (j=0; j<MAX_X743_CHANNELS_X_GROUP; j++)
						{
							fprintf(eventodecodificato, "\n\n################CHANNEL: %d################\n", j);                                    
							for (k = 0; k < Evt743->DataGroup[o].ChSize ; k++)
							{
								fprintf(eventodecodificato, "%d\n",
								(int) Evt743->DataGroup[o].DataChannel[j][k]);
							} //for (k = 0; k < Evt->DataGroup[i].ChSize ; k++)
						} //for (j=0; j<X743_CHANNELS_X_GROUP; j++)
					} //if (Evt->GrPresent[i] == 1)
				} //for (i=0; i<max_groups; i++)

				//Libero le risorse occupate da Evt743
				CAEN_DGTZ_FreeEvent(handle, (void **) &Evt743);

			}

		}	//for (i = 0; (unsigned int) i < numEvents; i++)

		free(buffer);
		buffer = NULL;
	} // for (l=0;l<number_of_lines;l++)

	//Stampo il numero di burst di dati decodificati
	printf("number of burst decoded: %d\n", number_of_lines);

	//Chiudo i files aperti
	fclose(eventodecodificato);
	fclose(datirozzi);
	fclose(sizedatirozzi);

	//Chiudo il digitizer
	ret = CAEN_DGTZ_CloseDigitizer(handle);
}
