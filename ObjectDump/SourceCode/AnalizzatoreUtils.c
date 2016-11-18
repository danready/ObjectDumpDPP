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
* @file AnalizzatoreUtils.c
* @brief This file contains the implementation of the utility functions used by the scanner.
* @details Il file AnalizzatoreLessicale.flex puo' generare, usando flex, uno scanner per il file di configurazione.
* Il contenuto delle stringhe riconosciute deve essere poi estrapolato e messo nel ConfObject: le funzioni seguenti eseguono questo compito.
* Ogni funzione esegue un compito molto specifico: sono progettate guardando alla singola stringa di configurazione da cui ricavare il contenuto.
* @author Daniele Berto
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <regex.h>
#include <stdbool.h>
#include "DefineGeneral.h"
#include "ConfObject.h"

///La funzione reg_matches serve per comparare una stringa con una espressione regolare. Restituisce true se e' stata trovata l'espressione regolare pattern nella stringa str.
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

char *
FindPointer (char *yytext)
{
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

int
OutputFileFormat (char *yytext)
{
  char *punt;
  punt = FindPointer (yytext);
  if (reg_matches (punt, "[Bb][Ii][Nn][Aa][Rr][Yy]"))
    return 0;
  else if (reg_matches (punt, "[Aa][Ss][Cc][Ii][Ii]"))
    return 1;
  else
    return -1;
}

int
OutputRisingFalling (char *yytext)
{
  char *punt;
  punt = FindPointer (yytext);
  if (reg_matches (punt, "[rR][iI][sS][iI][nN][gG]"))
    return 0;
  else if (reg_matches (punt, "[fF][aA][lL][lL][iI][nN][gG]"))
    return 1;
  else
    return -1;
}

int
OutputNIMTTL (char *yytext)
{
	char *punt;
	punt = FindPointer (yytext);
	if (reg_matches (punt, "[Nn][Ii][Mm]"))
		return 0;
	else if (reg_matches (punt, "[Tt][Tt][Ll]"))
		return 1;
	else
		return -1;
}

int
YesNoAnswer (char *yytext)
{
	char *punt;
	punt = FindPointer (yytext);
	if (reg_matches (punt, "[Yy][Ee][Ss]"))
		return 1;
	else if (reg_matches (punt, "[Nn][Oo]"))
		return 0;
	else
		return -1;
}

int
GetAcquisitionMode (char *yytext)
{
	char *punt;
	punt = FindPointer (yytext);
	if (reg_matches
	(punt, "[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[oO][nN][lL][yY]"))
		return 0;
	else
	if (reg_matches
	(punt,
	"[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[aA][nN][dD]_[tT][rR][gG][oO][uU][tT]"))
		return 1;
	else if (reg_matches (punt, "[dD][iI][sS][aA][bB][lL][eE][dD]"))
		return 2;
	else
		return -1;
}

void
GetOpenInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int Type;
	int LinkNumber;
	int NodeNumber;
	int BaseAddress;
	punt = FindPointer (yytext);
	
	if (reg_matches (punt, "[Uu][Ss][Bb]"))
		Type = 0;
	else if (reg_matches (punt, "[Pp][Cc][Ii]"))
		Type = 1;
	else
		Type = -1;
		
	LinkNumber = FindIntegerValue (punt);
	punt = FindPointer (punt);
	NodeNumber = FindIntegerValue (punt);
	punt = FindPointer (punt);
	punt = FindPointer (punt);
	BaseAddress = strtoul (punt, NULL, 16);

	mioconfig->LinkType = Type;
	mioconfig->LinkNumber = LinkNumber;
	mioconfig->ConetNode = NodeNumber;
	mioconfig->VMEBaseAddress = BaseAddress;

	//printf ("%d %d %d %d\n", Type, LinkNumber, NodeNumber, BaseAddress);
}

void
GetWriteRegisterInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int Address;
	int Data;
	int Mask;
	punt = FindPointer (yytext);
	Address = strtoul (punt, NULL, 16);
	punt = FindPointer (punt);
	Data = strtoul (punt, NULL, 16);
	punt = FindPointer (punt);
	Mask = strtoul (punt, NULL, 16);
	mioconfig->Address_register = Address;
	mioconfig->Data_register = Data;
	mioconfig->Mask_register = Mask;
}


//*********************************************

void
ChInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int mod;
	int yes_no;
	int channel;
	int channel_trigger;


	channel = FindIntegerValue (yytext);


	if (channel < MAXCHANNELOBJECT) 
	{
		mioconfig->channels[channel].set = 1;
		mioconfig->channels[channel].numChannel = channel;

		punt = FindPointer (yytext);
		punt = FindPointer (punt);


		if (reg_matches (punt, "[Ee][Nn][Aa][Bb][Ll][Ee]_[iI][nN][pP][uU][tT]"))
			mod = 0;
		else if (reg_matches (punt, "[dD][cC]_[oO][fF][fF][sS][eE][tT]"))
			mod = 1;
		else if (reg_matches (punt, "[tT][rR][iI][gG][gG][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]"))
			mod = 2;
		else if (reg_matches
		(punt, "[cC][hH][aA][nN][nN][eE][lL]_[tT][rR][iI][gG][gG][eE][rR]"))
			mod = 3;
		else
			mod = -1;


		if (mod == 0)
		{				//ENABLE_INPUT
			punt = FindPointer (punt);
			if (reg_matches (punt, "[Yy][Ee][Ss]"))
			{
				yes_no = 1;
				mioconfig->channels[channel].enable_input = yes_no;
			}
			else if (reg_matches (punt, "[Nn][Oo]"))
			{
				yes_no = 0;
				mioconfig->channels[channel].enable_input = yes_no;
			}
		}

		else if (mod == 1)
		{				//DC_OFFSET
			punt = FindPointer (punt);
			int dc_offset = strtoul (punt, NULL, 16);
			mioconfig->channels[channel].dc_offset = dc_offset;
		}

		else if (mod == 2)
		{				//TRIGGER_THRESHOLD
			punt = FindPointer (punt);
			int trigger_threshold = strtoul (punt, NULL, 16);
			mioconfig->channels[channel].trigger_threshold = trigger_threshold;
			//mioconfig->channels[channel].trigger_threshold;
		}

		else if (mod == 3)
		{				//CHANNEL_TRIGGER
			punt = FindPointer (punt);
			if (reg_matches
			(punt,
			"[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[oO][nN][lL][yY]"))
			{
				channel_trigger = 0;
				mioconfig->channels[channel].channel_trigger = channel_trigger;
			}
			else
			if (reg_matches
			(punt,
			"[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[aA][nN][dD]_[tT][rR][gG][oO][uU][tT]"))
			{
				channel_trigger = 1;
				mioconfig->channels[channel].channel_trigger = channel_trigger;
			}
			else if (reg_matches (punt, "[dD][iI][sS][aA][bB][lL][eE][dD]"))
			{
				channel_trigger = 2;
				mioconfig->channels[channel].channel_trigger = channel_trigger;
			}
		} // else if (mod == 3)
	} //if (channel < MAXCHANNELOBJECT) 
}  //END_FUNCTION

//***********************************************

void
GroupInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int mod;
	int yes_no;
	int group;
	//int group_mask;

	group = FindIntegerValue (yytext);

	if (group < MAXGROUPOBJECT)
	{

		mioconfig->groups[group].set = 1;
		mioconfig->groups[group].numGroup = group;

		punt = FindPointer (yytext);
		punt = FindPointer (punt);
		if (reg_matches (punt, "[Ee][Nn][Aa][Bb][Ll][Ee]_[iI][nN][pP][uU][tT]"))
			mod = 0;
		else if (reg_matches (punt, "[dD][cC]_[oO][fF][fF][sS][eE][tT]"))
			mod = 1;
		else if (reg_matches (punt, "[tT][rR][iI][gG][gG][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]"))
			mod = 2;
		else if (reg_matches (punt, "[gG][rR][oO][uU][pP]_[tT][rR][gG]_[eE][nN][aA][bB][lL][eE]_[mM][aA][sS][kK]"))
			mod = 3;
		else
			mod = -1;

		if (mod == 0)
		{				//ENABLE_INPUT
			punt = FindPointer (punt);
			if (reg_matches (punt, "[Yy][Ee][Ss]"))
			{
				yes_no = 1;
				mioconfig->groups[group].enable_input = yes_no;
			}
			else if (reg_matches (punt, "[Nn][Oo]"))
			{
				yes_no = 0;
				mioconfig->groups[group].enable_input = yes_no;
			}
		}

		else if (mod == 1)
		{				//DC_OFFSET
			punt = FindPointer (punt);
			int dc_offset = strtoul (punt, NULL, 16);
			mioconfig->groups[group].dc_offset = dc_offset;
		}

		else if (mod == 2)
		{				//TRIGGER_THRESHOLD
			punt = FindPointer (punt);
			int trigger_threshold = strtoul (punt, NULL, 16);
			mioconfig->groups[group].trigger_threshold = trigger_threshold;
		}

		else if (mod == 3)
		{				//GROUP_TRG_ENABLE_MASK
			punt = FindPointer (punt);
			int group_mask = strtoul (punt, NULL, 16);
			mioconfig->groups[group].group_trg_enable_mask = group_mask;
		} // else if (mod == 3)
	} // if (group < MAXGROUPOBJECT)
}  //END_FUNCTION

//************************************************

void
AllInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int mod;
	int yes_no;
	punt = FindPointer (yytext);
	if (reg_matches (punt, "[Ee][Nn][Aa][Bb][Ll][Ee]_[iI][nN][pP][uU][tT]"))
		mod = 0;
	else if (reg_matches (punt, "[dD][cC]_[oO][fF][fF][sS][eE][tT]"))
		mod = 1;
	else
	if (reg_matches
	(punt,
	"[tT][rR][iI][gG][gG][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]"))
		mod = 2;
	else
		mod = -1;

	if (mod == 0)
	{				//ENABLE_INPUT

		punt = FindPointer (punt);
		if (reg_matches (punt, "[Yy][Ee][Ss]"))
		{
			yes_no = 1;
			mioconfig->enable_input = yes_no;
		}
		else if (reg_matches (punt, "[Nn][Oo]"))
		{
			yes_no = 0;
			mioconfig->enable_input = yes_no;
		}

	}

	else if (mod == 1)
	{				//DC_OFFSET
		punt = FindPointer (punt);
		int dc_offset = strtoul (punt, NULL, 16);
		mioconfig->dc_offset = dc_offset;
	}

	else if (mod == 2)
	{				//TRIGGER_THRESHOLD
		punt = FindPointer (punt);
		int trigger_threshold = strtoul (punt, NULL, 16);
		mioconfig->trigger_threshold = trigger_threshold;
	}
}				//END_FUNCTION

//**********************************************

void
FastInformation (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int mod;
	//int yes_no;
	int fast;
	fast = FindIntegerValue (yytext);

	if (fast < MAXFASTOBJECT)
	{

		mioconfig->fasts[fast].set = 1;
		mioconfig->fasts[fast].numFast = fast + 1;


		punt = FindPointer (yytext);
		punt = FindPointer (punt);

		if (reg_matches (punt, "[dD][cC]_[oO][fF][fF][sS][eE][tT]"))
			mod = 0;
		else if (reg_matches
		(punt,
		"[tT][rR][iI][gG][gG][eE][rR]_[tT][hH][rR][eE][sS][hH][oO][lL][dD]"))
			mod = 1;
		else
			mod = -1;

		if (mod == 0)
		{	//DC_OFFSET
			punt = FindPointer (punt);
			int dc_offset = strtoul (punt, NULL, 16);
			mioconfig->fasts[fast].dc_offset = dc_offset;
		}

		else if (mod == 1)
		{
			//TRIGGER_THRESHOLD
			punt = FindPointer (punt);
			int trigger_threshold = strtoul (punt, NULL, 16);
			mioconfig->fasts[fast].trigger_threshold = trigger_threshold;
		}
	} // if (fast < MAXFASTOBJECT)
}				//END_FUNCTION

void
ChannelEnableMask (char *yytext, ConfObject * mioconfig)
{
	int i;
	unsigned int bit = 1;
	unsigned int maschera;

	char *punt;
	int channel_enable_mask;
	punt = FindPointer (yytext);
	mioconfig->channel_enable_mask = strtoul (punt, NULL, 16);

	maschera = (unsigned int) mioconfig->channel_enable_mask;
	for (i = 0; i < MAXCHANNELOBJECT; i++)
	{
		if (maschera & bit)
		{
			mioconfig->channels[i].enable_input = 1;
			mioconfig->channels[i].set = 1;
			mioconfig->channels[i].numChannel = i;
		}
		maschera >>= 1;
	}
}

void
GroupEnableMask (char *yytext, ConfObject * mioconfig)
{
	int i;
	unsigned int bit = 1;
	unsigned int maschera;

	char *punt;
	int channel_enable_mask;
	punt = FindPointer (yytext);
	mioconfig->group_enable_mask = strtoul (punt, NULL, 16);

	maschera = (unsigned int) mioconfig->group_enable_mask;
	for (i = 0; i < MAXGROUPOBJECT; i++)
	{
		if (maschera & bit)
		{
			mioconfig->groups[i].enable_input = 1;
			mioconfig->groups[i].set = 1;
			mioconfig->groups[i].numGroup = i;
		}
		maschera >>= 1;
	}
}

void
ChannelTriggerEnableMask (char *yytext, ConfObject * mioconfig)
{
	char *punt;
	int channel_trigger_enable_mask;
	punt = FindPointer (yytext);
	mioconfig->self_trigger_enable_mask = strtoul (punt, NULL, 16);
	punt = FindPointer (yytext);

	if (reg_matches
	(punt, "[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[oO][nN][lL][yY]"))
	{
		mioconfig->self_trigger_enable_mask_mode = 0;
	}
	else
	if (reg_matches
	(punt,
	"[aA][cC][qQ][uU][iI][sS][iI][tT][iI][oO][nN]_[aA][nN][dD]_[tT][rR][gG][oO][uU][tT]"))
	{
		mioconfig->self_trigger_enable_mask_mode = 1;
	}
	else if (reg_matches (punt, "[dD][iI][sS][aA][bB][lL][eE][dD]"))
	{
		mioconfig->self_trigger_enable_mask_mode = 2;
	}
}


#include <iostream>

void WriteRegister(char *yytext, ConfObject *mioconfig)
{
    char *punt;
    int registerAddress;
    int registerData;
    punt = FindPointer (yytext);    //skip to the next word
    punt = FindPointer (punt);    //skip to the next word
    //std::cout << punt << std::endl;
    registerAddress = strtoul (punt, NULL, 16);
    punt = FindPointer (punt);
    registerData = strtoul (punt, NULL, 16);

    //search for the first register allocation in the ConfObject not used
    int i;
    for(i=0; i< MAXREGISTERS; i++)
    {
        if(mioconfig->registers[i].address==-1) break;
    }
    if(i==MAXREGISTERS-1){
        std::cerr << "Error: register slots finished";
        return;
    }
    mioconfig->registers[i].address = registerAddress;
    mioconfig->registers[i].data = registerData;
    //std::cout << "Settato registro " << registerAddress << " al valore " << registerData << std::endl;
}
