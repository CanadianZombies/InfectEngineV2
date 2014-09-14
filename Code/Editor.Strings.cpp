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

int colour_strlen ( const std::string &txt );
char *StringEditor_DeleteLine ( char *, int );
char *StringEditor_AddLine ( char *, char *, int );
const char *StringEditor_NumberLines ( const char * );
const char *StringEditor_GetLine ( const char *str, char *line );
const char *StringEditor_FormatStringLines ( const char *oldstring, int line1, int line2 );
bool StringEditor_CheckPunctuation ( const char *str );

char *assign_string ( const std::string &str )
{
	static char *ret;

	if ( str.empty() ) {
		return NULL;
	}

	// -- generate our string with +1 to account for \0 (null space) at the end.
	ret = new char[str.length() + 1];
	strncpy ( ret, C_STR ( str ), str.length() + 1 );

	return ret;
}

// -- smash the tilde
void smash_tilde ( const char *str )
{
	char *mstr = ( char * ) str;

	for ( ; *mstr != '\0'; mstr++ ) {
		if ( *mstr == '~' )
		{ *mstr = '-'; }
	}
	str = mstr;
}

// remove_tilde by David Simmerson
std::string remove_tilde ( std::string str )
{
	std::string outstr;
	for ( int x = 0; str[x] != '\0'; x++ ) {
		if ( str[x] == '~' )
		{ outstr.append ( "-" ); }
		else {
			char b[10];
			snprintf ( b, 10, "%c", str[x] );
			outstr.append ( b );
		}
	}

	return outstr;
}

std::string amp_fix ( const std::string &str )
{
	std::string outstr;

	for ( int x = 0; str[x] != '\0'; x++ ) {
		if ( str[x] == '&' )
		{ outstr.append ( "&&" ); }
		else {
			char b[10];
			snprintf ( b, 10, "%c", str[x] );
			outstr.append ( b );
		}
	}

	return outstr;
}


std::string remove_char ( std::string str, char v )
{
	std::string outstr;
	for ( int x = 0; str[x] != '\0'; x++ ) {
		if ( str[x] != v ) {
			char b[10];
			snprintf ( b, 10, "%c", str[x] );
			outstr.append ( b );
		}
	}

	return outstr;
}

// -- retrieve a line count
int string_linecount ( const char *str )
{
	char *s;
	int  cnt = 0;

	for ( s = ( char * ) str; *s != '\0'; ) {
		if ( *s++ == '\n' )
		{ cnt++; }
	}


	return cnt;
}

char *get_line ( char *str, char *buf )
{
	int tmp = 0;
	bool found = false;

	while ( *str ) {
		if ( *str == '\n' ) {
			found = true;
			break;
		}

		buf[tmp++] = * ( str++ );
	}

	if ( found ) {
		if ( * ( str + 1 ) == '\r' )
		{ str += 2; }
		else
		{ str += 1; }
	}

	buf[tmp] = '\0';

	return str;
}

const char *StringEditor_NumberLines ( const char *sstring )
{
	int cnt = 1;
	static char buf[MAX_STRING_LENGTH] = {'\0'};
	char buf2[MAX_STRING_LENGTH] = {'\0'};
	char tmpb[MAX_STRING_LENGTH] = {'\0'};

	buf[0] = '\0';

	if ( IS_NULLSTR ( sstring ) )
	{  return ""; }

	while ( *sstring ) {
		sstring = get_line ( ( char * ) sstring, tmpb );
		sprintf ( buf2, ":%2d: %s\n\r", cnt++, tmpb );
		strcat ( buf, buf2 );
	}


	return buf;
}

void string_header ( Creature *cr, char *str )
{
	int count = MAX_STRING_LENGTH;

	if ( !IS_NULLSTR ( str ) )
	{ count = MAX_STRING_LENGTH - strlen ( str ); }

	writeBuffer ( CLEAR_SCREEN, cr );
	writeBuffer ( "+=======================================+==================================+\n\r", cr );
	writeBuffer ( "|                                 String Editor                            |\n\r", cr );
	writeBuffer ( "+=======================================+==================================+\n\r", cr );
	writeBuffer ( "|                         For help, type :h on a new line                  |\n\r", cr );
	writeBuffer ( Format ( "|                    Characters Remaining:  %5d of %5d                 | \n\r", count, MAX_STRING_LENGTH ), cr );
	writeBuffer ( "+=======================================+==================================+\n\r", cr );
	return;
}

char *StringEditor_FormatString ( char *oldstring )
{
	char xbuf[MAX_STRING_LENGTH];
	char xbuf2[MAX_STRING_LENGTH];
	char *rdesc;
	int i = 0;
	bool cap = true;

	xbuf[0] = xbuf2[0] = 0;

	i = 0;

	if ( strlen ( oldstring ) >= ( MAX_STRING_LENGTH - 4 ) ) {
		log_hd ( LOG_ERROR, "String to StringEditor_FormatString() longer than MAX_STRING_LENGTH." );

		return ( oldstring );
	}

	for ( rdesc = oldstring; *rdesc; rdesc++ ) {
		if ( *rdesc == '\n' ) {
			if ( xbuf[i - 1] != ' ' ) {
				xbuf[i] = ' ';
				i++;
			}
		} else if ( *rdesc == '\r' ) { ; }
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
			cap = true;
		} else {
			xbuf[i] = *rdesc;
			if ( cap ) {
				cap = false;
				xbuf[i] = toupper ( xbuf[i] );
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
			log_hd ( LOG_ERROR, "StringEditor_FormatString: No spaces" );
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

char *first_arg ( char *argument, char *arg_first, bool fCase )
{
	char cEnd;

	if ( argument == NULL ) {
		log_hd ( LOG_ERROR, "first_arg called with NULL argument, processing to cause detectable crash." );
	}

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
		if ( fCase ) { *arg_first = tolower ( *argument ); }
		else { *arg_first = *argument; }
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while ( *argument == ' ' )
	{ argument++; }


	return argument;
}

void EnterStringEditor ( Creature *cr, char **pEditString )
{
	string_header ( cr, ( char * ) *pEditString );
	Socket *sock = cr->desc;

	// -- lets table this for now.
	if ( sock->incomm[0] != '\0' ) {
		memset ( sock->incomm, 0, sizeof ( sock->incomm ) );
	}

	// -- lets make sure we have an actual string to edit, so assign one if it is NULL.
	if ( IS_NULLSTR ( *pEditString ) ) {
		*pEditString = assign_string ( "" );
	}

	// -- write out the lines with numbers associated to them.
	if ( !IS_NULLSTR ( *pEditString ) ) {
		writeBuffer ( StringEditor_NumberLines ( *pEditString ), cr );
	}

	// -- assign our editing string, and our backup string. (incase we want to abandon our changes)
	sock->pEditString = ( char ** ) pEditString;
	if ( !IS_NULLSTR ( *pEditString ) ) {
		sock->pEditBackup = assign_string ( *pEditString );
	} else {
		sock->pEditBackup = NULL;
	}
	return;
}

const char *StringEditor_DeleteLastLine ( const char *sstring )
{
	int len;
	bool found = false;

	char xbuf[MAX_STRING_LENGTH];

	xbuf[0] = '\0';

	if ( IS_NULLSTR ( sstring ) ) {

		return ( assign_string ( xbuf ) );
	}

	strncpy ( xbuf, sstring, MAX_STRING_LENGTH );

	for ( len = strlen ( xbuf ); len > 0; len-- ) {
		if ( xbuf[len] == '\r' ) {
			if ( !found ) {

				if ( len > 0 )
				{ len--; }
				found = true;
			} else {

				xbuf[len + 1] = '\0';
				PURGE_DATA ( sstring );

				return ( assign_string ( xbuf ) );
			}
		}
	}
	xbuf[0] = '\0';

	PURGE_DATA ( sstring );

	return ( assign_string ( xbuf ) );

}

char * string_replace ( const char * orig, const char * old, const char * inew )
{
	static char xbuf[MAX_STRING_LENGTH] = {'\0'};

	xbuf[0] = '\0';
	strcpy ( xbuf, orig );
	if ( strstr ( orig, old ) != NULL ) {
		int i = 0;
		i = strlen ( orig ) - strlen ( strstr ( orig, old ) );
		xbuf[i] = '\0';
		strcat ( xbuf, inew );
		strcat ( xbuf, &orig[i + strlen ( old )] );
		PURGE_DATA ( orig );
	}


	return assign_string ( xbuf );
}

void StringEditorOptions_exit ( Creature *cr, bool save )
{
	// -- restore the old data!
	if ( !save ) {
		writeBuffer ( "Changes cancelled.\n\r", cr );
		PURGE_DATA ( *cr->desc->pEditString );
		// -- if ch->desc->pEditBackup is NULL, then it will set it to NULL. (jackpot)
		if ( IS_NULLSTR ( cr->desc->pEditBackup ) )
		{ *cr->desc->pEditString = NULL; }
		else
		{ *cr->desc->pEditString = assign_string ( cr->desc->pEditBackup ); }


		// -- abandoned file editing, so we don't corrupt the file by accident.
		/*		if ( cr->queries.queryfunc == builder_editscript ) {
					FILE *fp = ( FILE * ) cr->queries.querydata;

					if ( fp != NULL ) { i_close ( fp ); }
				}
		*/

		PURGE_DATA ( cr->desc->pEditBackup );
		cr->desc->pEditString = NULL;
		cr->queries.queryfunc = NULL;					// -- used as a method to identify ourselves.
		return;
	}

	// -- we were editing a file, fixinate it!
	/*	if ( cr->queries.queryfunc == builder_editscript ) {
			endScriptEditor ( cr );
		}
	*/

	PURGE_DATA ( cr->desc->pEditBackup );
	cr->desc->pEditString = NULL;
	cr->queries.queryfunc = NULL;					// -- used as a method to identify ourselves.
	return;
}

void StringEditorOptions ( Creature *cr, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	int line;

	memset ( buf, 0, sizeof ( buf ) );

	smash_tilde ( argument );

	if ( *argument == ':' ) {
		char arg1 [MAX_INPUT_LENGTH];
		char arg2 [MAX_INPUT_LENGTH];
		char arg3 [MAX_INPUT_LENGTH];
		char tmparg3 [MAX_INPUT_LENGTH];

		argument = ChopC ( argument, arg1 );

		// execute regular commands!
		if ( SameString ( arg1 + 1, "/" ) ) {
			interpret ( cr, argument );
			writeBuffer ( "Command performed.\n\r", cr );
			return;
		}

		argument = first_arg ( argument, arg2, false );
		strcpy ( tmparg3, argument );
		argument = first_arg ( argument, arg3, false );

		if ( SameString ( arg1 + 1, "c" ) ) {
			writeBuffer ( "String cleared.\n\r", cr );
			PURGE_DATA ( *cr->desc->pEditString );
			*cr->desc->pEditString = NULL;
			return;
		}

		if ( SameString ( arg1 + 1, "s" ) ) {
			string_header ( cr, *cr->desc->pEditString );
			writeBuffer ( StringEditor_NumberLines ( *cr->desc->pEditString ), cr );
			return;
		}

		if ( SameString ( arg1 + 1, "r" ) ) {
			if ( arg2[0] == '\0' ) {
				writeBuffer ( "usage:  :r \"old string\" \"new string\"\n\r", cr );
				return;
			}

			*cr->desc->pEditString = string_replace ( *cr->desc->pEditString, arg2, arg3 );
			writeBuffer ( Format ( "'%s' replaced with '%s'.\n\r", arg2, arg3 ), cr );
			return;
		}

		if ( SameString ( arg1 + 1, "f" ) ) {
			if ( arg2[0] != '\0' ) {
				int line1, line2;

				if ( arg3[0] == '\0' ) {
					writeBuffer (
						"Must specify a begin and end line number.\n\r", cr );
					return;
				}

				if ( !is_number ( arg2 ) || !is_number ( arg3 ) ) {
					writeBuffer ( "Must specify numbers for the begin and end line number.\n\r", cr );
					return;
				}

				line1 = atoi ( arg2 );
				line1 = URANGE ( 1, line1, string_linecount ( *cr->desc->pEditString ) + 1 );

				line2 = atoi ( arg3 );
				line2 = URANGE ( 1, line2, string_linecount ( *cr->desc->pEditString ) + 1 );

				if ( line1 > line2 ) {
					line  = line2;
					line2 = line1;
					line1 = line;
				}

				*cr->desc->pEditString = ( char * ) StringEditor_FormatStringLines ( *cr->desc->pEditString, line1, line2 );
				writeBuffer ( Format ( "String formatted from line %d to %d.\n\r", line1, line2 ), cr );
				return;
			} else {
				*cr->desc->pEditString = StringEditor_FormatString ( *cr->desc->pEditString );
			}
			writeBuffer ( "String formatted.\n\r", cr );
			return;
		}

		if ( SameString ( arg1 + 1, "d" ) ) {
			*cr->desc->pEditString = ( char * ) StringEditor_DeleteLastLine ( *cr->desc->pEditString );
			writeBuffer ( " Line removed.\n\r", cr );
			return;
		}

		if ( SameString ( arg1 + 1, "ld" ) ) {
			*cr->desc->pEditString = StringEditor_DeleteLine ( *cr->desc->pEditString, atoi ( arg2 ) );
			writeBuffer ( "Line deleted.\n\r", cr );
			return;
		}

		if ( SameString ( arg1 + 1, "li" ) ) {
			*cr->desc->pEditString = StringEditor_AddLine ( *cr->desc->pEditString, tmparg3, atoi ( arg2 ) );
			writeBuffer ( "Line inserted.\n\r", cr );
			return;
		}

		if ( SameString ( arg1 + 1, "lr" ) ) {
			*cr->desc->pEditString = StringEditor_DeleteLine ( *cr->desc->pEditString, atoi ( arg2 ) );
			*cr->desc->pEditString = StringEditor_AddLine ( *cr->desc->pEditString, tmparg3, atoi ( arg2 ) );
			writeBuffer ( "Line replaced.\n\r", cr );
			return;
		}

		if ( SameString ( arg1 + 1, "q" ) ) {
			StringEditorOptions_exit ( cr, true );
			return;
		}

		if ( SameString ( arg1 + 1, "@" ) ) {
			StringEditorOptions_exit ( cr, false );
			return;
		}

		if ( SameString ( arg1 + 1, "h" ) ) {
			string_header ( cr, *cr->desc->pEditString );	// -- toss it in (useful)
			// -- top bar in string_header (no need to double it up)
			writeBuffer ( "|               String Editor help (commands on blank line):               |\n\r", cr );
			writeBuffer ( "+=======================================+==================================+\n\r", cr );
			writeBuffer ( "|  :r 'old' 'new' - replace a word      |  :s        - show current string |\n\r", cr );
			writeBuffer ( "|  :h             - show this menu      |  :f        - <num1> <num2>       |\n\r", cr );
			writeBuffer ( "|  :f             - (word wrap) string  | (Formats between specified lines)|\n\r", cr );
			writeBuffer ( "+=======================================+==================================+\n\r", cr );
			writeBuffer ( "|  :c             - clears entire string|  :d        - deletes last line   |\n\r", cr );
			writeBuffer ( "|  :ld <num>      - delete line number  |  :li       - <num> <str>         |\n\r", cr );
			writeBuffer ( "|                                       |     insert <str> on line <num>   |\n\r", cr );
			writeBuffer ( "+=======================================+==================================+\n\r", cr );
			writeBuffer ( "|  :/ <command>   - Execute Command     |  :lr       - <num> <str>         |\n\r", cr );
			writeBuffer ( "|                                       |    (replaces line num with str_  |\n\r", cr );
			writeBuffer ( "+=======================================+==================================+\n\r", cr );
			writeBuffer ( "|  :q               - complete string(saves changes)                       |\n\r", cr );
			writeBuffer ( "|  :@               - cancel string(reverts changes)                       |\n\r", cr );
			writeBuffer ( "+==========================================================================+\n\r\n\r", cr );
			return;
		}

		writeBuffer ( "String Editor: Unknown Option.\n\r", cr );
		return;
	}

	strcpy ( buf, *cr->desc->pEditString ? *cr->desc->pEditString : "" );

	// -- lets take a gander at our count
	int count = MAX_STRING_LENGTH;

	// -- ensure we are counting appropriately.
	if ( !IS_NULLSTR ( *cr->desc->pEditString ) ) {
		count = MAX_STRING_LENGTH - strlen ( *cr->desc->pEditString ) - strlen ( argument );
	}

	// -- will we exceed our max limit?
	if ( count <= 4 ) {
		writeBuffer ( Format ( "The string would exceed the maximum character limit of: %d\n\r", MAX_STRING_LENGTH ), cr );
		return;
	}

	// -- tilde smashing because we utilize tilde as a delimiter in certain files.
	smash_tilde ( argument );

	// -- assign the buffer argument
	strcat ( buf, argument );

	// -- assign \n\r to stretch the next line.
	strcat ( buf, "\n\r" );

	// -- purge the old string
	PURGE_DATA ( *cr->desc->pEditString );

	// -- assign the new editing string
	*cr->desc->pEditString = assign_string ( buf );
	return;
}


char * string_unpad ( char * argument )
{
	char buf[MAX_STRING_LENGTH];
	char *s;

	memset ( buf, 0, sizeof ( buf ) );

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

char * string_proper ( char * argument )
{
	char *s;

	s = argument;

	while ( *s != '\0' ) {
		if ( *s != ' ' ) {
			*s = toupper ( *s );
			while ( *s != ' ' && *s != '\0' )
			{ s++; }
		} else {
			s++;
		}
	}

	return argument;
}

char* all_capitalize ( const char *str )
{
	static char strcap [ MAX_STRING_LENGTH ];
	int  i;

	memset ( strcap, 0, sizeof ( strcap ) );

	for ( i = 0; str[i] != '\0'; i++ )
	{ strcap[i] = toupper ( str[i] ); }
	strcap[i] = '\0';


	return strcap;
}

char *ChopC ( const char *argument, char *first_arg )
{
	char cEnd;

	while ( isspace ( *argument ) )
	{ argument++; }

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
	{ cEnd = *argument++; }

	while ( *argument != '\0' ) {
		if ( *argument == cEnd ) {
			argument++;
			break;
		}
		*first_arg = LOWER ( *argument );
		first_arg++;
		argument++;
	}
	*first_arg = '\0';

	while ( isspace ( *argument ) )
	{ argument++; }


	return ( char * ) argument;
}

std::string ChopString ( const std::string &argument, std::string &first )
{
	std::string::size_type start, stop, stop2;
	char find;

	start = 0;

	if ( argument.length() < 1 || argument.empty() ) {
		first = "";

		return "";
	}

	if ( argument[0] == ' ' ) {
		start = argument.find_first_not_of ( ' ' );

		if ( start == argument.npos ) {
			first = "";

			return "";
		}
	}

	// Quotes or space?
	switch ( argument[start] ) {
		case '\'':
			find = '\'';
			start++;
			break;
		case '\"':
			find = '\"';
			start++;
			break;
		default:
			find = ' ';
	}

	// Find end of argument.
	stop = argument.find_first_of ( find, start );

	// Empty leftovers?
	if ( stop == argument.npos ) {
		first = argument.substr ( start );

		return "";
	}

	// Update first
	first = argument.substr ( start, ( stop - start ) );

	// Strip leading spaces from leftovers
	stop2 = argument.find_first_not_of ( ' ', stop + 1 );

	// Empty leftovers?
	if ( stop2 == argument.npos )
	{  return ""; }



	// Return leftovers.
	return argument.substr ( stop2 );
}

// -- capitalize the first letter of the string.
char *capitalize ( const char *str )
{
	static char strcap[MAX_STRING_LENGTH] = {'\0'};
	int i;

	for ( i = 0; str[i] != '\0'; i++ )
	{ strcap[i] = LOWER ( str[i] ); }
	strcap[i] = '\0';
	strcap[0] = UPPER ( strcap[0] );


	return strcap;
}

const char *strip_crlf ( const char *str )
{
	static char newstr[MAX_STRING_LENGTH];
	char buffer[MAX_STRING_LENGTH];
	int i, j;

	memset ( buffer, 0, sizeof ( buffer ) );

	strncpy ( buffer, str, sizeof ( buffer ) );

	// -- memset the string
	memset ( newstr, 0, sizeof ( newstr ) );

	for ( i = j = 0; buffer[i] != '\0'; i++ ) {
		if ( buffer[i] != '\r' && buffer[i] != '\n' ) {
			newstr[j++] = buffer[i];
		}
	}
	newstr[j] = '\0';

	return newstr;
}

const char *StringEditor_FormatStringLines ( const char *oldstring, int line1, int line2 )
{
	const char *str = oldstring;
	char	xbuf[MAX_STRING_LENGTH];
	std::string block1 ( "" );
	std::string block2 ( "" );
	std::string block3 ( "" );
	int	 i;

	if ( IS_NULLSTR ( oldstring ) )
	{  return ""; }

	/* Get the first block - not going to format this. */
	for ( i = 1; i < line1 && *str != '\0'; i++ ) {
		str = StringEditor_GetLine ( str, xbuf );
		block1.append ( Format ( "%s\n\r", xbuf ) );
	}

	/* Get the block to format. */
	for ( i = line1; i <= line2 && *str != '\0'; i++ ) {
		str = StringEditor_GetLine ( str, xbuf );
		block2.append ( Format ( "%s\n\r", xbuf ) );
	}

	/* Get the last block - not going to format this. */
	for ( ; *str != '\0'; ) {
		str = StringEditor_GetLine ( str, xbuf );
		block3.append ( Format ( "%s\n\r", xbuf ) );
	}

	/* Format the middle block & free it. */
	str = assign_string ( C_STR ( block2 ) );
	str = StringEditor_FormatString ( ( char * ) str );

	/* Add formatted block to first block & free it. */
	block1.append ( str );
	PURGE_DATA ( str );

	/* Add the last block  */
	block1.append ( block3 );
	/* Check the length. of the string to ensure we aren't exceeding massive lengths.*/
	if ( block1.length() >= MAX_STRING_LENGTH - 6 ) {

		return oldstring;
	}

	/* Free oldstring and copy new. */
	PURGE_DATA ( oldstring );
	str = assign_string ( C_STR ( block1 ) );

	return str;
}

// -- returns true if it has punctuation
bool StringEditor_CheckPunctuation ( const char *str )
{
	bool punctuation = false;
	const char *p;

	if ( IS_NULLSTR ( str ) )
	{  return false; }

	for ( p = str; *p != '\0'; ) {
		if ( *p == '#' )
		{ p += 2; }
		else if ( *p == '!' || *p == '.' || *p == '?' || *p == ','
				  ||        *p == ':' || *p == ';' )
		{ p++, punctuation = true; }
		else if ( isspace ( *p ) )
		{ p++; }
		else
		{ p++, punctuation = false; }
	}


	return punctuation;
}

const char *StringEditor_GetLine ( const char *str, char *line )
{
	int  tmp = 0;
	bool found = false;

	while ( *str != '\0' ) {
		if ( *str == '\n' ) {
			found = true;
			break;
		}
		line[tmp++] = * ( str++ );
	}

	if ( found ) {
		if ( * ( str + 1 ) == '\r' )
		{ str += 2; }
		else
		{ str += 1; }
	}

	line[tmp] = '\0';


	return str;
}

char *StringEditor_DeleteLine ( char *string, int line )
{
	char *strtmp = string;
	char buf[MAX_STRING_LENGTH] = {'\0'};
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


char *StringEditor_AddLine ( char *string, char *newstr, int line )
{
	char *strtmp = string;
	int cnt = 1, tmp = 0;
	bool done = false;
	char buf[MAX_STRING_LENGTH] = {'\0'};

	buf[0] = '\0';

	for ( ; *strtmp != '\0' || ( !done && cnt == line ); strtmp++ ) {
		if ( cnt == line && !done ) {
			strcat ( buf, newstr );
			strcat ( buf, "\n\r" );
			tmp += strlen ( newstr ) + 2;
			cnt++;
			done = true;
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

#define STREP_SIZE (MAX_STRING_LENGTH * 5)
char *substr ( const char *orig, const char *first, const char *last )
{
	char *a, *b, *result;
	static char buf[5][STREP_SIZE];
	static int i;
	int len;

	if ( IS_NULLSTR ( orig ) )
	{  return NULL; }

	++i, i %= 5;
	result = buf[i];

	if ( ! ( a = ( char * ) strstr ( first, orig ) ) ) {
		strcpy ( result, orig );

		return result;
	}

	if ( !IS_NULLSTR ( last ) && ( b = ( char * ) strstr ( orig, last ) ) )
	{ len = strlen ( a ) - strlen ( b ); }
	else
	{ len = strlen ( a ); }

	strncpy ( result, a + strlen ( first ), len );
	result[len] = '\0';


	return result;
}

std::string CapitalSentence ( const std::string &str )
{
	std::string _buf ( "" );
	bool didPeriod = false;

	for ( size_t _x = 0; _x <= str.length(); _x++ ) {
		if ( _x == 0 ) // first letter of the string is capitalized.
		{ _buf = toupper ( str[0] ); }
		else {
			// are we a letter that needs to be capitalized.
			if ( didPeriod && isalpha ( str[_x] ) ) {
				_buf[_x] = toupper ( str[_x] );
				didPeriod = false;
				continue;
			}
			// are we a period?  Setup our next sentence :)
			if ( str[_x] == '.' )
			{ didPeriod = true; }

			// just a letter/number/etc, push it to the new buffer.
			_buf[_x] = str[_x];
		}
	}

	return ( _buf );
}

std::string getArg ( const std::string &str, int arg )
{
	if ( str.empty() )
	{  return ""; }

	std::string data;
	std::string argument;
	data = ChopString ( str, argument );
	if ( arg == 0 )
	{  return argument; }

	for ( int x = 1; x <= arg; x++ ) {
		data = ChopString ( data, argument );
		if ( arg == x ) {

			return argument;
		}
	}


	// return an empty string.
	return "";
}

// return everything after arg.
std::string getAfter ( const std::string &str, int arg )
{
	if ( str.empty() )
	{  return ""; }

	std::string data;
	std::string argument;
	data = ChopString ( str, argument );
	if ( arg == 0 )
	{  return data; }

	for ( int x = 1; x <= arg; x++ ) {
		data = ChopString ( data, argument );
		if ( arg == x ) {

			return data;
		}
	}

	return "";
}

std::string center_string ( const std::string &str, int width = 80 )
{
	int len, pos;
	std::string outstr ( "" );
	len = ( width - colour_strlen ( str ) ) / 2;
#if defined (_DEBUG_)
	Log::instance().log ( eLOGLOW, Format ( "center_string: total length: %d", len ) );
#endif
	for ( pos = 0; pos < len; pos++ ) {
		outstr.append ( " " );
	}
	outstr.append ( str );
	outstr.append ( "\n\r" );

	return outstr;
}

const char *wrapstr ( const char *str )
{
	static char strwrap[MAX_STRING_LENGTH] = {'\0'};
	unsigned int i;
	int count = 0;

	memset ( strwrap, 0, MAX_STRING_LENGTH );

	for ( i = 0; i < strlen ( str ); i++ ) {
		count++;
		if ( count > 66 && str[i] == ' ' ) {
			strwrap[i] = '\n';
			strwrap[i + 1] = '\r';
			count = 0;
		} else {
			strwrap[i] = str[i];
		}
	}
	strwrap[i] = '\0';

	return strwrap;
}

// -- remove the space
const char *smash_space ( const std::string &str_sm )
{
	if ( str_sm.empty() ) {
		log_hd ( LOG_ERROR, "smash_space called with a empty string!" );

		return NULL;
	}
	char *str = ( char * ) C_STR ( str_sm );
	static char strwrap[MAX_STRING_LENGTH] = {'\0'};
	memset ( strwrap, 0, sizeof ( strwrap ) );

	for ( int i = 0; str[i] != '\0'; i++ ) {
		if ( str[i] == ' ' )
		{ strwrap[i] = '_'; }
		else { strwrap[i] = str[i]; }
	}

	return strwrap;
}

int colour_strlen ( const std::string &txt )
{
	try {
		int pos, len;
		len = 0;

		if ( txt.empty() ) {  return 0; }

		for ( pos = 0; txt[pos] != '\0'; pos++ ) {
			if ( txt[pos] != '^' && txt[pos] != '\a' ) {
				len++;
				continue;
			}
			pos++;
			// this is an in-depth colour tag!
			if ( txt[pos] == '[' || txt[pos] == '\a' ) {
				// loop through here!
				int cnt = 0;
				while ( txt[pos] != '\0' ) {
					pos++;
					cnt++;
					if ( txt[pos] == ']' )
					{ break; }
					// colour-tag isn't accurate!
					if ( cnt > 6 )
					{ break; }
				}
			} else {
				if ( txt[pos] == '^' )
				{ len++; }
			}
		} // end for-loop
		log_hd ( LOG_DEBUG, Format ( "colour_strlen returning: %d", len ) );

		return len;
	} catch ( ... ) {
		CATCH ( false );

		return 0;
	}
	// -- we should *NEVER* reach this point, but if we do, lets be smart about it.

	return 0;
}


// -- EOF