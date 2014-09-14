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

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern  int     _filbuf         args ( ( FILE * ) );
#endif
extern FILE *                  fpArea;
extern char                    strArea[MAX_INPUT_LENGTH];

/* local procedures */
void load_thread ( const char *name, NOTE_DATA **list, int type, time_t recycle_time );
void parse_note ( Creature *ch, const char *argument, int type );
bool hide_note ( Creature *ch, NOTE_DATA *pnote );

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;

int count_spool ( Creature *ch, NOTE_DATA *spool )
{
	int count = 0;
	NOTE_DATA *pnote;

	for ( pnote = spool; pnote != NULL; pnote = pnote->next )
		if ( !hide_note ( ch, pnote ) )
		{ count++; }

	return count;
}

DefineCommand ( cmd_unread )
{
	char buf[MAX_STRING_LENGTH];
	int count;
	bool found = FALSE;

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ( count = count_spool ( ch, news_list ) ) > 0 ) {
		found = TRUE;
		snprintf ( buf, sizeof ( buf ), "There %s %d new news article%s waiting.\n\r",
				   count > 1 ? "are" : "is", count, count > 1 ? "s" : "" );
		writeBuffer ( buf, ch );
	}
	if ( ( count = count_spool ( ch, changes_list ) ) > 0 ) {
		found = TRUE;
		snprintf ( buf, sizeof ( buf ), "There %s %d change%s waiting to be read.\n\r",
				   count > 1 ? "are" : "is", count, count > 1 ? "s" : "" );
		writeBuffer ( buf, ch );
	}
	if ( ( count = count_spool ( ch, note_list ) ) > 0 ) {
		found = TRUE;
		snprintf ( buf, sizeof ( buf ), "You have %d new note%s waiting.\n\r",
				   count, count > 1 ? "s" : "" );
		writeBuffer ( buf, ch );
	}
	if ( ( count = count_spool ( ch, idea_list ) ) > 0 ) {
		found = TRUE;
		snprintf ( buf, sizeof ( buf ), "You have %d unread idea%s to peruse.\n\r",
				   count, count > 1 ? "s" : "" );
		writeBuffer ( buf, ch );
	}
	if ( IS_TRUSTED ( ch, ANGEL ) && ( count = count_spool ( ch, penalty_list ) ) > 0 ) {
		found = TRUE;
		snprintf ( buf, sizeof ( buf ), "%d %s been added.\n\r",
				   count, count > 1 ? "penalties have" : "penalty has" );
		writeBuffer ( buf, ch );
	}

	if ( !found )
	{ writeBuffer ( "You have no unread notes.\n\r", ch ); }
}

DefineCommand ( cmd_note )
{
	parse_note ( ch, argument, NOTE_NOTE );
}

DefineCommand ( cmd_idea )
{
	parse_note ( ch, argument, NOTE_IDEA );
}

DefineCommand ( cmd_penalty )
{
	parse_note ( ch, argument, NOTE_PENALTY );
}

DefineCommand ( cmd_news )
{
	parse_note ( ch, argument, NOTE_NEWS );
}

DefineCommand ( cmd_changes )
{
	parse_note ( ch, argument, NOTE_CHANGES );
}

void save_notes ( int type )
{
	FILE *fp;
	const char *name;
	NOTE_DATA *pnote;

	switch ( type ) {
		default:
			return;
		case NOTE_NOTE:
			name = NOTE_FILE;
			pnote = note_list;
			break;
		case NOTE_IDEA:
			name = IDEA_FILE;
			pnote = idea_list;
			break;
		case NOTE_PENALTY:
			name = PENALTY_FILE;
			pnote = penalty_list;
			break;
		case NOTE_NEWS:
			name = NEWS_FILE;
			pnote = news_list;
			break;
		case NOTE_CHANGES:
			name = CHANGES_FILE;
			pnote = changes_list;
			break;
	}

	fclose ( fpReserve );
	if ( ( fp = fopen ( name, "w" ) ) == NULL ) {
		ReportErrno ( name );
	} else {
		for ( ; pnote != NULL; pnote = pnote->next ) {
			fprintf ( fp, "Sender  %s~\n", pnote->sender );
			fprintf ( fp, "Date    %s~\n", pnote->date );
			fprintf ( fp, "Stamp   %ld\n", pnote->date_stamp );
			fprintf ( fp, "To      %s~\n", pnote->to_list );
			fprintf ( fp, "Subject %s~\n", pnote->subject );
			fprintf ( fp, "Text\n%s~\n",   pnote->text );
		}
		fclose ( fp );
		fpReserve = fopen ( NULL_FILE, "r" );
		return;
	}
}
void load_notes ( void )
{
	load_thread ( NOTE_FILE, &note_list, NOTE_NOTE, 14 * 24 * 60 * 60 );
	load_thread ( IDEA_FILE, &idea_list, NOTE_IDEA, 28 * 24 * 60 * 60 );
	load_thread ( PENALTY_FILE, &penalty_list, NOTE_PENALTY, 0 );
	load_thread ( NEWS_FILE, &news_list, NOTE_NEWS, 0 );
	load_thread ( CHANGES_FILE, &changes_list, NOTE_CHANGES, 0 );
}

void load_thread ( const char *name, NOTE_DATA **list, int type, time_t recycle_time )
{
	FILE *fp;
	NOTE_DATA *pnotelast;

	if ( ( fp = fopen ( name, "r" ) ) == NULL )
	{ return; }

	pnotelast = NULL;
	for ( ; ; ) {
		NOTE_DATA *pnote;
		char letter;

		do {
			letter = getc ( fp );
			if ( feof ( fp ) ) {
				fclose ( fp );
				return;
			}
		} while ( isspace ( letter ) );
		ungetc ( letter, fp );

		ALLOC_DATA ( pnote, NOTE_DATA, 1 );

		if ( str_cmp ( fread_word ( fp ), "sender" ) )
		{ break; }
		pnote->sender   = fread_string ( fp );

		if ( str_cmp ( fread_word ( fp ), "date" ) )
		{ break; }
		pnote->date     = fread_string ( fp );

		if ( str_cmp ( fread_word ( fp ), "stamp" ) )
		{ break; }
		pnote->date_stamp = fread_number ( fp );

		if ( str_cmp ( fread_word ( fp ), "to" ) )
		{ break; }
		pnote->to_list  = fread_string ( fp );

		if ( str_cmp ( fread_word ( fp ), "subject" ) )
		{ break; }
		pnote->subject  = fread_string ( fp );

		if ( str_cmp ( fread_word ( fp ), "text" ) )
		{ break; }
		pnote->text     = fread_string ( fp );

		if ( recycle_time && pnote->date_stamp < current_time - recycle_time ) {
			recycle_note ( pnote );
			continue;
		}

		pnote->type = type;

		if ( *list == NULL )
		{ *list           = pnote; }
		else
		{ pnotelast->next     = pnote; }

		pnotelast       = pnote;
	}

	strcpy ( strArea, NOTE_FILE );
	fpArea = fp;
	log_hd ( LOG_ERROR, "Load_notes: bad key word." );
	exit ( 1 );
	return;
}

void append_note ( NOTE_DATA *pnote )
{
	FILE *fp;
	const char *name;
	NOTE_DATA **list;
	NOTE_DATA *last;

	switch ( pnote->type ) {
		default:
			return;
		case NOTE_NOTE:
			name = NOTE_FILE;
			list = &note_list;
			break;
		case NOTE_IDEA:
			name = IDEA_FILE;
			list = &idea_list;
			break;
		case NOTE_PENALTY:
			name = PENALTY_FILE;
			list = &penalty_list;
			break;
		case NOTE_NEWS:
			name = NEWS_FILE;
			list = &news_list;
			break;
		case NOTE_CHANGES:
			name = CHANGES_FILE;
			list = &changes_list;
			break;
	}

	if ( *list == NULL )
	{ *list = pnote; }
	else {
		for ( last = *list; last->next != NULL; last = last->next );
		last->next = pnote;
	}

	fclose ( fpReserve );
	if ( ( fp = fopen ( name, "a" ) ) == NULL ) {
		ReportErrno ( name );
	} else {
		fprintf ( fp, "Sender  %s~\n", pnote->sender );
		fprintf ( fp, "Date    %s~\n", pnote->date );
		fprintf ( fp, "Stamp   %ld\n", pnote->date_stamp );
		fprintf ( fp, "To      %s~\n", pnote->to_list );
		fprintf ( fp, "Subject %s~\n", pnote->subject );
		fprintf ( fp, "Text\n%s~\n", pnote->text );
		fclose ( fp );
	}
	fpReserve = fopen ( NULL_FILE, "r" );
}

bool is_note_to ( Creature *ch, NOTE_DATA *pnote )
{
	if ( !str_cmp ( ch->name, pnote->sender ) )
	{ return TRUE; }

	if ( is_exact_name ( "all", pnote->to_list ) )
	{ return TRUE; }

	if ( IsStaff ( ch ) && is_exact_name ( "immortal", pnote->to_list ) )
	{ return TRUE; }

	if ( ch->clan && is_exact_name ( ( char * ) clan_table[ch->clan].name, pnote->to_list ) )
	{ return TRUE; }

	if ( is_exact_name ( ch->name, pnote->to_list ) )
	{ return TRUE; }

	return FALSE;
}



void note_attach ( Creature *ch, int type )
{
	NOTE_DATA *pnote;

	if ( ch->pnote != NULL )
	{ return; }

	pnote = new_note();

	pnote->next		= NULL;
	pnote->sender	= assign_string ( ch->name );
	pnote->date		= assign_string ( "" );
	pnote->to_list	= assign_string ( "" );
	pnote->subject	= assign_string ( "" );
	pnote->text		= assign_string ( "" );
	pnote->type		= type;
	ch->pnote		= pnote;
	return;
}



void note_remove ( Creature *ch, NOTE_DATA *pnote, bool idelete )
{
	char to_new[MAX_INPUT_LENGTH];
	char to_one[MAX_INPUT_LENGTH];
	NOTE_DATA *prev;
	NOTE_DATA **list;
	char *to_list;

	if ( !idelete ) {
		/* make a new list */
		to_new[0]	= '\0';
		to_list	= pnote->to_list;
		while ( *to_list != '\0' ) {
			to_list	= ChopC ( to_list, to_one );
			if ( to_one[0] != '\0' && str_cmp ( ch->name, to_one ) ) {
				strcat ( to_new, " " );
				strcat ( to_new, to_one );
			}
		}
		/* Just a simple recipient removal? */
		if ( str_cmp ( ch->name, pnote->sender ) && to_new[0] != '\0' ) {
			PURGE_DATA ( pnote->to_list );
			pnote->to_list = assign_string ( to_new + 1 );
			return;
		}
	}
	/* nuke the whole note */

	switch ( pnote->type ) {
		default:
			return;
		case NOTE_NOTE:
			list = &note_list;
			break;
		case NOTE_IDEA:
			list = &idea_list;
			break;
		case NOTE_PENALTY:
			list = &penalty_list;
			break;
		case NOTE_NEWS:
			list = &news_list;
			break;
		case NOTE_CHANGES:
			list = &changes_list;
			break;
	}

	/*
	 * Remove note from linked list.
	 */
	if ( pnote == *list ) {
		*list = pnote->next;
	} else {
		for ( prev = *list; prev != NULL; prev = prev->next ) {
			if ( prev->next == pnote )
			{ break; }
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Note_remove: pnote not found." );
			return;
		}

		prev->next = pnote->next;
	}

	save_notes ( pnote->type );
	recycle_note ( pnote );
	return;
}

bool hide_note ( Creature *ch, NOTE_DATA *pnote )
{
	time_t last_read;

	if ( IS_NPC ( ch ) )
	{ return TRUE; }

	switch ( pnote->type ) {
		default:
			return TRUE;
		case NOTE_NOTE:
			last_read = ch->pcdata->last_note;
			break;
		case NOTE_IDEA:
			last_read = ch->pcdata->last_idea;
			break;
		case NOTE_PENALTY:
			last_read = ch->pcdata->last_penalty;
			break;
		case NOTE_NEWS:
			last_read = ch->pcdata->last_news;
			break;
		case NOTE_CHANGES:
			last_read = ch->pcdata->last_changes;
			break;
	}

	if ( pnote->date_stamp <= last_read )
	{ return TRUE; }

	if ( !str_cmp ( ch->name, pnote->sender ) )
	{ return TRUE; }

	if ( !is_note_to ( ch, pnote ) )
	{ return TRUE; }

	return FALSE;
}

void update_read ( Creature *ch, NOTE_DATA *pnote )
{
	time_t stamp;

	if ( IS_NPC ( ch ) )
	{ return; }

	stamp = pnote->date_stamp;

	switch ( pnote->type ) {
		default:
			return;
		case NOTE_NOTE:
			ch->pcdata->last_note = UMAX ( ch->pcdata->last_note, stamp );
			break;
		case NOTE_IDEA:
			ch->pcdata->last_idea = UMAX ( ch->pcdata->last_idea, stamp );
			break;
		case NOTE_PENALTY:
			ch->pcdata->last_penalty = UMAX ( ch->pcdata->last_penalty, stamp );
			break;
		case NOTE_NEWS:
			ch->pcdata->last_news = UMAX ( ch->pcdata->last_news, stamp );
			break;
		case NOTE_CHANGES:
			ch->pcdata->last_changes = UMAX ( ch->pcdata->last_changes, stamp );
			break;
	}
}

void parse_note ( Creature *ch, const char *argument, int type )
{
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	NOTE_DATA *pnote;
	NOTE_DATA **list;
	const char *list_name;
	int vnum;
	int anum;

	if ( IS_NPC ( ch ) )
	{ return; }

	switch ( type ) {
		default:
			return;
		case NOTE_NOTE:
			list = &note_list;
			list_name = "notes";
			break;
		case NOTE_IDEA:
			list = &idea_list;
			list_name = "ideas";
			break;
		case NOTE_PENALTY:
			list = &penalty_list;
			list_name = "penalties";
			break;
		case NOTE_NEWS:
			list = &news_list;
			list_name = "news";
			break;
		case NOTE_CHANGES:
			list = &changes_list;
			list_name = "changes";
			break;
	}

	argument = ChopC ( argument, arg );
	smash_tilde ( argument );

	if ( arg[0] == '\0' || !str_prefix ( arg, "read" ) ) {
		bool fAll;

		if ( !str_cmp ( argument, "all" ) ) {
			fAll = TRUE;
			anum = 0;
		}

		else if ( argument[0] == '\0' || !str_prefix ( argument, "next" ) )
			/* read next unread note */
		{
			vnum = 0;
			for ( pnote = *list; pnote != NULL; pnote = pnote->next ) {
				if ( !hide_note ( ch, pnote ) ) {
					sprintf ( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
							  vnum,
							  pnote->sender,
							  pnote->subject,
							  pnote->date,
							  pnote->to_list );
					writeBuffer ( buf, ch );
					writePage ( pnote->text, ch );
					update_read ( ch, pnote );
					return;
				} else if ( is_note_to ( ch, pnote ) )
				{ vnum++; }
			}
			snprintf ( buf, sizeof ( buf ), "You have no unread %s.\n\r", list_name );
			writeBuffer ( buf, ch );
			return;
		}

		else if ( is_number ( argument ) ) {
			fAll = FALSE;
			anum = atoi ( argument );
		} else {
			writeBuffer ( "Read which number?\n\r", ch );
			return;
		}

		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next ) {
			if ( is_note_to ( ch, pnote ) && ( vnum++ == anum || fAll ) ) {
				sprintf ( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
						  vnum - 1,
						  pnote->sender,
						  pnote->subject,
						  pnote->date,
						  pnote->to_list
						);
				writeBuffer ( buf, ch );
				writePage ( pnote->text, ch );
				update_read ( ch, pnote );
				return;
			}
		}

		snprintf ( buf, sizeof ( buf ), "There aren't that many %s.\n\r", list_name );
		writeBuffer ( buf, ch );
		return;
	}

	if ( !str_prefix ( arg, "list" ) ) {
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next ) {
			if ( is_note_to ( ch, pnote ) ) {
				sprintf ( buf, "[%3d%s] %s: %s\n\r",
						  vnum, hide_note ( ch, pnote ) ? " " : "N",
						  pnote->sender, pnote->subject );
				writeBuffer ( buf, ch );
				vnum++;
			}
		}
		if ( !vnum ) {
			switch ( type ) {
				case NOTE_NOTE:
					writeBuffer ( "There are no notes for you.\n\r", ch );
					break;
				case NOTE_IDEA:
					writeBuffer ( "There are no ideas for you.\n\r", ch );
					break;
				case NOTE_PENALTY:
					writeBuffer ( "There are no penalties for you.\n\r", ch );
					break;
				case NOTE_NEWS:
					writeBuffer ( "There is no news for you.\n\r", ch );
					break;
				case NOTE_CHANGES:
					writeBuffer ( "There are no changes for you.\n\r", ch );
					break;
			}
		}
		return;
	}

	if ( !str_prefix ( arg, "remove" ) ) {
		if ( !is_number ( argument ) ) {
			writeBuffer ( "Note remove which number?\n\r", ch );
			return;
		}

		anum = atoi ( argument );
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next ) {
			if ( is_note_to ( ch, pnote ) && vnum++ == anum ) {
				note_remove ( ch, pnote, FALSE );
				writeBuffer ( "Ok.\n\r", ch );
				return;
			}
		}

		snprintf ( buf, sizeof ( buf ), "There aren't that many %s.", list_name );
		writeBuffer ( buf, ch );
		return;
	}

	if ( !str_prefix ( arg, "delete" ) && get_trust ( ch ) >= MAX_LEVEL - 1 ) {
		if ( !is_number ( argument ) ) {
			writeBuffer ( "Note delete which number?\n\r", ch );
			return;
		}

		anum = atoi ( argument );
		vnum = 0;
		for ( pnote = *list; pnote != NULL; pnote = pnote->next ) {
			if ( is_note_to ( ch, pnote ) && vnum++ == anum ) {
				note_remove ( ch, pnote, TRUE );
				writeBuffer ( "Ok.\n\r", ch );
				return;
			}
		}

		snprintf ( buf, sizeof ( buf ), "There aren't that many %s.", list_name );
		writeBuffer ( buf, ch );
		return;
	}

	if ( !str_prefix ( arg, "catchup" ) ) {
		switch ( type ) {
			case NOTE_NOTE:
				ch->pcdata->last_note = current_time;
				break;
			case NOTE_IDEA:
				ch->pcdata->last_idea = current_time;
				break;
			case NOTE_PENALTY:
				ch->pcdata->last_penalty = current_time;
				break;
			case NOTE_NEWS:
				ch->pcdata->last_news = current_time;
				break;
			case NOTE_CHANGES:
				ch->pcdata->last_changes = current_time;
				break;
		}
		return;
	}

	/* below this point only certain people can edit notes */
	if ( ( type == NOTE_NEWS && !IS_TRUSTED ( ch, ANGEL ) )
			||  ( type == NOTE_CHANGES && !IS_TRUSTED ( ch, CREATOR ) ) ) {
		snprintf ( buf, sizeof ( buf ), "You aren't high enough level to write %s.", list_name );
		writeBuffer ( buf, ch );
		return;
	}

	if ( !str_cmp ( arg, "+" ) ) {
		note_attach ( ch, type );
		if ( ch->pnote->type != type ) {
			writeBuffer (
				"You already have a different note in progress.\n\r", ch );
			return;
		}

		if ( strlen ( ch->pnote->text ) + strlen ( argument ) >= 4096 ) {
			writeBuffer ( "Note too long.\n\r", ch );
			return;
		}

		buffer = new_buf();

		add_buf ( buffer, ch->pnote->text );
		add_buf ( buffer, argument );
		add_buf ( buffer, "\n\r" );
		PURGE_DATA ( ch->pnote->text );
		ch->pnote->text = assign_string ( buf_string ( buffer ) );
		recycle_buf ( buffer );
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "-" ) ) {
		int len;
		bool found = FALSE;

		note_attach ( ch, type );
		if ( ch->pnote->type != type ) {
			writeBuffer (
				"You already have a different note in progress.\n\r", ch );
			return;
		}

		if ( ch->pnote->text == NULL || ch->pnote->text[0] == '\0' ) {
			writeBuffer ( "No lines left to remove.\n\r", ch );
			return;
		}

		strcpy ( buf, ch->pnote->text );

		for ( len = strlen ( buf ); len > 0; len-- ) {
			if ( buf[len] == '\r' ) {
				if ( !found ) { /* back it up */
					if ( len > 0 )
					{ len--; }
					found = TRUE;
				} else { /* found the second one */
					buf[len + 1] = '\0';
					PURGE_DATA ( ch->pnote->text );
					ch->pnote->text = assign_string ( buf );
					return;
				}
			}
		}
		buf[0] = '\0';
		PURGE_DATA ( ch->pnote->text );
		ch->pnote->text = assign_string ( buf );
		return;
	}

	if ( !str_prefix ( arg, "subject" ) ) {
		note_attach ( ch, type );
		if ( ch->pnote->type != type ) {
			writeBuffer (
				"You already have a different note in progress.\n\r", ch );
			return;
		}

		PURGE_DATA ( ch->pnote->subject );
		ch->pnote->subject = assign_string ( argument );
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "to" ) ) {
		note_attach ( ch, type );
		if ( ch->pnote->type != type ) {
			writeBuffer (
				"You already have a different note in progress.\n\r", ch );
			return;
		}
		PURGE_DATA ( ch->pnote->to_list );
		ch->pnote->to_list = assign_string ( argument );
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "clear" ) ) {
		if ( ch->pnote != NULL ) {
			recycle_note ( ch->pnote );
			ch->pnote = NULL;
		}

		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "show" ) ) {
		if ( ch->pnote == NULL ) {
			writeBuffer ( "You have no note in progress.\n\r", ch );
			return;
		}

		if ( ch->pnote->type != type ) {
			writeBuffer ( "You aren't working on that kind of note.\n\r", ch );
			return;
		}

		sprintf ( buf, "%s: %s\n\rTo: %s\n\r",
				  ch->pnote->sender,
				  ch->pnote->subject,
				  ch->pnote->to_list
				);
		writeBuffer ( buf, ch );
		writeBuffer ( ch->pnote->text, ch );
		return;
	}

	if ( !str_prefix ( arg, "post" ) || !str_prefix ( arg, "send" ) ) {
		char *strtime;

		if ( ch->pnote == NULL ) {
			writeBuffer ( "You have no note in progress.\n\r", ch );
			return;
		}

		if ( ch->pnote->type != type ) {
			writeBuffer ( "You aren't working on that kind of note.\n\r", ch );
			return;
		}

		if ( !str_cmp ( ch->pnote->to_list, "" ) ) {
			writeBuffer (
				"You need to provide a recipient (name, all, or immortal).\n\r",
				ch );
			return;
		}

		if ( !str_cmp ( ch->pnote->subject, "" ) ) {
			writeBuffer ( "You need to provide a subject.\n\r", ch );
			return;
		}

		ch->pnote->next			= NULL;
		strtime				= ctime ( &current_time );
		strtime[strlen ( strtime ) - 1]	= '\0';
		ch->pnote->date			= assign_string ( strtime );
		ch->pnote->date_stamp		= current_time;

		append_note ( ch->pnote );
		ch->pnote = NULL;
		return;
	}

	writeBuffer ( "You can't do that.\n\r", ch );
	return;
}

