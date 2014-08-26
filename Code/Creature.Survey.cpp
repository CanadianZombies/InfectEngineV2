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

const char *distance[4] = {
	"right here.", "nearby to the %s.", "not far %s.", "off in the distance %s."
};

void scan_list           args ( ( RoomData *scan_room, Creature *ch,
								  sh_int depth, sh_int door ) );
void scan_char           args ( ( Creature *victim, Creature *ch,
								  sh_int depth, sh_int door ) );

DefineCommand ( cmd_scan )
{
	char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
	RoomData *scan_room;
	Exit *pExit;
	sh_int door, depth;

	argument = one_argument ( argument, arg1 );

	if ( arg1[0] == '\0' ) {
		act ( "$n looks all around.", ch, NULL, NULL, TO_ROOM );
		writeBuffer ( "Looking around you see:\n\r", ch );
		scan_list ( ch->in_room, ch, 0, -1 );

		for ( door = 0; door < 6; door++ ) {
			if ( ( pExit = ch ->in_room->exit[door] ) != NULL )
			{ scan_list ( pExit->u1.to_room, ch, 1, door ); }
		}
		return;
	} else if ( !str_cmp ( arg1, "n" ) || !str_cmp ( arg1, "north" ) ) { door = 0; }
	else if ( !str_cmp ( arg1, "e" ) || !str_cmp ( arg1, "east" ) )  { door = 1; }
	else if ( !str_cmp ( arg1, "s" ) || !str_cmp ( arg1, "south" ) ) { door = 2; }
	else if ( !str_cmp ( arg1, "w" ) || !str_cmp ( arg1, "west" ) )  { door = 3; }
	else if ( !str_cmp ( arg1, "u" ) || !str_cmp ( arg1, "up" ) )   { door = 4; }
	else if ( !str_cmp ( arg1, "d" ) || !str_cmp ( arg1, "down" ) )  { door = 5; }
	else { writeBuffer ( "Which way do you want to scan?\n\r", ch ); return; }

	act ( "You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR );
	act ( "$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM );
	snprintf ( buf, sizeof ( buf ), "Looking %s you see:\n\r", dir_name[door] );

	scan_room = ch->in_room;

	for ( depth = 1; depth < 4; depth++ ) {
		if ( ( pExit = scan_room->exit[door] ) != NULL ) {
			scan_room = pExit->u1.to_room;
			scan_list ( pExit->u1.to_room, ch, depth, door );
		}
	}
	return;
}

void scan_list ( RoomData *scan_room, Creature *ch, sh_int depth,
				 sh_int door )
{
	Creature *rch;

	if ( scan_room == NULL ) { return; }
	for ( rch = scan_room->people; rch != NULL; rch = rch->next_in_room ) {
		if ( rch == ch ) { continue; }
		if ( !IS_NPC ( rch ) && rch->invis_level > get_trust ( ch ) ) { continue; }
		if ( can_see ( ch, rch ) ) { scan_char ( rch, ch, depth, door ); }
	}
	return;
}

void scan_char ( Creature *victim, Creature *ch, sh_int depth, sh_int door )
{
	char buf[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];

	buf[0] = '\0';

	strcat ( buf, PERS ( victim, ch ) );
	strcat ( buf, ", " );
	sprintf ( buf2, distance[depth], dir_name[door] );
	strcat ( buf, buf2 );
	strcat ( buf, "\n\r" );

	writeBuffer ( buf, ch );
	return;
}
