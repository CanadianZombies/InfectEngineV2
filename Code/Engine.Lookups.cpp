/*###################################################################################
#                    Infected City powered by InfectEngine                          #
#            InfectEngine is powered by CombatMUD Core Infrastructure               #
#  Original Diku Mud Copyright (c) 1990, 1991 by Sebastian Hammer, Michael Sifert   #
#               Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe                  #
# Merc Copyright (c) 1992, 1993 by Michael Chastain, Michael Quan, and Mitchell Tse #
#      ROM 2.4 Copyright (c) 1993-1998 Russ Taylor, Brian Moore, Gabrielle Taylor   #
#####################################################################################
# InfectEngine and CombatMUD Engine are both ground up C++ MUDs by David Simmerson  #
# InfectEngine V2 is an attempt to Hybrid ROM24B6 and InfectEngineV1 and CombatMUD  #
# together.  All source falls under the DIKU license, Merc and ROM licences however #
# if individual functions are retrieved from this base that were not originally part#
#   of Diku, Merc or ROM they will fall under the MIT license as per the original   #
#                      Licenses for CombatMUD and InfectEngine.                     #
#####################################################################################
# InfectEngine Copyright (c) 2010-2014 David Simmerson                              #
# CombatMUD Copyright (c) 2007-2014 David Simmerson                                 #
#                                                                                   #
# Permission is hereby granted, A, to any person obtaining a copy                   #
# of this software and associated documentation files (the "Software"), to deal     #
# in the Software without restriction, including without limitation the rights      #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         #
# copies of the Software, and to permit persons to whom the Software is             #
# furnished to do so, subject to the following conditions:                          #
#                                                                                   #
# The above copyright notice and this permission notice shall be included in        #
# all copies or substantial portions of the Software.                               #
#                                                                                   #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN         #
# THE SOFTWARE.                                                                     #
###################################################################################*/

#include "Engine.h"

int flag_lookup ( const char *name, const struct flag_type *flag_table )
{
	int flag;

	for ( flag = 0; flag_table[flag].name != NULL; flag++ ) {
		if ( LOWER ( name[0] ) == LOWER ( flag_table[flag].name[0] )
				&&  !str_prefix ( name, flag_table[flag].name ) )
		{ return flag_table[flag].bit; }
	}

	return NO_FLAG;
}

int clan_lookup ( const char *name )
{
	int clan;

	for ( clan = 0; clan < MAX_CLAN; clan++ ) {
		if ( LOWER ( name[0] ) == LOWER ( clan_table[clan].name[0] )
				&&  !str_prefix ( name, clan_table[clan].name ) )
		{ return clan; }
	}

	return 0;
}

int position_lookup ( const char *name )
{
	int pos;

	for ( pos = 0; position_table[pos].name != NULL; pos++ ) {
		if ( LOWER ( name[0] ) == LOWER ( position_table[pos].name[0] )
				&&  !str_prefix ( name, position_table[pos].name ) )
		{ return pos; }
	}

	return -1;
}

int sex_lookup ( const char *name )
{
	int sex;

	for ( sex = 0; sex_table[sex].name != NULL; sex++ ) {
		if ( LOWER ( name[0] ) == LOWER ( sex_table[sex].name[0] )
				&&  !str_prefix ( name, sex_table[sex].name ) )
		{ return sex; }
	}

	return -1;
}

int size_lookup ( const char *name )
{
	int size;

	for ( size = 0; size_table[size].name != NULL; size++ ) {
		if ( LOWER ( name[0] ) == LOWER ( size_table[size].name[0] )
				&&  !str_prefix ( name, size_table[size].name ) )
		{ return size; }
	}

	return -1;
}

/* returns race number */
int race_lookup ( const char *name )
{
	int race;

	for ( race = 0; race_table[race].name != NULL; race++ ) {
		if ( LOWER ( name[0] ) == LOWER ( race_table[race].name[0] )
				&&  !str_prefix ( name, race_table[race].name ) )
		{ return race; }
	}

	return 0;
}

int item_lookup ( const char *name )
{
	int type;

	for ( type = 0; item_table[type].name != NULL; type++ ) {
		if ( LOWER ( name[0] ) == LOWER ( item_table[type].name[0] )
				&&  !str_prefix ( name, item_table[type].name ) )
		{ return item_table[type].type; }
	}

	return -1;
}

int liq_lookup ( const char *name )
{
	int liq;

	for ( liq = 0; liq_table[liq].liq_name != NULL; liq++ ) {
		if ( LOWER ( name[0] ) == LOWER ( liq_table[liq].liq_name[0] )
				&& !str_prefix ( name, liq_table[liq].liq_name ) )
		{ return liq; }
	}

	return -1;
}

HELP_DATA * help_lookup ( const char *keyword )
{
	HELP_DATA *pHelp;
	char temp[MIL], argall[MIL];

	argall[0] = '\0';

	while ( keyword[0] != '\0' ) {
		keyword = ChopC ( keyword, temp );
		if ( argall[0] != '\0' )
		{ strcat ( argall, " " ); }
		strcat ( argall, temp );
	}

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
		if ( is_name ( argall, pHelp->keyword ) )
		{ return pHelp; }

	return NULL;
}

HELP_AREA * had_lookup ( const char *arg )
{
	HELP_AREA * temp;
	extern HELP_AREA * had_list;

	for ( temp = had_list; temp; temp = temp->next )
		if ( SameString ( arg, temp->filename ) )
		{ return temp; }

	return NULL;
}
