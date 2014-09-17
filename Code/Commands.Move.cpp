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

const char *dir_name	[]		= {
	"north", "east", "south", "west", "up", "down"
};

const	int	rev_dir		[]		= {
	2, 3, 0, 1, 5, 4
};

const	int	movement_loss	[SECT_MAX]	= {
	1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args ( ( Creature *ch, char *arg ) );
bool	has_key		args ( ( Creature *ch, int key ) );
void	raw_kill	args ( ( Creature *victim ) );



void move_char ( Creature *ch, int door, bool follow )
{
	Creature *fch;
	Creature *fch_next;
	RoomData *in_room;
	RoomData *to_room;
	Exit *pexit;

	if ( door < 0 || door > 5 ) {
		log_hd ( LOG_ERROR, Format ( "move_char: bad door %d.", door ) );
		return;
	}

	/*
	 * Exit trigger, if activated, bail out. Only PCs are triggered.
	 */
	if ( !IS_NPC ( ch ) && mp_exit_trigger ( ch, door ) )
	{ return; }

	in_room = ch->in_room;
	if ( ( pexit   = in_room->exit[door] ) == NULL
			||   ( to_room = pexit->u1.to_room   ) == NULL
			||	 !can_see_room ( ch, pexit->u1.to_room ) ) {
		writeBuffer ( "Alas, you cannot go that way.\n\r", ch );
		return;
	}

	if ( IS_SET ( pexit->exit_info, EX_CLOSED )
			&&  ( !IS_AFFECTED ( ch, AFF_PASS_DOOR ) || IS_SET ( pexit->exit_info, EX_NOPASS ) )
			&&   !IS_TRUSTED ( ch, ANGEL ) ) {
		act ( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM )
			&&   ch->master != NULL
			&&   in_room == ch->master->in_room ) {
		writeBuffer ( "What?  And leave your beloved master?\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, to_room ) && room_is_private ( to_room ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( ch ) ) {
		int iClass, iGuild;
		int move;

		for ( iClass = 0; iClass < MAX_CLASS; iClass++ ) {
			for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++ ) {
				if ( iClass != ch->archetype
						&&   to_room->vnum == archetype_table[iClass].guild[iGuild] ) {
					writeBuffer ( "You aren't allowed in there.\n\r", ch );
					return;
				}
			}
		}

		if ( in_room->sector_type == SECT_AIR
				||   to_room->sector_type == SECT_AIR ) {
			if ( !IS_AFFECTED ( ch, AFF_FLYING ) && !IsStaff ( ch ) ) {
				writeBuffer ( "You can't fly.\n\r", ch );
				return;
			}
		}

		if ( ( in_room->sector_type == SECT_WATER_NOSWIM
				||    to_room->sector_type == SECT_WATER_NOSWIM )
				&&    !IS_AFFECTED ( ch, AFF_FLYING ) ) {
			Item *obj;
			bool found;

			/*
			 * Look for a boat.
			 */
			found = FALSE;

			if ( IsStaff ( ch ) )
			{ found = TRUE; }

			for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
				if ( obj->item_type == ITEM_BOAT ) {
					found = TRUE;
					break;
				}
			}
			if ( !found ) {
				writeBuffer ( "You need a boat to go there.\n\r", ch );
				return;
			}
		}

		move = movement_loss[UMIN ( SECT_MAX - 1, in_room->sector_type )]
			   + movement_loss[UMIN ( SECT_MAX - 1, to_room->sector_type )]
			   ;

		move /= 2;  /* i.e. the average */


		/* conditional effects */
		if ( IS_AFFECTED ( ch, AFF_FLYING ) || IS_AFFECTED ( ch, AFF_HASTE ) )
		{ move /= 2; }

		if ( IS_AFFECTED ( ch, AFF_SLOW ) )
		{ move *= 2; }

		if ( ch->move < move ) {
			writeBuffer ( "You are too exhausted.\n\r", ch );
			return;
		}

		WAIT_STATE ( ch, 1 );
		ch->move -= move;
	}

	if ( !IS_AFFECTED ( ch, AFF_SNEAK ) &&   ch->invis_level < LEVEL_HERO )
	{ act ( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM ); }

	char_from_room ( ch );
	char_to_room ( ch, to_room );
	if ( !IS_AFFECTED ( ch, AFF_SNEAK ) && ch->invis_level < LEVEL_HERO )
	{ act ( "$n has arrived.", ch, NULL, NULL, TO_ROOM ); }

	cmd_function ( ch, &cmd_look, "auto" );

	if ( in_room == to_room ) /* no circular follows */
	{ return; }

	for ( fch = in_room->people; fch != NULL; fch = fch_next ) {
		fch_next = fch->next_in_room;

		if ( fch->master == ch && IS_AFFECTED ( fch, AFF_CHARM )
				&&   fch->position < POS_STANDING )
		{ cmd_function ( fch, &cmd_stand, "" ); }

		if ( fch->master == ch && fch->position == POS_STANDING
				&&   can_see_room ( fch, to_room ) ) {

			if ( IS_SET ( ch->in_room->room_flags, ROOM_LAW )
					&&  ( IS_NPC ( fch ) && IS_SET ( fch->act, ACT_AGGRESSIVE ) ) ) {
				act ( "You can't bring $N into the city.",
					  ch, NULL, fch, TO_CHAR );
				act ( "You aren't allowed in the city.",
					  fch, NULL, NULL, TO_CHAR );
				continue;
			}

			act ( "You follow $N.", fch, NULL, ch, TO_CHAR );
			move_char ( fch, door, TRUE );
		}
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



DefineCommand ( cmd_north )
{
	move_char ( ch, DIR_NORTH, FALSE );
	return;
}



DefineCommand ( cmd_east )
{
	move_char ( ch, DIR_EAST, FALSE );
	return;
}



DefineCommand ( cmd_south )
{
	move_char ( ch, DIR_SOUTH, FALSE );
	return;
}

DefineCommand ( cmd_west )
{
	move_char ( ch, DIR_WEST, FALSE );
	return;
}

DefineCommand ( cmd_up )
{
	move_char ( ch, DIR_UP, FALSE );
	return;
}

DefineCommand ( cmd_down )
{
	move_char ( ch, DIR_DOWN, FALSE );
	return;
}

int find_door ( Creature *ch, char *arg )
{
	Exit *pexit;
	int door;

	if ( !str_cmp ( arg, "n" ) || !str_cmp ( arg, "north" ) ) { door = 0; }
	else if ( !str_cmp ( arg, "e" ) || !str_cmp ( arg, "east"  ) ) { door = 1; }
	else if ( !str_cmp ( arg, "s" ) || !str_cmp ( arg, "south" ) ) { door = 2; }
	else if ( !str_cmp ( arg, "w" ) || !str_cmp ( arg, "west"  ) ) { door = 3; }
	else if ( !str_cmp ( arg, "u" ) || !str_cmp ( arg, "up"    ) ) { door = 4; }
	else if ( !str_cmp ( arg, "d" ) || !str_cmp ( arg, "down"  ) ) { door = 5; }
	else {
		for ( door = 0; door <= 5; door++ ) {
			if ( ( pexit = ch->in_room->exit[door] ) != NULL
					&&   IS_SET ( pexit->exit_info, EX_ISDOOR )
					&&   pexit->keyword != NULL
					&&   is_name ( arg, pexit->keyword ) )
			{ return door; }
		}
		act ( "I see no $T here.", ch, NULL, arg, TO_CHAR );
		return -1;
	}

	if ( ( pexit = ch->in_room->exit[door] ) == NULL ) {
		act ( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
		return -1;
	}

	if ( !IS_SET ( pexit->exit_info, EX_ISDOOR ) ) {
		writeBuffer ( "You can't do that.\n\r", ch );
		return -1;
	}

	return door;
}

DefineCommand ( cmd_open )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int door;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Open what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		/* open portal */
		if ( obj->item_type == ITEM_PORTAL ) {
			if ( !IS_SET ( obj->value[1], EX_ISDOOR ) ) {
				writeBuffer ( "You can't do that.\n\r", ch );
				return;
			}

			if ( !IS_SET ( obj->value[1], EX_CLOSED ) ) {
				writeBuffer ( "It's already open.\n\r", ch );
				return;
			}

			if ( IS_SET ( obj->value[1], EX_LOCKED ) ) {
				writeBuffer ( "It's locked.\n\r", ch );
				return;
			}

			REMOVE_BIT ( obj->value[1], EX_CLOSED );
			act ( "You open $p.", ch, obj, NULL, TO_CHAR );
			act ( "$n opens $p.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'open object' */
		if ( obj->item_type != ITEM_CONTAINER ) {
			if ( obj->item_type != ITEM_TREASURECHEST ) {
				writeBuffer ( "That's not a container.\n\r", ch );
				return;
			}
		}
		if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
		{ writeBuffer ( "It's already open.\n\r",      ch ); return; }
		if ( !IS_SET ( obj->value[1], CONT_CLOSEABLE ) )
		{ writeBuffer ( "You can't do that.\n\r",      ch ); return; }
		if ( IS_SET ( obj->value[1], CONT_LOCKED ) )
		{ writeBuffer ( "It's locked.\n\r",            ch ); return; }

		if ( obj->item_type == ITEM_TREASURECHEST ) {
			// -- we are treapped?
			if ( obj->value[3] > 0 )
			{ writeBuffer ( "BOOM! hehehe.\n\r", ch ); }
		}

		REMOVE_BIT ( obj->value[1], CONT_CLOSED );
		act ( "You open $p.", ch, obj, NULL, TO_CHAR );
		act ( "$n opens $p.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( ( door = find_door ( ch, arg ) ) >= 0 ) {
		/* 'open door' */
		RoomData *to_room;
		Exit *pexit;
		Exit *pexit_rev;

		pexit = ch->in_room->exit[door];
		if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
		{ writeBuffer ( "It's already open.\n\r",      ch ); return; }
		if (  IS_SET ( pexit->exit_info, EX_LOCKED ) )
		{ writeBuffer ( "It's locked.\n\r",            ch ); return; }

		REMOVE_BIT ( pexit->exit_info, EX_CLOSED );
		act ( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
		writeBuffer ( "Ok.\n\r", ch );

		/* open the other side */
		if ( ( to_room   = pexit->u1.to_room            ) != NULL
				&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
				&&   pexit_rev->u1.to_room == ch->in_room ) {
			Creature *rch;

			REMOVE_BIT ( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
			{ act ( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR ); }
		}
	}

	return;
}

DefineCommand ( cmd_close )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int door;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Close what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL ) {

			if ( !IS_SET ( obj->value[1], EX_ISDOOR )
					||   IS_SET ( obj->value[1], EX_NOCLOSE ) ) {
				writeBuffer ( "You can't do that.\n\r", ch );
				return;
			}

			if ( IS_SET ( obj->value[1], EX_CLOSED ) ) {
				writeBuffer ( "It's already closed.\n\r", ch );
				return;
			}

			SET_BIT ( obj->value[1], EX_CLOSED );
			act ( "You close $p.", ch, obj, NULL, TO_CHAR );
			act ( "$n closes $p.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'close object' */
		if ( obj->item_type != ITEM_CONTAINER ) {
			if ( obj->item_type != ITEM_TREASURECHEST ) {
				writeBuffer ( "That's not a container.\n\r", ch );
				return;
			}
		}
		if ( IS_SET ( obj->value[1], CONT_CLOSED ) )
		{ writeBuffer ( "It's already closed.\n\r",    ch ); return; }
		if ( !IS_SET ( obj->value[1], CONT_CLOSEABLE ) )
		{ writeBuffer ( "You can't do that.\n\r",      ch ); return; }

		SET_BIT ( obj->value[1], CONT_CLOSED );
		act ( "You close $p.", ch, obj, NULL, TO_CHAR );
		act ( "$n closes $p.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( ( door = find_door ( ch, arg ) ) >= 0 ) {
		/* 'close door' */
		RoomData *to_room;
		Exit *pexit;
		Exit *pexit_rev;

		pexit	= ch->in_room->exit[door];
		if ( IS_SET ( pexit->exit_info, EX_CLOSED ) )
		{ writeBuffer ( "It's already closed.\n\r",    ch ); return; }

		SET_BIT ( pexit->exit_info, EX_CLOSED );
		act ( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
		writeBuffer ( "Ok.\n\r", ch );

		/* close the other side */
		if ( ( to_room   = pexit->u1.to_room            ) != NULL
				&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
				&&   pexit_rev->u1.to_room == ch->in_room ) {
			Creature *rch;

			SET_BIT ( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
			{ act ( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR ); }
		}
	}

	return;
}

bool has_key ( Creature *ch, int key )
{
	Item *obj;

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->pIndexData->vnum == key )
		{ return TRUE; }
	}

	return FALSE;
}

DefineCommand ( cmd_lock )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int door;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Lock what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL ) {
			if ( !IS_SET ( obj->value[1], EX_ISDOOR )
					||  IS_SET ( obj->value[1], EX_NOCLOSE ) ) {
				writeBuffer ( "You can't do that.\n\r", ch );
				return;
			}
			if ( !IS_SET ( obj->value[1], EX_CLOSED ) ) {
				writeBuffer ( "It's not closed.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 || IS_SET ( obj->value[1], EX_NOLOCK ) ) {
				writeBuffer ( "It can't be locked.\n\r", ch );
				return;
			}

			if ( !has_key ( ch, obj->value[4] ) ) {
				writeBuffer ( "You lack the key.\n\r", ch );
				return;
			}

			if ( IS_SET ( obj->value[1], EX_LOCKED ) ) {
				writeBuffer ( "It's already locked.\n\r", ch );
				return;
			}

			SET_BIT ( obj->value[1], EX_LOCKED );
			act ( "You lock $p.", ch, obj, NULL, TO_CHAR );
			act ( "$n locks $p.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'lock object' */
		if ( obj->item_type != ITEM_CONTAINER ) {
			if ( obj->item_type != ITEM_TREASURECHEST ) {
				writeBuffer ( "That's not a container.\n\r", ch );
				return;
			}
		}
		if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( obj->value[2] < 0 )
		{ writeBuffer ( "It can't be locked.\n\r",     ch ); return; }
		if ( !has_key ( ch, obj->value[2] ) )
		{ writeBuffer ( "You lack the key.\n\r",       ch ); return; }
		if ( IS_SET ( obj->value[1], CONT_LOCKED ) )
		{ writeBuffer ( "It's already locked.\n\r",    ch ); return; }

		SET_BIT ( obj->value[1], CONT_LOCKED );
		act ( "You lock $p.", ch, obj, NULL, TO_CHAR );
		act ( "$n locks $p.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( ( door = find_door ( ch, arg ) ) >= 0 ) {
		/* 'lock door' */
		RoomData *to_room;
		Exit *pexit;
		Exit *pexit_rev;

		pexit	= ch->in_room->exit[door];
		if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( pexit->key < 0 )
		{ writeBuffer ( "It can't be locked.\n\r",     ch ); return; }
		if ( !has_key ( ch, pexit->key ) )
		{ writeBuffer ( "You lack the key.\n\r",       ch ); return; }
		if ( IS_SET ( pexit->exit_info, EX_LOCKED ) )
		{ writeBuffer ( "It's already locked.\n\r",    ch ); return; }

		SET_BIT ( pexit->exit_info, EX_LOCKED );
		writeBuffer ( "*Click*\n\r", ch );
		act ( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

		/* lock the other side */
		if ( ( to_room   = pexit->u1.to_room            ) != NULL
				&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
				&&   pexit_rev->u1.to_room == ch->in_room ) {
			SET_BIT ( pexit_rev->exit_info, EX_LOCKED );
		}
	}

	return;
}

DefineCommand ( cmd_unlock )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int door;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Unlock what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL ) {
			if ( !IS_SET ( obj->value[1], EX_ISDOOR ) ) {
				writeBuffer ( "You can't do that.\n\r", ch );
				return;
			}

			if ( !IS_SET ( obj->value[1], EX_CLOSED ) ) {
				writeBuffer ( "It's not closed.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 ) {
				writeBuffer ( "It can't be unlocked.\n\r", ch );
				return;
			}

			if ( !has_key ( ch, obj->value[4] ) ) {
				writeBuffer ( "You lack the key.\n\r", ch );
				return;
			}

			if ( !IS_SET ( obj->value[1], EX_LOCKED ) ) {
				writeBuffer ( "It's already unlocked.\n\r", ch );
				return;
			}

			REMOVE_BIT ( obj->value[1], EX_LOCKED );
			act ( "You unlock $p.", ch, obj, NULL, TO_CHAR );
			act ( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
			return;
		}

		if ( obj->item_type == ITEM_TREASURECHEST ) {
			writeBuffer ( "You unlock the treasure chest.\n\r", ch );
			writeBuffer ( "We now check to see if it's trapped, and if so, if it's disarmed.\n\r", ch );
			writeBuffer ( "If not, make loud scary noises and go boom!\n\r", ch );
			return;
		}

		/* 'unlock object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{ writeBuffer ( "That's not a container.\n\r", ch ); return; }
		if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( obj->value[2] < 0 )
		{ writeBuffer ( "It can't be unlocked.\n\r",   ch ); return; }
		if ( !has_key ( ch, obj->value[2] ) )
		{ writeBuffer ( "You lack the key.\n\r",       ch ); return; }
		if ( !IS_SET ( obj->value[1], CONT_LOCKED ) )
		{ writeBuffer ( "It's already unlocked.\n\r",  ch ); return; }

		REMOVE_BIT ( obj->value[1], CONT_LOCKED );
		act ( "You unlock $p.", ch, obj, NULL, TO_CHAR );
		act ( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( ( door = find_door ( ch, arg ) ) >= 0 ) {
		/* 'unlock door' */
		RoomData *to_room;
		Exit *pexit;
		Exit *pexit_rev;

		pexit = ch->in_room->exit[door];
		if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( pexit->key < 0 )
		{ writeBuffer ( "It can't be unlocked.\n\r",   ch ); return; }
		if ( !has_key ( ch, pexit->key ) )
		{ writeBuffer ( "You lack the key.\n\r",       ch ); return; }
		if ( !IS_SET ( pexit->exit_info, EX_LOCKED ) )
		{ writeBuffer ( "It's already unlocked.\n\r",  ch ); return; }

		REMOVE_BIT ( pexit->exit_info, EX_LOCKED );
		writeBuffer ( "*Click*\n\r", ch );
		act ( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

		/* unlock the other side */
		if ( ( to_room   = pexit->u1.to_room            ) != NULL
				&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
				&&   pexit_rev->u1.to_room == ch->in_room ) {
			REMOVE_BIT ( pexit_rev->exit_info, EX_LOCKED );
		}
	}

	return;
}

void trap_damage ( Creature *ch, Item *obj )
{
	int dam;
	bool die = FALSE;

	dam = obj->value[4];

	if ( obj->value[3] == TRAP_BOOMER ) {
		dam *= 3;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "A sudden explosion erupts, knocking $n back and shattering $p to splinters!", ch, obj, NULL, TO_ROOM );
		act ( "A sudden explosion erupts, knocking you back and shattering $p to splinters!", ch, obj, NULL, TO_CHAR );
	}
	if ( obj->value[3] == TRAP_GASSER ) {
		dam *= 4;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "A cloud of noxious gas pours from a $p $n is holding.", ch, obj, NULL, TO_ROOM );
		act ( "A cloud of noxious gas pours from $p.", ch, obj, NULL, TO_CHAR );
	}
	if ( obj->value[3] == TRAP_NEEDLE ) {
		dam *= 2;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "$n jerks $s hand away from $p with a muttered curse.", ch, obj, NULL, TO_ROOM );
		act ( "A thin needle from $p pricks your hand!", ch, obj, NULL, TO_CHAR );
	}
	if ( obj->value[3] == TRAP_BLADE ) {
		dam *= 3;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "A thin blade slices out from $p $n is holding, slashing $s hand!", ch, obj, NULL, TO_ROOM );
		act ( "A thin blade slices out from $p, slashing your hand!", ch, obj, NULL, TO_CHAR );
	}
	if ( obj->value[3] == TRAP_MANA ) {
		dam *= 4;
		ch->mana -= UMIN ( dam, ch->max_mana );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "$n's eyes roll back into $s skull.", ch, obj, NULL, TO_ROOM );
		act ( "You feel your eyes roll back into your skull.", ch, obj, NULL, TO_ROOM );
	}
	if ( obj->value[3] == TRAP_FIRE ) {
		dam *= 5;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "A spray of fire leaps out to sear $n, burning $p like kindling!", ch, obj, NULL, TO_ROOM );
		act ( "A spray of fire leaps out to sear you, burning $p like kindling!", ch, obj, NULL, TO_CHAR );
	}
	if ( obj->value[3] == TRAP_ACID ) {
		dam *= 5;
		ch->hit -= UMIN ( dam, ch->max_hit );
		if ( ch->hit <= 0 )
		{ die = TRUE; }
		act ( "A spray of acid from $p catches $n in the face!", ch, obj, NULL, TO_ROOM );
		act ( "A spray of acid from $p catches you in the face!", ch, obj, NULL, TO_CHAR );
	}
	if ( die ) {
		writeBuffer ( "You have been KILLED!!\n\r\n\r", ch );
		raw_kill ( ch );
	}

	return;
}

DefineCommand ( cmd_unarm )
{
	Item *obj;
	int rank, base, oppose;
	//bool fail = FALSE;

	/* search obj for traps */
	if ( ( obj = get_obj_carry ( ch, argument, ch ) ) == NULL ) {
		writeBuffer ( "Unarm what? You don't see that here.\n\r", ch );
		return;
	}
	if ( obj->item_type != ITEM_TREASURECHEST ) {
		writeBuffer ( "That's not a treasure chest.\n\r", ch );
		return;
	}

	rank = ch->pcdata->learned[gsn_unarm_traps];
	rank += get_curr_stat ( ch, STAT_DEX );
	rank += get_curr_stat ( ch, STAT_WIS );
	base = Math::instance().range ( 1, 100 );
	oppose = obj->value[4];

	base += oppose;

	if ( Math::instance().range ( 1, rank ) < Math::instance().range ( 1, base ) && obj->value[3] > TRAP_NONE ) {
		act ( "$n fiddles with $p for a moment, concentrating intently.", ch, obj, NULL, TO_ROOM );
		act ( "You attempt to unarm the trap on $p, but manage to spring it instead!", ch, obj, NULL, TO_CHAR );

		if ( obj->value[3] == TRAP_BOOMER ) {
			trap_damage ( ch, obj );

			/* if the box explodes or erupts in flame, remove it */
			check_improve ( ch, gsn_unarm_traps, FALSE, 3 );
			obj_from_char ( obj );
			extract_obj ( obj );
		}
		/* blade needs to sever limbs in the future */
		if ( obj->value[3] == TRAP_BLADE || obj->value[3] == TRAP_MANA ) {
			trap_damage ( ch, obj );
		}
		/* acid - nasty stuff */
		if ( obj->value[3] == TRAP_ACID ) {
			Item *aObj, *aObj_next;

			trap_damage ( ch, obj );
			check_improve ( ch, gsn_unarm_traps, FALSE, 3 );

			for ( aObj = ch->carrying; aObj != NULL; aObj = aObj_next ) {
				aObj_next = aObj->next_content;
				acid_effect ( aObj, obj->value[4], obj->value[4], TARGET_OBJ );
				break;
			}
		}
		/* poison */
		if ( obj->value[3] == TRAP_NEEDLE || obj->value[3] == TRAP_GASSER ) {
			/* get the char */
			if ( obj->value[3] == TRAP_NEEDLE ) {
				Affect af;
				trap_damage ( ch, obj );
				check_improve ( ch, gsn_unarm_traps, FALSE, 3 );

				af.type = gsn_poison;
				af.duration = obj->value[4];
				af.location = APPLY_STR;
				af.modifier = -3;
				af.bitvector = AFF_PLAGUE;
				affect_join ( ch, &af );
			}
			/* get the room */
			if ( obj->value[3] == TRAP_GASSER ) {
				Affect af;
				trap_damage ( ch, obj );
				check_improve ( ch, gsn_unarm_traps, FALSE, 3 );
				/* if save, they held their breath */
				af.type = gsn_poison;
				af.duration = obj->value[4];
				af.location = APPLY_DEX;
				af.modifier = -3;
				af.bitvector = AFF_PLAGUE;
				affect_join ( ch, &af );
			}
		}
		if ( obj->value[3] == TRAP_FIRE ) {
			Item *aObj, *aObj_next;

			trap_damage ( ch, obj );

			/* if the box explodes or erupts in flame, remove it */
			for ( aObj = ch->carrying; aObj != NULL; aObj = aObj_next ) {
				aObj_next = aObj->next_content;
				fire_effect ( aObj, obj->value[4], obj->value[4], TARGET_OBJ );
				break;
			}
			check_improve ( ch, gsn_unarm_traps, FALSE, 3 );
			obj_from_char ( obj );
			extract_obj ( obj );
		}

	} else if ( Math::instance().range ( 1, rank / 2 ) < Math::instance().range ( 1, base ) && obj->value[3] > TRAP_NONE ) {
		act ( "$n fiddles with $p for a moment, concentrating intently.", ch, obj, NULL, TO_ROOM );
		act ( "You attempt to unarm the trap on $p, but can't quite figure out the mechanism.", ch, obj, NULL, TO_CHAR );
		check_improve ( ch, gsn_unarm_traps, FALSE, 6 );
		return;
	} else if ( obj->value[3] == TRAP_NONE ) {
		act ( "$p doesn't appear to be trapped.", ch, obj, NULL, TO_CHAR );
		return;
	}
	/* success */
	else {
		act ( "$n fiddles with $p for a moment, concentrating intently.", ch, obj, NULL, TO_ROOM );
		act ( "You fiddle with the trap on $p until you manage to render the mechanism useless!", ch, obj, NULL, TO_CHAR );
		check_improve ( ch, gsn_unarm_traps, TRUE, 6 );
		obj->value[3] = TRAP_NONE;
		return;
	}
	return;
}

DefineCommand ( cmd_search )
{
	Creature *vch;
	Item *obj;
	int chance, num_found, rank, base, oppose;
	bool found;
	char arg[MIL];

	argument = ChopC ( argument, arg );

	/* init the variables */
	if ( IS_NPC ( ch ) )
	{ chance = 75; }
	else
	{ chance = get_skill ( ch, gsn_search ); }
	num_found = 0;
	found = FALSE;

	/*if ( ch->level < skill_table[gsn_search].skill_level[ch->class] )
	{
	writeBuffer("{cMight as well search for the needle in the haystack.\n\r", ch );
	return;
	}*/
	if ( IS_NPC ( ch ) ) {
		/* npcs dont get checked for skill */

		if ( arg[0] == '\0' ) {
			act ( "$n searches around the area for a moment.", ch, NULL, NULL, TO_ROOM );
			for ( vch = ch->in_room->people; vch != NULL; vch = vch->next ) {
				/* hunt up some hiders */
				if ( vch->in_room != ch->in_room )
				{ continue; }
				if ( vch == ch )
				{ continue; }
				/* do this partly to keep people from finding wizi imms, and
				* to allow mobs to be unsearchable */
				if ( vch->level >= LEVEL_HERO && ch->level < LEVEL_HERO )
				{ continue; }
				/* formulate the chance */
				if ( ch->level >= vch->level )
				{ chance += 1; }
				else
				{ chance -= 2; }
				if ( get_curr_stat ( ch, STAT_INT ) >= get_curr_stat ( vch, STAT_INT ) )
				{ chance += 1; }
				else
				{ chance -= 2; }
				if ( get_curr_stat ( ch, STAT_DEX ) >= get_curr_stat ( vch, STAT_DEX ) )
				{ chance += 1; }
				else
				{ chance -= 2; }
				if ( get_curr_stat ( ch, STAT_WIS ) >= get_curr_stat ( vch, STAT_WIS ) )
				{ chance += 1; }
				else
				{ chance -= 2; }

				/* Always a chance to fail */
				if ( chance > 99 ) { chance = 99; }
				/* ok, now show the found people */
				if ( Math::instance().percent() > chance )
				{ continue; }
				if ( is_affected ( vch, gsn_invis )
						|| is_affected ( vch, gsn_sneak )
						|| IS_AFFECTED ( vch, AFF_SNEAK )
						|| IS_AFFECTED ( vch, AFF_INVISIBLE )
						|| IS_AFFECTED ( vch, AFF_HIDE )
						|| is_affected ( vch, gsn_hide ) ) {
					writeBuffer ( Format ( "You see %s.\n\r", vch->name ), ch );
					writeBuffer ( Format ( "%s has spotted you!\n\r", ch->name ), vch );
					affect_strip ( vch, gsn_invis );
					affect_strip ( vch, gsn_sneak );
					REMOVE_BIT ( vch->affected_by, AFF_HIDE );
					REMOVE_BIT ( vch->affected_by, AFF_INVISIBLE );
					REMOVE_BIT ( vch->affected_by, AFF_SNEAK );
					found = TRUE;
					num_found += 1;
					check_improve ( ch, gsn_search, TRUE, 9 );
				}
			}
			/* AWW uses hidden portals. Remove this loop if not needed. */
			for ( obj = ch->in_room->contents; obj; obj = obj->next_content ) {
				if ( obj->item_type == ITEM_PORTAL ) {
					writeBuffer ( Format ( "You see %s.\n\r", obj->short_descr ), ch );
					found = TRUE;
					num_found += 1;
				}
			}
			if ( !found ) {
				writeBuffer ( "You didn't find anyone here.\n\r", ch );
				if ( IsStaff ( ch ) ) {
					writeBuffer ( Format ( "Chance was %d.\n\r", chance ), ch );
				}
			}
			/* make em wait longer for each person found */
			WAIT_STATE ( ch, PULSE_VIOLENCE * num_found );
			return;
		}

	}
	if ( !IS_NPC ( ch ) ) {
		if ( get_skill ( ch, gsn_search ) <= 0 ) {
			writeBuffer ( "You dont know enough about searching.\n\r", ch );
			return;
		}
	}

	if ( arg[0] == '\0' ) {
		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next ) {
			/* hunt up some hiders */
			if ( vch->in_room != ch->in_room )
			{ continue; }
			if ( vch == ch )
			{ continue; }
			/* formulate the chance */
			if ( ch->level >= vch->level )
			{ chance += 1; }
			else
			{ chance -= 2; }
			if ( get_curr_stat ( ch, STAT_INT ) >= get_curr_stat ( vch, STAT_INT ) )
			{ chance += 1; }
			else
			{ chance -= 2; }
			if ( get_curr_stat ( ch, STAT_DEX ) >= get_curr_stat ( vch, STAT_DEX ) )
			{ chance += 1; }
			else
			{ chance -= 2; }
			if ( get_curr_stat ( ch, STAT_WIS ) >= get_curr_stat ( vch, STAT_WIS ) )
			{ chance += 1; }
			else
			{ chance -= 2; }

			/* Always a chance to fail */
			if ( chance > 99 ) { chance = 99; }
			/* ok, now show the found people */
			if ( Math::instance().percent() > chance )
			{ continue; }
			if ( is_affected ( vch, gsn_invis )
					|| is_affected ( vch, gsn_sneak )
					|| IS_AFFECTED ( vch, AFF_SNEAK )
					|| IS_AFFECTED ( vch, AFF_INVISIBLE )
					|| IS_AFFECTED ( vch, AFF_HIDE )
					|| is_affected ( vch, gsn_hide ) ) {
				writeBuffer ( Format ( "You see %s.\n\r", vch->name ), ch );
				writeBuffer ( Format ( "%s has spotted you!\n\r", ch->name ), vch );
				affect_strip ( vch, gsn_invis );
				affect_strip ( vch, gsn_sneak );
				REMOVE_BIT ( vch->affected_by, AFF_HIDE );
				REMOVE_BIT ( vch->affected_by, AFF_INVISIBLE );
				REMOVE_BIT ( vch->affected_by, AFF_SNEAK );
				found = TRUE;
				num_found += 1;
				check_improve ( ch, gsn_search, TRUE, 9 );
			}
		}
		/* AWW uses hidden portals. Remove this loop if not needed. */
		for ( obj = ch->in_room->contents; obj; obj = obj->next_content ) {
			if ( obj->item_type == ITEM_PORTAL ) {
				writeBuffer ( Format ( "You see %s.\n\r", obj->short_descr ), ch );
				found = TRUE;
				num_found += 1;
			}
		}
		if ( !found ) {
			writeBuffer ( "You didn't find anyone here.\n\r", ch );
			if ( IsStaff ( ch ) ) {
				writeBuffer ( Format ( "Chance is %d.\n\r", chance ), ch );
			}
		}
		/* make em wait longer for each person found */
		WAIT_STATE ( ch, PULSE_VIOLENCE * num_found );
		return;
	}
	/* search obj for traps */
	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		writeBuffer ( "Search what? You don't see that here.\n\r", ch );
		return;
	}
	if ( obj->item_type != ITEM_TREASURECHEST ) {
		writeBuffer ( "That's not a treasure chest.\n\r", ch );
		return;
	}

	rank = ch->pcdata->learned[gsn_unarm_traps];
	rank += get_curr_stat ( ch, STAT_INT );
	rank += get_curr_stat ( ch, STAT_DEX );
	base = Math::instance().range ( 1, 100 );
	oppose = obj->value[4];

	base += oppose;

	if ( Math::instance().range ( 1, rank ) < Math::instance().range ( 1, base ) ) {
		writeBuffer ( "It doesn't appear to be trapped.\n\r", ch );
		return;
	} else if ( obj->value[3] == TRAP_NONE ) {
		writeBuffer ( "It doesn't appear to be trapped.\n\r", ch );
		return;
	} else {
		writeBuffer ( Format ( "You see a %s.\n\r", obj->value[3] == TRAP_BOOMER ? "small black cube inside the lock" :
							   obj->value[3] == TRAP_GASSER ? "small green cannister hidden within the lock" :
							   obj->value[3] == TRAP_NEEDLE ? "small needle dripping with a green liquid inside the lock" :
							   obj->value[3] == TRAP_BLADE ? "razor sharp blade hidden within the lock" :
							   obj->value[3] == TRAP_MANA ? "small shimmering sphere tucked inside the lock" :
							   obj->value[3] == TRAP_FIRE ? "reddish-gold casement placed behind the lock" :
							   obj->value[3] == TRAP_ACID ? "bluish-green vial rigged to burst inside the lock" :
							   "odd shape within the lock" ), ch );
	}
	return;
}

DefineCommand ( cmd_pick )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *gch;
	Item *obj;
	int door;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Pick what?\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, skill_table[gsn_pick_lock].beats );

	/* look for guards */
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
		if ( IS_NPC ( gch ) && IS_AWAKE ( gch ) && ch->level + 5 < gch->level ) {
			act ( "$N is standing too close to the lock.",
				  ch, NULL, gch, TO_CHAR );
			return;
		}
	}

	if ( !IS_NPC ( ch ) && Math::instance().percent( ) > get_skill ( ch, gsn_pick_lock ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		check_improve ( ch, gsn_pick_lock, FALSE, 2 );
		return;
	}

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL ) {
			if ( !IS_SET ( obj->value[1], EX_ISDOOR ) ) {
				writeBuffer ( "You can't do that.\n\r", ch );
				return;
			}

			if ( !IS_SET ( obj->value[1], EX_CLOSED ) ) {
				writeBuffer ( "It's not closed.\n\r", ch );
				return;
			}

			if ( obj->value[4] < 0 ) {
				writeBuffer ( "It can't be unlocked.\n\r", ch );
				return;
			}

			if ( IS_SET ( obj->value[1], EX_PICKPROOF ) ) {
				writeBuffer ( "You failed.\n\r", ch );
				return;
			}

			REMOVE_BIT ( obj->value[1], EX_LOCKED );
			act ( "You pick the lock on $p.", ch, obj, NULL, TO_CHAR );
			act ( "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM );
			check_improve ( ch, gsn_pick_lock, TRUE, 2 );
			return;
		}


		/* 'pick object' */
		if ( obj->item_type != ITEM_CONTAINER ) {
			if ( obj->item_type != ITEM_TREASURECHEST ) {
				writeBuffer ( "That's not a container.\n\r", ch );
				return;
			}
		}
		if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( obj->value[2] < 0 )
		{ writeBuffer ( "It can't be unlocked.\n\r",   ch ); return; }
		if ( !IS_SET ( obj->value[1], CONT_LOCKED ) )
		{ writeBuffer ( "It's already unlocked.\n\r",  ch ); return; }
		if ( IS_SET ( obj->value[1], CONT_PICKPROOF ) )
		{ writeBuffer ( "You failed.\n\r",             ch ); return; }

		if ( obj->item_type == ITEM_TREASURECHEST ) {
			int lock_diff = obj->value[2];
			int chance = get_skill ( ch, gsn_pick_lock );
			int result = Math::instance().range ( chance * 8 / 12, chance ) + dex_app[get_curr_stat ( ch, STAT_DEX )].defensive;
			result += 5; // -- pick->condition / 10;
			if ( result < lock_diff ) {
				writeBuffer ( Format ( "You attempt to pick the lock on %s, but fail.\n\r", obj->short_descr ), ch );
				/*ptc(ch,"Result was %d chance, and you needed to go over %d lock_diff.\n\r", result, lock_diff);*/
				obj->value[2] -= dex_app[get_curr_stat ( ch, STAT_DEX )].defensive / 10;
				if ( Math::instance().percent() > Math::instance().percent() + dex_app[get_curr_stat ( ch, STAT_DEX )].defensive ) {
					act ( "You wince as you feel your lockpick bend inside the lock.", ch, NULL, NULL, TO_CHAR );
					act ( "You hear a wince from $n as $e tests $s skill at lockpicking.", ch, NULL, NULL, TO_ROOM );
					/*					pick->condition -= Math::instance().range ( 1, 3 );
										if ( pick->condition < 1 ) {
											act ( "With a *snap* your lockpick breaks off inside the lock.", ch, NULL, NULL, TO_CHAR );
											act ( "You hear a *snap* and a muttered curse from $n as $s lockpick breaks in two.", ch, NULL, NULL, TO_ROOM );
											extract_obj ( pick );
										} */
				}
				/* traps! these only kick in if not unarmed first */
				if ( obj->value[3] == TRAP_BOOMER ) {
					trap_damage ( ch, obj );

					/* if the box explodes or erupts in flame, remove it */
					if ( obj->value[3] == TRAP_BOOMER || obj->value[3] == TRAP_FIRE ) {
						obj_from_char ( obj );
						extract_obj ( obj );
						return;
					}
				}
				/* blade needs to sever limbs in the future */
				if ( obj->value[3] == TRAP_BLADE || obj->value[3] == TRAP_MANA ) {
					trap_damage ( ch, obj );
				}
				/* acid - nasty stuff */
				if ( obj->value[3] == TRAP_ACID ) {
					Item *aObj, *aObj_next;

					trap_damage ( ch, obj );

					for ( aObj = ch->carrying; aObj != NULL; aObj = aObj_next ) {
						aObj_next = aObj->next_content;
						acid_effect ( aObj, obj->value[4], obj->value[4], TARGET_OBJ );
						break;
					}
				}
				/* poison */
				if ( obj->value[3] == TRAP_NEEDLE || obj->value[3] == TRAP_GASSER ) {
					/* get the char */
					if ( obj->value[3] == TRAP_NEEDLE ) {
						Affect af;
						trap_damage ( ch, obj );

						af.type = gsn_poison;
						af.duration = obj->value[4];
						af.location = APPLY_STR;
						af.modifier = -3;
						af.bitvector = AFF_PLAGUE;
						affect_join ( ch, &af );
					}
					/* get the room */
					if ( obj->value[3] == TRAP_GASSER ) {
						Affect af;
						trap_damage ( ch, obj );
						/* if save, they held their breath */
						af.type = gsn_poison;
						af.duration = obj->value[4];
						af.location = APPLY_DEX;
						af.modifier = -3;
						af.bitvector = AFF_PLAGUE;
						affect_join ( ch, &af );
					}
				}
				if ( obj->value[3] == TRAP_FIRE ) {
					Item *aObj, *aObj_next;

					trap_damage ( ch, obj );

					/* if the box explodes or erupts in flame, remove it */
					for ( aObj = ch->carrying; aObj != NULL; aObj = aObj_next ) {
						aObj_next = aObj->next_content;
						fire_effect ( aObj, obj->value[4], obj->value[4], TARGET_OBJ );
						break;
					}
					obj_from_char ( obj );
					extract_obj ( obj );
					return;
				}
				/* end traps */

				check_improve ( ch, gsn_pick_lock, FALSE, 9 );
				return;
			}
		}

		REMOVE_BIT ( obj->value[1], CONT_LOCKED );
		act ( "You pick the lock on $p.", ch, obj, NULL, TO_CHAR );
		act ( "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM );
		check_improve ( ch, gsn_pick_lock, TRUE, 2 );
		return;
	}

	if ( ( door = find_door ( ch, arg ) ) >= 0 ) {
		/* 'pick door' */
		RoomData *to_room;
		Exit *pexit;
		Exit *pexit_rev;

		pexit = ch->in_room->exit[door];
		if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) && !IsStaff ( ch ) )
		{ writeBuffer ( "It's not closed.\n\r",        ch ); return; }
		if ( pexit->key < 0 && !IsStaff ( ch ) )
		{ writeBuffer ( "It can't be picked.\n\r",     ch ); return; }
		if ( !IS_SET ( pexit->exit_info, EX_LOCKED ) )
		{ writeBuffer ( "It's already unlocked.\n\r",  ch ); return; }
		if ( IS_SET ( pexit->exit_info, EX_PICKPROOF ) && !IsStaff ( ch ) )
		{ writeBuffer ( "You failed.\n\r",             ch ); return; }

		REMOVE_BIT ( pexit->exit_info, EX_LOCKED );
		writeBuffer ( "*Click*\n\r", ch );
		act ( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
		check_improve ( ch, gsn_pick_lock, TRUE, 2 );

		/* pick the other side */
		if ( ( to_room   = pexit->u1.to_room            ) != NULL
				&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
				&&   pexit_rev->u1.to_room == ch->in_room ) {
			REMOVE_BIT ( pexit_rev->exit_info, EX_LOCKED );
		}
	}

	return;
}

DefineCommand ( cmd_stand )
{
	Item *obj = NULL;

	if ( argument[0] != '\0' ) {
		if ( ch->position == POS_FIGHTING ) {
			writeBuffer ( "Maybe you should finish fighting first?\n\r", ch );
			return;
		}
		obj = get_obj_list ( ch, argument, ch->in_room->contents );
		if ( obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
		if ( obj->item_type != ITEM_FURNITURE
				||  ( !IS_SET ( obj->value[2], STAND_AT )
					  &&   !IS_SET ( obj->value[2], STAND_ON )
					  &&   !IS_SET ( obj->value[2], STAND_IN ) ) ) {
			writeBuffer ( "You can't seem to find a place to stand.\n\r", ch );
			return;
		}
		if ( ch->on != obj && count_users ( obj ) >= obj->value[0] ) {
			act_new ( "There's no room to stand on $p.",
					  ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}
		ch->on = obj;
	}

	switch ( ch->position ) {
		case POS_SLEEPING:
			if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
			{ writeBuffer ( "You can't wake up!\n\r", ch ); return; }

			if ( obj == NULL ) {
				writeBuffer ( "You wake and stand up.\n\r", ch );
				act ( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
				ch->on = NULL;
			} else if ( IS_SET ( obj->value[2], STAND_AT ) ) {
				act_new ( "You wake and stand at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], STAND_ON ) ) {
				act_new ( "You wake and stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act_new ( "You wake and stand in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_STANDING;
			cmd_function ( ch, &cmd_look, "auto" );
			break;

		case POS_RESTING:
		case POS_SITTING:
			if ( obj == NULL ) {
				writeBuffer ( "You stand up.\n\r", ch );
				act ( "$n stands up.", ch, NULL, NULL, TO_ROOM );
				ch->on = NULL;
			} else if ( IS_SET ( obj->value[2], STAND_AT ) ) {
				act ( "You stand at $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n stands at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], STAND_ON ) ) {
				act ( "You stand on $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n stands on $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act ( "You stand in $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n stands on $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_STANDING;
			break;

		case POS_STANDING:
			writeBuffer ( "You are already standing.\n\r", ch );
			break;

		case POS_FIGHTING:
			writeBuffer ( "You are already fighting!\n\r", ch );
			break;
	}

	return;
}

DefineCommand ( cmd_rest )
{
	Item *obj = NULL;

	if ( ch->position == POS_FIGHTING ) {
		writeBuffer ( "You are already fighting!\n\r", ch );
		return;
	}

	/* okay, now that we know we can rest, find an object to rest on */
	if ( argument[0] != '\0' ) {
		obj = get_obj_list ( ch, argument, ch->in_room->contents );
		if ( obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else { obj = ch->on; }

	if ( obj != NULL ) {
		if ( obj->item_type != ITEM_FURNITURE
				||  ( !IS_SET ( obj->value[2], REST_ON )
					  &&   !IS_SET ( obj->value[2], REST_IN )
					  &&   !IS_SET ( obj->value[2], REST_AT ) ) ) {
			writeBuffer ( "You can't rest on that.\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users ( obj ) >= obj->value[0] ) {
			act_new ( "There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		ch->on = obj;
	}

	switch ( ch->position ) {
		case POS_SLEEPING:
			if ( IS_AFFECTED ( ch, AFF_SLEEP ) ) {
				writeBuffer ( "You can't wake up!\n\r", ch );
				return;
			}

			if ( obj == NULL ) {
				writeBuffer ( "You wake up and start resting.\n\r", ch );
				act ( "$n wakes up and starts resting.", ch, NULL, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_AT ) ) {
				act_new ( "You wake up and rest at $p.",
						  ch, obj, NULL, TO_CHAR, POS_SLEEPING );
				act ( "$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_ON ) ) {
				act_new ( "You wake up and rest on $p.",
						  ch, obj, NULL, TO_CHAR, POS_SLEEPING );
				act ( "$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act_new ( "You wake up and rest in $p.",
						  ch, obj, NULL, TO_CHAR, POS_SLEEPING );
				act ( "$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_RESTING;
			break;

		case POS_RESTING:
			writeBuffer ( "You are already resting.\n\r", ch );
			break;

		case POS_STANDING:
			if ( obj == NULL ) {
				writeBuffer ( "You rest.\n\r", ch );
				act ( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_AT ) ) {
				act ( "You sit down at $p and rest.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_ON ) ) {
				act ( "You sit on $p and rest.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits on $p and rests.", ch, obj, NULL, TO_ROOM );
			} else {
				act ( "You rest in $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n rests in $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_RESTING;
			break;

		case POS_SITTING:
			if ( obj == NULL ) {
				writeBuffer ( "You rest.\n\r", ch );
				act ( "$n rests.", ch, NULL, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_AT ) ) {
				act ( "You rest at $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n rests at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], REST_ON ) ) {
				act ( "You rest on $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n rests on $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act ( "You rest in $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n rests in $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_RESTING;
			break;
	}


	return;
}

DefineCommand ( cmd_sit )
{
	Item *obj = NULL;

	if ( ch->position == POS_FIGHTING ) {
		writeBuffer ( "Maybe you should finish this fight first?\n\r", ch );
		return;
	}

	/* okay, now that we know we can sit, find an object to sit on */
	if ( argument[0] != '\0' ) {
		obj = get_obj_list ( ch, argument, ch->in_room->contents );
		if ( obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else { obj = ch->on; }

	if ( obj != NULL ) {
		if ( obj->item_type != ITEM_FURNITURE
				||  ( !IS_SET ( obj->value[2], SIT_ON )
					  &&   !IS_SET ( obj->value[2], SIT_IN )
					  &&   !IS_SET ( obj->value[2], SIT_AT ) ) ) {
			writeBuffer ( "You can't sit on that.\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users ( obj ) >= obj->value[0] ) {
			act_new ( "There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		ch->on = obj;
	}
	switch ( ch->position ) {
		case POS_SLEEPING:
			if ( IS_AFFECTED ( ch, AFF_SLEEP ) ) {
				writeBuffer ( "You can't wake up!\n\r", ch );
				return;
			}

			if ( obj == NULL ) {
				writeBuffer ( "You wake and sit up.\n\r", ch );
				act ( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], SIT_AT ) ) {
				act_new ( "You wake and sit at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], SIT_ON ) ) {
				act_new ( "You wake and sit on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act_new ( "You wake and sit in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD );
				act ( "$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM );
			}

			ch->position = POS_SITTING;
			break;
		case POS_RESTING:
			if ( obj == NULL )
			{ writeBuffer ( "You stop resting.\n\r", ch ); }
			else if ( IS_SET ( obj->value[2], SIT_AT ) ) {
				act ( "You sit at $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits at $p.", ch, obj, NULL, TO_ROOM );
			}

			else if ( IS_SET ( obj->value[2], SIT_ON ) ) {
				act ( "You sit on $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits on $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_SITTING;
			break;
		case POS_SITTING:
			writeBuffer ( "You are already sitting down.\n\r", ch );
			break;
		case POS_STANDING:
			if ( obj == NULL ) {
				writeBuffer ( "You sit down.\n\r", ch );
				act ( "$n sits down on the ground.", ch, NULL, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], SIT_AT ) ) {
				act ( "You sit down at $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits down at $p.", ch, obj, NULL, TO_ROOM );
			} else if ( IS_SET ( obj->value[2], SIT_ON ) ) {
				act ( "You sit on $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits on $p.", ch, obj, NULL, TO_ROOM );
			} else {
				act ( "You sit down in $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n sits down in $p.", ch, obj, NULL, TO_ROOM );
			}
			ch->position = POS_SITTING;
			break;
	}
	return;
}

DefineCommand ( cmd_sleep )
{
	Item *obj = NULL;

	switch ( ch->position ) {
		case POS_SLEEPING:
			writeBuffer ( "You are already sleeping.\n\r", ch );
			break;

		case POS_RESTING:
		case POS_SITTING:
		case POS_STANDING:
			if ( argument[0] == '\0' && ch->on == NULL ) {
				writeBuffer ( "You go to sleep.\n\r", ch );
				act ( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
				ch->position = POS_SLEEPING;
			} else { /* find an object and sleep on it */
				if ( argument[0] == '\0' )
				{ obj = ch->on; }
				else
				{ obj = get_obj_list ( ch, argument,  ch->in_room->contents ); }

				if ( obj == NULL ) {
					writeBuffer ( "You don't see that here.\n\r", ch );
					return;
				}
				if ( obj->item_type != ITEM_FURNITURE
						||  ( !IS_SET ( obj->value[2], SLEEP_ON )
							  &&   !IS_SET ( obj->value[2], SLEEP_IN )
							  &&	 !IS_SET ( obj->value[2], SLEEP_AT ) ) ) {
					writeBuffer ( "You can't sleep on that!\n\r", ch );
					return;
				}

				if ( ch->on != obj && count_users ( obj ) >= obj->value[0] ) {
					act_new ( "There is no room on $p for you.",
							  ch, obj, NULL, TO_CHAR, POS_DEAD );
					return;
				}

				ch->on = obj;
				if ( IS_SET ( obj->value[2], SLEEP_AT ) ) {
					act ( "You go to sleep at $p.", ch, obj, NULL, TO_CHAR );
					act ( "$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM );
				} else if ( IS_SET ( obj->value[2], SLEEP_ON ) ) {
					act ( "You go to sleep on $p.", ch, obj, NULL, TO_CHAR );
					act ( "$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM );
				} else {
					act ( "You go to sleep in $p.", ch, obj, NULL, TO_CHAR );
					act ( "$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM );
				}
				ch->position = POS_SLEEPING;
			}
			break;

		case POS_FIGHTING:
			writeBuffer ( "You are already fighting!\n\r", ch );
			break;
	}

	return;
}

DefineCommand ( cmd_wake )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' )
	{ cmd_function ( ch, &cmd_stand, "" ); return; }

	if ( !IS_AWAKE ( ch ) )
	{ writeBuffer ( "You are asleep yourself!\n\r",       ch ); return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ writeBuffer ( "They aren't here.\n\r",              ch ); return; }

	if ( IS_AWAKE ( victim ) )
	{ act ( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

	if ( IS_AFFECTED ( victim, AFF_SLEEP ) )
	{ act ( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

	act_new ( "$n wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
	cmd_function ( ch, &cmd_stand, "" );
	return;
}

DefineCommand ( cmd_sneak )
{
	Affect af;

	writeBuffer ( "You attempt to move silently.\n\r", ch );
	affect_strip ( ch, gsn_sneak );

	if ( IS_AFFECTED ( ch, AFF_SNEAK ) )
	{ return; }

	if ( Math::instance().percent( ) < get_skill ( ch, gsn_sneak ) ) {
		check_improve ( ch, gsn_sneak, TRUE, 3 );
		af.where     = TO_AFFECTS;
		af.type      = gsn_sneak;
		af.level     = ch->level;
		af.duration  = ch->level;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_SNEAK;
		affect_to_char ( ch, &af );
	} else
	{ check_improve ( ch, gsn_sneak, FALSE, 3 ); }

	return;
}

DefineCommand ( cmd_hide )
{
	writeBuffer ( "You attempt to hide.\n\r", ch );

	if ( IS_AFFECTED ( ch, AFF_HIDE ) )
	{ REMOVE_BIT ( ch->affected_by, AFF_HIDE ); }

	if ( Math::instance().percent( ) < get_skill ( ch, gsn_hide ) ) {
		SET_BIT ( ch->affected_by, AFF_HIDE );
		check_improve ( ch, gsn_hide, TRUE, 3 );
	} else
	{ check_improve ( ch, gsn_hide, FALSE, 3 ); }

	return;
}

/*
 * Contributed by Alander.
 */
DefineCommand ( cmd_visible )
{
	affect_strip ( ch, gsn_invis			);
	affect_strip ( ch, gsn_mass_invis			);
	affect_strip ( ch, gsn_sneak			);
	REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
	REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
	REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_recall )
{
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	RoomData *location;

	if ( IS_NPC ( ch ) && !IS_SET ( ch->act, ACT_PET ) ) {
		writeBuffer ( "Only players can recall.\n\r", ch );
		return;
	}

	act ( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

	if ( ( location = get_room_index ( ROOM_VNUM_TEMPLE ) ) == NULL ) {
		writeBuffer ( "You are completely lost.\n\r", ch );
		return;
	}

	if ( ch->in_room == location )
	{ return; }

	if ( IS_SET ( ch->in_room->room_flags, ROOM_NO_RECALL )
			||   IS_AFFECTED ( ch, AFF_CURSE ) ) {
		writeBuffer ( "Mota has forsaken you.\n\r", ch );
		return;
	}

	if ( ( victim = ch->fighting ) != NULL ) {
		int lose, skill;

		skill = get_skill ( ch, gsn_recall );

		if ( Math::instance().percent() < 80 * skill / 100 ) {
			check_improve ( ch, gsn_recall, FALSE, 6 );
			WAIT_STATE ( ch, 4 );
			sprintf ( buf, "You failed!.\n\r" );
			writeBuffer ( buf, ch );
			return;
		}

		lose = ( ch->desc != NULL ) ? 25 : 50;
		gain_exp ( ch, 0 - lose );
		check_improve ( ch, gsn_recall, TRUE, 4 );
		sprintf ( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
		writeBuffer ( buf, ch );
		stop_fighting ( ch, TRUE );

	}

	ch->move /= 2;
	act ( "$n disappears.", ch, NULL, NULL, TO_ROOM );
	char_from_room ( ch );
	char_to_room ( ch, location );
	act ( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
	cmd_function ( ch, &cmd_look, "auto" );

	if ( ch->pet != NULL )
	{ cmd_function ( ch->pet, &cmd_recall, "" ); }

	return;
}

DefineCommand ( cmd_train )
{
	char buf[MAX_STRING_LENGTH];
	Creature *mob;
	int stat = - 1;
	const char *pOutput = NULL;
	int cost;

	if ( IS_NPC ( ch ) )
	{ return; }

	/*
	 * Check for trainer.
	 */
	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room ) {
		if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_TRAIN ) )
		{ break; }
	}

	if ( mob == NULL ) {
		writeBuffer ( "You can't do that here.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		sprintf ( buf, "You have %d training sessions.\n\r", ch->train );
		writeBuffer ( buf, ch );
		argument = "foo";
	}

	cost = 1;

	if ( !str_cmp ( argument, "str" ) ) {
		if ( archetype_table[ch->archetype].attr_prime == STAT_STR )
		{ cost    = 1; }
		stat        = STAT_STR;
		pOutput     = "strength";
	}

	else if ( !str_cmp ( argument, "int" ) ) {
		if ( archetype_table[ch->archetype].attr_prime == STAT_INT )
		{ cost    = 1; }
		stat	    = STAT_INT;
		pOutput     = "intelligence";
	}

	else if ( !str_cmp ( argument, "wis" ) ) {
		if ( archetype_table[ch->archetype].attr_prime == STAT_WIS )
		{ cost    = 1; }
		stat	    = STAT_WIS;
		pOutput     = "wisdom";
	}

	else if ( !str_cmp ( argument, "dex" ) ) {
		if ( archetype_table[ch->archetype].attr_prime == STAT_DEX )
		{ cost    = 1; }
		stat  	    = STAT_DEX;
		pOutput     = "dexterity";
	}

	else if ( !str_cmp ( argument, "con" ) ) {
		if ( archetype_table[ch->archetype].attr_prime == STAT_CON )
		{ cost    = 1; }
		stat	    = STAT_CON;
		pOutput     = "constitution";
	}

	else if ( !str_cmp ( argument, "hp" ) )
	{ cost = 1; }

	else if ( !str_cmp ( argument, "mana" ) )
	{ cost = 1; }

	else {
		strcpy ( buf, "You can train:" );
		if ( ch->perm_stat[STAT_STR] < get_max_train ( ch, STAT_STR ) )
		{ strcat ( buf, " str" ); }
		if ( ch->perm_stat[STAT_INT] < get_max_train ( ch, STAT_INT ) )
		{ strcat ( buf, " int" ); }
		if ( ch->perm_stat[STAT_WIS] < get_max_train ( ch, STAT_WIS ) )
		{ strcat ( buf, " wis" ); }
		if ( ch->perm_stat[STAT_DEX] < get_max_train ( ch, STAT_DEX ) )
		{ strcat ( buf, " dex" ); }
		if ( ch->perm_stat[STAT_CON] < get_max_train ( ch, STAT_CON ) )
		{ strcat ( buf, " con" ); }
		strcat ( buf, " hp mana" );

		if ( buf[strlen ( buf ) - 1] != ':' ) {
			strcat ( buf, ".\n\r" );
			writeBuffer ( buf, ch );
		} else {
			/*
			 * This message dedicated to Jordan ... you big stud!
			 */
			act ( "You have nothing left to train, you $T!",
				  ch, NULL,
				  ch->sex == SEX_MALE   ? "big stud" :
				  ch->sex == SEX_FEMALE ? "hot babe" :
				  "wild thing",
				  TO_CHAR );
		}

		return;
	}

	if ( !str_cmp ( "hp", argument ) ) {
		if ( cost > ch->train ) {
			writeBuffer ( "You don't have enough training sessions.\n\r", ch );
			return;
		}

		ch->train -= cost;
		ch->pcdata->perm_hit += 10;
		ch->max_hit += 10;
		ch->hit += 10;
		act ( "Your durability increases!", ch, NULL, NULL, TO_CHAR );
		act ( "$n's durability increases!", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( !str_cmp ( "mana", argument ) ) {
		if ( cost > ch->train ) {
			writeBuffer ( "You don't have enough training sessions.\n\r", ch );
			return;
		}

		ch->train -= cost;
		ch->pcdata->perm_mana += 10;
		ch->max_mana += 10;
		ch->mana += 10;
		act ( "Your power increases!", ch, NULL, NULL, TO_CHAR );
		act ( "$n's power increases!", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( ch->perm_stat[stat]  >= get_max_train ( ch, stat ) ) {
		act ( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
		return;
	}

	if ( cost > ch->train ) {
		writeBuffer ( "You don't have enough training sessions.\n\r", ch );
		return;
	}

	ch->train		-= cost;

	ch->perm_stat[stat]		+= 1;
	act ( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
	act ( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	return;
}

// -- EOF


