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
* @file TcpUser.h 
*/

/**
* @brief The TcpUser class provides an useful way to store data about the users of the server.
*
* @author Daniele Berto
*/

#ifndef TCPUSER_H
#define TCPUSER_H

#include "DefineGeneral.h"

class TcpUser
{
public:

  /**
   * @brief The command_sent_by_user variable contains the informations about the command sent by the user.
   * See "DefineGeneral.h" for other informations about the available commands.
   */
  char command_sent_by_user;

  /**
   * @brief Some commands need more than one parameter to be executed (ex. write register address data).
   * The first_parameter array stores the first parameter of a composite command.
   */
  char first_parameter[STANDARDBUFFERLIMIT];

  /**
   * @brief The second_parameter array stores the second parameter of a composite command.
   */
  char second_parameter[STANDARDBUFFERLIMIT];

  /**
   * @brief The register_address variable stores the address of the register being written or being read.
   */
  int register_address;

  /**
   * @brief The register_data variable stores the data being written in the register indicated by register_address.
   */
  int register_data;

  /**
   * @brief The user_sockid variable contains the sockid of the user who sent the command.
   */
  int user_sockid;

  /**
   * @brief The TcpUser constructor sets to 0 the attribute "command_sent_by_user", to -1 "register_address" and "register_data"
   * and to 0 first_parameter and second_parameter.
   */
  TcpUser ();
};

#endif
