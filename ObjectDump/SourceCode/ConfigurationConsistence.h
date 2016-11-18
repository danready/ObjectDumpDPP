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
* @file ConfigurationConsistence.h 
*/

/**
* @brief The ConfigurationConsistence class provides useful methods for checking the consistence of a configuration file.
* @details This class require Analizzatore.h parser.
* @author Daniele Berto
*/

#ifndef CONFIGURATIONCONSISTENCE_H
#define CONFIGURATIONCONSISTENCE_H


class ConfigurationConsistence
{
public:

  /**
   * @brief The ConfigurationConsistenceConfFileInit checks if the OPEN attribute in the configuration file is well formed.
   * @details It prints recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int ConfigurationConsistenceConfFileInit (const char *conf_file_path);

  /**
   * @brief The ConfigurationConsistenceConfFileInit checks if the OPEN attribute in the configuration file is well formed.
   * @details It does not print recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int ConfigurationConsistenceConfFileInitNoPrint (const char
						   *conf_file_path);

  /**
   * @brief The ConfigurationConsistenceConfFileSetupEssentialWithInitCheck checks if the OPEN attribute in the configuration file is well formed and if the other 
   * essentials setup attribute are well formed too.
   * @details It prints recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int ConfigurationConsistenceConfFileSetupEssentialWithInitCheck (const char
								   *conf_file_path);

  /**
   * @brief The ConfigurationConsistenceConfFileSetupEssentialWithInitCheck checks if the other 
   * essentials setup attribute are well formed too.
   * @details It does not print recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int
    ConfigurationConsistenceConfFileSetupEssentialWithInitCheckNoPrint (const
									char
									*conf_file_path);

  /**
   * @brief The ConfigurationConsistenceConfFileSetupEssentialWithInitCheck checks if the other 
   * essentials setup attribute are well formed.
   * @details It prints recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int ConfigurationConsistenceConfFileSetupEssentialWithoutInitCheck (const
								      char
								      *conf_file_path);

  /**
   * @brief The ConfigurationConsistenceConfFileSetupEssentialWithInitCheck checks if the other 
   * essentials setup attribute are well formed.
   * @details It does not print recognized line.
   * @param conf_file_path is the path of the configuration file
   * @return int 0 if no errors are detected. 
   */
  int
    ConfigurationConsistenceConfFileSetupEssentialWithoutInitCheckNoPrint
    (const char *conf_file_path);
};

#endif
