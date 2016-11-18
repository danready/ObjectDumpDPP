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
* @file DefineCommands.h
* @brief This file contains the codes of the commands used to send command via TCP/IP.
* @author Daniele Berto
*/

#ifndef DEFINECLIENT_H
#define DEFINECLIENT_H

///The dimension of the buffer being sent to the server.
#define STANDARDBUFFERLIMIT 1000

#define SERVERPORT 1111

#define INIT 1
#define SETUP 2
#define PRESTART 3
#define PRESTOP 4
#define VISTART 5
#define VISTOP 6
#define START 7
#define STOP 8
#define SEND 9
#define CLOSE 10
#define QUIT 11
#define RAWSTART 12
#define RAWSTOP 13
#define PRINT 14
#define CHECK 16
#define CHKCONF 17
#define MORE 18
#define CHANGECONF 21
#define CHANGEDATA 22
#define CHANGELOG 23
#define WRITEREGISTER 24
#define READREGISTER 25
#define PRINTFILES 28
#define STATUS 30

#endif
