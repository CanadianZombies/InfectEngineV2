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

char *string_linedel ( char *, int );
char *string_lineadd ( char *, char *, int );
char *numlineas ( char * );

/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit ( Creature *ch, char **pString )
{
	writeBuffer ( "-========- Entering EDIT Mode -=========-\n\r", ch );
	writeBuffer ( "    Type .h on a new line for help\n\r", ch );
	writeBuffer ( " Terminate with a ~ or @ on a blank line.\n\r", ch );
	writeBuffer ( "-=======================================-\n\r", ch );

	if ( *pString == NULL ) {
		*pString = assign_string ( "" );
	} else {
		**pString = '\0';
	}

	ch->desc->pString = pString;

	return;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append ( Creature *ch, char **pString )
{
	writeBuffer ( "-=======- Entering APPEND Mode -========-\n\r", ch );
	writeBuffer ( "    Type .h on a new line for help\n\r", ch );
	writeBuffer ( " Terminate with a ~ or @ on a blank line.\n\r", ch );
	writeBuffer ( "-=======================================-\n\r", ch );

	if ( *pString == NULL ) {
		*pString = assign_string ( "" );
	}
	writeBuffer ( numlineas ( *pString ), ch );

	/* numlineas entrega el string con \n\r */
	/*  if ( *(*pString + strlen( *pString ) - 1) != '\r' )
		writeBuffer( "\n\r", ch ); */

	ch->desc->pString = pString;

	return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace ( const char * orig, const char * old, const char * inew )
{
	char xbuf[MAX_STRING_LENGTH];
	int i;

	xbuf[0] = '\0';
	strcpy ( xbuf, orig );
	if ( strstr ( orig, old ) != NULL ) {
		i = strlen ( orig ) - strlen ( strstr ( orig, old ) );
		xbuf[i] = '\0';
		strcat ( xbuf, inew );
		strcat ( xbuf, &orig[i + strlen ( old )] );
		PURGE_DATA ( orig );
	}

	return assign_string ( xbuf );
}



/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add ( Creature *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	/*
	 * Thanks to James Seng
	 */
	smash_tilde ( argument );

	if ( *argument == '.' ) {
		char arg1 [MAX_INPUT_LENGTH];
		char arg2 [MAX_INPUT_LENGTH];
		char arg3 [MAX_INPUT_LENGTH];
		char tmparg3 [MAX_INPUT_LENGTH];

		argument = one_argument ( argument, arg1 );
		argument = first_arg ( argument, arg2, FALSE );
		strcpy ( tmparg3, argument );
		argument = first_arg ( argument, arg3, FALSE );

		if ( !str_cmp ( arg1, ".c" ) ) {
			writeBuffer ( "String cleared.\n\r", ch );
			PURGE_DATA ( *ch->desc->pString );
			*ch->desc->pString = assign_string ( "" );
			return;
		}

		if ( !str_cmp ( arg1, ".s" ) ) {
			writeBuffer ( "String so far:\n\r", ch );
			writeBuffer ( numlineas ( *ch->desc->pString ), ch );
			return;
		}

		if ( !str_cmp ( arg1, ".r" ) ) {
			if ( arg2[0] == '\0' ) {
				writeBuffer (
					"usage:  .r \"old string\" \"new string\"\n\r", ch );
				return;
			}

			*ch->desc->pString =
				string_replace ( *ch->desc->pString, arg2, arg3 );
			sprintf ( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
			writeBuffer ( buf, ch );
			return;
		}

		if ( !str_cmp ( arg1, ".f" ) ) {
			*ch->desc->pString = format_string ( *ch->desc->pString );
			writeBuffer ( "String formatted.\n\r", ch );
			return;
		}

		if ( !str_cmp ( arg1, ".ld" ) ) {
			*ch->desc->pString = string_linedel ( *ch->desc->pString, atoi ( arg2 ) );
			writeBuffer ( "Line deleted.\n\r", ch );
			return;
		}

		if ( !str_cmp ( arg1, ".li" ) ) {
			*ch->desc->pString = string_lineadd ( *ch->desc->pString, tmparg3, atoi ( arg2 ) );
			writeBuffer ( "Line inserted.\n\r", ch );
			return;
		}

		if ( !str_cmp ( arg1, ".lr" ) ) {
			*ch->desc->pString = string_linedel ( *ch->desc->pString, atoi ( arg2 ) );
			*ch->desc->pString = string_lineadd ( *ch->desc->pString, tmparg3, atoi ( arg2 ) );
			writeBuffer ( "Line replaced.\n\r", ch );
			return;
		}

		if ( !str_cmp ( arg1, ".h" ) ) {
			writeBuffer ( "Sedit help (commands on blank line):   \n\r", ch );
			writeBuffer ( ".r 'old' 'new'   - replace a substring \n\r", ch );
			writeBuffer ( "                   (requires '', \"\") \n\r", ch );
			writeBuffer ( ".h               - get help (this info)\n\r", ch );
			writeBuffer ( ".s               - show string so far  \n\r", ch );
			writeBuffer ( ".f               - (word wrap) string  \n\r", ch );
			writeBuffer ( ".c               - clear string so far \n\r", ch );
			writeBuffer ( ".ld <num>        - delete line number <num>\n\r", ch );
			writeBuffer ( ".li <num> <str>  - insert <str> on line <num>\n\r", ch );
			writeBuffer ( ".lr <num> <str>  - replace line <num> with <str>\n\r", ch );
			writeBuffer ( "@                - end string          \n\r", ch );
			return;
		}

		writeBuffer ( "SEdit:  Invalid dot command.\n\r", ch );
		return;
	}

	if ( *argument == '~' || *argument == '@' ) {
		if ( ch->desc->editor == ED_MPCODE ) { /* para los mobprogs */
			NPCData *mob;
			int hash;
			MPROG_LIST *mpl;
			MPROG_CODE *mpc;

			EDIT_MPCODE ( ch, mpc );

			if ( mpc != NULL )
				for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
					for ( mob = mob_index_hash[hash]; mob; mob = mob->next )
						for ( mpl = mob->mprogs; mpl; mpl = mpl->next )
							if ( mpl->vnum == mpc->vnum ) {
								sprintf ( buf, "Updated mob %d.\n\r", mob->vnum );
								writeBuffer ( buf, ch );
								mpl->code = mpc->code;
							}
		}

		ch->desc->pString = NULL;
		return;
	}

	strcpy ( buf, *ch->desc->pString );

	/*
	 * Truncate strings to MAX_STRING_LENGTH.
	 * --------------------------------------
	 */
	if ( strlen ( buf ) + strlen ( argument ) >= ( MAX_STRING_LENGTH - 4 ) ) {
		writeBuffer ( "String too long, last line skipped.\n\r", ch );

		/* Force character out of editing mode. */
		ch->desc->pString = NULL;
		return;
	}

	/*
	 * Ensure no tilde's inside string.
	 * --------------------------------
	 */
	smash_tilde ( argument );

	strcat ( buf, argument );
	strcat ( buf, "\n\r" );
	PURGE_DATA ( *ch->desc->pString );
	*ch->desc->pString = assign_string ( buf );
	return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string ( char *oldstring /*, bool fSpace */ )
{
	char xbuf[MAX_STRING_LENGTH];
	char xbuf2[MAX_STRING_LENGTH];
	char *rdesc;
	int i = 0;
	bool cap = TRUE;

	xbuf[0] = xbuf2[0] = 0;

	i = 0;

	for ( rdesc = oldstring; *rdesc; rdesc++ ) {
		if ( *rdesc == '\n' ) {
			if ( xbuf[i - 1] != ' ' ) {
				xbuf[i] = ' ';
				i++;
			}
		} else if ( *rdesc == '\r' ) ;
		else if ( *rdesc == ' ' ) {
			if ( xbuf[i - 1] != ' ' ) {
				xbuf[i] = ' ';
				i++;
			}
		} else if ( *rdesc == ')' ) {
			if ( xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' &&
					( xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!' ) ) {
				xbuf[i - 2] = *rdesc;
				xbuf[i - 1] = ' ';
				xbuf[i] = ' ';
				i++;
			} else {
				xbuf[i] = *rdesc;
				i++;
			}
		} else if ( *rdesc == '.' || *rdesc == '?' || *rdesc == '!' ) {
			if ( xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' &&
					( xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!' ) ) {
				xbuf[i - 2] = *rdesc;
				if ( * ( rdesc + 1 ) != '\"' ) {
					xbuf[i - 1] = ' ';
					xbuf[i] = ' ';
					i++;
				} else {
					xbuf[i - 1] = '\"';
					xbuf[i] = ' ';
					xbuf[i + 1] = ' ';
					i += 2;
					rdesc++;
				}
			} else {
				xbuf[i] = *rdesc;
				if ( * ( rdesc + 1 ) != '\"' ) {
					xbuf[i + 1] = ' ';
					xbuf[i + 2] = ' ';
					i += 3;
				} else {
					xbuf[i + 1] = '\"';
					xbuf[i + 2] = ' ';
					xbuf[i + 3] = ' ';
					i += 4;
					rdesc++;
				}
			}
			cap = TRUE;
		} else {
			xbuf[i] = *rdesc;
			if ( cap ) {
				cap = FALSE;
				xbuf[i] = UPPER ( xbuf[i] );
			}
			i++;
		}
	}
	xbuf[i] = 0;
	strcpy ( xbuf2, xbuf );

	rdesc = xbuf2;

	xbuf[0] = 0;

	for ( ; ; ) {
		for ( i = 0; i < 77; i++ ) {
			if ( !* ( rdesc + i ) ) { break; }
		}
		if ( i < 77 ) {
			break;
		}
		for ( i = ( xbuf[0] ? 76 : 73 ) ; i ; i-- ) {
			if ( * ( rdesc + i ) == ' ' ) { break; }
		}
		if ( i ) {
			* ( rdesc + i ) = 0;
			strcat ( xbuf, rdesc );
			strcat ( xbuf, "\n\r" );
			rdesc += i + 1;
			while ( *rdesc == ' ' ) { rdesc++; }
		} else {
			log_hd ( LOG_ERROR, Format ( "%s: No spaces", __PRETTY_FUNCTION__ ) );
			* ( rdesc + 75 ) = 0;
			strcat ( xbuf, rdesc );
			strcat ( xbuf, "-\n\r" );
			rdesc += 76;
		}
	}
	while ( * ( rdesc + i ) && ( * ( rdesc + i ) == ' ' ||
								 * ( rdesc + i ) == '\n' ||
								 * ( rdesc + i ) == '\r' ) )
	{ i--; }
	* ( rdesc + i + 1 ) = 0;
	strcat ( xbuf, rdesc );
	if ( xbuf[strlen ( xbuf ) - 2] != '\n' )
	{ strcat ( xbuf, "\n\r" ); }

	PURGE_DATA ( oldstring );
	return ( assign_string ( xbuf ) );
}



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg ( char *argument, char *arg_first, bool fCase )
{
	char cEnd;

	while ( *argument == ' ' )
	{ argument++; }

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"'
			|| *argument == '%'  || *argument == '(' ) {
		if ( *argument == '(' ) {
			cEnd = ')';
			argument++;
		} else { cEnd = *argument++; }
	}

	while ( *argument != '\0' ) {
		if ( *argument == cEnd ) {
			argument++;
			break;
		}
		if ( fCase ) { *arg_first = LOWER ( *argument ); }
		else { *arg_first = *argument; }
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while ( *argument == ' ' )
	{ argument++; }

	return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad ( char * argument )
{
	char buf[MAX_STRING_LENGTH];
	char *s;

	s = argument;

	while ( *s == ' ' )
	{ s++; }

	strcpy ( buf, s );
	s = buf;

	if ( *s != '\0' ) {
		while ( *s != '\0' )
		{ s++; }
		s--;

		while ( *s == ' ' )
		{ s--; }
		s++;
		*s = '\0';
	}

	PURGE_DATA ( argument );
	return assign_string ( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper ( char * argument )
{
	char *s;

	s = argument;

	while ( *s != '\0' ) {
		if ( *s != ' ' ) {
			*s = UPPER ( *s );
			while ( *s != ' ' && *s != '\0' )
			{ s++; }
		} else {
			s++;
		}
	}

	return argument;
}

char *string_linedel ( char *string, int line )
{
	char *strtmp = string;
	char buf[MAX_STRING_LENGTH];
	int cnt = 1, tmp = 0;

	buf[0] = '\0';

	for ( ; *strtmp != '\0'; strtmp++ ) {
		if ( cnt != line )
		{ buf[tmp++] = *strtmp; }

		if ( *strtmp == '\n' ) {
			if ( * ( strtmp + 1 ) == '\r' ) {
				if ( cnt != line )
				{ buf[tmp++] = * ( ++strtmp ); }
				else
				{ ++strtmp; }
			}

			cnt++;
		}
	}

	buf[tmp] = '\0';

	PURGE_DATA ( string );
	return assign_string ( buf );
}

char *string_lineadd ( char *string, char *newstr, int line )
{
	char *strtmp = string;
	int cnt = 1, tmp = 0;
	bool done = FALSE;
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	for ( ; *strtmp != '\0' || ( !done && cnt == line ); strtmp++ ) {
		if ( cnt == line && !done ) {
			strcat ( buf, newstr );
			strcat ( buf, "\n\r" );
			tmp += strlen ( newstr ) + 2;
			cnt++;
			done = TRUE;
		}

		buf[tmp++] = *strtmp;

		if ( done && *strtmp == '\0' )
		{ break; }

		if ( *strtmp == '\n' ) {
			if ( * ( strtmp + 1 ) == '\r' )
			{ buf[tmp++] = * ( ++strtmp ); }

			cnt++;
		}

		buf[tmp] = '\0';
	}

	PURGE_DATA ( string );
	return assign_string ( buf );
}

/* buf queda con la linea sin \n\r */
char *lgetline ( char *str, char *buf )
{
	int tmp = 0;
	bool found = FALSE;

	while ( *str ) {
		if ( *str == '\n' ) {
			found = TRUE;
			break;
		}

		buf[tmp++] = * ( str++ );
	}

	if ( found ) {
		if ( * ( str + 1 ) == '\r' )
		{ str += 2; }
		else
		{ str += 1; }
	} /* para que quedemos en el inicio de la prox linea */

	buf[tmp] = '\0';

	return str;
}

char *numlineas ( char *string )
{
	int cnt = 1;
	static char buf[MAX_STRING_LENGTH * 2];
	char buf2[MAX_STRING_LENGTH], tmpb[MAX_STRING_LENGTH];

	buf[0] = '\0';

	while ( *string ) {
		string = lgetline ( string, tmpb );
		sprintf ( buf2, "%2d. %s\n\r", cnt++, tmpb );
		strcat ( buf, buf2 );
	}

	return buf;
}
