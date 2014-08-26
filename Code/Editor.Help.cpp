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

#define HEDIT( fun )           bool fun(Creature *ch, const char*argument)
#define EDIT_HELP(ch, help)	( help = (HELP_DATA *) ch->desc->pEdit )

extern HELP_AREA * had_list;

const struct olc_cmd_type hedit_table[] = {
	/*	{	command		function	}, */

	{	"keyword",	hedit_keyword	},
	{	"texto",	hedit_text	},
	{	"new",		hedit_new	},
	{	"nivel",	hedit_level	},
	{	"commands",	show_commands	},
	{	"delete",	hedit_delete	},
	{	"list",		hedit_list	},
	{	"show",		hedit_show	},
	{	"?",		show_help	},

	{	NULL,		0		}
};

HELP_AREA * get_help_area ( HELP_DATA *help )
{
	HELP_AREA * temp;
	HELP_DATA * thelp;

	for ( temp = had_list; temp; temp = temp->next )
		for ( thelp = temp->first; thelp; thelp = thelp->next_area )
			if ( thelp == help )
			{ return temp; }

	return NULL;
}

HEDIT ( hedit_show )
{
	HELP_DATA * help;
	char buf[MSL * 2];

	EDIT_HELP ( ch, help );

	sprintf ( buf, "Keyword : [%s]\n\r"
			  "Level   : [%d]\n\r"
			  "Text    :\n\r"
			  "%s-END-\n\r",
			  help->keyword,
			  help->level,
			  help->text );
	writeBuffer ( buf, ch );

	return FALSE;
}

HEDIT ( hedit_level )
{
	HELP_DATA *help;
	int lev;

	EDIT_HELP ( ch, help );

	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax : level [-1..MAX_LEVEL]\n\r", ch );
		return FALSE;
	}

	lev = atoi ( argument );

	if ( lev < -1 || lev > MAX_LEVEL ) {
		printf_to_char ( ch, "HEdit : Level must be between -1 and %d, inclusive.\n\r", MAX_LEVEL );
		return FALSE;
	}

	help->level = lev;
	writeBuffer ( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT ( hedit_keyword )
{
	HELP_DATA *help;

	EDIT_HELP ( ch, help );

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax : keyword [keyword(s)]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( help->keyword );
	help->keyword = assign_string ( argument );

	writeBuffer ( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT ( hedit_new )
{
	char arg[MIL], fullarg[MIL];
	HELP_AREA *had;
	HELP_DATA *help;
	extern HELP_DATA *help_last;

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Sintaxis : new [name]\n\r", ch );
		writeBuffer ( "           new [area] [name]\n\r", ch );
		return FALSE;
	}

	strcpy ( fullarg, argument );
	argument = one_argument ( argument, arg );

	if ( ! ( had = had_lookup ( arg ) ) ) {
		had = ch->in_room->area->helps;
		argument = fullarg;
	}

	if ( help_lookup ( argument ) ) {
		writeBuffer ( "HEdit : A helpfile with that name already exists.\n\r", ch );
		return FALSE;
	}

	if ( !had ) { /* the area currently has no helps */
		had		= new_had();
		had->filename	= assign_string ( ch->in_room->area->file_name );
		had->area	= ch->in_room->area;
		had->first	= NULL;
		had->last	= NULL;
		had->changed	= TRUE;
		had->next	= had_list;
		had_list	= had;
		ch->in_room->area->helps = had;
		SET_BIT ( ch->in_room->area->area_flags, AREA_CHANGED );
	}

	help		= new_help();
	help->level	= 0;
	help->keyword	= assign_string ( argument );
	help->text	= assign_string ( "" );

	if ( help_last )
	{ help_last->next	= help; }

	if ( help_first == NULL )
	{ help_first = help; }

	help_last	= help;
	help->next	= NULL;

	if ( !had->first )
	{ had->first	= help; }
	if ( !had->last )
	{ had->last	= help; }

	had->last->next_area	= help;
	had->last		= help;
	help->next_area		= NULL;

	ch->desc->pEdit		= ( HELP_DATA * ) help;
	ch->desc->editor	= ED_HELP;

	writeBuffer ( "Ok.\n\r", ch );
	return FALSE;
}

HEDIT ( hedit_text )
{
	HELP_DATA *help;

	EDIT_HELP ( ch, help );

	if ( !IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: text\n\r", ch );
		return FALSE;
	}

	string_append ( ch, &help->text );

	return TRUE;
}

void hedit ( Creature *ch, const char *argument )
{
	HELP_DATA * pHelp;
	HELP_AREA *had;
	char arg[MAX_INPUT_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int cmd;

	smash_tilde ( argument );
	strcpy ( arg, argument );
	argument = one_argument ( argument, command );

	EDIT_HELP ( ch, pHelp );

	had = get_help_area ( pHelp );

	if ( had == NULL ) {
		log_hd ( LOG_ERROR, Format ( "hedit : helpfil '%s' is NULL", pHelp->keyword ) );
		edit_done ( ch );
		return;
	}

	if ( ch->pcdata->security < 9 ) {
		writeBuffer ( "HEdit: Sorry, but your security rating is too low.\n\r", ch );
		edit_done ( ch );
		return;
	}

	if ( command[0] == '\0' ) {
		hedit_show ( ch, argument );
		return;
	}

	if ( !str_cmp ( command, "done" ) ) {
		edit_done ( ch );
		return;
	}

	for ( cmd = 0; hedit_table[cmd].name != NULL; cmd++ ) {
		if ( !str_prefix ( command, hedit_table[cmd].name ) ) {
			if ( ( *hedit_table[cmd].olc_fun ) ( ch, argument ) )
			{ had->changed = TRUE; }
			return;
		}
	}

	interpret ( ch, arg );
	return;
}

DefineCommand ( cmd_hedit )
{
	HELP_DATA * pHelp;

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ( pHelp = help_lookup ( argument ) ) == NULL ) {
		writeBuffer ( "HEdit : That helpfile does not exist.\n\r", ch );
		return;
	}

	ch->desc->pEdit		= ( void * ) pHelp;
	ch->desc->editor	= ED_HELP;

	return;
}

HEDIT ( hedit_delete )
{
	HELP_DATA * pHelp, * temp;
	HELP_AREA * had;
	Socket *d;
	bool found = FALSE;

	EDIT_HELP ( ch, pHelp );

	for ( d = socket_list; d; d = d->next )
		if ( d->editor == ED_HELP && pHelp == ( HELP_DATA * ) d->pEdit )
		{ edit_done ( d->character ); }

	if ( help_first == pHelp )
	{ help_first = help_first->next; }
	else {
		for ( temp = help_first; temp; temp = temp->next ) {
			if ( temp->next == pHelp )
			{ break; }
		}

		if ( !temp ) {
			log_hd ( LOG_ERROR, Format ( "hedit_delete : help '%s' not found in the help_first list!", pHelp->keyword ) );
			return FALSE;
		}

		temp->next = pHelp->next;
	}

	for ( had = had_list; had; had = had->next ) {
		if ( pHelp == had->first ) {
			found = TRUE;
			had->first = had->first->next_area;
		} else {
			for ( temp = had->first; temp; temp = temp->next_area ) {
				if ( temp->next_area == pHelp )
				{ break; }
			}
			if ( temp ) {
				temp->next_area = pHelp->next_area;
				found = TRUE;
				break;
			}
		}
	}
	if ( !found ) {
		log_hd ( LOG_ERROR, Format ( "hedit_delete : help %s not found in had_list!", pHelp->keyword ) );
		return FALSE;
	}

	recycle_help ( pHelp );

	writeBuffer ( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT ( hedit_list )
{
	char buf[MIL];
	int cnt = 0;
	HELP_DATA *pHelp;
	BUFFER *buffer;

	EDIT_HELP ( ch, pHelp );

	if ( !str_cmp ( argument, "all" ) ) {
		buffer = new_buf();

		for ( pHelp = help_first; pHelp; pHelp = pHelp->next ) {
			sprintf ( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
					  cnt % 4 == 3 ? "\n\r" : " " );
			add_buf ( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
		{ add_buf ( buffer, "\n\r" ); }

		writePage ( buf_string ( buffer ), ch );
		return FALSE;
	}

	if ( !str_cmp ( argument, "area" ) ) {
		if ( ch->in_room->area->helps == NULL ) {
			writeBuffer ( "There are no helps in this area.\n\r", ch );
			return FALSE;
		}

		buffer = new_buf();

		for ( pHelp = ch->in_room->area->helps->first; pHelp; pHelp = pHelp->next_area ) {
			sprintf ( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
					  cnt % 4 == 3 ? "\n\r" : " " );
			add_buf ( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
		{ add_buf ( buffer, "\n\r" ); }

		writePage ( buf_string ( buffer ), ch );
		return FALSE;
	}

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax : list all\n\r", ch );
		writeBuffer ( "         list area\n\r", ch );
		return FALSE;
	}

	return FALSE;
}
