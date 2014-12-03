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

Ban *ban_list;

void save_bans ( void )
{
	Ban *pban;
	FILE *fp;
	bool found = FALSE;

	fclose ( fpReserve );
	if ( ( fp = fopen ( BAN_FILE, "w" ) ) == NULL ) {
		ReportErrno ( BAN_FILE );
	}

	for ( pban = ban_list; pban != NULL; pban = pban->next ) {
		// -- we don't save temporary bans.
		if ( IS_SET ( pban->ban_flags, BAN_TEMP)) { continue; }
		
		if ( IS_SET ( pban->ban_flags, BAN_PERMANENT ) ) {
			found = TRUE;
			fprintf ( fp, "%-20s %-2d %s\n", pban->name, pban->level,
					  print_flags ( pban->ban_flags ) );
		}
	}

	fclose ( fp );
	fpReserve = fopen ( NULL_FILE, "r" );
	if ( !found )
	{ unlink ( BAN_FILE ); }
}

void load_bans ( void )
{
	FILE *fp;
	Ban *ban_last;

	if ( ( fp = fopen ( BAN_FILE, "r" ) ) == NULL )
	{ return; }

	ban_last = NULL;
	for ( ; ; ) {
		Ban *pban;
		if ( feof ( fp ) ) {
			fclose ( fp );
			return;
		}

		pban = new_ban();

		pban->name = assign_string ( fread_word ( fp ) );
		pban->level = fread_number ( fp );
		pban->ban_flags = fread_flag ( fp );
		fread_to_eol ( fp );

		if ( ban_list == NULL )
		{ ban_list = pban; }
		else
		{ ban_last->next = pban; }
		ban_last = pban;
	}
}

bool check_ban ( char *site, int type )
{
	Ban *pban;
	char host[MAX_STRING_LENGTH];

	strcpy ( host, capitalize ( site ) );
	host[0] = LOWER ( host[0] );

	for ( pban = ban_list; pban != NULL; pban = pban->next ) {
		if ( !IS_SET ( pban->ban_flags, type ) )
		{ continue; }

		if ( IS_SET ( pban->ban_flags, BAN_PREFIX )
				&&  IS_SET ( pban->ban_flags, BAN_SUFFIX )
				&&  strstr ( pban->name, host ) != NULL )
		{ return TRUE; }

		if ( IS_SET ( pban->ban_flags, BAN_PREFIX )
				&&  !str_suffix ( pban->name, host ) )
		{ return TRUE; }

		if ( IS_SET ( pban->ban_flags, BAN_SUFFIX )
				&&  !str_prefix ( pban->name, host ) )
		{ return TRUE; }
	}

	return FALSE;
}


void ban_site ( Creature *ch, const char *argument, bool fPerm )
{
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	char *name;
	BUFFER *buffer;
	Ban *pban, *prev;
	bool prefix = FALSE, suffix = FALSE;
	int type;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' ) {
		if ( ban_list == NULL ) {
			if(ch) {
				writeBuffer ( "No sites banned at this time.\n\r", ch );
			}
			return;
		}
		buffer = new_buf();
		if(ch) {
			add_buf ( buffer, "Banned sites  level  type     status\n\r" );
			for ( pban = ban_list; pban != NULL; pban = pban->next ) {
				sprintf ( buf2, "%s%s%s",
					  	IS_SET ( pban->ban_flags, BAN_PREFIX ) ? "*" : "",
					  	pban->name,
					  	IS_SET ( pban->ban_flags, BAN_SUFFIX ) ? "*" : "" );
				snprintf ( buf, sizeof ( buf ), "%-12s    %-3d  %-7s  %s\n\r",
					   	buf2, pban->level,
					   	IS_SET ( pban->ban_flags, BAN_TEMP ) ? "temp" :
					   	IS_SET ( pban->ban_flags, BAN_NEWBIES ) ? "newbies" :
					   	IS_SET ( pban->ban_flags, BAN_PERMIT )  ? "permit"  :
					   	IS_SET ( pban->ban_flags, BAN_ALL )     ? "all"	: "",
					   	IS_SET ( pban->ban_flags, BAN_PERMANENT ) ? "perm" : "temp" );
				add_buf ( buffer, buf );
			}
			writePage ( buf_string ( buffer ), ch );
		}
		recycle_buf ( buffer );
		return;
	}

	/* find out what type of ban */
	if ( arg2[0] == '\0' || !str_prefix ( arg2, "all" ) )
	{ type = BAN_ALL; }
	else if ( !str_prefix ( arg2, "newbies" ) )
	{ type = BAN_NEWBIES; }
	else if(!str_prefix(arg2, "temp")) {
		type = BAN_TEMP;
	}
	else if ( !str_prefix ( arg2, "permit" ) )
	{ type = BAN_PERMIT; }
	else {
		if(ch) {
			writeBuffer ( "Acceptable ban types are all, newbies, and permit, temp.\n\r",
					  ch );
		}
		return;
	}

	name = arg1;

	if ( name[0] == '*' ) {
		prefix = TRUE;
		name++;
	}

	if ( name[strlen ( name ) - 1] == '*' ) {
		suffix = TRUE;
		name[strlen ( name ) - 1] = '\0';
	}

	if ( strlen ( name ) == 0 ) {
		if(ch) {
			writeBuffer ( "You have to ban SOMETHING.\n\r", ch );
		}
		return;
	}

	prev = NULL;
	for ( pban = ban_list; pban != NULL; prev = pban, pban = pban->next ) {
		if ( SameString ( name, pban->name ) ) {
			if(ch) {
				if ( pban->level > get_trust ( ch ) ) {
					writeBuffer ( "That ban was set by a higher power.\n\r", ch );
					return;
				} else {
					if ( prev == NULL )
					{ ban_list = pban->next; }
					else
					{ prev->next = pban->next; }
					recycle_ban ( pban );
				}
			} else {
				if ( prev == NULL )
				{ ban_list = pban->next; }
				else
				{ prev->next = pban->next; }
				recycle_ban ( pban );				
			}
		}
	}

	pban = new_ban();
	pban->name = assign_string ( name );
	pban->level = ch ? get_trust ( ch ) : 60;

	/* set ban type */
	pban->ban_flags = type;

	if ( prefix )
	{ SET_BIT ( pban->ban_flags, BAN_PREFIX ); }
	if ( suffix )
	{ SET_BIT ( pban->ban_flags, BAN_SUFFIX ); }
	if ( fPerm )
	{ SET_BIT ( pban->ban_flags, BAN_PERMANENT ); }

	pban->next  = ban_list;
	ban_list    = pban;
	save_bans();
	if(ch) {
		snprintf ( buf, sizeof ( buf ), "%s has been banned.\n\r", pban->name );
		writeBuffer ( buf, ch );
	}
	return;
}

DefineCommand ( cmd_ban )
{
	ban_site ( ch, argument, FALSE );
}

DefineCommand ( cmd_permban )
{
	ban_site ( ch, argument, TRUE );
}

DefineCommand ( cmd_allow )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Ban *prev;
	Ban *curr;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Remove which site from the ban list?\n\r", ch );
		return;
	}

	prev = NULL;
	for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next ) {
		if ( SameString ( arg, curr->name ) ) {
			if ( curr->level > get_trust ( ch ) ) {
				writeBuffer (
					"You are not powerful enough to lift that ban.\n\r", ch );
				return;
			}
			if ( prev == NULL )
			{ ban_list   = ban_list->next; }
			else
			{ prev->next = curr->next; }

			recycle_ban ( curr );
			snprintf ( buf, sizeof ( buf ), "Ban on %s lifted.\n\r", arg );
			writeBuffer ( buf, ch );
			save_bans();
			return;
		}
	}

	writeBuffer ( "Site is not banned.\n\r", ch );
	return;
}


