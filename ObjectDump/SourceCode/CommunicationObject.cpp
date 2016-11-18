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
* @file CommunicationObject.cpp
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include "DefineCommands.h"
#include "TcpUser.h"
#include "ConfObject.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DigitizerObject.h"
#include "DigitizerObjectGeneric.h"
#include "RawData.h"
#include "DigitizerStateMachine.h"
#include "ApplicationSetup.h"
#include "Input.h"
#include "CommunicationObject.h"
#include "DigitizerFlowControl.h"
#include "ConfigurationConsistence.h"
#include "AnalizzatoreUtils.h"
#include "Analizzatore.h"
#include "OutputModule.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;

void
CommunicationObject::Main ()
{

	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	int client_sock;  /*Socket descriptor del client. */
	int c;  /*Lunghezza di sockaddr_in. */
	struct sockaddr_in server, client;
	int socket_desc;
	socket_desc = socket (AF_INET, SOCK_STREAM, 0);

	/*Inizializzo la struttura sockaddr_in */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(SERVERIP);
	server.sin_port = htons (SERVERPORT);

	int flag1 = 1;
	int result1 = setsockopt(client_sock,   /* socket affected */
		IPPROTO_TCP,     /* set option at TCP level */
		TCP_NODELAY,     /* name of option */
		(char *) &flag1,  /* the cast is historical cruft */
		sizeof(int));    /* length of option value */

	/*Effettuo il bind */
	bind (socket_desc, (struct sockaddr *) &server, sizeof (server));

	while (go)
	{

		listen (socket_desc, 1);

		c = sizeof (struct sockaddr_in);

		if (go == 0)
		{
			break;
		}

		client_sock =
		accept (socket_desc, (struct sockaddr *) &client, (socklen_t *) & c);

		int flag = 1;
		int result = setsockopt(client_sock,  /* socket affected */
			IPPROTO_TCP,     /* set option at TCP level */
			TCP_NODELAY,     /* name of option */
			(char *) &flag,  /* the cast is historical cruft */
			sizeof(int));    /* length of option value */

		if (go == 0)
		{
			break;
		}

		if (client_sock >= 0)
		{
			output_module->OutputModuleStdoutOff();
			output_module->OutputModuleSockidOn(client_sock);
			output_module->TcpUserArrayInsert (client_sock);
			output_module->Output("Comunicazione col server stabilita\n");

			worker_thread = new thread (&CommunicationObject::Worker, this,
			(void *) &client_sock);

			worker_thread->detach();
		}

	}				// while go

}


void 
CommunicationObject::Worker (void *socket_desc)
{
	OutputModule *output_module;
	output_module = OutputModule::Instance ();

	int j;
	int sock = *(int *) socket_desc;
	int read_size;
	char buffer[STANDARDBUFFERLIMIT];
	const char *my_punt;
	bzero (buffer, STANDARDBUFFERLIMIT);

	while (go)
	{
		bzero (buffer, STANDARDBUFFERLIMIT);
		if ((read_size = recv (sock, buffer, STANDARDBUFFERLIMIT, MSG_NOSIGNAL)) <= 0)
		{
			//fprintf (stderr, "Comunicazione con il client %d interrotta\nobjectDump>",
			//   sock);
			output_module->TcpUserArrayDelete (sock);
			return;
		}

		unique_lock<mutex> ReservedKeyBoardInputAreaHandle(ReservedKeyBoardInputArea);

		if (num_mex == MAXCOMMAND)
		BlockedProducerInput.wait(ReservedKeyBoardInputAreaHandle);
		coda = (coda + 1) % MAXCOMMAND;
		command[coda].command_sent_by_user = buffer[3];

		if (buffer[3] == CHANGECONF || buffer[3] == CHANGEDATA
		|| buffer[3] == CHANGELOG || buffer[3] == WRITEREGISTER
		|| buffer[3] == READREGISTER || buffer[3] == RAWSTART)
		{
			bzero (command[coda].first_parameter, STANDARDBUFFERLIMIT);
			my_punt = buffer + 4;
			strncpy (command[coda].first_parameter, my_punt, STANDARDBUFFERLIMIT - 1);
			//~ fprintf (stderr, "command[coda].first_parameter: %s\n",
			//~ command[coda].first_parameter);
		}

		command[coda].user_sockid = sock;
		num_mex++;
		pthread_mutex_lock (&DigitizerFlowControl::input_flow_mutex);
		pthread_cond_signal (&DigitizerFlowControl::input_flow_cond);
		pthread_mutex_unlock (&DigitizerFlowControl::input_flow_mutex);

		BlockedConsumerInput.notify_one();

	}				// while go

	BlockedConsumerInput.notify_one();
}


void
CommunicationObject::CommunicationObjectInit ()
{
	go = 1;
	main_thread = new thread (&CommunicationObject::Main, this);
}


TcpUser CommunicationObject::GetCommand ()
{
	TcpUser tmp;
	tmp.command_sent_by_user = 0;
	tmp.user_sockid = -1;
	unique_lock<mutex> ReservedKeyBoardInputAreaHandle(ReservedKeyBoardInputArea);

	if (num_mex == 0)
	{
		tmp.command_sent_by_user = 0;
		tmp.user_sockid = -1;
	}
	else
	{
		tmp = command[testa];

		command[testa].command_sent_by_user = 0;
		command[testa].user_sockid = -1;
		bzero (command[testa].first_parameter, STANDARDBUFFERLIMIT);

		testa = (testa + 1) % MAXCOMMAND;

		num_mex--;
		BlockedProducerInput.notify_one();
	}
	return tmp;
}


void
CommunicationObject::Finish ()
{
	int i;

	go = 0;

	for (i = 0; i < 10; i++)
		BlockedProducerInput.notify_one();
}


CommunicationObject::CommunicationObject ()
{
  num_mex = 0;
  coda = 0;
  testa = 1;
}
