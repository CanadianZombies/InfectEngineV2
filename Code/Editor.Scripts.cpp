/* The following code is based on ILAB OLC by Jason Dinkel */
/* Mobprogram code by Lordrom for Nevermore Mud */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"

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
	argument = one_argument ( argument, command );

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

	argument = one_argument ( argument, command );

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
		string_append ( ch, &pMcode->code );
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
		if ( fAll || ENTRE ( ch->in_room->area->min_vnum, mprg->vnum, ch->in_room->area->max_vnum ) ) {
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