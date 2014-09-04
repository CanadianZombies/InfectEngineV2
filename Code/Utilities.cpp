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

#include <dirent.h>

// -- External variables
extern char                    strArea[MAX_INPUT_LENGTH];
extern FILE *                  fpArea;

void _Error ( const std::string &errorStr, const std::string &fileStr, const std::string &funcStr, int lineOfFile )
{
	// -- log with p-error, ensure we have our time and all pertinent associated data.
	perror ( Format ( "(%s)%s:%s:%d : Errno: (%d):(%s), %s", grab_time_log ( time ( 0 ) ), C_STR ( fileStr ), C_STR ( funcStr ), lineOfFile, errno, strerror ( errno ), C_STR ( errorStr ) ) );
	errno = 0; // -- reset our errno value
}

void catchException ( bool willAbort, const std::string &file, const std::string &function, int lineATcatch )
{
	try {
		throw;
	} catch ( std::exception &e ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> " << e.what() << std::endl; }
	} catch ( int a ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> " << a << std::endl; }
	} catch ( std::string s ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> " << s << std::endl; }
	} catch ( const char *ti ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> " << ti << std::endl; }
	}  catch ( char *t ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> " << t << std::endl; }
	} catch ( ... ) {
		{ std::cerr << "Thrown exception from " << file << ":" << function << " -> Unknown exception!" << std::endl; }
	}

	// -- attempt to log our exception properly!
	ExceptionTrace();

	// are we crashing out of the mud ?  Unrecoverable error encountered.
	if ( willAbort ) {
		std::cerr << "Aborting The Infected City." << std::endl;
		SUICIDE;
	}
	return;
}


const char *grab_date_log ( time_t the_time )
{
	static char lickmenow[200];
	struct tm *tm_ptr;

	memset ( lickmenow, 0, sizeof ( lickmenow ) );

	tm_ptr = localtime ( &the_time );

	//* Easier then my ex girlfriend. *//
	snprintf ( lickmenow, 200, "%02d-%02d-%02d", tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1, tm_ptr->tm_mday );
	return lickmenow;
}
// *Same as grab_time, just without the \n at the end.* //
const char *grab_time_log ( time_t the_ttime )
{
	static char lick_menow[200];
	struct tm *tmt_ptr;

	memset ( lick_menow, 0, sizeof ( lick_menow ) );
	tmt_ptr = localtime ( &the_ttime );

	snprintf ( lick_menow, 200, "%02d:%02d:%02d", tmt_ptr->tm_hour, tmt_ptr->tm_min, tmt_ptr->tm_sec );
	return lick_menow;
}

// -- sitrep = log mechanism (coder functionality)
void sitrep ( int bitvector, const std::string &str )
{
	Socket *sok, *sok_n;
	for ( sok = socket_list; sok; sok = sok_n ) {
		sok_n = sok->next;
		Socket *s = sok;

		if ( s->character && s->connected == CON_PLAYING ) {
			Creature *cr = s->character;
			if ( IS_SET ( cr->sitrep, bitvector ) ) {
				writeBuffer ( Format ( "\ay{\aYSITREP\ay} \aR: \an%s\n\r", C_STR ( str ) ), cr );
			}
		}
	}
	return;
}

bool file_exists ( const char *name )
{
	struct stat fst;

	if ( IS_NULLSTR ( name ) )
	{ return false; }

	if ( stat ( name, &fst ) != -1 )
	{ return true; }
	else
	{ return false; }

	return false;
}


// ------------------------------------------------------------------------------
// -- Function:     _log_hd
// -- Arguments:    flag identifier, logged string
// -- Example:      log_hd(LOG_BASIC, "Generic Log Entry Here");
// -- Example 2:    log_hd(LOG_BASIC|LOG_ERROR, "Multipurpose flagged log creates the log entry in two logfiles.");
// -- Example 3:    log_hd(LOG_ALL, "This puts a log entry in EVERY log file.");
// -- Date Written: 15 January 2013
// -- Revised Date: 29 July 2014
// ------------------------------------------------------------------------------
// -- Purpose of Function:
// -- The purpose of this function is the keep accurate and well written logs.
// -- Upon calling this function, we attempt to create a new directory within the
// -- log directory for the current date, if that date exists, we attempt to create
// -- a log for the current pid.
// --       Example: /home/mudlogin/InfectEngine/Logs/2014-07-29/54451/logname.log
// -- By sorting like this, logs will not become too large, and can be reviewed by date
// -- and by PID if necessary.  For those who hotreboot often or crash, keeping detailed
// -- track of your log files by PID allows for faster detection/correction of associated
// -- issues.
// ------------------------------------------------------------------------------
// -- Upon appropriately logging detection we attempt to sitrep out the new log
// -- to those whom can view those particular logs via the sitrep system.
// -- If multiple flags are associated with the log entry, then multiple sitreps will
// -- be made.  This can get spammy if staff are listening to too many sitrep channels at
// -- once.
// ------------------------------------------------------------------------------
void _log_hd ( long logFlag, const char *mFile, const char *mFunction, int mLine, const std::string &logStr )
{
	try {
		static bool called_tzset = false;
		static int debugCounter = 0;
		static int current_yday = 0;
		static int last_yday = 0;
		// called before time(0); to make sure we get the most accurate time
		if ( !called_tzset ) {
			tzset();
			called_tzset = true;
			debugCounter = 0;
			current_yday = 0;
			last_yday = 0;
			unlink ( Format ( "%s%s/%d/%s", LOG_DIR, grab_date_log ( time ( 0 ) ), getpid(), DEBUG_FILE ) ); // -- wipe out old debugging information
		}

		// -- cdt current date time
		time_t cdt = time ( 0 );

		struct tm *tyme = localtime ( &cdt );
		if ( last_yday == 0 ) {
			last_yday = tyme->tm_yday;
		}
		current_yday = tyme->tm_yday;

		// -- associate the time and date appropriately
		const char *the_time = grab_time_log ( cdt );
		const char *the_date = grab_date_log ( cdt );

		// -- if this happens there is a big problem!
		if ( !the_time )
		{ the_time = "{ No Time }"; }

		// -- if this happens, we have BIGGER issues.
		if ( !the_date )
		{ the_date = "Today"; }

		struct Log_types {
			const char *extension;
			const char *broadcast;
			const char *colour;
			long logtype;
			int crFlagSitrep;
		};

		const struct Log_types log_table[] = {
			{"log",         "Log: Basic",    "\a[F542]",    LOG_BASIC,          CF_SEE_LOGS},
			{"error",       "Log: Error",    "\a[F500]",    LOG_ERROR,          CF_SEE_BUGS},
			{"security",    "Log: Security", "\a[F213]",    LOG_SECURITY,       CF_SEE_SECURITY},
			{"commands",    "Log: Command",  "\a[F455]",    LOG_COMMAND,        CF_SEE_COMMANDS},
			{"debug",       "Log: Debug",    "\a[F343]",    LOG_DEBUG,          CF_SEE_DEBUG},
			{"script",	"Log: Script",	 "\a[F535]",    LOG_SCRIPT,         CF_SEE_SCRIPT},
			{"suicide",	"Log: Suicide",  "\a[F500]",    LOG_SUICIDE,          CF_SEE_ABORT},
			{NULL, NULL, NULL, -1, -1 },
		};

		// -- No need to go any further if we are just outputting to the stdout
		// -- otherwise we want our proper logs handled and we continue.
		if ( IS_SET ( logFlag, LOG_DEBUG ) )
		{ std::cout << "\t<< stdout << " << the_time << " << (" << debugCounter << ") " << logStr << std::endl; }
		else
		{ std::cout << "<< stdout << " << the_time << " << " << logStr << std::endl; }

		// -- establish the current-date for the logs, write the log to a pid within the directory.
		struct stat st;
		if ( stat ( Format ( "%s%s", LOG_DIR, the_date ), &st ) == 0 )
		{ }
		else {
			// -- unable to make the directory for the logs?
			if ( mkdir ( Format ( "%s%s", LOG_DIR, the_date ), 0777 ) ) {
				SUICIDE_REAL;
			}
		}

		// -- create the sub-directory with the pid for the log
		if ( stat ( Format ( "%s%s/%d", LOG_DIR, the_date, getpid() ), &st ) == 0 )
		{ }
		else {
			// -- unable to make the directory for the logs by pid?
			if ( mkdir ( Format ( "%s%s/%d", LOG_DIR, the_date, getpid() ), 0777 ) ) {
				SUICIDE_REAL;
			}
		}

		// -- handle our logging mechanism's
		for ( int logX = 0; log_table[logX].extension != NULL; logX++ ) {
			// -- not the same day anymore, the first log entry will have to be the update from the previous day
			// -- simply because we like having bloated logs and keeping ourselves informed of such things.
			if ( last_yday != current_yday ) {
				FILE *fp = NULL;

				if ( log_table[logX].logtype == LOG_DEBUG ) {
					debugCounter = 0;
					if ( ( fp = fopen ( Format ( "%s%s/%d/%s", LOG_DIR, the_date, getpid(), DEBUG_FILE ), "a" ) ) != NULL ) {
						fprintf ( fp, "\t%s : %s\n", the_time, Format ( "Continuing new logfile from previous yday: %d", last_yday ) );
						fprintf ( fp, "\tEngine Version: %s\n", getVersion() );
					}
				} else {
					// -- not a debug message? Log it to its appropriate log.
					if ( ( fp = fopen ( Format ( "%s%s/%d/%s.log", LOG_DIR, the_date, getpid(), log_table[logX].extension ), "a" ) ) != NULL ) {
						fprintf ( fp, "\t%s : %s\n", the_time, Format ( "Continuing new logfile from previous yday: %d", last_yday ) );
						fprintf ( fp, "\tEngine Version: %s\n", getVersion() );
					}
				}
				fflush ( fp );
				fclose ( fp );
			}

			// -- now we check to see if we are going to be logging to th extreme or not!
			if ( ( IS_SET ( logFlag, log_table[logX].logtype ) || IS_SET ( logFlag, LOG_ALL ) ) ) {
				FILE *fp = NULL;
				std::string logOutStr = logStr;

				// -- debugging districts the good ole fashioned way.
				if ( log_table[logX].logtype == LOG_ERROR && logFlag == LOG_ERROR ) {
					if ( fpArea != NULL ) {
						int iLine;
						int iChar;

						if ( fpArea == stdin ) {
							iLine = 0;
						} else {
							iChar = ftell ( fpArea );
							fseek ( fpArea, 0, 0 );
							for ( iLine = 0; ftell ( fpArea ) < iChar; iLine++ ) {
								while ( getc ( fpArea ) != '\n' )
									;
							}
							fseek ( fpArea, iChar, 0 );
						}
						// -- not a debug message? Log it to its appropriate log.
						if ( ( fp = fopen ( Format ( "%s%s/%d/%s.log", LOG_DIR, the_date, getpid(), log_table[logX].extension ), "a" ) ) != NULL ) {
							fprintf ( fp, "\t[*****]: FILE: %s LINE: %d\n", strArea, iLine );
						}
						fflush ( fp );
						fclose ( fp );
					}
				}

				// -- no matter what only debug messages end up in the runtime.debug file.
				if ( log_table[logX].logtype == LOG_DEBUG ) {
					// -- We only keep the last 500 debug messages before we wipe the file and
					// -- create a new one.  Purpose behind this is to eliminate massive debug logs
					// -- and by all account, no debug file should be empty.  This is an excellent
					// -- way to log things leading up to a crash, or simply a great way to log
					// -- the current stack leading up to a shutdown/crash/etc.
					if ( ++debugCounter > 500 ) {
						unlink ( Format ( "%s%s/%d/%s", LOG_DIR, the_date, getpid(), DEBUG_FILE ) );
						debugCounter = 1;
					}
					if ( ( fp = fopen ( Format ( "%s%s/%d/%s", LOG_DIR, the_date, getpid(), DEBUG_FILE ), "a" ) ) != NULL ) {

						// -- always ensure our Engine Version is associated with all new files
						if ( debugCounter == 1 ) {
							fprintf ( fp, "Engine Version: %s\n", getVersion() );
						}
						fprintf ( fp, "\t(%d)%s : %s : %s : %d : %s\n", debugCounter, the_time, mFile, mFunction, mLine, C_STR ( logStr ) );
					}
					// -- sitrep our log out to those who can listen to it.
					sitrep ( log_table[logX].crFlagSitrep, Format ( "\a[F350](\a[F541]%s\a[F350]) (%d) %s%s : %s\an", log_table[logX].broadcast, debugCounter, log_table[logX].colour, the_time, C_STR ( logOutStr ) ) );

				} else {
					bool exists = true;
					if ( !file_exists ( Format ( "%s%s/%d/%s.log", LOG_DIR, the_date, getpid(), log_table[logX].extension ) ) ) {
						exists = false;
					}
					// -- not a debug message? Log it to its appropriate log.
					if ( ( fp = fopen ( Format ( "%s%s/%d/%s.log", LOG_DIR, the_date, getpid(), log_table[logX].extension ), "a" ) ) != NULL ) {
						// -- if we didn't exist, we put the engine version at the top of the log file
						if ( !exists ) {
							fprintf ( fp, "Engine Version: %s\n", getVersion() );
						}
						fprintf ( fp, "\t%s : %s : %s : %d : %s\n", the_time, mFile, mFunction, mLine, C_STR ( logStr ) );
					}
					// -- sitrep our log out to those who can listen to it.
					sitrep ( log_table[logX].crFlagSitrep, Format ( "\a[F350](\a[F541]%s\a[F350]) %s%s : %s\an", log_table[logX].broadcast, log_table[logX].colour, the_time, C_STR ( logOutStr ) ) );
				}

				fflush ( fp );             // -- flush the file pointer
				fclose ( fp );             // -- close the file pointer
			}
		}
		// -- finally change the last_yday to the current yday once we have processed everything
		// -- and our conditions are met appropriately.
		if ( last_yday != current_yday ) {
			last_yday = current_yday;   // -- make us the current yday (julian date)
		}
	} catch ( ... ) {
		// -- at least log where the log was called from
		std::cout << "Unknown error encountered during log_hd processing (" << mFile << " : " << mFunction << " : " << mLine << ")" << std::endl;
	}

	// -- Force a suicide after the logging is completed!
	if ( IS_SET ( logFlag, LOG_SUICIDE ) ) {
		SUICIDE_REAL;
	}

	return;
}

const char *Format ( const char *fmt, ... )
{
	static char textString[35][MAX_STRING_LENGTH];

	static int _FormatTicker;

	// -- reset to 0 if we exceed MAX_NESTED_FORMAT , we should never nest this many Formats
	// -- but if we do, we will have problems and need to expand at that point in time.
	// -- however I do not forsee us experiencing that large of a upgrade
	if ( ++_FormatTicker >= 35 ) {
		_FormatTicker = 0;
	}

	try {
		int lTick = _FormatTicker;

		// -- GCC has deprecated this method, hopefully this will change
		// -- in the future so we can use this.
		//              TODO ( "Variable name Assignment - Unique identifiers" )

		// -- attempt to zeroize the current tick
		memset ( textString[lTick], 0, sizeof ( textString[0] ) );

		// -- Assign our variable length in totality!
		va_list args;
		va_start ( args, fmt );
		int length = vsnprintf ( textString[lTick], MAX_STRING_LENGTH, fmt, args );
		va_end ( args );

		if ( length == 0 ) {
			log_hd ( LOG_ERROR | LOG_DEBUG, "Format returned a zero length string. BAD MOJO, committing suicide!" );
			SUICIDE;
		}

		return textString[lTick];
	} catch ( ... ) {
		log_hd ( LOG_ERROR | LOG_DEBUG, "Unknown exception has been caught." );
	}
	return "";
}


const char *getVersion ( void )
{
	static char versionNumber[200];
	unsigned long x = 0;
	int major, minor, revision;

	// -- just attempting to fix a possible future issue.
	memset ( versionNumber, 0, sizeof ( versionNumber ) );

	major = minor = revision = 0;

	while ( x < mudVersion ) { // sentinel will take care of this.
		revision++;
		if ( revision >= 100 ) {
			revision = 0;
			minor++;
			if ( minor >= 100 ) {
				major++;
				minor = 0;
			}
		}
		x++;
	}

	// create our version display string. x.y.z(build type, alpha, beta) (build: w)
	snprintf ( versionNumber, 200, "%02d.%02d.%02d%s (build: %ld)", major, minor, revision, BUILD_TYPE, mudVersion );
	return versionNumber;
}


void createDirectory ( const std::string &dirString )
{
	struct stat st;
	if ( stat ( C_STR ( dirString ), &st ) == 0 )
	{ } // -- no longer log if the directory already exists. (its wasteful spam)
	else {
		std::cout << "\tDirectory: " << dirString << " : Does not Exist, creating directory." << std::endl;
		if ( mkdir ( C_STR ( dirString ), 0777 ) ) {
			std::cout << "\t\tDirectory: " << dirString << " : Could not build directory!" << std::endl;
		}
	}
}

void build_directories ( void )
{
	struct bd_st {
		const char * bName;
		bool a_to_z;
	};

	const struct bd_st dirStruct[] = {
		{ BIN_DIR, false },
		{ LIB_DIR, false },
		{ LOG_DIR, false },
		{ INTERMUD_DIR, false },
		{ ACCOUNT_DIR, true },
		{ PLAYER_DIR, true },
		{ GOD_DIR,  false },
		{ WORLD_DIR, false },
		{ SCRIPT_DIR, false },
		{ WEB_DIR, false },
		{ SNIPPET_DIR, false },
		{ SCREENSHOT_DIR, false },
		{ NULL, false },
	};

	std::cout << "Checking directories for completeness." << std::endl;

	// -- do the loop-de-loops
	for ( int x = 0; dirStruct[x].bName != NULL; x++ ) {
		createDirectory ( Format ( "%s", dirStruct[x].bName ) );

		if ( dirStruct[x].a_to_z ) {
			for ( char h = 'A'; h <= 'Z'; h++ ) {
				createDirectory ( Format ( "%s/%c", dirStruct[x].bName, tolower ( h ) ) );
			}
		}
	}

	std::cout << "Directory check completed." << std::endl;
}

char * str_str ( const char *astr, const char *bstr )
{

	if ( astr == NULL ) {
		log_hd ( LOG_ERROR, "str_str: null astr." );
		return NULL;
	}

	if ( bstr == NULL ) {
		log_hd ( LOG_ERROR, "str_str: null bstr." );
		return NULL;
	}

	// cheap hack.
	return strstr ( ( char* ) astr, ( char* ) bstr );
}

/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp ( const char *astr, const char *bstr )
{
	if ( astr == NULL ) {
		log_hd ( LOG_ERROR, "Str_cmp: null astr." );
		return TRUE;
	}

	if ( bstr == NULL ) {
		log_hd ( LOG_ERROR, "Str_cmp: null bstr." );
		return TRUE;
	}

	for ( ; *astr || *bstr; astr++, bstr++ ) {
		if ( LOWER ( *astr ) != LOWER ( *bstr ) )
		{ return TRUE; }
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix ( const char *astr, const char *bstr )
{
	if ( astr == NULL ) {
		log_hd ( LOG_ERROR, "Strn_cmp: null astr." );
		return TRUE;
	}

	if ( bstr == NULL ) {
		log_hd ( LOG_ERROR, "Strn_cmp: null bstr." );
		return TRUE;
	}

	for ( ; *astr; astr++, bstr++ ) {
		if ( LOWER ( *astr ) != LOWER ( *bstr ) )
		{ return TRUE; }
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix ( const char *astr, const char *bstr )
{
	int sstr1;
	int sstr2;
	int ichar;
	char c0;

	if ( ( c0 = LOWER ( astr[0] ) ) == '\0' )
	{ return FALSE; }

	sstr1 = strlen ( astr );
	sstr2 = strlen ( bstr );

	for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ ) {
		if ( c0 == LOWER ( bstr[ichar] ) && !str_prefix ( astr, bstr + ichar ) )
		{ return FALSE; }
	}

	return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix ( const char *astr, const char *bstr )
{
	int sstr1;
	int sstr2;

	sstr1 = strlen ( astr );
	sstr2 = strlen ( bstr );
	if ( sstr1 <= sstr2 && !str_cmp ( astr, bstr + sstr2 - sstr1 ) )
	{ return FALSE; }
	else
	{ return TRUE; }
}



void announce ( const std::string &outStr )
{
	try {
		Socket *sd, *sd_next;

		if ( outStr.empty() ) { return; }

		for ( sd = socket_list; sd; sd = sd_next ) {
			Socket *s = sd;
			sd_next = sd->next;

			if ( !s->character ) { continue; }

			writeBuffer ( Format ( "\a[F355]{\a[F552]ANNOUNCEMENT\a[F355]} ^y<^C%s^y>^n \a[F355]{\a[F552]ANNOUNCEMENT\a[F355]}\an \r\n", C_STR ( outStr ) ), s->character );
		}
	} catch ( ... ) {
		log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "%s encountered an untrapped error!", __PRETTY_FUNCTION__ ) );
	}
	return;
}

const char *wrapstr ( const char *str )
{
	static char strwrap[MAX_OUTPUT_BUFFER] = {'\0'};
	unsigned int i;
	int count = 0;

	memset ( strwrap, 0, MAX_OUTPUT_BUFFER );

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

const char *whoami ( void )
{
	struct passwd *pwd;
	uid_t uid;
	uid = getuid(  );
	if ( ( pwd = getpwuid ( uid ) ) ) {
		return ( pwd->pw_name );
	}

	return "unknown";
}

void make_grid ( char *argument )
{
	/* GridMaker 1.0 by Muerte of MND */
	/* fixed for rom by Rashin of TGH2: Project X */

	FILE *fp;
	char arg1[10], arg2[10], arg3[10];
	int width, height, vstart, vend, vnum, n, e, s, w, line_pos;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );
	argument = one_argument ( argument, arg3 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		//send_to_char("SYNTAX: worldmake <start vnum> <width> <height>\n\r", ch);
		return;
	}

	width = atoi ( arg2 );
	height = atoi ( arg3 );
	vstart = atoi ( arg1 );
	vend = vstart + ( width * height ) - 1;

	if ( ( fp = fopen ( Format ( "%sworld.are", WORLD_DIR ), "w" ) ) == NULL ) {
		return;
	}

	fprintf ( fp, "#AREADATA\nName World~\nBuilders None~\nVNUMs %d %d\nCredits Omega~\nSecurity 9\nEnd\n", vstart, vend );
	fprintf ( fp, "\n\n\n#ROOMS\n" );

	for ( vnum = vstart; vnum <= vend; vnum++ ) {
		/* for every room */
		n = vnum - width;
		s = vnum + width;
		e = vnum + 1;
		w = vnum - 1;

		/*where it is on the line 0 to (width-1)*/
		line_pos = ( vnum - vstart + 1 ) % ( width );
		if ( line_pos == 0 )
		{ line_pos = width; }

		/*north border*/
		if ( ( vnum >= vstart ) && ( vnum < vstart + width ) ) {
			n = 0;
		}
		/*south border*/
		if ( ( vnum > vend - width ) && ( vnum <= vend ) ) {
			s = 0;
		}
		/*east border*/
		if ( ( vnum - vstart + 1 ) % ( width ) == 0 ) {
			e = 0;
		}
		/*west border*/
		if ( ( vnum - vstart + 1 ) % ( width ) == 1 ) {
			w = 0;
		}

		fprintf ( fp, "#%d\nNAME~\n\n~\n0 0 0\n", vnum );
		if ( n > 0 )
		{ fprintf ( fp, "D0\n\n~\n~\n0 0 %d\n", n ); }
		if ( e > 0 )
		{ fprintf ( fp, "D1\n\n~\n~\n0 0 %d\n", e ); }
		if ( s > 0 )
		{ fprintf ( fp, "D2\n\n~\n~\n0 0 %d\n", s ); }
		if ( w > 0 )
		{ fprintf ( fp, "D3\n\n~\n~\n0 0 %d\n", w ); }

		fprintf ( fp, "S\n" );
	}

	fprintf ( fp, "#0\n" );
	fprintf ( fp, "\n\n\n#$\n" );
	fclose ( fp );
	return;
}

// -- EOF
