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

#define MPEDIT( fun )           bool fun(Creature *ch, const char*argument)


const struct olc_cmd_type mpedit_table[] = {
	/*	{	command		function	}, */

	{	"commands",	show_commands	},
	{	"create",	mpedit_create	},
	{	"code",		mpedit_code	},
	{	"show",		mpedit_show	},
	{	"list",		mpedit_list	},
	{	"?",		show_help	},
	{	NULL,		0		}
};

void mpedit ( Creature *ch, const char *argument )
{
	MPROG_CODE *pMcode;
	char arg[MAX_INPUT_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int cmd;
	Zone *ad;

	smash_tilde ( argument );
	strcpy ( arg, argument );
	argument = ChopC ( argument, command );

	EDIT_MPCODE ( ch, pMcode );

	if ( pMcode ) {
		ad = get_vnum_area ( pMcode->vnum );

		if ( ad == NULL ) { /* ??? */
			edit_done ( ch );
			return;
		}

		if ( !IS_BUILDER ( ch, ad ) ) {
			writeBuffer ( "MPEdit: Insufficient security to modify code.\n\r", ch );
			edit_done ( ch );
			return;
		}
	}

	if ( command[0] == '\0' ) {
		mpedit_show ( ch, argument );
		return;
	}

	if ( !str_cmp ( command, "done" ) ) {
		edit_done ( ch );
		return;
	}

	for ( cmd = 0; mpedit_table[cmd].name != NULL; cmd++ ) {
		if ( !str_prefix ( command, mpedit_table[cmd].name ) ) {
			if ( ( *mpedit_table[cmd].olc_fun ) ( ch, argument ) && pMcode )
				if ( ( ad = get_vnum_area ( pMcode->vnum ) ) != NULL )
				{ SET_BIT ( ad->area_flags, AREA_CHANGED ); }
			return;
		}
	}

	interpret ( ch, arg );

	return;
}

DefineCommand ( cmd_mpedit )
{
	MPROG_CODE *pMcode;
	char command[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, command );

	if ( is_number ( command ) ) {
		int vnum = atoi ( command );
		Zone *ad;

		if ( ( pMcode = get_mprog_index ( vnum ) ) == NULL ) {
			writeBuffer ( "MPEdit : That vnum does not exist.\n\r", ch );
			return;
		}

		ad = get_vnum_area ( vnum );

		if ( ad == NULL ) {
			writeBuffer ( "MPEdit : That vnum has not been assigned to an area.\n\r", ch );
			return;
		}

		if ( !IS_BUILDER ( ch, ad ) ) {
			writeBuffer ( "MPEdit : You do not have access to that area.\n\r", ch );
			return;
		}

		ch->desc->pEdit		= ( void * ) pMcode;
		ch->desc->editor	= ED_MPCODE;

		return;
	}

	if ( !str_cmp ( command, "create" ) ) {
		if ( argument[0] == '\0' ) {
			writeBuffer ( "Sintaxis : mpedit create [vnum]\n\r", ch );
			return;
		}

		mpedit_create ( ch, argument );
		return;
	}

	writeBuffer ( "Sintaxis : mpedit [vnum]\n\r", ch );
	writeBuffer ( "           mpedit create [vnum]\n\r", ch );

	return;
}

MPEDIT ( mpedit_create )
{
	MPROG_CODE *pMcode;
	int value = atoi ( argument );
	Zone *ad;

	if ( IS_NULLSTR ( argument ) || value < 1 ) {
		writeBuffer ( "Sintaxis : mpedit create [vnum]\n\r", ch );
		return FALSE;
	}

	ad = get_vnum_area ( value );

	if ( ad == NULL ) {
		writeBuffer ( "MPEdit : That vnum is not assigned to an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER ( ch, ad ) ) {
		writeBuffer ( "MPEdit : You do not have a high enough security rating.\n\r", ch );
		return FALSE;
	}

	if ( get_mprog_index ( value ) ) {
		writeBuffer ( "MPEdit: Code vnum already exists.\n\r", ch );
		return FALSE;
	}

	pMcode			= new_mpcode();
	pMcode->vnum		= value;
	pMcode->next		= mprog_list;
	mprog_list			= pMcode;
	ch->desc->pEdit		= ( void * ) pMcode;
	ch->desc->editor		= ED_MPCODE;

	writeBuffer ( "MobProgram code created.\n\r", ch );

	return TRUE;
}

MPEDIT ( mpedit_show )
{
	MPROG_CODE *pMcode;
	char buf[MAX_STRING_LENGTH];

	EDIT_MPCODE ( ch, pMcode );

	snprintf ( buf, sizeof ( buf ),
			   "Vnum:       [%d]\n\r"
			   "Code:\n\r%s\n\r",
			   pMcode->vnum, pMcode->code );
	writeBuffer ( buf, ch );

	return FALSE;
}

MPEDIT ( mpedit_code )
{
	MPROG_CODE *pMcode;
	EDIT_MPCODE ( ch, pMcode );

	if ( argument[0] == '\0' ) {
		EnterStringEditor ( ch, &pMcode->code );
		return TRUE;
	}

	writeBuffer ( "Syntax: code\n\r", ch );
	return FALSE;
}

MPEDIT ( mpedit_list )
{
	int count = 1;
	MPROG_CODE *mprg;
	char buf[MAX_STRING_LENGTH];
	BUFFER *buffer;
	bool fAll = !str_cmp ( argument, "all" );
	char blah;
	Zone *ad;

	buffer = new_buf();

	for ( mprg = mprog_list; mprg != NULL; mprg = mprg->next )
		if ( fAll || ENTRE ( IN_ROOM ( ch )->area->min_vnum, mprg->vnum, IN_ROOM ( ch )->area->max_vnum ) ) {
			ad = get_vnum_area ( mprg->vnum );

			if ( ad == NULL )
			{ blah = '?'; }
			else if ( IS_BUILDER ( ch, ad ) )
			{ blah = '*'; }
			else
			{ blah = ' '; }

			snprintf ( buf, sizeof ( buf ), "[%3d] (%c) %5d\n\r", count, blah, mprg->vnum );
			add_buf ( buffer, buf );

			count++;
		}

	if ( count == 1 ) {
		if ( fAll )
		{ add_buf ( buffer, "There are currently no existing mprogs.\n\r" ); }
		else
		{ add_buf ( buffer, "There are no mprogs in this area.\n\r" ); }
	}

	writePage ( buf_string ( buffer ), ch );
	recycle_buf ( buffer );

	return FALSE;
}
