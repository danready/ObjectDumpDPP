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
* @file ClientApplication.c
*
* @brief This file contains the client_application
*
**/

/*
Questo sorgente, se compilato con il make file contenuto nella cartella principale del progetto, produce il programma objectdumpclient, il lato client del programma
objectdump.
Objectdumpclient consente di inviare ad objectdump comandi in input via TCP/IP. La lista dei codici che il client puo' inviare e' contenuta nel file DefineCommands.h.
Ricordo che affinche' il server riconosca i comandi inviati via TCP/IP, i dati inviati devono essere un array di caratteri contenente nelle prime tre posizioni i numeri
25 21 17 e poi il codice del comando da inviare.
L'output mandato dal server e' ascoltato da un thread separato che esegue la funzione "ricevitore_function".
Per lanciare il programma occorre aggiungere l'opzione -i serveraddress dopo il nome dell'eseguibile.
*/

#include "DefineClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

//Variabili globali contenenti la socket di comunicazione col server e i dati da inviare e ricevere.
int sockfd, n;
int comando_da_inviare;
char inputline[STANDARDBUFFERLIMIT];
char sendline[STANDARDBUFFERLIMIT];
char receiveline[STANDARDBUFFERLIMIT];
int go = 0;
int no_new_line = 0;

//Funzione che restituisce un puntatore alla prima parola contenuta in yytext
char *
FindPointer (char *yytext)
{
	printf("Lunghezza: %d \n ", (int)strlen(yytext));
	int i = 0;
	char *punt;
	
	while (yytext[i] != ' ' && yytext[i] != '\t' && i<strlen(yytext)-1)
		i++;
	
	while ((yytext[i] == ' ' || yytext[i] == '\t') && i<strlen(yytext)-1)
		i++;
	
	punt = yytext + i;
	return punt;
}

int
FindIntegerValue (char *yytext)
{
	char *punt;
	int integer_value;
	punt = FindPointer (yytext);
	integer_value = atoi (punt);
	return integer_value;
}


//Funzione eseguita dal thread che riceve l'output dal server
void * ricevitore_function (void * nothing)
{
	int i;
	while (go)
	{
		bzero(receiveline, STANDARDBUFFERLIMIT);

		if (recv (sockfd, receiveline, STANDARDBUFFERLIMIT, 0) <= 0)
		{
			fprintf (stdout, "Comunicazione col server interrotta\n");
			shutdown (sockfd, 2);
			go = 0;
		}
		else 
		{
			//If necessario per stampare correttamente a video l'output del server.
			if (receiveline[strlen(receiveline)-1] == 10)
				fprintf(stdout, "%s", receiveline);
			else if(receiveline[strlen(receiveline)-1] != 10)
			{
				fprintf(stdout, "%s", receiveline);
				if (strlen(receiveline) > 1 && no_new_line == 0) 
					fprintf (stdout, "\n");
			} // else if(receiveline[strlen(receiveline)-1] != 10)
		} //} else {
	} //while (go)
} //void * ricevitore_function (void * nothing)

///Confronta la stringa puntata da str con il pattern indicato da pattern. Restituisci true se e' possibile trovare pattern in str, false al contrario.
bool reg_matches(const char *str, const char *pattern)
{
	regex_t re;
	int ret;

	if (regcomp(&re, pattern, REG_EXTENDED) != 0)
		return false;

	ret = regexec(&re, str, (size_t) 0, NULL, 0);
	regfree(&re);

	if (ret == 0)
		return true;

	return false;
}

//Stampa i comandi disponibile. La funzione e' chiamata quando l'utente digita il comando help.
void Help()
{
	printf ("Available command list:\n");
	printf ("init: open the digitizer\n");
	printf ("setup: setup the digitizer\n");
	printf ("start: start the data acquisition\n");
	printf ("stop: stop the data acquisition\n");
	printf ("prestart: start the preprocessing thread\n");
	printf ("prestop: stop the preprocessing thread\n");
	printf ("vistart [channelnumber]: start the visualization thread\n");
	printf ("vistop: stop the visualization thread\n");
	printf ("rawstart: start the raw data writing thread\n");
	printf ("rawstop: stop the raw data writing thread\n");
	printf ("close: close the digitizer\n");
	printf ("send: send a software trigger\n");
	printf ("help\n");
	printf ("check: check the correctness of the configuration file\n");
	printf ("chkconf: print the content of the configuration file\n");
	printf ("write register 0x[register] 0x[data]\n");
	printf ("read register 0x[register]\n");
	printf ("-f [conf file path]: change the configuration file path\n");
	printf ("-d [data file path]: change the data file path\n");
	printf ("-l [log file path]: change the log file path\n");
	printf ("print: print the internal configuration object used to configure the digitizer\n");
	printf ("print files: print the path of the configuration file, of the data file and of the log file\n");
	printf ("status: print the status of the threads acquisition, preprocessing, raw data and visualization\n");
	printf ("more: display the content of the logfile\n");
	printf ("exit: quit program\n");
	printf ("quit: quit program\n");
}


  /**
  * @brief The command_parser function returns a number which correspond to the command inserted by the user in accordance with the our conventions. See DefineCommands.h
  * file for informations about the codes of the available commands.
  * @param inputline contains the command being interpreted
  * @return int
  **/
int
command_parser (char *inputline , char *sendline)
{ 
	char * my_punt;
	const char *my_punt_const;
	no_new_line = 0;

	if (reg_matches(inputline, "^[iI][Nn][Ii][Tt][ \t]*$"))
	{
		//printf ("1\n");
		return INIT;
	}
	else if (reg_matches (inputline, "^[Ss][Ee][Tt][Uu][Pp][ \t]*$"))
	{
		//printf ("2\n");
		return SETUP;
	}
	else if (reg_matches (inputline, "^[pP][rR][eE][sS][tT][aA][rR][tT][ \t]*$"))
	{
		//printf ("3\n");
		return PRESTART;
	}
	else if (reg_matches (inputline, "^[pP][Rr][Ee][Ss][Tt][Oo][Pp][ \t]*$"))
	{
		//printf ("4\n");
		return PRESTOP;
	}
	else if (reg_matches (inputline, "^[sS][tT][aA][rR][tT][ \t]*$"))
	{
		//printf ("7\n");
		return START;
	}
	else if (reg_matches (inputline, "^[sS][tT][oO][pP][ \t]*$"))
	{
		//printf ("8\n");
		return STOP;
	}
	else if (reg_matches (inputline, "^[Ss][Ee][Nn][Dd][ \t]*$"))
	{
		//printf ("9\n");
		return SEND;
	}
	else if (reg_matches (inputline, "^[cC][lL][oO][sS][eE][ \t]*$"))
	{
		//printf ("10\n");
		return CLOSE;
	}
	else if (reg_matches (inputline, "^[qQ][uU][iI][tT][ \t]*$"))
	{
		//printf ("11\n");
		return QUIT;
	}
	else if (reg_matches (inputline, "^[rR][aA][wW][sS][tT][aA][rR][tT]([ \t]+[0-9]{1,8}){0,1}[ \t]*$"))
	{
		if (FindIntegerValue(inputline) != 0)
		{
			my_punt = FindPointer (inputline);
			strcpy(sendline + 4, my_punt);
		}
		//printf ("12\n");
		return RAWSTART;
	}
	else if (reg_matches (inputline, "^[Rr][Aa][Ww][Ss][Tt][Oo][Pp][ \t]*$"))
	{
		//printf ("13\n");
		return RAWSTOP;
	}
	else if (reg_matches (inputline, "^[pP][rR][iI][nN][tT][ \t]*$"))
	{
		//printf ("14\n");
		return PRINT;
	}
	else if (reg_matches (inputline, "^[Cc][Hh][Ee][Cc][Kk][ \t]*$"))
	{
		//printf ("16\n");
		return CHECK;
	}
	else if (reg_matches (inputline, "^[Cc][Hh][Kk][Cc][Oo][Nn][Ff][ \t]*$"))
	{
		//printf ("17\n");
		return CHKCONF;
	}
	else if (reg_matches (inputline, "^[Mm][Oo][Rr][Ee][ \t]*$"))
	{
		no_new_line = 1;
		//printf ("18\n");
		return MORE;
	}
	else if (reg_matches (inputline, "^[Ss][Tt][Aa][Tt][Uu][Ss][ \t]*$"))
	{
		//printf ("17\n");
		return STATUS;
	}
	else if (reg_matches (inputline, "^[-][Ff][ ].+$"))
	{
		//printf ("18\n");
		return CHANGECONF;
	}
	else if (reg_matches (inputline, "^[-][Dd][ ].+$"))
	{
		//printf ("18\n");
		return CHANGEDATA;
	}
	else if (reg_matches (inputline, "^[-][Ll][ ].+$"))
	{
		//printf ("18\n");
		return CHANGELOG;
	}
	else if (reg_matches (inputline, "^[pP][rR][iI][nN][tT][ \t]+[Ff][Ii][Ll][Ee][Ss][ \t]*$"))
	{
		//printf ("18\n");
		return PRINTFILES;
	}
	else if (reg_matches (inputline, "^[Ww][Rr][Ii][Tt][Ee][ \t]+[Rr][Ee][Gg][Ii][Ss][Tt][Ee][Rr][ \t]+(0x[0-9a-fA-F]{1,16})[ \t]+(0x[0-9a-fA-F]{1,16})[ \t]*$"))	//5==print
	{
		my_punt = FindPointer (inputline);
		my_punt = FindPointer (my_punt);
		strcpy(sendline + 4, my_punt);
		return WRITEREGISTER;
	}
	else if (reg_matches (inputline, "^[Rr][Ee][Aa][Dd][ \t]+[Rr][Ee][Gg][Ii][Ss][Tt][Ee][Rr][ \t]+(0x[0-9a-fA-F]{1,4})[ \t]*$"))	//5==print
	{
		my_punt = FindPointer (inputline);
		my_punt = FindPointer (my_punt);
		strcpy(sendline + 4, my_punt);
		return READREGISTER;
	}
	else if (reg_matches (inputline, "^[hH][eE][lL][pP][ \t]*$"))
	{ 
		Help();
		return -1;
	}
	else if (reg_matches (inputline, "^[Ee][Xx][Ii][Tt]$"))
	{
		fprintf(stdout, "Exiting...\n");
		return -1;
	}
	else
	{
		fprintf (stderr, "Unrecognized command\n");
		return -1;
	}
}

  /**
  * @brief The main function of the client application.
  * @param argc is the number of the main arguments
  * @param argv are the main arguments
  * @return int
  **/
int
main (int argc, char **argv)
{ 

	//Variabili per ottenere l'indirizzo del server dagli argomenti del main.
	const char *server_address;
	server_address == NULL;

	int c = 0;

	int flag_arg = 0;

	opterr = 0;

	while ((c = getopt (argc, argv, "i:")) != -1)
		switch (c)
		{
			case 'i':

				server_address = (char *)malloc(strlen(optarg) + 1);
				strcpy((char *)server_address, optarg);
				flag_arg = 1;
				break;

			case '?':

				if (optopt == 'i')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
				fprintf (stderr,
				   "Unknown option character `\\x%x'.\n",
				   optopt);
				break;
		}

	if (flag_arg == 0)
	{
		fprintf(stderr, "You have not insert server address: use -i flag\n");
		fprintf(stderr, "usage: [executablepath] -i [serveraddress]\n");
		return 1;	
	}

	//Inserisco queste informazioni dentro sendline in modo tale che il server riconosca i dati inviategli.
	sendline[0] = 25;
	sendline[1] = 21;
	sendline[2] = 17;
	sendline[4] = '\0';

	const char * my_punt;

	//thread id del thread che ascolta i messaggi provenienti dal server
	pthread_t ricevitore;

	//Codice necessario all'apertura di una socket con il client.
	struct sockaddr_in servaddr, cliaddr;
	char recvline[STANDARDBUFFERLIMIT];
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr (server_address);
	servaddr.sin_port = htons (SERVERPORT);

	fprintf(stdout, "Welcome to objectDump tcp service, press help for getting the available command list\n");

	connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
	perror(" ");

	//Creo il thread ricevitore, controllato dalla variabile go.
	go = 1;
	pthread_create(&ricevitore, NULL, ricevitore_function, NULL);

	//Ciclo di fetching dell'input da tastiera il ciclo termina quando l'utente inserisce il comando exit.
	do
	{ 
		fflush(stdout);
		bzero(inputline, STANDARDBUFFERLIMIT);
		//Prelevo un input da tastiera
		fgets (inputline, STANDARDBUFFERLIMIT, stdin);
		
		if (inputline[strlen (inputline) - 1] == '\n')
			inputline[strlen (inputline) - 1] = '\0';
		
		//Metto in sendline il codice corrispondente al comando inserito dall'utente.
		//Se il comando non e' riconosciuto, viene restituito -1 e non e' inviato nulla al server.
		sendline[3] = (char) command_parser (inputline, sendline);

		//Se il client invia un comando per modificare il path dei files usati dal server, e' necessario ricavare dalla stringa in input il nuovo path
		//e inviarlo al server.
		if (sendline[3] == CHANGECONF || sendline[3] == CHANGEDATA || sendline[3] == CHANGELOG)
		{
			my_punt = inputline + 3;
			strcpy(sendline + 4, my_punt);	
		}

		//Se il comando inserito dall'utente non e' stato riconosciuto, al server non e' inviato nella.
		if (sendline[3] != -1)
		{
			send (sockfd, sendline, STANDARDBUFFERLIMIT, 0);
		}
	}
	//Il ciclo termina quando l'utente inserisce il comando exit.
	while (reg_matches (inputline, "^[Ee][Xx][Ii][Tt][ \t]*$") == false);

	//Il programma deve terminare, quindi interrompo anche il thread ricevitore
	go = 0;

	//Chiudo la socket di comunicazione col server.
	shutdown (sockfd, 2);
} //int main (int argc, char **argv)


