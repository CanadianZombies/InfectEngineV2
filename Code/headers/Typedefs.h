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


#ifndef _Typedefs_H
#define _Typedefs_H

#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define Command( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define Command( fun )		CmdData    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;

#ifndef __cpluplus
typedef int				bool;
#endif
#define unix
#else
typedef short   int			sh_int;

#ifndef __cplusplus
typedef unsigned char			bool;
#endif
#endif

/* ea */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

/*
 * Structure types.
 */
typedef struct	affect_data		Affect;
typedef struct	area_data		Zone;
typedef struct	ban_data		Ban;
typedef struct 	buf_type	 	BUFFER;
typedef struct	char_data		Creature;
typedef struct	descriptor_data		Socket;
typedef struct	exit_data		Exit;
typedef struct	extra_descr_data	DescriptionData;
typedef struct	help_data		HELP_DATA;
typedef struct	help_area_data		HELP_AREA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mem_data		MobMemory;
typedef struct	mob_index_data		NPCData;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		Item;
typedef struct	obj_index_data		ItemData;
typedef struct	pc_data			PlayerData;
typedef struct  gen_data		GEN_DATA;
typedef struct	reset_data		Reset;
typedef struct	room_index_data		RoomData;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		EnvironmentDataData;
typedef struct  mprog_list		MPROG_LIST;
typedef struct  mprog_code		MPROG_CODE;


/*
 * Function types.
 */
typedef	bool OLC_FUN		args ( ( Creature *ch, const char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun

typedef	void CmdData	args ( ( Creature *ch, const char *L_command, const char *argument, int cmd ) );
typedef bool SPEC_FUN	args ( ( Creature *ch ) );
typedef void SPELL_FUN	args ( ( int sn, int level, Creature *ch, void *vo,
								 int target ) );
typedef enum { EV_CPP = 0, EV_LUA } event_types;

#ifdef TELCMDS
char *telcmds[] = {
	"SE", "NOP", "DMARK", "BRK", "IP", "AO", "AYT", "EC",
	"EL", "GA", "SB", "WILL", "WONT", "DO", "DONT", "IAC",
};
#endif

#ifdef TELOPTS
#define	NTELOPTS	(1+TELOPT_EOR)
char *telopts[NTELOPTS] = {
	"BINARY", "ECHO", "RCP", "SUPPRESS GO AHEAD", "NAME",
	"STATUS", "TIMING MARK", "RCTE", "NAOL", "NAOP",
	"NAOCRD", "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS",
	"NAOVTD", "NAOLFD", "EXTEND ASCII", "LOGOUT", "BYTE MACRO",
	"DATA ENTRY TERMINAL", "SUPDUP", "SUPDUP OUTPUT",
	"SEND LOCATION", "TERMINAL TYPE", "END OF RECORD",
};
#endif


#endif

// -- EOF

