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
* @file DefineGeneral.h
* @brief This file contains same parameters used by the program. In particular, it contains the dimension of the queues used by the aquisition threads.
* @author Daniele Berto
*/

#ifndef DEFINEGENERAL_H
#define DEFINEGENERAL_H

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1111

#define STANDARDBUFFERLIMIT 1000

///The dimension of the rawdata thread queue.
#define RAWDATAQUEUE 10

///The dimension of the visualization thread queue.
#define VISUALIZATIONQUEUE 10

///The dimension of the preprocessing thread queue.
#define PREPROCESSINGQUEUE 10

///The max number of channels managed by the program.
#define MAXCHANNELOBJECT 64

///The max number of groups managed by the program.
#define MAXGROUPOBJECT 8

///The max number of fast channels managed by the program.
#define MAXFASTOBJECT 2

///The max number of channels managed by the program.
#define MAXREGISTERS 200

///The dimension of the command buffer queue (see CommunicationObject class).
#define MAXCOMMAND 3

//The output modality chosen by the user with the "-m" flag when he launches the program.
#define ALL 2
#define ONLYUSER 0
#define ONLYTCP 1

#endif
