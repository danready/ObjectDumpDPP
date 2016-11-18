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
* @file AnalizzatoreUtils.h
* @brief This file contains the declaration of the utility functions used by the scanner.
* @details These functions are called in the flex file. <br> They help the yylex() function to extract the information from the matched strings.
* @author Daniele Berto
*/

#ifndef ANALIZZATOREUTILS_H
#define ANALIZZATOREUTILS_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
* @brief The FindPointer function returns a pointer to the first useful character after the command.
* @details Ex: the string "POST_TRIGGER  20" is in yytext and we call punt = find_pointer(yytext). After that 
* punt contains a pointer to the character '2'.
* This function is used by many other functions of this file.
* @param yytext contains a matched string.
* @return char
*/
char *FindPointer (char *yytext);

/**
* @brief The FindIntegerValue function returns the value of the first integer after the command.
* @details Ex: the string "POST_TRIGGER  20" is in yytext and we call value = find_integer_value(yytext). After that 
* value contains contains the integer 20.
* @param yytext contains a matched string.
* @return int
*/
int FindIntegerValue (char *yytext);

/**
* @brief The OutputFileFormat function returns an integer that represents the format of the output file of the application.
* @details See ConfObject documentation for other informations.
* @param yytext contains a matched string.
* @return int
*/
int OutputFileFormat (char *yytext);

/**
* @brief The OutputRisingFalling function returns an integer that decides whether the trigger occurs on the rising or falling edge of the signal.
* @details See ConfObject documentation for other informations.
* @param yytext contains a matched string.
* @return int
*/
int OutputRisingFalling (char *yytext);

/**
* @brief The OutputNIMTTL function returns an integer that represents the type of the front panel I/O LEMO connectors. 
* @details See ConfObject documentation for other informations.
* @param yytext contains a matched string.
* @return int
*/
int OutputNIMTTL (char *yytext);

/**
* @brief The YesNoAnswer function returns an integer that represents yes or no. 
* @details Values: 0 = NO, 1 = YES, other=UNSET.
* @param yytext contains a matched string.
* @return int
*/
int YesNoAnswer (char *yytext);

/**
* @brief The GetAcquisitionMode function returns an integer that represents the acquisition mode.
* @details Values: 0 = ACQUISITION_ONLY, 1 = ACQUISITION_AND_TRGOUT, 2 = DISABLED, other=UNSET.
* @param yytext contains a matched string.
* @return int
*/
int GetAcquisitionMode (char *yytext);

/**
* @brief The GetOpenInformation function gets the information for opening the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "OPEN PCI 0 0 0x11110000", so we can call GetOpenInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void GetOpenInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The GetWriteRegisterInformation function gets the information for writing a specific register of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "WRITE_REGISTER 1080 0000 0100", so we can call GetWriteRegisterInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void GetWriteRegisterInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The ChInformation function gets the information for setting a specific channel of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "CH 1 CHANNEL_TRIGGER DISABLED", so we can call ChInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void ChInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The GroupInformation function gets the information for setting a specific group of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "GR 1 CHANNEL_TRIGGER DISABLED", so we can call GroupInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void GroupInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The AllInformation function gets the information for setting all the channels of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "ALL TRIGGER_THRESHOLD 0x10000000", so we can call AllInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void AllInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The FastInformation function gets the information for setting TRn channels of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "FAST TRIGGER_THRESHOLD 0x10000000", so we can call FastInformation (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void FastInformation (char *yytext, ConfObject * mioconfig);

/**
* @brief The ChannelEnableMask function gets the information for enabling or not the input from the channels.
* It gets them from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "CHANNEL_ENABLE_MASK 0x00000111", so we can call ChannelEnableMask (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void ChannelEnableMask (char *yytext, ConfObject * mioconfig);

/**
* @brief The GroupEnableMask function gets the information for enabling or not the input from the groups.
* It gets them from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "GROUP_ENABLE_MASK 0x10000000", so we can call GroupEnableMask (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void GroupEnableMask (char *yytext, ConfObject * mioconfig);

/**
* @brief The ChannelTriggerEnableMask function gets the information for setting TRn channels of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "FAST TRIGGER_THRESHOLD 0x10000000", so we can call ChannelTriggerEnableMask (yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void ChannelTriggerEnableMask (char *yytext, ConfObject * mioconfig);

/**
* @brief The WriteRegister function gets the information for setting a register of the digitizer from the yytext string and puts it in a ConfObject.
* @details Ex. yytext contains the string "write register 0x113c 0x00b7", so we can call WriteRegister(yytext, mioconfig).
* @param yytext contains a matched string.
* @param mioconfig is where the retrieved informations are stored.
* @return void
*/
void WriteRegister (char *yytext, ConfObject * mioconfig);

#endif
