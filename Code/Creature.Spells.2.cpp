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

extern char *target_name;

void spell_farsight ( int sn, int level, Creature *ch, void *vo, int target )
{
	if ( IS_AFFECTED ( ch, AFF_BLIND ) ) {
		writeBuffer ( "Maybe it would help if you could see?\n\r", ch );
		return;
	}

	cmd_function ( ch, &cmd_scan, target_name );
}


void spell_portal ( int sn, int level, Creature *ch, void *vo, int target )
{
	Creature *victim;
	Item *portal, *stone;

	if ( ( victim = get_char_world ( ch, target_name ) ) == NULL
			||   victim == ch
			||   victim->in_room == NULL
			||   !can_see_room ( ch, victim->in_room )
			||   IS_SET ( victim->in_room->room_flags, ROOM_SAFE )
			||   IS_SET ( victim->in_room->room_flags, ROOM_PRIVATE )
			||   IS_SET ( victim->in_room->room_flags, ROOM_SOLITARY )
			||   IS_SET ( victim->in_room->room_flags, ROOM_NO_RECALL )
			||   IS_SET ( ch->in_room->room_flags, ROOM_NO_RECALL )
			||   victim->level >= level + 3
			||   ( !IS_NPC ( victim ) && victim->level >= LEVEL_HERO ) /* NOT trust */
			||   ( IS_NPC ( victim ) && IS_SET ( victim->imm_flags, IMM_SUMMON ) )
			||   ( IS_NPC ( victim ) && saves_spell ( level, victim, DAM_NONE ) )
			||	( is_clan ( victim ) && !is_same_clan ( ch, victim ) ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	stone = get_eq_char ( ch, WEAR_HOLD );
	if ( !IsStaff ( ch )
			&&  ( stone == NULL || stone->item_type != ITEM_WARP_STONE ) ) {
		writeBuffer ( "You lack the proper component for this spell.\n\r", ch );
		return;
	}

	if ( stone != NULL && stone->item_type == ITEM_WARP_STONE ) {
		act ( "You draw upon the power of $p.", ch, stone, NULL, TO_CHAR );
		act ( "It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR );
		extract_obj ( stone );
	}

	portal = create_object ( get_obj_index ( OBJ_VNUM_PORTAL ), 0 );
	portal->timer = 2 + level / 25;
	portal->value[3] = victim->in_room->vnum;

	obj_to_room ( portal, ch->in_room );

	act ( "$p rises up from the ground.", ch, portal, NULL, TO_ROOM );
	act ( "$p rises up before you.", ch, portal, NULL, TO_CHAR );
}

void spell_nexus ( int sn, int level, Creature *ch, void *vo, int target )
{
	Creature *victim;
	Item *portal, *stone;
	RoomData *to_room, *from_room;

	from_room = ch->in_room;

	if ( ( victim = get_char_world ( ch, target_name ) ) == NULL
			||   victim == ch
			||   ( to_room = victim->in_room ) == NULL
			||   !can_see_room ( ch, to_room ) || !can_see_room ( ch, from_room )
			||   IS_SET ( to_room->room_flags, ROOM_SAFE )
			||	 IS_SET ( from_room->room_flags, ROOM_SAFE )
			||   IS_SET ( to_room->room_flags, ROOM_PRIVATE )
			||   IS_SET ( to_room->room_flags, ROOM_SOLITARY )
			||   IS_SET ( to_room->room_flags, ROOM_NO_RECALL )
			||   IS_SET ( from_room->room_flags, ROOM_NO_RECALL )
			||   victim->level >= level + 3
			||   ( !IS_NPC ( victim ) && victim->level >= LEVEL_HERO ) /* NOT trust */
			||   ( IS_NPC ( victim ) && IS_SET ( victim->imm_flags, IMM_SUMMON ) )
			||   ( IS_NPC ( victim ) && saves_spell ( level, victim, DAM_NONE ) )
			||	 ( is_clan ( victim ) && !is_same_clan ( ch, victim ) ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	stone = get_eq_char ( ch, WEAR_HOLD );
	if ( !IsStaff ( ch )
			&&  ( stone == NULL || stone->item_type != ITEM_WARP_STONE ) ) {
		writeBuffer ( "You lack the proper component for this spell.\n\r", ch );
		return;
	}

	if ( stone != NULL && stone->item_type == ITEM_WARP_STONE ) {
		act ( "You draw upon the power of $p.", ch, stone, NULL, TO_CHAR );
		act ( "It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR );
		extract_obj ( stone );
	}

	/* portal one */
	portal = create_object ( get_obj_index ( OBJ_VNUM_PORTAL ), 0 );
	portal->timer = 1 + level / 10;
	portal->value[3] = to_room->vnum;

	obj_to_room ( portal, from_room );

	act ( "$p rises up from the ground.", ch, portal, NULL, TO_ROOM );
	act ( "$p rises up before you.", ch, portal, NULL, TO_CHAR );

	/* no second portal if rooms are the same */
	if ( to_room == from_room )
	{ return; }

	/* portal two */
	portal = create_object ( get_obj_index ( OBJ_VNUM_PORTAL ), 0 );
	portal->timer = 1 + level / 10;
	portal->value[3] = from_room->vnum;

	obj_to_room ( portal, to_room );

	if ( to_room->people != NULL ) {
		act ( "$p rises up from the ground.", to_room->people, portal, NULL, TO_ROOM );
		act ( "$p rises up from the ground.", to_room->people, portal, NULL, TO_CHAR );
	}
}
