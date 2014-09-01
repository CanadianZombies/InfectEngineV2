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

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args ( ( int  ) );
extern	int	malloc_verify	args ( ( void ) );
#endif


#if defined(unix)
#include <signal.h>
#endif


/*
 * Socket and TCP/IP stuff.
 */
#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
const	char 	go_ahead_str	[] = { ( char ) IAC, ( char ) GA, '\0' };
#endif


/*
    Linux shouldn't need these. If you have a problem compiling, try
    uncommenting these functions.
*/
/*
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	listen		args( ( int s, int backlog ) );
*/

int	close		args ( ( int fd ) );
int	gettimeofday	args ( ( struct timeval *tp, struct timezone *tzp ) );
/* int	read		args( ( int fd, char *buf, int nbyte ) ); */
int	select		args ( ( int width, fd_set *readfds, fd_set *writefds,
						 fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args ( ( int domain, int type, int protocol ) );
/* int	write		args( ( int fd, char *buf, int nbyte ) ); */ /* read,write in unistd.h */

/*
 * Global variables.
 */
Socket *   socket_list;	/* All open descriptors		*/
Socket *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    is_shutdown;		/* Shutdown			*/
bool		    lockdown;		/* Game is lockdowned		*/
bool		    newbielockdown;		/* Game is newbielockdowned		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* time of this pulse */
bool		    MOBtrigger = TRUE;  /* act() switch                 */


/*
 * OS-dependent local functions.
 */

#if defined(unix)
void	RunMudLoop		args ( ( int control ) );
int	init_socket		args ( ( int port ) );
void	init_descriptor		args ( ( int control ) );
bool	read_from_descriptor	args ( ( Socket *d ) );
bool	write_to_descriptor	args ( ( int desc, const char *txt, int length ) );
#endif


/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args ( ( char *name ) );
bool	check_reconnect		args ( ( Socket *d, char *name,
									 bool fConn ) );
bool	check_playing		args ( ( Socket *d, char *name ) );
int	main			args ( ( int argc, char **argv ) );
void	nanny			args ( ( Socket *d, char *argument ) );
bool	process_output		args ( ( Socket *d, bool fPrompt ) );
void	read_from_buffer	args ( ( Socket *d ) );
void	stop_idling		args ( ( Creature *ch ) );
void    bust_a_prompt           args ( ( Creature *ch ) );


int main ( int argc, char **argv )
{
	struct timeval now_time;
	int port;
	int control;

	/*
	 * Init time.
	 */
	gettimeofday ( &now_time, NULL );
	current_time 	= ( time_t ) now_time.tv_sec;
	strcpy ( str_boot_time, ctime ( &current_time ) );

	/*
	 * Reserve one channel for our use.
	 */
	if ( ( fpReserve = fopen ( NULL_FILE, "r" ) ) == NULL ) {
		ReportErrno ( NULL_FILE );
		exit ( 1 );
	}

	/*
	 * Get the port number.
	 */
	port = 4000;
	if ( argc > 1 ) {
		if ( !is_number ( argv[1] ) ) {
			fprintf ( stderr, "Usage: %s [port #]\n", argv[0] );
			exit ( 1 );
		} else if ( ( port = atoi ( argv[1] ) ) <= 1024 ) {
			fprintf ( stderr, "Port number must be above 1024.\n" );
			exit ( 1 );
		}
	}

	build_directories();

	{
		FILE *fp = fopen ( "InfectEngine.pid", "w" );

		if ( fp ) {
			fprintf ( fp, "%d\n", getpid() );

			fflush ( fp );
			fclose ( fp );
			log_hd ( LOG_ALL, Format ( "InfectEngine is attached to pid %d", getpid() ) );
		} else {
			log_hd ( LOG_ERROR, Format ( "Could not write the pid number (%d) in InfectEngine.pid", getpid() ) );
		}
	}

	// -- attempt to generate our new event manager
	new EventManager();

	log_hd ( LOG_ALL, Format ( "Attempting to gain control of socket port #%d...", port ) );
	control = init_socket ( port );

	// -- boot_db does all its own processing
	boot_db( );

	EventManager::instance().BootupEvents();

	log_hd ( LOG_ALL, Format ( "InfectEngine v%s is now accepting connections on port %d.", getVersion(), port ) );

	RunMudLoop ( control );

	if ( EventManager::instancePtr() ) {
		delete EventManager::instancePtr();
	}

	log_hd ( LOG_ALL, Format ( "Control Value: %d is scheduled to be closed...", control ) );
	close ( control );

	log_hd ( LOG_ALL, "InfectEngine has terminated properly." );
	exit ( 0 );
	return 0;
}

#if defined(unix)
int init_socket ( int port )
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int x = 1;
	int fd;

	if ( ( fd = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		ReportErrno ( "Init_socket: socket" );
		exit ( 1 );
	}

	if ( setsockopt ( fd, SOL_SOCKET, SO_REUSEADDR,
					  ( char * ) &x, sizeof ( x ) ) < 0 ) {
		ReportErrno ( "Init_socket: SO_REUSEADDR" );
		close ( fd );
		exit ( 1 );
	}

#if defined(SO_DONTLINGER) && !defined(SYSV)
	{
		struct	linger	ld;

		ld.l_onoff  = 1;
		ld.l_linger = 1000;

		if ( setsockopt ( fd, SOL_SOCKET, SO_DONTLINGER,
						  ( char * ) &ld, sizeof ( ld ) ) < 0 ) {
			ReportErrno ( "Init_socket: SO_DONTLINGER" );
			close ( fd );
			exit ( 1 );
		}
	}
#endif

	sa		    = sa_zero;
	sa.sin_family   = AF_INET;
	sa.sin_port	    = htons ( port );

	if ( bind ( fd, ( struct sockaddr * ) &sa, sizeof ( sa ) ) < 0 ) {
		ReportErrno ( "Init socket: bind" );
		close ( fd );
		exit ( 1 );
	}


	if ( listen ( fd, 3 ) < 0 ) {
		ReportErrno ( "Init socket: listen" );
		close ( fd );
		exit ( 1 );
	}

	return fd;
}
#endif

#if defined(unix)
void RunMudLoop ( int control )
{
	static struct timeval null_time;
	struct timeval last_time;

	signal ( SIGPIPE, SIG_IGN );
	gettimeofday ( &last_time, NULL );
	current_time = ( time_t ) last_time.tv_sec;
	bool first_loop = true;

	/* Main loop */
	while ( !is_shutdown ) {
		fd_set in_set;
		fd_set out_set;
		fd_set exc_set;
		Socket *d;
		int maxdesc;

#if defined(MALLOC_DEBUG)
		if ( malloc_verify( ) != 1 )
		{ abort( ); }
#endif

		/*
		 * Poll all active descriptors.
		 */
		FD_ZERO ( &in_set  );
		FD_ZERO ( &out_set );
		FD_ZERO ( &exc_set );
		FD_SET ( control, &in_set );
		maxdesc	= control;
		for ( d = socket_list; d; d = d->next ) {
			maxdesc = UMAX ( maxdesc, d->descriptor );
			FD_SET ( d->descriptor, &in_set  );
			FD_SET ( d->descriptor, &out_set );
			FD_SET ( d->descriptor, &exc_set );
		}

		if ( select ( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) < 0 ) {
			ReportErrno ( "Game_loop: select: poll" );
			exit ( 1 );
		}

		/*
		 * New connection?
		 */
		if ( FD_ISSET ( control, &in_set ) )
		{ init_descriptor ( control ); }

		/*
		 * Kick out the freaky folks.
		 */
		for ( d = socket_list; d != NULL; d = d_next ) {
			d_next = d->next;
			if ( FD_ISSET ( d->descriptor, &exc_set ) ) {
				FD_CLR ( d->descriptor, &in_set  );
				FD_CLR ( d->descriptor, &out_set );
				if ( d->character && d->connected == CON_PLAYING )
				{ save_char_obj ( d->character ); }
				d->outtop	= 0;
				close_socket ( d );
			}
		}

		/*
		 * Process input.
		 */
		for ( d = socket_list; d != NULL; d = d_next ) {
			d_next	= d->next;
			d->fcommand	= FALSE;

			if ( FD_ISSET ( d->descriptor, &in_set ) ) {
				if ( d->character != NULL )
				{ d->character->timer = 0; }
				if ( !read_from_descriptor ( d ) ) {
					FD_CLR ( d->descriptor, &out_set );
					if ( d->character != NULL && d->connected == CON_PLAYING )
					{ save_char_obj ( d->character ); }
					d->outtop	= 0;
					close_socket ( d );
					continue;
				}
			}

			if ( d->character != NULL && d->character->daze > 0 )
			{ --d->character->daze; }

			if ( d->character != NULL && d->character->wait > 0 ) {
				--d->character->wait;
				continue;
			}

			read_from_buffer ( d );
			if ( d->incomm[0] != '\0' ) {
				d->fcommand	= TRUE;
				if ( d->pProtocol != NULL )
				{ d->pProtocol->WriteOOB = 0; }


				stop_idling ( d->character );

				/* OLC */
				if ( d->showstr_point )
				{ show_string ( d, d->incomm ); }
				else if ( d->character && d->character->queries.querycommand ) {
					log_hd ( LOG_COMMAND, Format ( "Q-Player:  %s ::  Argument:  %s",  d->character->name,
												   d->incomm ? d->incomm : "" ) );
					( *d->character->queries.queryfunc )
					( d->character, Format ( "queried_command:%p", d->character->queries.queryfunc ),
					  d->incomm, d->character->queries.querycommand );
				} else if ( d->pString )
				{ string_add ( d->character, d->incomm ); }
				else {
					switch ( d->connected ) {
						case CON_PLAYING:
							if ( !run_olc_editor ( d ) )
							{ substitute_alias ( d, d->incomm ); }
							break;
						default:
							nanny ( d, d->incomm );
							break;
					}
				}
				d->incomm[0]	= '\0';
			}
		}

		// -- attempt to control our errors and our updaters
		try {
			update_handler( );
		} catch ( ... ) {
			CATCH ( false );
		}

		// -- push our EventManager
		EventManager::instance().updateEvents();

		// -- handle output
		for ( d = socket_list; d != NULL; d = d_next ) {
			d_next = d->next;

			if ( ( d->fcommand || d->outtop > 0 )
					&&   FD_ISSET ( d->descriptor, &out_set ) ) {
				if ( !process_output ( d, TRUE ) ) {
					if ( d->character != NULL && d->connected == CON_PLAYING )
					{ save_char_obj ( d->character ); }
					d->outtop	= 0;
					close_socket ( d );
				}
			}
		}



		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			gettimeofday ( &now_time, NULL );
			usecDelta	= ( ( int ) last_time.tv_usec ) - ( ( int ) now_time.tv_usec )
						  + 1000000 / PULSE_PER_SECOND;
			secDelta	= ( ( int ) last_time.tv_sec ) - ( ( int ) now_time.tv_sec );
			while ( usecDelta < 0 ) {
				usecDelta += 1000000;
				secDelta  -= 1;
			}

			while ( usecDelta >= 1000000 ) {
				usecDelta -= 1000000;
				secDelta  += 1;
			}

			if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) ) {
				struct timeval stall_time;

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec  = secDelta;
				if ( select ( 0, NULL, NULL, NULL, &stall_time ) < 0 ) {
					ReportErrno ( "Game_loop: select: stall" );
					exit ( 1 );
				}
			}
		}

		gettimeofday ( &last_time, NULL );
		current_time = ( time_t ) last_time.tv_sec;

		if ( first_loop ) {
			first_loop = false;
			log_hd ( LOG_DEBUG, "Successfully looped through the first iteration of the MUD." );
		}

	}

	return;
}
#endif



#if defined(unix)
void init_descriptor ( int control )
{
	char buf[MAX_STRING_LENGTH];
	Socket *dnew;
	struct sockaddr_in sock;
	struct hostent *from;
	int desc;
	socklen_t size;

	size = sizeof ( sock );
	getsockname ( control, ( struct sockaddr * ) &sock, &size );
	if ( ( desc = accept ( control, ( struct sockaddr * ) &sock, &size ) ) < 0 ) {
		ReportErrno ( "init_descriptor: accept" );
		return;
	}

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

	if ( fcntl ( desc, F_SETFL, FNDELAY ) == -1 ) {
		ReportErrno ( "init_descriptor: fcntl: FNDELAY" );
		return;
	}

	/*
	 * Cons a new descriptor.
	 */
	dnew = new_descriptor();

	dnew->descriptor	= desc;
	dnew->connected	= CON_GET_NAME;
	dnew->showstr_head	= NULL;
	dnew->showstr_point = NULL;
	dnew->outsize	= 2000;
	dnew->pEdit		= NULL;			/* OLC */
	dnew->pString	= NULL;			/* OLC */
	dnew->editor	= 0;			/* OLC */
	ALLOC_DATA ( dnew->outbuf, char, dnew->outsize );
	dnew->pProtocol     = ProtocolCreate();

	size = sizeof ( sock );
	if ( getpeername ( desc, ( struct sockaddr * ) &sock, &size ) < 0 ) {
		ReportErrno ( "init_descriptor: getpeername" );
		dnew->host = assign_string ( "(unknown)" );
	} else {
		/*
		 * Would be nice to use inet_ntoa here but it takes a struct arg,
		 * which ain't very compatible between gcc and system libraries.
		 */
		int addr;

		addr = ntohl ( sock.sin_addr.s_addr );
		sprintf ( buf, "%d.%d.%d.%d",
				  ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
				  ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
				);
		log_hd ( LOG_SECURITY, Format ( "Sock.sinaddr:  %s", buf ) );
		from = gethostbyaddr ( ( char * ) &sock.sin_addr,
							   sizeof ( sock.sin_addr ), AF_INET );
		dnew->host = assign_string ( from ? from->h_name : buf );
	}

	/*
	 * Swiftest: I added the following to ban sites.  I don't
	 * endorse banning of sites, but Copper has few descriptors now
	 * and some people from certain sites keep abusing access by
	 * using automated 'autodialers' and leaving connections hanging.
	 *
	 * Furey: added suffix check by request of Nickel of HiddenWorlds.
	 */
	if ( check_ban ( dnew->host, BAN_ALL ) ) {
		write_to_descriptor ( desc, "Your site has been banned from this mud.\n\r", 0 );
		close ( desc );
		recycle_descriptor ( dnew );
		return;
	}
	// -- add our sockets to the list
	dnew->next		= socket_list;
	socket_list		= dnew;

	ProtocolNegotiate ( dnew );

	// -- send our greeting message
	{
		extern char * help_greeting;
		if ( help_greeting[0] == '.' )
		{ write_to_buffer ( dnew, help_greeting + 1, 0 ); }
		else
		{ write_to_buffer ( dnew, help_greeting  , 0 ); }
	}

	return;
}
#endif



void close_socket ( Socket *dclose )
{
	Creature *ch;

	if ( dclose->outtop > 0 )
	{ process_output ( dclose, FALSE ); }

	if ( dclose->snoop_by != NULL ) {
		write_to_buffer ( dclose->snoop_by,
						  "Your victim has left the game.\n\r", 0 );
	}

	{
		Socket *d;

		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->snoop_by == dclose )
			{ d->snoop_by = NULL; }
		}
	}

	if ( ( ch = dclose->character ) != NULL ) {
		log_hd ( LOG_SECURITY, Format ( "Closing link to %s.", ch->name ) );
		/* cut down on wiznet spam when rebooting */
		if ( dclose->connected == CON_PLAYING && !is_shutdown ) {
			act ( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
			wiznet ( "Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0 );
			ch->desc = NULL;
		} else {
			recycle_char ( dclose->original ? dclose->original :
						   dclose->character );
		}
	}

	if ( d_next == dclose )
	{ d_next = d_next->next; }

	if ( dclose == socket_list ) {
		socket_list = socket_list->next;
	} else {
		Socket *d;

		for ( d = socket_list; d && d->next != dclose; d = d->next )
			;
		if ( d != NULL )
		{ d->next = dclose->next; }
		else
		{ log_hd ( LOG_ERROR, "Close_socket: dclose not found." ); }
	}

	ProtocolDestroy ( dclose->pProtocol );

	close ( dclose->descriptor );
	recycle_descriptor ( dclose );
	return;
}



bool read_from_descriptor ( Socket *d )
{
	int iStart;
	static char read_buf[MAX_PROTOCOL_BUFFER];
	read_buf[0] = '\0';

	/* Hold horses if pending command already. */
	if ( d->incomm[0] != '\0' )
	{ return TRUE; }

	/* Check for overflow. */
	iStart = 0;
	if ( strlen ( d->inbuf ) >= sizeof ( d->inbuf ) - 10 ) {
		log_hd ( LOG_SECURITY, Format ( "%s input overflow! (possible spamming)", d->host ) );
		write_to_descriptor ( d->descriptor,
							  "\n\r*** Please refrain from Spamming ***\n\r", 0 );
		return FALSE;
	}

	while ( true ) {
		int nRead;

		nRead = read ( d->descriptor, read_buf + iStart,
					   sizeof ( read_buf ) - 10 - iStart );
		if ( nRead > 0 ) {
			iStart += nRead;
			if ( read_buf[iStart - 1] == '\n' || read_buf[iStart - 1] == '\r' )
			{ break; }
		} else if ( nRead == 0 ) {
			log_hd ( LOG_ERROR, Format ( "EOF encountered on read from %s.", d->host ) );
			return FALSE;
		} else if ( errno == EWOULDBLOCK )
		{ break; }
		else {
			ReportErrno ( "Read_from_descriptor" );
			return FALSE;
		}
	}

	read_buf[iStart] = '\0';
	ProtocolInput ( d, read_buf, iStart, d->inbuf );
	return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer ( Socket *d )
{
	int i, j, k;

	/*
	 * Hold horses if pending command already.
	 */
	if ( d->incomm[0] != '\0' )
	{ return; }

	// -- look for a new line.
	for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ ) {
		if ( d->inbuf[i] == '\0' )
		{ return; }
	}

	/*
	 * Canonical input processing.
	 */
	for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ ) {
		if ( k >= MAX_INPUT_LENGTH - 2 ) {
			write_to_descriptor ( d->descriptor, "Line too long.\n\r", 0 );

			/* skip the rest of the line */
			for ( ; d->inbuf[i] != '\0'; i++ ) {
				if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
				{ break; }
			}
			d->inbuf[i]   = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}

		if ( d->inbuf[i] == '\b' && k > 0 )
		{ --k; }
		else if ( isascii ( d->inbuf[i] ) && isprint ( d->inbuf[i] ) )
		{ d->incomm[k++] = d->inbuf[i]; }
	}

	/*
	 * Finish off the line.
	 */
	if ( k == 0 )
	{ d->incomm[k++] = ' '; }
	d->incomm[k] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */

	if ( k > 1 || d->incomm[0] == '!' ) {
		if ( d->incomm[0] != '!' && strcmp ( d->incomm, d->inlast ) ) {
			d->repeat = 0;
		} else {
			if ( ++d->repeat >= 25 && d->character
					&&  d->connected == CON_PLAYING ) {
				log_hd ( LOG_SECURITY, Format ( "%s input spamming!", d->host ) );
				wiznet ( "$N is input spamming!!",
						 d->character, NULL, WIZ_SPAM, 0, get_trust ( d->character ) );
				if ( d->incomm[0] == '!' )
					wiznet ( d->inlast, d->character, NULL, WIZ_SPAM, 0,
							 get_trust ( d->character ) );
				else
					wiznet ( d->incomm, d->character, NULL, WIZ_SPAM, 0,
							 get_trust ( d->character ) );

				d->repeat = 0;
				/*
						write_to_descriptor( d->descriptor,
						    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
						strcpy( d->incomm, "quit" );
				*/
			}
		}
	}


	/*
	 * Do '!' substitution.
	 */
	if ( d->incomm[0] == '!' )
	{ strcpy ( d->incomm, d->inlast ); }
	else
	{ strcpy ( d->inlast, d->incomm ); }

	/*
	 * Shift the input buffer.
	 */
	while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	{ i++; }
	for ( j = 0; ( d->inbuf[j] = d->inbuf[i + j] ) != '\0'; j++ )
		;
	return;
}



/*
 * Low level output function.
 */
bool process_output ( Socket *d, bool fPrompt )
{
	extern bool is_shutdown;

	// -- display our prompt (if necessary)
	if (  d->character && ( ( d->character->queries.querycommand ) || ( d->character->queries.queryintcommand ) ) ) {
		// -- display our queryprompt
		write_to_buffer ( d, d->character->queries.queryprompt, 0 );

		// -- zeroize it so we don't have any problems (should stop the spamming of screenreaders)
		memset ( d->character->queries.queryprompt, 0, sizeof ( d->character->queries.queryprompt ) );
	} else if ( d->pProtocol->WriteOOB )
		; /* The last sent data was OOB, so do NOT draw the prompt */
	else if ( !is_shutdown && d->showstr_point )
	{ write_to_buffer ( d, "\r\n\r\n\a[F500] { \a[F535]Shoot that Return Key \a[F500]} \an\r\n\r\n", 0 ); }
	else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
	{ write_to_buffer ( d, "} ", 2 ); }
	else if ( fPrompt && d->connected == CON_PLAYING ) {
		Creature *ch;
		Creature *victim;

		ch = d->character;

		/* battle prompt */
		if ( ( victim = ch->fighting ) != NULL && can_see ( ch, victim ) ) {
			int percent;
			char wound[100];
			char buf[MAX_STRING_LENGTH];

			if ( victim->max_hit > 0 )
			{ percent = victim->hit * 100 / victim->max_hit; }
			else
			{ percent = -1; }

			if ( percent >= 100 )
			{ sprintf ( wound, "is in excellent condition." ); }
			else if ( percent >= 90 )
			{ sprintf ( wound, "has a few scratches." ); }
			else if ( percent >= 75 )
			{ sprintf ( wound, "has some small wounds and bruises." ); }
			else if ( percent >= 50 )
			{ sprintf ( wound, "has quite a few wounds." ); }
			else if ( percent >= 30 )
			{ sprintf ( wound, "has some big nasty wounds and scratches." ); }
			else if ( percent >= 15 )
			{ sprintf ( wound, "looks pretty hurt." ); }
			else if ( percent >= 0 )
			{ sprintf ( wound, "is in awful condition." ); }
			else
			{ sprintf ( wound, "is bleeding to death." ); }

			snprintf ( buf, sizeof ( buf ), "%s %s \n\r",
					   IS_NPC ( victim ) ? victim->short_descr : victim->name, wound );
			buf[0] = UPPER ( buf[0] );
			write_to_buffer ( d, buf, 0 );
		}


		ch = d->original ? d->original : d->character;
		if ( !IS_SET ( ch->comm, COMM_COMPACT ) )
		{ write_to_buffer ( d, "\n\r", 2 ); }


		if ( IS_SET ( ch->comm, COMM_PROMPT ) )
		{ bust_a_prompt ( d->character ); }

		if ( IS_SET ( ch->comm, COMM_TELNET_GA ) )
		{ write_to_buffer ( d, go_ahead_str, 0 ); }
	}

	/*
	 * Short-circuit if nothing to write.
	 */
	if ( d->outtop == 0 )
	{ return TRUE; }

	// -- snooping control
	if ( d->snoop_by != NULL ) {
		if ( d->character != NULL )
		{ write_to_buffer ( d->snoop_by, Format ( "\ac{ \aR%s \ac}\r\n", d->character->name ), 0 ); }
		write_to_buffer ( d->snoop_by, "\ay-----------------------------------------------------\an\r\n", 0 );
		write_to_buffer ( d->snoop_by, d->outbuf, d->outtop );
		write_to_buffer ( d->snoop_by, "\ay-----------------------------------------------------\an\r\n", 0 );
	}

	// -- write our output
	if ( !write_to_descriptor ( d->descriptor, d->outbuf, d->outtop ) ) {
		d->outtop = 0;
		return FALSE;
	} else {
		d->outtop = 0;
		return TRUE;
	}
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt ( Creature *ch )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	const char *str;
	const char *i;
	char *point;
	char doors[MAX_INPUT_LENGTH];
	Exit *pexit;
	bool found;
	const char *dir_name[] = {"N", "E", "S", "W", "U", "D"};
	int door;

	point = buf;
	str = ch->prompt;
	if ( str == NULL || str[0] == '\0' ) {
		sprintf ( buf, "<%dhp %dm %dmv> %s",
				  ch->hit, ch->mana, ch->move, ch->prefix );
		writeBuffer ( buf, ch );
		return;
	}

	if ( IS_SET ( ch->comm, COMM_AFK ) ) {
		writeBuffer ( "<AFK> ", ch );
		return;
	}

	if ( ch->desc && ch->desc->editor ) {
		writeBuffer ( Format ( "\ac{\ay%s \aR: \ay%s\ac}\an ", olc_ed_name ( ch ), olc_ed_vnum ( ch ) ), ch );
		return;
	}


	while ( *str != '\0' ) {
		if ( *str != '%' ) {
			*point++ = *str++;
			continue;
		}
		++str;
		switch ( *str ) {
			default :
				i = " ";
				break;
			case 'e':
				found = FALSE;
				doors[0] = '\0';
				for ( door = 0; door < 6; door++ ) {
					if ( ( pexit = ch->in_room->exit[door] ) != NULL
							&&  pexit ->u1.to_room != NULL
							&&  ( can_see_room ( ch, pexit->u1.to_room )
								  ||   ( IS_AFFECTED ( ch, AFF_INFRARED )
										 &&    !IS_AFFECTED ( ch, AFF_BLIND ) ) )
							&&  !IS_SET ( pexit->exit_info, EX_CLOSED ) ) {
						found = TRUE;
						strcat ( doors, dir_name[door] );
					}
				}
				if ( !found )
				{ strcat ( buf, "none" ); }
				sprintf ( buf2, "%s", doors );
				i = buf2;
				break;
			case 'c' :
				sprintf ( buf2, "%s", "\n\r" );
				i = buf2;
				break;
			case 'h' :
				sprintf ( buf2, "%d", ch->hit );
				i = buf2;
				break;
			case 'H' :
				sprintf ( buf2, "%d", ch->max_hit );
				i = buf2;
				break;
			case 'm' :
				sprintf ( buf2, "%d", ch->mana );
				i = buf2;
				break;
			case 'M' :
				sprintf ( buf2, "%d", ch->max_mana );
				i = buf2;
				break;
			case 'v' :
				sprintf ( buf2, "%d", ch->move );
				i = buf2;
				break;
			case 'V' :
				sprintf ( buf2, "%d", ch->max_move );
				i = buf2;
				break;
			case 'x' :
				sprintf ( buf2, "%d", ch->exp );
				i = buf2;
				break;
			case 'X' :
				sprintf ( buf2, "%d", IS_NPC ( ch ) ? 0 :
						  ( ch->level + 1 ) * exp_per_level ( ch, ch->pcdata->points ) - ch->exp );
				i = buf2;
				break;
			case 'g' :
				sprintf ( buf2, "%ld", ch->gold );
				i = buf2;
				break;
			case 's' :
				sprintf ( buf2, "%ld", ch->silver );
				i = buf2;
				break;
			case 'a' :
				if ( ch->level > 9 )
				{ sprintf ( buf2, "%d", ch->alignment ); }
				else
					sprintf ( buf2, "%s", IS_GOOD ( ch ) ? "good" : IS_EVIL ( ch ) ?
							  "evil" : "neutral" );
				i = buf2;
				break;
			case 'r' :
				if ( ch->in_room != NULL )
					sprintf ( buf2, "%s",
							  ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) ) ||
								( !IS_AFFECTED ( ch, AFF_BLIND ) && !room_is_dark ( ch->in_room ) ) )
							  ? ch->in_room->name : "darkness" );
				else
				{ sprintf ( buf2, " " ); }
				i = buf2;
				break;
			case 'R' :
				if ( IsStaff ( ch ) && ch->in_room != NULL )
				{ sprintf ( buf2, "%d", ch->in_room->vnum ); }
				else
				{ sprintf ( buf2, " " ); }
				i = buf2;
				break;
			case 'z' :
				if ( IsStaff ( ch ) && ch->in_room != NULL )
				{ sprintf ( buf2, "%s", ch->in_room->area->name ); }
				else
				{ sprintf ( buf2, " " ); }
				i = buf2;
				break;
			case '%' :
				sprintf ( buf2, "%%" );
				i = buf2;
				break;
			case 'o' :
				sprintf ( buf2, "%s", olc_ed_name ( ch ) );
				i = buf2;
				break;
			case 'O' :
				sprintf ( buf2, "%s", olc_ed_vnum ( ch ) );
				i = buf2;
				break;
		}
		++str;
		while ( ( *point = *i ) != '\0' )
		{ ++point, ++i; }
	}
	write_to_buffer ( ch->desc, buf, point - buf );

	if ( !IS_NULLSTR ( ch->prefix ) )
	{ write_to_buffer ( ch->desc, ch->prefix, 0 ); }
	return;
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer ( Socket *d, const char *txt, int length )
{
	txt = ProtocolOutput ( d, txt, &length );
	if ( d->pProtocol->WriteOOB > 0 )
	{ --d->pProtocol->WriteOOB; }


	/*
	 * Find length in case caller didn't.
	 */
	if ( length <= 0 )
	{ length = strlen ( txt ); }

	/*
	 * Initial \n\r if needed.
	 */
	if ( d->outtop == 0 && !d->fcommand && !d->pProtocol->WriteOOB ) {
		d->outbuf[0]	= '\n';
		d->outbuf[1]	= '\r';
		d->outtop	= 2;
	}

	/*
	 * Expand the buffer as needed.
	 */
	while ( d->outtop + length >= d->outsize ) {
		char *outbuf;

		if ( d->outsize >= 32000 ) {
			// -- attempt to get the buffer overflow logged properly.
			log_hd ( LOG_ERROR, Format ( "Buffer overflow: %d/%s\n\r", d->descriptor, d->host ) );
			close_socket ( d );
			return;
		}
		ALLOC_DATA ( outbuf, char, 2 * d->outsize );

		strncpy ( outbuf, d->outbuf, d->outtop );
		PURGE_DATA ( d->outbuf );
		d->outbuf   = outbuf;
		d->outsize *= 2;
	}

	/*
	 * Copy.
	 */
	strncpy ( d->outbuf + d->outtop, txt, length );
	d->outtop += length;
	return;
}


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor ( int desc, const char *txt, int length )
{
	int iStart;
	int nWrite;
	int nBlock;

	if ( length <= 0 )
	{ length = strlen ( txt ); }

	for ( iStart = 0; iStart < length; iStart += nWrite ) {
		nBlock = UMIN ( length - iStart, 4096 );
		if ( ( nWrite = write ( desc, txt + iStart, nBlock ) ) < 0 )
		{ ReportErrno ( "Write_to_descriptor" ); return FALSE; }
	}

	return TRUE;
}


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny ( Socket *d, char *argument )
{
	Socket *d_old, *d_next;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *ch;
	char *pwdnew;
	char *p;
	int iClass, race, i, weapon;
	bool fOld;

	while ( isspace ( *argument ) )
	{ argument++; }

	ch = d->character;

	switch ( d->connected ) {

		default:
			log_hd ( LOG_ERROR, Format ( "Nanny: bad d(%d)(%p)->connected %d.", d->descriptor, d, d->connected ) );
			close_socket ( d );
			return;

		case CON_GET_NAME:
			if ( argument[0] == '\0' ) {
				close_socket ( d );
				return;
			}

			argument[0] = UPPER ( argument[0] );
			if ( !check_parse_name ( argument ) ) {
				write_to_buffer ( d, "Illegal name, try another.\n\rName: ", 0 );
				return;
			}

			fOld = load_char_obj ( d, argument );
			ch   = d->character;

			if ( IS_SET ( ch->act, PLR_DENY ) ) {
				log_hd ( LOG_SECURITY, Format ( "Denying access to %s@%s.", argument, d->host ) );
				write_to_buffer ( d, "You are denied access.\n\r", 0 );
				close_socket ( d );
				return;
			}

			if ( check_ban ( d->host, BAN_PERMIT ) && !IS_SET ( ch->act, PLR_PERMIT ) ) {
				write_to_buffer ( d, "Your site has been banned from this mud.\n\r", 0 );
				close_socket ( d );
				return;
			}

			if ( check_reconnect ( d, argument, FALSE ) ) {
				fOld = TRUE;
			} else {
				if ( lockdown && !IsStaff ( ch ) ) {
					write_to_buffer ( d, "The game is currently in lockdown.\n\r", 0 );
					close_socket ( d );
					return;
				}
			}

			if ( fOld ) {
				/* Old player */
				write_to_buffer ( d, "Password: ", 0 );
				ProtocolNoEcho ( d, true );

				d->connected = CON_GET_OLD_PASSWORD;
				return;
			} else {
				/* New player */
				if ( newbielockdown ) {
					write_to_buffer ( d, "The game is currently blocking new players.\n\r", 0 );
					close_socket ( d );
					return;
				}

				if ( check_ban ( d->host, BAN_NEWBIES ) ) {
					write_to_buffer ( d,
									  "New players are not allowed from your site.\n\r", 0 );
					close_socket ( d );
					return;
				}

				sprintf ( buf, "Did I get that right, %s (Y/N)? ", argument );
				write_to_buffer ( d, buf, 0 );
				d->connected = CON_CONFIRM_NEW_NAME;
				return;
			}
			break;

		case CON_GET_OLD_PASSWORD:
#if defined(unix)
			write_to_buffer ( d, "\n\r", 2 );
#endif

			if ( strcmp ( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) ) {
				write_to_buffer ( d, "Wrong password.\n\r", 0 );
				close_socket ( d );
				return;
			}

			ProtocolNoEcho ( d, false );

			if ( check_playing ( d, ch->name ) )
			{ return; }

			if ( check_reconnect ( d, ch->name, TRUE ) )
			{ return; }

			log_hd ( LOG_SECURITY, Format ( "%s@%s has connected.", ch->name, d->host ) );
			wiznet ( Format ( "%s@%s has connected.", ch->name, d->host ), NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );

			if ( IsStaff ( ch ) ) {
				cmd_function ( ch, &cmd_help, "imotd" );
				d->connected = CON_READ_IMOTD;
			} else {
				cmd_function ( ch, &cmd_help, "motd" );
				d->connected = CON_READ_MOTD;
			}
			break;

		/* RT code for breaking link */

		case CON_BREAK_CONNECT:
			switch ( *argument ) {
				case 'y' :
				case 'Y':
					for ( d_old = socket_list; d_old != NULL; d_old = d_next ) {
						d_next = d_old->next;
						if ( d_old == d || d_old->character == NULL )
						{ continue; }

						if ( str_cmp ( ch->name, d_old->original ?
									   d_old->original->name : d_old->character->name ) )
						{ continue; }

						close_socket ( d_old );
					}
					if ( check_reconnect ( d, ch->name, TRUE ) )
					{ return; }
					write_to_buffer ( d, "Reconnect attempt failed.\n\rName: ", 0 );
					if ( d->character != NULL ) {
						recycle_char ( d->character );
						d->character = NULL;
					}
					d->connected = CON_GET_NAME;
					break;

				case 'n' :
				case 'N':
					write_to_buffer ( d, "Name: ", 0 );
					if ( d->character != NULL ) {
						recycle_char ( d->character );
						d->character = NULL;
					}
					d->connected = CON_GET_NAME;
					break;

				default:
					write_to_buffer ( d, "Please type Y or N? ", 0 );
					break;
			}
			break;

		case CON_CONFIRM_NEW_NAME:
			switch ( *argument ) {
				case 'y':
				case 'Y':
					sprintf ( buf, "New character.\n\rGive me a password for %s",
							  ch->name );
					write_to_buffer ( d, buf, 0 );
					ProtocolNoEcho ( d, true );

					d->connected = CON_GET_NEW_PASSWORD;
					break;

				case 'n':
				case 'N':
					write_to_buffer ( d, "Ok, what IS it, then? ", 0 );
					recycle_char ( d->character );
					d->character = NULL;
					d->connected = CON_GET_NAME;
					break;

				default:
					write_to_buffer ( d, "Please type Yes or No? ", 0 );
					break;
			}
			break;

		case CON_GET_NEW_PASSWORD:
#if defined(unix)
			write_to_buffer ( d, "\n\r", 2 );
#endif

			if ( strlen ( argument ) < 7 ) {
				write_to_buffer ( d,
								  "Password must be at least seven characters long.\n\rPassword: ",
								  0 );
				return;
			}

			if ( !strstr ( argument, "1" ) &&
					!strstr ( argument, "2" ) &&
					!strstr ( argument, "3" ) &&
					!strstr ( argument, "4" ) &&
					!strstr ( argument, "5" ) &&
					!strstr ( argument, "6" ) &&
					!strstr ( argument, "7" ) &&
					!strstr ( argument, "8" ) &&
					!strstr ( argument, "9" ) &&
					!strstr ( argument, "0" ) ) {
				write_to_buffer ( d, "You must have a number in your password.\r\nPassword: ", 0 );
				return;
			}

			pwdnew = crypt ( argument, ch->name );
			for ( p = pwdnew; *p != '\0'; p++ ) {
				if ( *p == '~' ) {
					write_to_buffer ( d,
									  "New password not acceptable, try again.\n\rPassword: ",
									  0 );
					return;
				}
			}

			PURGE_DATA ( ch->pcdata->pwd );
			ch->pcdata->pwd	= assign_string ( pwdnew );
			write_to_buffer ( d, "Please retype password: ", 0 );
			d->connected = CON_CONFIRM_NEW_PASSWORD;
			break;

		case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
			write_to_buffer ( d, "\n\r", 2 );
#endif

			if ( strcmp ( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) ) {
				write_to_buffer ( d, "Passwords don't match.\n\rRetype password: ",
								  0 );
				d->connected = CON_GET_NEW_PASSWORD;
				return;
			}

			ProtocolNoEcho ( d, false );
			write_to_buffer ( d, "The following races are available:\n\r  ", 0 );
			for ( race = 1; race_table[race].name != NULL; race++ ) {
				if ( !race_table[race].pc_race )
				{ break; }
				write_to_buffer ( d, race_table[race].name, 0 );
				write_to_buffer ( d, " ", 1 );
			}
			write_to_buffer ( d, "\n\r", 0 );
			write_to_buffer ( d, "What is your race (help for more information)? ", 0 );
			d->connected = CON_GET_NEW_RACE;
			break;

		case CON_GET_NEW_RACE:
			one_argument ( argument, arg );

			if ( !strcmp ( arg, "help" ) ) {
				argument = one_argument ( argument, arg );
				if ( argument[0] == '\0' )
				{ cmd_function ( ch, &cmd_help, "race help" ); }
				else
				{ cmd_function ( ch, &cmd_help, argument ); }
				write_to_buffer ( d,
								  "What is your race (help for more information)? ", 0 );
				break;
			}

			race = race_lookup ( argument );

			if ( race == 0 || !race_table[race].pc_race ) {
				write_to_buffer ( d, "That is not a valid race.\n\r", 0 );
				write_to_buffer ( d, "The following races are available:\n\r  ", 0 );
				for ( race = 1; race_table[race].name != NULL; race++ ) {
					if ( !race_table[race].pc_race )
					{ break; }
					write_to_buffer ( d, race_table[race].name, 0 );
					write_to_buffer ( d, " ", 1 );
				}
				write_to_buffer ( d, "\n\r", 0 );
				write_to_buffer ( d,
								  "What is your race? (help for more information) ", 0 );
				break;
			}

			ch->race = race;
			/* initialize stats */
			for ( i = 0; i < MAX_STATS; i++ )
			{ ch->perm_stat[i] = pc_race_table[race].stats[i]; }
			ch->affected_by = ch->affected_by | race_table[race].aff;
			ch->imm_flags	= ch->imm_flags | race_table[race].imm;
			ch->res_flags	= ch->res_flags | race_table[race].res;
			ch->vuln_flags	= ch->vuln_flags | race_table[race].vuln;
			ch->form	= race_table[race].form;
			ch->parts	= race_table[race].parts;

			/* add skills */
			for ( i = 0; i < 5; i++ ) {
				if ( pc_race_table[race].skills[i] == NULL )
				{ break; }
				group_add ( ch, pc_race_table[race].skills[i], FALSE );
			}
			/* add cost */
			ch->pcdata->points = pc_race_table[race].points;
			ch->size = pc_race_table[race].size;

			write_to_buffer ( d, "What is your sex (M/F)? ", 0 );
			d->connected = CON_GET_NEW_SEX;
			break;


		case CON_GET_NEW_SEX:
			switch ( argument[0] ) {
				case 'm':
				case 'M':
					ch->sex = SEX_MALE;
					ch->pcdata->true_sex = SEX_MALE;
					break;
				case 'f':
				case 'F':
					ch->sex = SEX_FEMALE;
					ch->pcdata->true_sex = SEX_FEMALE;
					break;
				default:
					write_to_buffer ( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
					return;
			}

			strcpy ( buf, "Select a archetype [" );
			for ( iClass = 0; iClass < MAX_CLASS; iClass++ ) {
				if ( iClass > 0 )
				{ strcat ( buf, " " ); }
				strcat ( buf, archetype_table[iClass].name );
			}
			strcat ( buf, "]: " );
			write_to_buffer ( d, buf, 0 );
			d->connected = CON_GET_NEW_CLASS;
			break;

		case CON_GET_NEW_CLASS:
			iClass = archetype_lookup ( argument );

			if ( iClass == -1 ) {
				write_to_buffer ( d,
								  "That's not a archetype.\n\rWhat IS your archetype? ", 0 );
				return;
			}

			ch->archetype = iClass;

			log_hd ( LOG_SECURITY, Format ( "%s@%s new player.", ch->name, d->host ) );
			wiznet ( "Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0 );
			wiznet ( Format ( "%s@%s new player.", ch->name, d->host ), NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );

			write_to_buffer ( d, "\n\r", 2 );
			write_to_buffer ( d, "You may be good, neutral, or evil.\n\r", 0 );
			write_to_buffer ( d, "Which alignment (G/N/E)? ", 0 );
			d->connected = CON_GET_ALIGNMENT;
			break;

		case CON_GET_ALIGNMENT:
			switch ( argument[0] ) {
				case 'g' :
				case 'G' :
					ch->alignment = 750;
					break;
				case 'n' :
				case 'N' :
					ch->alignment = 0;
					break;
				case 'e' :
				case 'E' :
					ch->alignment = -750;
					break;
				default:
					write_to_buffer ( d, "That's not a valid alignment.\n\r", 0 );
					write_to_buffer ( d, "Which alignment (G/N/E)? ", 0 );
					return;
			}

			write_to_buffer ( d, "\n\r", 0 );

			group_add ( ch, "rom basics", FALSE );
			group_add ( ch, archetype_table[ch->archetype].base_group, FALSE );
			ch->pcdata->learned[gsn_recall] = 50;
			write_to_buffer ( d, "Do you wish to customize this character?\n\r", 0 );
			write_to_buffer ( d, "Customization takes time, but allows a wider range of skills and abilities.\n\r", 0 );
			write_to_buffer ( d, "Customize (Y/N)? ", 0 );
			d->connected = CON_DEFAULT_CHOICE;
			break;

		case CON_DEFAULT_CHOICE:
			write_to_buffer ( d, "\n\r", 2 );
			switch ( argument[0] ) {
				case 'y':
				case 'Y':
					ch->gen_data = new_gen_data();
					ch->gen_data->points_chosen = ch->pcdata->points;
					cmd_function ( ch, &cmd_help, "group header" );
					list_group_costs ( ch );
					write_to_buffer ( d, "You already have the following skills:\n\r", 0 );
					cmd_function ( ch, &cmd_skills, "" );
					cmd_function ( ch, &cmd_help, "menu choice" );
					d->connected = CON_GEN_GROUPS;
					break;
				case 'n':
				case 'N':
					group_add ( ch, archetype_table[ch->archetype].default_group, TRUE );
					write_to_buffer ( d, "\n\r", 2 );
					write_to_buffer ( d,
									  "Please pick a weapon from the following choices:\n\r", 0 );
					buf[0] = '\0';
					for ( i = 0; weapon_table[i].name != NULL; i++ )
						if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 ) {
							strcat ( buf, weapon_table[i].name );
							strcat ( buf, " " );
						}
					strcat ( buf, "\n\rYour choice? " );
					write_to_buffer ( d, buf, 0 );
					d->connected = CON_PICK_WEAPON;
					break;
				default:
					write_to_buffer ( d, "Please answer (Y/N)? ", 0 );
					return;
			}
			break;

		case CON_PICK_WEAPON:
			write_to_buffer ( d, "\n\r", 2 );
			weapon = weapon_lookup ( argument );
			if ( weapon == -1 || ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0 ) {
				write_to_buffer ( d,
								  "That's not a valid selection. Choices are:\n\r", 0 );
				buf[0] = '\0';
				for ( i = 0; weapon_table[i].name != NULL; i++ )
					if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 ) {
						strcat ( buf, weapon_table[i].name );
						strcat ( buf, " " );
					}
				strcat ( buf, "\n\rYour choice? " );
				write_to_buffer ( d, buf, 0 );
				return;
			}

			ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
			write_to_buffer ( d, "\n\r", 2 );
			cmd_function ( ch, &cmd_help, "motd" );
			d->connected = CON_READ_MOTD;
			break;

		case CON_GEN_GROUPS:
			writeBuffer ( "\n\r", ch );

			if ( !str_cmp ( argument, "done" ) ) {
				if ( ch->pcdata->points == pc_race_table[ch->race].points ) {
					writeBuffer ( "You didn't pick anything.\n\r", ch );
					break;
				}

				if ( ch->pcdata->points <= 40 + pc_race_table[ch->race].points ) {
					snprintf ( buf, sizeof ( buf ),
							   "You must take at least %d points of skills and groups",
							   40 + pc_race_table[ch->race].points );
					writeBuffer ( buf, ch );
					break;
				}

				snprintf ( buf, sizeof ( buf ), "Creation points: %d\n\r", ch->pcdata->points );
				writeBuffer ( buf, ch );
				snprintf ( buf, sizeof ( buf ), "Experience per level: %d\n\r",
						   exp_per_level ( ch, ch->gen_data->points_chosen ) );
				if ( ch->pcdata->points < 40 )
				{ ch->train = ( 40 - ch->pcdata->points + 1 ) / 2; }
				recycle_gen_data ( ch->gen_data );
				ch->gen_data = NULL;
				writeBuffer ( buf, ch );
				write_to_buffer ( d, "\n\r", 2 );
				write_to_buffer ( d,
								  "Please pick a weapon from the following choices:\n\r", 0 );
				buf[0] = '\0';
				for ( i = 0; weapon_table[i].name != NULL; i++ )
					if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 ) {
						strcat ( buf, weapon_table[i].name );
						strcat ( buf, " " );
					}
				strcat ( buf, "\n\rYour choice? " );
				write_to_buffer ( d, buf, 0 );
				d->connected = CON_PICK_WEAPON;
				break;
			}

			if ( !parse_gen_groups ( ch, argument ) )
				writeBuffer (
					"Choices are: list,learned,premise,add,drop,info,help, and done.\n\r"
					, ch );

			cmd_function ( ch, &cmd_help, "menu choice" );
			break;

		case CON_READ_IMOTD:
			write_to_buffer ( d, "\n\r", 2 );
			cmd_function ( ch, &cmd_help, "motd" );
			d->connected = CON_READ_MOTD;
			break;

		case CON_READ_MOTD:
			if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0' ) {
				write_to_buffer ( d, "Warning! Null password!\n\r", 0 );
				write_to_buffer ( d, "Please report old password with bug.\n\r", 0 );
				write_to_buffer ( d, "Type 'password null <new password>' to fix.\n\r", 0 );
			}

			write_to_buffer ( d, "\n\r\awThe \arI\aRn\arf\aRe\arc\aRt\ar\aRe\ard \awCity welcomes you to the chaos!\an\n\r", 0 );

			ch->next	= char_list;
			char_list	= ch;

			d->connected	= CON_PLAYING;
			reset_char ( ch );

			if ( ch->level == 0 ) {

				ch->perm_stat[archetype_table[ch->archetype].attr_prime] += 3;

				ch->level	= 1;
				ch->exp	= exp_per_level ( ch, ch->pcdata->points );
				ch->hit	= ch->max_hit;
				ch->mana	= ch->max_mana;
				ch->move	= ch->max_move;
				ch->train	 = 3;
				ch->practice = 5;

				set_title ( ch, ( char * ) "is attempting to survive level 1." );

				cmd_function ( ch, &cmd_outfit, "" );
				obj_to_char ( create_object ( get_obj_index ( OBJ_VNUM_MAP ), 0 ), ch );

				char_to_room ( ch, get_room_index ( ROOM_VNUM_SCHOOL ) );
				writeBuffer ( "\n\r", ch );
				cmd_function ( ch, &cmd_help, "newbie info" );
				writeBuffer ( "\n\r", ch );
			} else if ( ch->in_room != NULL ) {
				char_to_room ( ch, ch->in_room );
			} else if ( IsStaff ( ch ) ) {
				char_to_room ( ch, get_room_index ( ROOM_VNUM_CHAT ) );
			} else {
				char_to_room ( ch, get_room_index ( ROOM_VNUM_TEMPLE ) );
			}

			act ( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
			cmd_function ( ch, &cmd_look, "auto" );

			MXPSendTag ( d, "<VERSION>" );

			wiznet ( "$N has left real life behind.", ch, NULL,
					 WIZ_LOGINS, WIZ_SITES, get_trust ( ch ) );

			if ( ch->pet != NULL ) {
				char_to_room ( ch->pet, ch->in_room );
				act ( "$n has entered the game.", ch->pet, NULL, NULL, TO_ROOM );
			}

			// -- increment our total connections.
			connect_count++;

			// -- haven't proven ourselves stable, so save them all!
			FILE *fp = NULL;
			if ( ( fp = fopen ( CONNECTED_DAT, "w" ) ) != NULL ) {
				fwrite ( &connect_count, sizeof ( unsigned long int ), 1, fp );
				fclose ( fp );
			}

			// -- make it known that we have jumped up like this!
			writeBuffer ( Format ( "\n\rYou are the [%ld] person to connect since February 3rd, 2014.\n\r", connect_count ), ch );
			if ( ( connect_count % 500 ) == 0 ) {
				log_hd ( LOG_BASIC, Format ( "%s WAS THE %ld person to connect to %s!", ch->name, connect_count, "The Infected City" ) );
				announce ( Format ( "**** %s IS THE %ld PERSON TO CONNECT TO THE INFECTED CITY!", ch->name, connect_count ) );
				tweetStatement ( Format ( "%s was the %ld connection to %s", ch->name, connect_count, "The Infected City" ) );
			}

			cmd_function ( ch, &cmd_unread, "" );
			break;
	}

	return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name ( char *name )
{
	int clan;

	/*
	 * Reserved words.
	 */
	if ( is_exact_name ( name, "all auto immortal self someone something the you loner none new delete" ) ) {
		return FALSE;
	}

	/* check clans */
	for ( clan = 0; clan < MAX_CLAN; clan++ ) {
		if ( LOWER ( name[0] ) == LOWER ( clan_table[clan].name[0] )
				&&  !str_cmp ( name, clan_table[clan].name ) )
		{ return FALSE; }
	}

	if ( str_cmp ( capitalize ( name ), "Alander" ) && ( !str_prefix ( "Alan", name )
			|| !str_suffix ( "Alander", name ) ) )
	{ return FALSE; }

	/*
	 * Length restrictions.
	 */

	if ( strlen ( name ) <  2 )
	{ return FALSE; }

#if defined(MSDOS)
	if ( strlen ( name ) >  8 )
	{ return FALSE; }
#endif

#if defined(macintosh) || defined(unix)
	if ( strlen ( name ) > 12 )
	{ return FALSE; }
#endif

	// -- no naming ourselves after commands.
	/*for ( int cmd = 0; cmd_table[cmd].name; cmd++ ) {
		if ( !str_cmp ( name, cmd_table[cmd].name ) ) {
			return false;
		}
	} */

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll, adjcaps = FALSE, cleancaps = FALSE;
		int total_caps = 0;

		fIll = TRUE;
		for ( pc = name; *pc != '\0'; pc++ ) {
			if ( !isalpha ( *pc ) )
			{ return FALSE; }

			if ( isupper ( *pc ) ) { /* ugly anti-caps hack */
				if ( adjcaps )
				{ cleancaps = TRUE; }
				total_caps++;
				adjcaps = TRUE;
			} else
			{ adjcaps = FALSE; }

			if ( LOWER ( *pc ) != 'i' && LOWER ( *pc ) != 'l' )
			{ fIll = FALSE; }
		}

		if ( fIll )
		{ return FALSE; }

		if ( cleancaps || ( total_caps > ( ( signed int ) strlen ( name ) ) / 2 && ( signed int ) strlen ( name ) < 3 ) )
		{ return FALSE; }
	}

	/*
	 * Prevent players from naming themselves after mobs.
	 */
	{
		extern NPCData *mob_index_hash[MAX_KEY_HASH];
		NPCData *pMobIndex;
		int iHash;

		for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
			for ( pMobIndex  = mob_index_hash[iHash];
					pMobIndex != NULL;
					pMobIndex  = pMobIndex->next ) {
				if ( is_name ( name, pMobIndex->player_name ) )
				{ return FALSE; }
			}
		}
	}

	return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect ( Socket *d, char *name, bool fConn )
{
	Creature *ch;

	for ( ch = char_list; ch != NULL; ch = ch->next ) {
		if ( !IS_NPC ( ch )
				&&   ( !fConn || ch->desc == NULL )
				&&   !str_cmp ( d->character->name, ch->name ) ) {
			if ( fConn == FALSE ) {
				PURGE_DATA ( d->character->pcdata->pwd );
				d->character->pcdata->pwd = assign_string ( ch->pcdata->pwd );
			} else {
				recycle_char ( d->character );
				d->character = ch;
				ch->desc	 = d;
				ch->timer	 = 0;
				writeBuffer (
					"Reconnecting. Type replay to see missed tells.\n\r", ch );
				act ( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );

				MXPSendTag ( d, "<VERSION>" );

				log_hd ( LOG_SECURITY, Format ( "%s@%s reconnected.", ch->name, d->host ) );
				wiznet ( "$N groks the fullness of $S link.",
						 ch, NULL, WIZ_LINKS, 0, 0 );
				d->connected = CON_PLAYING;
			}
			return TRUE;
		}
	}

	return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing ( Socket *d, char *name )
{
	Socket *dold;

	for ( dold = socket_list; dold; dold = dold->next ) {
		if ( dold != d
				&&   dold->character != NULL
				&&   dold->connected != CON_GET_NAME
				&&   dold->connected != CON_GET_OLD_PASSWORD
				&&   !str_cmp ( name, dold->original
								? dold->original->name : dold->character->name ) ) {
			write_to_buffer ( d, "That character is already playing.\n\r", 0 );
			write_to_buffer ( d, "Do you wish to connect anyway (Y/N)?", 0 );
			d->connected = CON_BREAK_CONNECT;
			return TRUE;
		}
	}

	return FALSE;
}



void stop_idling ( Creature *ch )
{
	if ( ch == NULL
			||   ch->desc == NULL
			||   ch->desc->connected != CON_PLAYING
			||   ch->was_in_room == NULL
			||   ch->in_room != get_room_index ( ROOM_VNUM_LIMBO ) )
	{ return; }

	ch->timer = 0;
	char_from_room ( ch );
	char_to_room ( ch, ch->was_in_room );
	ch->was_in_room	= NULL;
	act ( "$n has reformed from a pile of ash.", ch, NULL, NULL, TO_ROOM );
	return;
}



/*
 * Write to one char.
 */
void writeBuffer ( const char *txt, Creature *ch )
{
	if ( txt != NULL && ch->desc != NULL )
	{ write_to_buffer ( ch->desc, txt, strlen ( txt ) ); }
	return;
}

/*
 * Send a page to one char.
 */
void writePage ( const char *txt, Creature *ch )
{
	if ( txt == NULL || ch->desc == NULL )
	{ return; }

	if ( ch->lines == 0 ) {
		writeBuffer ( txt, ch );
		return;
	}


	ALLOC_DATA ( ch->desc->showstr_head, char, strlen ( txt ) + 1 );
	strcpy ( ch->desc->showstr_head, txt );
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string ( ch->desc, "" );
	return;
}


/* string pager */
void show_string ( struct descriptor_data *d, const char *input )
{
	char buffer[4 * MAX_STRING_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	register char *scan, *chk;
	int lines = 0, toggle = 1;
	int show_lines;

	one_argument ( input, buf );
	if ( buf[0] != '\0' ) {
		if ( d->showstr_head ) {
			PURGE_DATA ( d->showstr_head );
			d->showstr_head = 0;
		}
		d->showstr_point  = 0;
		return;
	}

	if ( d->character )
	{ show_lines = d->character->lines; }
	else
	{ show_lines = 0; }

	for ( scan = buffer; ; scan++, d->showstr_point++ ) {
		if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' )
				&& ( toggle = -toggle ) < 0 )
		{ lines++; }

		else if ( !*scan || ( show_lines > 0 && lines >= show_lines ) ) {
			*scan = '\0';
			write_to_buffer ( d, buffer, strlen ( buffer ) );
			for ( chk = d->showstr_point; isspace ( *chk ); chk++ );
			{
				if ( !*chk ) {
					if ( d->showstr_head ) {
						PURGE_DATA ( d->showstr_head );
						d->showstr_head = 0;
					}
					d->showstr_point  = 0;
				}
			}
			return;
		}
	}
	return;
}


/* quick sex fixer */
void fix_sex ( Creature *ch )
{
	if ( ch->sex < 0 || ch->sex > 2 )
	{ ch->sex = IS_NPC ( ch ) ? 0 : ch->pcdata->true_sex; }
}

void act_new ( const char *format, Creature *ch, const void *arg1,
			   const void *arg2, int type, int min_pos )
{
	static const char *he_she  [] = { "it",  "he",  "she" };
	static const char *him_her [] = { "it",  "him", "her" };
	static const char *his_her [] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	Creature *to;
	Creature *vch = ( Creature * ) arg2;
	Item *obj1 = ( Item  * ) arg1;
	Item *obj2 = ( Item  * ) arg2;
	const char *str;
	const char *i;
	char *point;

	/*
	 * Discard null and zero-length messages.
	 */
	if ( format == NULL || format[0] == '\0' )
	{ return; }

	/* discard null rooms and chars */
	if ( ch == NULL || ch->in_room == NULL )
	{ return; }

	to = ch->in_room->people;
	if ( type == TO_VICT ) {
		if ( vch == NULL ) {
			log_hd ( LOG_ERROR, "Act: null vch with TO_VICT." );
			return;
		}

		if ( vch->in_room == NULL )
		{ return; }

		to = vch->in_room->people;
	}

	for ( ; to != NULL; to = to->next_in_room ) {
		if ( ( !IS_NPC ( to ) && to->desc == NULL )
				||   ( IS_NPC ( to ) && !HAS_TRIGGER ( to, TRIG_ACT ) )
				||    to->position < min_pos )
		{ continue; }

		if ( ( type == TO_CHAR ) && to != ch )
		{ continue; }
		if ( type == TO_VICT && ( to != vch || to == ch ) )
		{ continue; }
		if ( type == TO_ROOM && to == ch )
		{ continue; }
		if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
		{ continue; }

		point   = buf;
		str     = format;
		while ( *str != '\0' ) {
			if ( *str != '$' ) {
				*point++ = *str++;
				continue;
			}
			++str;

			if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' ) {
				log_hd ( LOG_ERROR, Format ( "Act: missing arg2 for code %d.", *str ) );
				i = " <@@@> ";
			} else {
				switch ( *str ) {
					default:
						log_hd ( LOG_ERROR, Format ( "Act: bad code %d.", *str ) );
						i = " <@@@> ";
						break;
					/* Thx alex for 't' idea */
					case 't':
						i = ( char * ) arg1;
						break;
					case 'T':
						i = ( char * ) arg2;
						break;
					case 'n':
						i = PERS ( ch,  to  );
						break;
					case 'N':
						i = PERS ( vch, to  );
						break;
					case 'e':
						i = he_she  [URANGE ( 0, ch  ->sex, 2 )];
						break;
					case 'E':
						i = he_she  [URANGE ( 0, vch ->sex, 2 )];
						break;
					case 'm':
						i = him_her [URANGE ( 0, ch  ->sex, 2 )];
						break;
					case 'M':
						i = him_her [URANGE ( 0, vch ->sex, 2 )];
						break;
					case 's':
						i = his_her [URANGE ( 0, ch  ->sex, 2 )];
						break;
					case 'S':
						i = his_her [URANGE ( 0, vch ->sex, 2 )];
						break;

					case 'p':
						i = can_see_obj ( to, obj1 )
							? obj1->short_descr
							: "something";
						break;

					case 'P':
						i = can_see_obj ( to, obj2 )
							? obj2->short_descr
							: "something";
						break;

					case 'd':
						if ( arg2 == NULL || ( ( char * ) arg2 ) [0] == '\0' ) {
							i = "door";
						} else {
							one_argument ( ( char * ) arg2, fname );
							i = fname;
						}
						break;
				}
			}

			++str;
			while ( ( *point = *i ) != '\0' )
			{ ++point, ++i; }
		}

		*point++ = '\n';
		*point++ = '\r';
		*point   = '\0';
		buf[0]   = UPPER ( buf[0] );
		if ( to->desc != NULL )
		{ write_to_buffer ( to->desc, buf, point - buf ); }
		else if ( MOBtrigger )
		{ mp_act_trigger ( buf, to, ch, arg1, arg2, TRIG_ACT ); }
	}
	return;
}


/* source: EOD, by John Booth <???> */

void printf_to_char ( Creature *ch, const char *fmt, ... )
{
	char buf [MAX_STRING_LENGTH];

	va_list args;
	va_start ( args, fmt );
	vsprintf ( buf, fmt, args );
	va_end ( args );

	writeBuffer ( buf, ch );
}

