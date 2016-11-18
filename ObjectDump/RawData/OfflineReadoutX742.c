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
*@brief Il seguente programma consente di decodificare un insieme di dati non decodificati acquisiti da un digitizer CAEN della famiglia X742 con il programma objectDump.
*Il programma deve essere lanciato con la stringa: exexutablepath -r rawdatapath -s rawdatasize -o decodeeventpath. Ogni opzione e' necessaria.
*/

#include "X742CorrectionRoutines.h"

#include "x742.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "CAENDigitizer.h"

#include <iostream>
#include <sstream>
#include <TCanvas.h>
#include <TApplication.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <cstdlib>
#include <TTree.h>
#include <TFile.h>


int main (int argc, char *argv[])
{

	//Variabili di supporto per analizzare gli argomenti con cui e' lanciato il programma
	int c = 0;
	opterr = 0;
	//  char stringa[1000];
	//  bzero(stringa, 1000);

	//Puntatori ai paths dei files con cui e' lanciato il programma
	const char *raw_data_path;
	const char *decode_event_path;

	//Numero di argomenti con dui e' lanciato il programma
	int arguments = 0;

	CAEN_DGTZ_ErrorCode ret;

	//Puntatori ai paths dei files con cui e' lanciato il programma
	FILE *datirozzi;
	FILE *eventodecodificato;

	//Variabili per decodificare i dati contenuti nel file indicato dopo il flag -f
	CAEN_DGTZ_X742_EVENT_t *Evt742= NULL;
	CAEN_DGTZ_EventInfo_t eventInfo;
	char *myevtptr;
	uint32_t MyNumEvents;
	uint32_t buffersize;
	char *buffer;
	char *header;
	int dato_letto;
	int i, j, k, o, l;
	uint32_t numEvents;
	int conta = 0;
	char *evtptr;
	char ch;
	unsigned int TimeTag;
	int max_channels = 0;
	int max_groups = 0;
	uint32_t evtSize;

	//Questa variabile contiene la lunghezza del file che ricopre il ruolo che prima aveva buffersize.
	long file_size = 0;

	//Ricavo informazioni dagli argomenti del main
	while ((c = getopt (argc, argv, "r:o:")) != -1)
		switch (c)
		{
			case 'r':
				raw_data_path = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)raw_data_path, optarg);
				arguments++;
				break;
			case 'o':
				decode_event_path = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)decode_event_path, optarg);
				arguments++;
				break;
			case '?':
				if (optopt == 'r' || optopt == 'o')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
				break;
		}

	//Tutte le opzioni sono necessarie: se gli argomenti del main non sono due il programma fallisce.
	if (arguments != 2) 
	{
		fprintf(stderr, "Usage: exexutablepath -r rawdatapath -o decodeeventpath\n");
		fprintf(stderr, "Example: ./decode742 -r data.txt -o events.txt\n");		      
		return 1;
	}


	//Apro il file contenente i dati da decodificare e quello che conterra' i risultati della decodifica.
	datirozzi=fopen(raw_data_path,"r");
	eventodecodificato=fopen(decode_event_path,"w");
	if (datirozzi == NULL || eventodecodificato == NULL) 
	{
		fprintf(stderr, "I can't open specified files\n");
		exit(1);
	}

	TFile* f=new TFile("testfile.root","recreate");
	TTree* t=new TTree("test","test");
	double data[34][1024];

	t->Branch("data",data,"data[34][1024]/D ");

	CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];

	//carico le tabelle di calibrazione 
	char stringa[1000];

	for (i=0; i<MAX_X742_GROUP_SIZE; i++)
	{
		bzero(stringa, 1000);
		sprintf(stringa, "Corrections_gr%d", i);
		LoadCorrectionTable(stringa, &(X742Tables[i]));
	}

	////////////////////////////
	header=(char *)malloc(4); 
	////////////////////////////

	//Per ogni burst di dati
	while(!feof(datirozzi))
	{

		//Leggo i file da decodificare e li' posiziono in buffer.
		if(!fread(header,4,1,datirozzi))
			break;

		evtSize = *(long *)(header) & 0x0FFFFFFF;
		printf("evtsize %i\n",evtSize);
		buffersize = (evtSize *4);
		buffer=(char *)malloc(buffersize);
		fseek(datirozzi, -4, SEEK_CUR);
		
		if(!fread(buffer, buffersize, 1, datirozzi))
			break;

		X742_DecodeEvent (buffer, (void **)&Evt742);

		//Stampo alcune informazioni riguardanti l'evento decodificato.
		//			fprintf(eventodecodificato, "BoardID: %d\n", eventInfo.BoardId);
		fprintf(eventodecodificato, "\n\n\nEvento Numero: %d\n", conta);

		//Stampo sul file puntato da eventodecodificato i campioni presenti in ogni canale di ogni gruppo
		for (o=0; o<MAX_X742_GROUP_SIZE; o++)
		{
			if (Evt742->GrPresent[o] == 1)
			{

				fprintf(eventodecodificato, "\n\n################GROUP: %d################\n", o);
				fprintf(eventodecodificato, "Trigger Time Stamp: %u\n", Evt742->DataGroup[o].TriggerTimeTag);
				fprintf(eventodecodificato, "Frequency: %u\n", Evt742->DataGroup[o].Frequency);
				CAEN_DGTZ_DRS4Frequency_t freq;
									
				switch(Evt742->DataGroup[o].Frequency)
				{
					case 0:
						freq = CAEN_DGTZ_DRS4_5GHz;
						break;
					case 1:
						freq = CAEN_DGTZ_DRS4_2_5GHz;
						break;
					case 2:
						freq = CAEN_DGTZ_DRS4_1GHz;
						break;
				}
				
				ApplyDataCorrection(&(X742Tables[o]), freq, 7, &(Evt742->DataGroup[o]));
				for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
				{                           
					for (k = 0; k < Evt742->DataGroup[o].ChSize[j] ; k++)
					{
						
						if(k==0) 
							fprintf(eventodecodificato, "\n################CHANNEL: %d################\n", j);  
							      
						fprintf(eventodecodificato, "%d\n",
						(int) Evt742->DataGroup[o].DataChannel[j][k]);
						data[j+8*o][k]=Evt742->DataGroup[o].DataChannel[j][k];
						
					} //for (k = 0; k < Evt->DataGroup[i].ChSize[j] ; k++)
					///////Essendo DataChannel un puntatore, va deallocato manualmente, cioe'
					///////non basta solo il free(Evt742) alla fine.							
					free(Evt742->DataGroup[o].DataChannel[j]);
					///////
				} //for (j=0; j<MAX_X742_CHANNEL_SIZE; j++)
			} //if (Evt->GrPresent[i] == 1)
		} //for (i=0; i<MAX_X742_GROUP_SIZE; i++)


		free(Evt742);
		Evt742 = NULL;
		t->Fill();

		free(buffer);
		buffer = NULL;

	} // while (!feof(datirozzi))

	f->Write();
	f->Close();

	//Chiudo i files aperti
	fclose(eventodecodificato);
	fclose(datirozzi);

}
