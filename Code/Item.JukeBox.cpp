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

int channel_songs[MAX_GLOBAL + 1];
struct song_data song_table[MAX_SONGS];

void song_update ( void )
{
	Item *obj;
	Creature *victim;
	RoomData *room;
	Socket *d;
	char buf[MAX_STRING_LENGTH];
	char *line;
	int i;

	/* do the global song, if any */
	if ( channel_songs[1] >= MAX_SONGS )
	{ channel_songs[1] = -1; }

	if ( channel_songs[1] > -1 ) {
		if ( channel_songs[0] >= MAX_LINES
				||  channel_songs[0] >= song_table[channel_songs[1]].lines ) {
			channel_songs[0] = -1;

			/* advance songs */
			for ( i = 1; i < MAX_GLOBAL; i++ )
			{ channel_songs[i] = channel_songs[i + 1]; }
			channel_songs[MAX_GLOBAL] = -1;
		} else {
			if ( channel_songs[0] < 0 ) {
				snprintf ( buf, sizeof ( buf ), "Music: %s, %s",
						   song_table[channel_songs[1]].group,
						   song_table[channel_songs[1]].name );
				channel_songs[0] = 0;
			} else {
				snprintf ( buf, sizeof ( buf ), "Music: '%s'",
						   song_table[channel_songs[1]].lyrics[channel_songs[0]] );
				channel_songs[0]++;
			}

			for ( d = socket_list; d != NULL; d = d->next ) {
				victim = d->original ? d->original : d->character;

				if ( d->connected == STATE_PLAYING &&
						!IS_SET ( victim->comm, COMM_NOMUSIC ) &&
						!IS_SET ( victim->comm, COMM_QUIET ) )
				{ act_new ( "$t", d->character, buf, NULL, TO_CHAR, POS_SLEEPING ); }
			}
		}
	}

	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		if ( obj->item_type != ITEM_JUKEBOX || obj->value[1] < 0 )
		{ continue; }

		if ( obj->value[1] >= MAX_SONGS ) {
			obj->value[1] = -1;
			continue;
		}

		/* find which room to play in */

		if ( ( room = obj->in_room ) == NULL ) {
			if ( CARRIED_BY ( obj ) == NULL )
			{ continue; }
			else if ( ( room = CARRIED_BY ( obj )->in_room ) == NULL )
			{ continue; }
		}

		if ( obj->value[0] < 0 ) {
			snprintf ( buf, sizeof ( buf ), "$p starts playing %s, %s.",
					   song_table[obj->value[1]].group, song_table[obj->value[1]].name );
			if ( room->people != NULL )
			{ act ( buf, room->people, obj, NULL, TO_ALL ); }
			obj->value[0] = 0;
			continue;
		} else {
			if ( obj->value[0] >= MAX_LINES
					|| obj->value[0] >= song_table[obj->value[1]].lines ) {

				obj->value[0] = -1;

				/* scroll songs forward */
				obj->value[1] = obj->value[2];
				obj->value[2] = obj->value[3];
				obj->value[3] = obj->value[4];
				obj->value[4] = -1;
				continue;
			}

			line = song_table[obj->value[1]].lyrics[obj->value[0]];
			obj->value[0]++;
		}

		snprintf ( buf, sizeof ( buf ), "$p bops: '%s'", line );
		if ( room->people != NULL )
		{ act ( buf, room->people, obj, NULL, TO_ALL ); }
	}
}



void load_songs ( void )
{
	FILE *fp;
	int count = 0, lines, i;
	char letter;

	/* reset global */
	for ( i = 0; i <= MAX_GLOBAL; i++ )
	{ channel_songs[i] = -1; }

	if ( ( fp = fopen ( MUSIC_FILE, "r" ) ) == NULL ) {
		log_hd ( LOG_ERROR, "Couldn't open music file, no songs available." );
		fclose ( fp );
		return;
	}

	for ( count = 0; count < MAX_SONGS; count++ ) {
		letter = fread_letter ( fp );
		if ( letter == '#' ) {
			if ( count < MAX_SONGS )
			{ song_table[count].name = NULL; }
			fclose ( fp );
			return;
		} else
		{ ungetc ( letter, fp ); }

		song_table[count].group = fread_string ( fp );
		song_table[count].name 	= fread_string ( fp );

		/* read lyrics */
		lines = 0;

		for ( ; ; ) {
			letter = fread_letter ( fp );

			if ( letter == '~' ) {
				song_table[count].lines = lines;
				break;
			} else
			{ ungetc ( letter, fp ); }

			if ( lines >= MAX_LINES ) {
				log_hd ( LOG_ERROR, Format ( "Too many lines in a song -- limit is  %d.", MAX_LINES ) );
				break;
			}

			song_table[count].lyrics[lines] = fread_string_eol ( fp );
			lines++;
		}
	}
}

DefineCommand ( cmd_play )
{
	Item *juke;
	char *str, arg[MAX_INPUT_LENGTH];
	int song, i;
	bool global = FALSE;

	str = ChopC ( argument, arg );

	for ( juke = IN_ROOM ( ch )->contents; juke != NULL; juke = juke->next_content )
		if ( juke->item_type == ITEM_JUKEBOX && can_see_obj ( ch, juke ) )
		{ break; }

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Play what?\n\r", ch );
		return;
	}

	if ( juke == NULL ) {
		writeBuffer ( "You see nothing to play.\n\r", ch );
		return;
	}

	if ( SameString ( arg, "list" ) ) {
		BUFFER *buffer;
		char buf[MAX_STRING_LENGTH];
		int col = 0;
		bool artist = FALSE, match = FALSE;

		buffer = new_buf();
		argument = str;
		argument = ChopC ( argument, arg );

		if ( SameString ( arg, "artist" ) )
		{ artist = TRUE; }

		if ( argument[0] != '\0' )
		{ match = TRUE; }

		snprintf ( buf, sizeof ( buf ), "%s has the following songs available:\n\r",
				   juke->short_descr );
		add_buf ( buffer, capitalize ( buf ) );

		for ( i = 0; i < MAX_SONGS; i++ ) {
			if ( song_table[i].name == NULL )
			{ break; }

			if ( artist && ( !match
							 || 		   !str_prefix ( argument, song_table[i].group ) ) )
				snprintf ( buf, sizeof ( buf ), "%-39s %-39s\n\r",
						   song_table[i].group, song_table[i].name );
			else if ( !artist && ( !match
								   || 	 		 !str_prefix ( argument, song_table[i].name ) ) )
			{ snprintf ( buf, sizeof ( buf ), "%-35s ", song_table[i].name ); }
			else
			{ continue; }
			add_buf ( buffer, buf );
			if ( !artist && ++col % 2 == 0 )
			{ add_buf ( buffer, "\n\r" ); }
		}
		if ( !artist && col % 2 != 0 )
		{ add_buf ( buffer, "\n\r" ); }

		writePage ( buf_string ( buffer ), ch );
		recycle_buf ( buffer );
		return;
	}

	if ( SameString ( arg, "loud" ) ) {
		argument = str;
		global = TRUE;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Play what?\n\r", ch );
		return;
	}

	if ( ( global && channel_songs[MAX_GLOBAL] > -1 )
			||  ( !global && juke->value[4] > -1 ) ) {
		writeBuffer ( "The jukebox is full up right now.\n\r", ch );
		return;
	}

	for ( song = 0; song < MAX_SONGS; song++ ) {
		if ( song_table[song].name == NULL ) {
			writeBuffer ( "That song isn't available.\n\r", ch );
			return;
		}
		if ( !str_prefix ( argument, song_table[song].name ) )
		{ break; }
	}

	if ( song >= MAX_SONGS ) {
		writeBuffer ( "That song isn't available.\n\r", ch );
		return;
	}

	writeBuffer ( "Coming right up.\n\r", ch );

	if ( global ) {
		for ( i = 1; i <= MAX_GLOBAL; i++ )
			if ( channel_songs[i] < 0 ) {
				if ( i == 1 )
				{ channel_songs[0] = -1; }
				channel_songs[i] = song;
				return;
			}
	} else {
		for ( i = 1; i < 5; i++ )
			if ( juke->value[i] < 0 ) {
				if ( i == 1 )
				{ juke->value[0] = -1; }
				juke->value[i] = song;
				return;
			}
	}
}
