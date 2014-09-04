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

/* random room generation procedure */
RoomData  *get_random_room ( Creature *ch )
{
	RoomData *room;

	for ( ; ; ) {
		room = get_room_index ( Math::instance().range ( 0, 65535 ) );
		if ( room != NULL )
			if ( can_see_room ( ch, room )
					&&   !room_is_private ( room )
					&&   !IS_SET ( room->room_flags, ROOM_PRIVATE )
					&&   !IS_SET ( room->room_flags, ROOM_SOLITARY )
					&&   !IS_SET ( room->room_flags, ROOM_SAFE )
					&&   ( IS_NPC ( ch ) || IS_SET ( ch->act, ACT_AGGRESSIVE )
						   ||   !IS_SET ( room->room_flags, ROOM_LAW ) ) )
			{ break; }
	}

	return room;
}

/* RT Enter portals */
DefineCommand ( cmd_enter )
{
	RoomData *location;

	if ( ch->fighting != NULL )
	{ return; }

	/* nifty portal stuff */
	if ( argument[0] != '\0' ) {
		RoomData *old_room;
		Item *portal;
		Creature *fch, *fch_next;

		old_room = ch->in_room;

		portal = get_obj_list ( ch, argument,  ch->in_room->contents );

		if ( portal == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}

		if ( portal->item_type != ITEM_PORTAL
				||  ( IS_SET ( portal->value[1], EX_CLOSED ) && !IS_TRUSTED ( ch, ANGEL ) ) ) {
			writeBuffer ( "You can't seem to find a way in.\n\r", ch );
			return;
		}

		if ( !IS_TRUSTED ( ch, ANGEL ) && !IS_SET ( portal->value[2], GATE_NOCURSE )
				&&  ( IS_AFFECTED ( ch, AFF_CURSE )
					  ||   IS_SET ( old_room->room_flags, ROOM_NO_RECALL ) ) ) {
			writeBuffer ( "Something prevents you from leaving...\n\r", ch );
			return;
		}

		if ( IS_SET ( portal->value[2], GATE_RANDOM ) || portal->value[3] == -1 ) {
			location = get_random_room ( ch );
			portal->value[3] = location->vnum; /* for record keeping :) */
		} else if ( IS_SET ( portal->value[2], GATE_BUGGY ) && ( Math::instance().percent() < 5 ) )
		{ location = get_random_room ( ch ); }
		else
		{ location = get_room_index ( portal->value[3] ); }

		if ( location == NULL
				||  location == old_room
				||  !can_see_room ( ch, location )
				||  ( room_is_private ( location ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) ) {
			act ( "$p doesn't seem to go anywhere.", ch, portal, NULL, TO_CHAR );
			return;
		}

		if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_AGGRESSIVE )
				&&  IS_SET ( location->room_flags, ROOM_LAW ) ) {
			writeBuffer ( "Something prevents you from leaving...\n\r", ch );
			return;
		}

		act ( "$n steps into $p.", ch, portal, NULL, TO_ROOM );

		if ( IS_SET ( portal->value[2], GATE_NORMAL_EXIT ) )
		{ act ( "You enter $p.", ch, portal, NULL, TO_CHAR ); }
		else
			act ( "You walk through $p and find yourself somewhere else...",
				  ch, portal, NULL, TO_CHAR );

		char_from_room ( ch );
		char_to_room ( ch, location );

		if ( IS_SET ( portal->value[2], GATE_GOWITH ) ) { /* take the gate along */
			obj_from_room ( portal );
			obj_to_room ( portal, location );
		}

		if ( IS_SET ( portal->value[2], GATE_NORMAL_EXIT ) )
		{ act ( "$n has arrived.", ch, portal, NULL, TO_ROOM ); }
		else
		{ act ( "$n has arrived through $p.", ch, portal, NULL, TO_ROOM ); }

		cmd_function ( ch, &cmd_look, "auto" );

		/* charges */
		if ( portal->value[0] > 0 ) {
			portal->value[0]--;
			if ( portal->value[0] == 0 )
			{ portal->value[0] = -1; }
		}

		/* protect against circular follows */
		if ( old_room == location )
		{ return; }

		for ( fch = old_room->people; fch != NULL; fch = fch_next ) {
			fch_next = fch->next_in_room;

			if ( portal == NULL || portal->value[0] == -1 )
				/* no following through dead portals */
			{ continue; }

			if ( fch->master == ch && IS_AFFECTED ( fch, AFF_CHARM )
					&&   fch->position < POS_STANDING )
			{ cmd_function ( fch, &cmd_stand, "" ); }

			if ( fch->master == ch && fch->position == POS_STANDING ) {

				if ( IS_SET ( ch->in_room->room_flags, ROOM_LAW )
						&&  ( IS_NPC ( fch ) && IS_SET ( fch->act, ACT_AGGRESSIVE ) ) ) {
					act ( "You can't bring $N into the city.",
						  ch, NULL, fch, TO_CHAR );
					act ( "You aren't allowed in the city.",
						  fch, NULL, NULL, TO_CHAR );
					continue;
				}

				act ( "You follow $N.", fch, NULL, ch, TO_CHAR );
				cmd_function ( fch, &cmd_enter, argument );
			}
		}

		if ( portal != NULL && portal->value[0] == -1 ) {
			act ( "$p fades out of existence.", ch, portal, NULL, TO_CHAR );
			if ( ch->in_room == old_room )
			{ act ( "$p fades out of existence.", ch, portal, NULL, TO_ROOM ); }
			else if ( old_room->people != NULL ) {
				act ( "$p fades out of existence.",
					  old_room->people, portal, NULL, TO_CHAR );
				act ( "$p fades out of existence.",
					  old_room->people, portal, NULL, TO_ROOM );
			}
			extract_obj ( portal );
		}

		/*
		 * If someone is following the char, these triggers get activated
		 * for the followers before the char, but it's safer this way...
		 */
		if ( IS_NPC ( ch ) && HAS_TRIGGER ( ch, TRIG_ENTRY ) )
		{ mp_percent_trigger ( ch, NULL, NULL, NULL, TRIG_ENTRY ); }
		if ( !IS_NPC ( ch ) )
		{ mp_greet_trigger ( ch ); }

		return;
	}

	writeBuffer ( "Nope, can't do it.\n\r", ch );
	return;
}

// -- EOF


