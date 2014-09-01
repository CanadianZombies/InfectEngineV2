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
 * Local functions.
 */
#define CD Creature
#define OD Item
bool	remove_obj	args ( ( Creature *ch, int iWear, bool fReplace ) );
void	wear_obj	args ( ( Creature *ch, Item *obj, bool fReplace ) );
CD *	find_keeper	args ( ( Creature *ch ) );
int	get_cost	args ( ( Creature *keeper, Item *obj, bool fBuy ) );
void 	obj_to_keeper	args ( ( Item *obj, Creature *ch ) );
OD *	get_obj_keeper	args ( ( Creature *ch, Creature *keeper, char *argument ) );

#undef OD
#undef	CD

/* RT part of the corpse looting code */

bool can_loot ( Creature *ch, Item *obj )
{
	Creature *owner, *wch;

	if ( IsStaff ( ch ) )
	{ return TRUE; }

	if ( !obj->owner || obj->owner == NULL )
	{ return TRUE; }

	owner = NULL;
	for ( wch = char_list; wch != NULL ; wch = wch->next )
		if ( !str_cmp ( wch->name, obj->owner ) )
		{ owner = wch; }

	if ( owner == NULL )
	{ return TRUE; }

	if ( !str_cmp ( ch->name, owner->name ) )
	{ return TRUE; }

	if ( !IS_NPC ( owner ) && IS_SET ( owner->act, PLR_CANLOOT ) )
	{ return TRUE; }

	if ( is_same_group ( ch, owner ) )
	{ return TRUE; }

	return FALSE;
}


void get_obj ( Creature *ch, Item *obj, Item *container )
{
	/* variables for AUTOSPLIT */
	Creature *gch;
	int members;
	char buffer[100];

	if ( !CAN_WEAR ( obj, ITEM_TAKE ) ) {
		writeBuffer ( "You can't take that.\n\r", ch );
		return;
	}

	if ( ch->carry_number + get_obj_number ( obj ) > can_carry_n ( ch ) ) {
		act ( "$d: you can't carry that many items.",
			  ch, NULL, obj->name, TO_CHAR );
		return;
	}

	if ( ( !obj->in_obj || obj->in_obj->carried_by != ch )
			&&  ( get_carry_weight ( ch ) + get_obj_weight ( obj ) > can_carry_w ( ch ) ) ) {
		act ( "$d: you can't carry that much weight.",
			  ch, NULL, obj->name, TO_CHAR );
		return;
	}

	if ( !can_loot ( ch, obj ) ) {
		act ( "Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR );
		return;
	}

	if ( obj->in_room != NULL ) {
		for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room ) {
			if ( gch->on == obj ) {
				act ( "$N appears to be using $p.",
					  ch, obj, gch, TO_CHAR );
				return;
			}
		}
	}


	if ( container != NULL ) {
		if ( container->pIndexData->vnum == OBJ_VNUM_PIT
				&&  get_trust ( ch ) < obj->level ) {
			writeBuffer ( "You are not powerful enough to use it.\n\r", ch );
			return;
		}

		if ( container->pIndexData->vnum == OBJ_VNUM_PIT
				&&  !CAN_WEAR ( container, ITEM_TAKE )
				&&  !IS_OBJ_STAT ( obj, ITEM_HAD_TIMER ) )
		{ obj->timer = 0; }
		act ( "You get $p from $P.", ch, obj, container, TO_CHAR );
		act ( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
		REMOVE_BIT ( obj->extra_flags, ITEM_HAD_TIMER );
		obj_from_obj ( obj );
	} else {
		act ( "You get $p.", ch, obj, container, TO_CHAR );
		act ( "$n gets $p.", ch, obj, container, TO_ROOM );
		obj_from_room ( obj );
	}

	if ( obj->item_type == ITEM_MONEY ) {
		ch->silver += obj->value[0];
		ch->gold += obj->value[1];
		if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) ) {
			/* AUTOSPLIT code */
			members = 0;
			for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) {
				if ( !IS_AFFECTED ( gch, AFF_CHARM ) && is_same_group ( gch, ch ) )
				{ members++; }
			}

			if ( members > 1 && ( obj->value[0] > 1 || obj->value[1] ) ) {
				sprintf ( buffer, "%d %d", obj->value[0], obj->value[1] );
				cmd_function ( ch, &cmd_split, buffer );
			}
		}

		extract_obj ( obj );
	} else {
		obj_to_char ( obj, ch );
	}

	return;
}



DefineCommand ( cmd_get )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Item *obj;
	Item *obj_next;
	Item *container;
	bool found;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );

	if ( !str_cmp ( arg2, "from" ) )
	{ argument = one_argument ( argument, arg2 ); }

	/* Get type. */
	if ( arg1[0] == '\0' ) {
		writeBuffer ( "Get what?\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' ) {
		if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) ) {
			/* 'get obj' */
			obj = get_obj_list ( ch, arg1, ch->in_room->contents );
			if ( obj == NULL ) {
				act ( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
				return;
			}

			get_obj ( ch, obj, NULL );
		} else {
			/* 'get all' or 'get all.obj' */
			found = FALSE;
			for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next ) {
				obj_next = obj->next_content;
				if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) )
						&&   can_see_obj ( ch, obj ) ) {
					found = TRUE;
					get_obj ( ch, obj, NULL );
				}
			}

			if ( !found ) {
				if ( arg1[3] == '\0' )
				{ writeBuffer ( "I see nothing here.\n\r", ch ); }
				else
				{ act ( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR ); }
			}
		}
	} else {
		/* 'get ... container' */
		if ( !str_cmp ( arg2, "all" ) || !str_prefix ( "all.", arg2 ) ) {
			writeBuffer ( "You can't do that.\n\r", ch );
			return;
		}

		if ( ( container = get_obj_here ( ch, arg2 ) ) == NULL ) {
			act ( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
			return;
		}

		switch ( container->item_type ) {
			default:
				writeBuffer ( "That's not a container.\n\r", ch );
				return;

			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
				break;

			case ITEM_CORPSE_PC: {

					if ( !can_loot ( ch, container ) ) {
						writeBuffer ( "You can't do that.\n\r", ch );
						return;
					}
				}
		}

		if ( IS_SET ( container->value[1], CONT_CLOSED ) ) {
			act ( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
			return;
		}

		if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) ) {
			/* 'get obj container' */
			obj = get_obj_list ( ch, arg1, container->contains );
			if ( obj == NULL ) {
				act ( "I see nothing like that in the $T.",
					  ch, NULL, arg2, TO_CHAR );
				return;
			}
			get_obj ( ch, obj, container );
		} else {
			/* 'get all container' or 'get all.obj container' */
			found = FALSE;
			for ( obj = container->contains; obj != NULL; obj = obj_next ) {
				obj_next = obj->next_content;
				if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) )
						&&   can_see_obj ( ch, obj ) ) {
					found = TRUE;
					if ( container->pIndexData->vnum == OBJ_VNUM_PIT
							&&  !IsStaff ( ch ) ) {
						writeBuffer ( "Don't be so greedy!\n\r", ch );
						return;
					}
					get_obj ( ch, obj, container );
				}
			}

			if ( !found ) {
				if ( arg1[3] == '\0' )
					act ( "I see nothing in the $T.",
						  ch, NULL, arg2, TO_CHAR );
				else
					act ( "I see nothing like that in the $T.",
						  ch, NULL, arg2, TO_CHAR );
			}
		}
	}

	return;
}

DefineCommand ( cmd_put )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Item *container;
	Item *obj;
	Item *obj_next;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );

	if ( !str_cmp ( arg2, "in" ) || !str_cmp ( arg2, "on" ) )
	{ argument = one_argument ( argument, arg2 ); }

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Put what in what?\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg2, "all" ) || !str_prefix ( "all.", arg2 ) ) {
		writeBuffer ( "You can't do that.\n\r", ch );
		return;
	}

	if ( ( container = get_obj_here ( ch, arg2 ) ) == NULL ) {
		act ( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
		return;
	}

	if ( container->item_type != ITEM_CONTAINER ) {
		writeBuffer ( "That's not a container.\n\r", ch );
		return;
	}

	if ( IS_SET ( container->value[1], CONT_CLOSED ) ) {
		act ( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
		return;
	}

	if ( str_cmp ( arg1, "all" ) && str_prefix ( "all.", arg1 ) ) {
		/* 'put obj container' */
		if ( ( obj = get_obj_carry ( ch, arg1, ch ) ) == NULL ) {
			writeBuffer ( "You do not have that item.\n\r", ch );
			return;
		}

		if ( obj == container ) {
			writeBuffer ( "You can't fold it into itself.\n\r", ch );
			return;
		}

		if ( !can_drop_obj ( ch, obj ) ) {
			writeBuffer ( "You can't let go of it.\n\r", ch );
			return;
		}

		if ( WEIGHT_MULT ( obj ) != 100 ) {
			writeBuffer ( "You have a feeling that would be a bad idea.\n\r", ch );
			return;
		}

		if ( get_obj_weight ( obj ) + get_true_weight ( container )
				> ( container->value[0] * 10 )
				||  get_obj_weight ( obj ) > ( container->value[3] * 10 ) ) {
			writeBuffer ( "It won't fit.\n\r", ch );
			return;
		}

		if ( ( container->pIndexData->vnum == OBJ_VNUM_PIT
				&&  !CAN_WEAR ( container, ITEM_TAKE ) ) ) {
			if ( obj->timer )
			{ SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER ); }
			else
			{ obj->timer = number_range ( 100, 200 ); }
		}
		obj_from_char ( obj );
		obj_to_obj ( obj, container );

		if ( IS_SET ( container->value[1], CONT_PUT_ON ) ) {
			act ( "$n puts $p on $P.", ch, obj, container, TO_ROOM );
			act ( "You put $p on $P.", ch, obj, container, TO_CHAR );
		} else {
			act ( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
			act ( "You put $p in $P.", ch, obj, container, TO_CHAR );
		}
	} else {
		/* 'put all container' or 'put all.obj container' */
		for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;

			if ( ( arg1[3] == '\0' || is_name ( &arg1[4], obj->name ) )
					&&   can_see_obj ( ch, obj )
					&&   WEIGHT_MULT ( obj ) == 100
					&&   obj->wear_loc == WEAR_NONE
					&&   obj != container
					&&   can_drop_obj ( ch, obj )
					&&   get_obj_weight ( obj ) + get_true_weight ( container )
					<= ( container->value[0] * 10 )
					&&   get_obj_weight ( obj ) < ( container->value[3] * 10 ) ) {
				if ( container->pIndexData->vnum == OBJ_VNUM_PIT
						&&  !CAN_WEAR ( obj, ITEM_TAKE ) ) {
					if ( obj->timer )
					{ SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER ); }
					else
					{ obj->timer = number_range ( 100, 200 ); }
				}
				obj_from_char ( obj );
				obj_to_obj ( obj, container );

				if ( IS_SET ( container->value[1], CONT_PUT_ON ) ) {
					act ( "$n puts $p on $P.", ch, obj, container, TO_ROOM );
					act ( "You put $p on $P.", ch, obj, container, TO_CHAR );
				} else {
					act ( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
					act ( "You put $p in $P.", ch, obj, container, TO_CHAR );
				}
			}
		}
	}

	return;
}

DefineCommand ( cmd_drop )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	Item *obj_next;
	bool found;

	argument = one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Drop what?\n\r", ch );
		return;
	}

	if ( is_number ( arg ) ) {
		/* 'drop NNNN coins' */
		int amount, gold = 0, silver = 0;

		amount   = atoi ( arg );
		argument = one_argument ( argument, arg );
		if ( amount <= 0
				|| ( str_cmp ( arg, "coins" ) && str_cmp ( arg, "coin" ) &&
					 str_cmp ( arg, "gold"  ) && str_cmp ( arg, "silver" ) ) ) {
			writeBuffer ( "Sorry, you can't do that.\n\r", ch );
			return;
		}

		if ( !str_cmp ( arg, "coins" ) || !str_cmp ( arg, "coin" )
				||   !str_cmp ( arg, "silver" ) ) {
			if ( ch->silver < amount ) {
				writeBuffer ( "You don't have that much silver.\n\r", ch );
				return;
			}

			ch->silver -= amount;
			silver = amount;
		}

		else {
			if ( ch->gold < amount ) {
				writeBuffer ( "You don't have that much gold.\n\r", ch );
				return;
			}

			ch->gold -= amount;
			gold = amount;
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;

			switch ( obj->pIndexData->vnum ) {
				case OBJ_VNUM_SILVER_ONE:
					silver += 1;
					extract_obj ( obj );
					break;

				case OBJ_VNUM_GOLD_ONE:
					gold += 1;
					extract_obj ( obj );
					break;

				case OBJ_VNUM_SILVER_SOME:
					silver += obj->value[0];
					extract_obj ( obj );
					break;

				case OBJ_VNUM_GOLD_SOME:
					gold += obj->value[1];
					extract_obj ( obj );
					break;

				case OBJ_VNUM_COINS:
					silver += obj->value[0];
					gold += obj->value[1];
					extract_obj ( obj );
					break;
			}
		}

		obj_to_room ( create_money ( gold, silver ), ch->in_room );
		act ( "$n drops some coins.", ch, NULL, NULL, TO_ROOM );
		writeBuffer ( "OK.\n\r", ch );
		return;
	}

	if ( str_cmp ( arg, "all" ) && str_prefix ( "all.", arg ) ) {
		/* 'drop obj' */
		if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
			writeBuffer ( "You do not have that item.\n\r", ch );
			return;
		}

		if ( !can_drop_obj ( ch, obj ) ) {
			writeBuffer ( "You can't let go of it.\n\r", ch );
			return;
		}

		obj_from_char ( obj );
		obj_to_room ( obj, ch->in_room );
		act ( "$n drops $p.", ch, obj, NULL, TO_ROOM );
		act ( "You drop $p.", ch, obj, NULL, TO_CHAR );
		if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) ) {
			act ( "$p dissolves into smoke.", ch, obj, NULL, TO_ROOM );
			act ( "$p dissolves into smoke.", ch, obj, NULL, TO_CHAR );
			extract_obj ( obj );
		}
	} else {
		/* 'drop all' or 'drop all.obj' */
		found = FALSE;
		for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;

			if ( ( arg[3] == '\0' || is_name ( &arg[4], obj->name ) )
					&&   can_see_obj ( ch, obj )
					&&   obj->wear_loc == WEAR_NONE
					&&   can_drop_obj ( ch, obj ) ) {
				found = TRUE;
				obj_from_char ( obj );
				obj_to_room ( obj, ch->in_room );
				act ( "$n drops $p.", ch, obj, NULL, TO_ROOM );
				act ( "You drop $p.", ch, obj, NULL, TO_CHAR );
				if ( IS_OBJ_STAT ( obj, ITEM_MELT_DROP ) ) {
					act ( "$p dissolves into smoke.", ch, obj, NULL, TO_ROOM );
					act ( "$p dissolves into smoke.", ch, obj, NULL, TO_CHAR );
					extract_obj ( obj );
				}
			}
		}

		if ( !found ) {
			if ( arg[3] == '\0' )
				act ( "You are not carrying anything.",
					  ch, NULL, arg, TO_CHAR );
			else
				act ( "You are not carrying any $T.",
					  ch, NULL, &arg[4], TO_CHAR );
		}
	}

	return;
}

DefineCommand ( cmd_give )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	Item  *obj;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Give what to whom?\n\r", ch );
		return;
	}

	if ( is_number ( arg1 ) ) {
		/* 'give NNNN coins victim' */
		int amount;
		bool silver;

		amount   = atoi ( arg1 );
		if ( amount <= 0
				|| ( str_cmp ( arg2, "coins" ) && str_cmp ( arg2, "coin" ) &&
					 str_cmp ( arg2, "gold"  ) && str_cmp ( arg2, "silver" ) ) ) {
			writeBuffer ( "Sorry, you can't do that.\n\r", ch );
			return;
		}

		silver = str_cmp ( arg2, "gold" );

		argument = one_argument ( argument, arg2 );
		if ( arg2[0] == '\0' ) {
			writeBuffer ( "Give what to whom?\n\r", ch );
			return;
		}

		if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		if ( ( !silver && ch->gold < amount ) || ( silver && ch->silver < amount ) ) {
			writeBuffer ( "You haven't got that much.\n\r", ch );
			return;
		}

		if ( silver ) {
			ch->silver		-= amount;
			victim->silver 	+= amount;
		} else {
			ch->gold		-= amount;
			victim->gold	+= amount;
		}

		snprintf ( buf, sizeof ( buf ), "$n gives you %d %s.", amount, silver ? "silver" : "gold" );
		act ( buf, ch, NULL, victim, TO_VICT    );
		act ( "$n gives $N some coins.",  ch, NULL, victim, TO_NOTVICT );
		snprintf ( buf, sizeof ( buf ), "You give $N %d %s.", amount, silver ? "silver" : "gold" );
		act ( buf, ch, NULL, victim, TO_CHAR    );

		/*
		 * Bribe trigger
		 */
		if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_BRIBE ) )
		{ mp_bribe_trigger ( victim, ch, silver ? amount : amount * 100 ); }

		if ( IS_NPC ( victim ) && IS_SET ( victim->act, ACT_IS_CHANGER ) ) {
			int change;

			change = ( silver ? 95 * amount / 100 / 100
					   : 95 * amount );


			if ( !silver && change > victim->silver )
			{ victim->silver += change; }

			if ( silver && change > victim->gold )
			{ victim->gold += change; }

			if ( change < 1 && can_see ( victim, ch ) ) {
				act (
					"$n tells you 'I'm sorry, you did not give me enough to change.'"
					, victim, NULL, ch, TO_VICT );
				ch->reply = victim;
				snprintf ( buf, sizeof ( buf ), "%d %s %s",
						   amount, silver ? "silver" : "gold", ch->name );
				cmd_function ( victim, &cmd_give, buf );
			} else if ( can_see ( victim, ch ) ) {
				snprintf ( buf, sizeof ( buf ), "%d %s %s",
						   change, silver ? "gold" : "silver", ch->name );
				cmd_function ( victim, &cmd_give, buf );
				if ( silver ) {
					snprintf ( buf, sizeof ( buf ), "%d silver %s",
							   ( 95 * amount / 100 - change * 100 ), ch->name );
					cmd_function ( victim, &cmd_give, buf );
				}
				act ( "$n tells you 'Thank you, come again.'",
					  victim, NULL, ch, TO_VICT );
				ch->reply = victim;
			}
		}
		return;
	}

	if ( ( obj = get_obj_carry ( ch, arg1, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE ) {
		writeBuffer ( "You must remove it first.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) && victim->pIndexData->pShop != NULL ) {
		act ( "$N tells you 'Sorry, you'll have to sell that.'",
			  ch, NULL, victim, TO_CHAR );
		ch->reply = victim;
		return;
	}

	if ( !can_drop_obj ( ch, obj ) ) {
		writeBuffer ( "You can't let go of it.\n\r", ch );
		return;
	}

	if ( victim->carry_number + get_obj_number ( obj ) > can_carry_n ( victim ) ) {
		act ( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( get_carry_weight ( victim ) + get_obj_weight ( obj ) > can_carry_w ( victim ) ) {
		act ( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !can_see_obj ( victim, obj ) ) {
		act ( "$N can't see it.", ch, NULL, victim, TO_CHAR );
		return;
	}

	obj_from_char ( obj );
	obj_to_char ( obj, victim );
	MOBtrigger = FALSE;
	act ( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
	act ( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
	act ( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
	MOBtrigger = TRUE;

	/*
	 * Give trigger
	 */
	if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_GIVE ) )
	{ mp_give_trigger ( victim, ch, obj ); }

	return;
}


/* for poisoning weapons and food/drink */
DefineCommand ( cmd_envenom )
{
	Item *obj;
	Affect af;
	int percent, skill;

	/* find out what */
	if ( argument[0] == '\0' ) {
		writeBuffer ( "Envenom what item?\n\r", ch );
		return;
	}

	obj =  get_obj_list ( ch, argument, ch->carrying );

	if ( obj == NULL ) {
		writeBuffer ( "You don't have that item.\n\r", ch );
		return;
	}

	if ( ( skill = get_skill ( ch, gsn_envenom ) ) < 1 ) {
		writeBuffer ( "Are you crazy? You'd poison yourself!\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON ) {
		if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) || IS_OBJ_STAT ( obj, ITEM_BURN_PROOF ) ) {
			act ( "You fail to poison $p.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_percent() < skill ) { /* success! */
			act ( "$n treats $p with deadly poison.", ch, obj, NULL, TO_ROOM );
			act ( "You treat $p with deadly poison.", ch, obj, NULL, TO_CHAR );
			if ( !obj->value[3] ) {
				obj->value[3] = 1;
				check_improve ( ch, gsn_envenom, TRUE, 4 );
			}
			WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
			return;
		}

		act ( "You fail to poison $p.", ch, obj, NULL, TO_CHAR );
		if ( !obj->value[3] )
		{ check_improve ( ch, gsn_envenom, FALSE, 4 ); }
		WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
		return;
	}

	if ( obj->item_type == ITEM_WEAPON ) {
		if ( IS_WEAPON_STAT ( obj, WEAPON_FLAMING )
				||  IS_WEAPON_STAT ( obj, WEAPON_FROST )
				||  IS_WEAPON_STAT ( obj, WEAPON_VAMPIRIC )
				||  IS_WEAPON_STAT ( obj, WEAPON_SHARP )
				||  IS_WEAPON_STAT ( obj, WEAPON_VORPAL )
				||  IS_WEAPON_STAT ( obj, WEAPON_SHOCKING )
				||  IS_OBJ_STAT ( obj, ITEM_BLESS ) || IS_OBJ_STAT ( obj, ITEM_BURN_PROOF ) ) {
			act ( "You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( obj->value[3] < 0
				||  attack_table[obj->value[3]].damage == DAM_BASH ) {
			writeBuffer ( "You can only envenom edged weapons.\n\r", ch );
			return;
		}

		if ( IS_WEAPON_STAT ( obj, WEAPON_POISON ) ) {
			act ( "$p is already envenomed.", ch, obj, NULL, TO_CHAR );
			return;
		}

		percent = number_percent();
		if ( percent < skill ) {

			af.where     = TO_WEAPON;
			af.type      = gsn_poison;
			af.level     = ch->level * percent / 100;
			af.duration  = ch->level / 2 * percent / 100;
			af.location  = 0;
			af.modifier  = 0;
			af.bitvector = WEAPON_POISON;
			affect_to_obj ( obj, &af );

			act ( "$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM );
			act ( "You coat $p with venom.", ch, obj, NULL, TO_CHAR );
			check_improve ( ch, gsn_envenom, TRUE, 3 );
			WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
			return;
		} else {
			act ( "You fail to envenom $p.", ch, obj, NULL, TO_CHAR );
			check_improve ( ch, gsn_envenom, FALSE, 3 );
			WAIT_STATE ( ch, skill_table[gsn_envenom].beats );
			return;
		}
	}

	act ( "You can't poison $p.", ch, obj, NULL, TO_CHAR );
	return;
}

DefineCommand ( cmd_fill )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Item *obj;
	Item *fountain;
	bool found;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Fill what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	found = FALSE;
	for ( fountain = ch->in_room->contents; fountain != NULL;
			fountain = fountain->next_content ) {
		if ( fountain->item_type == ITEM_FOUNTAIN ) {
			found = TRUE;
			break;
		}
	}

	if ( !found ) {
		writeBuffer ( "There is no fountain here!\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_DRINK_CON ) {
		writeBuffer ( "You can't fill that.\n\r", ch );
		return;
	}

	if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] ) {
		writeBuffer ( "There is already another liquid in it.\n\r", ch );
		return;
	}

	if ( obj->value[1] >= obj->value[0] ) {
		writeBuffer ( "Your container is full.\n\r", ch );
		return;
	}

	snprintf ( buf, sizeof ( buf ), "You fill $p with %s from $P.",
			   liq_table[fountain->value[2]].liq_name );
	act ( buf, ch, obj, fountain, TO_CHAR );
	snprintf ( buf, sizeof ( buf ), "$n fills $p with %s from $P.",
			   liq_table[fountain->value[2]].liq_name );
	act ( buf, ch, obj, fountain, TO_ROOM );
	obj->value[2] = fountain->value[2];
	obj->value[1] = obj->value[0];
	return;
}

DefineCommand ( cmd_pour )
{
	char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
	Item *out, *in;
	Creature *vch = NULL;
	int amount;

	argument = one_argument ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "Pour what into what?\n\r", ch );
		return;
	}


	if ( ( out = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		writeBuffer ( "You don't have that item.\n\r", ch );
		return;
	}

	if ( out->item_type != ITEM_DRINK_CON ) {
		writeBuffer ( "That's not a drink container.\n\r", ch );
		return;
	}

	if ( !str_cmp ( argument, "out" ) ) {
		if ( out->value[1] == 0 ) {
			writeBuffer ( "It's already empty.\n\r", ch );
			return;
		}

		out->value[1] = 0;
		out->value[3] = 0;
		snprintf ( buf, sizeof ( buf ), "You invert $p, spilling %s all over the ground.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, out, NULL, TO_CHAR );

		snprintf ( buf, sizeof ( buf ), "$n inverts $p, spilling %s all over the ground.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, out, NULL, TO_ROOM );
		return;
	}

	if ( ( in = get_obj_here ( ch, argument ) ) == NULL ) {
		vch = get_char_room ( ch, argument );

		if ( vch == NULL ) {
			writeBuffer ( "Pour into what?\n\r", ch );
			return;
		}

		in = get_eq_char ( vch, WEAR_HOLD );

		if ( in == NULL ) {
			writeBuffer ( "They aren't holding anything.", ch );
			return;
		}
	}

	if ( in->item_type != ITEM_DRINK_CON ) {
		writeBuffer ( "You can only pour into other drink containers.\n\r", ch );
		return;
	}

	if ( in == out ) {
		writeBuffer ( "You cannot change the laws of physics!\n\r", ch );
		return;
	}

	if ( in->value[1] != 0 && in->value[2] != out->value[2] ) {
		writeBuffer ( "They don't hold the same liquid.\n\r", ch );
		return;
	}

	if ( out->value[1] == 0 ) {
		act ( "There's nothing in $p to pour.", ch, out, NULL, TO_CHAR );
		return;
	}

	if ( in->value[1] >= in->value[0] ) {
		act ( "$p is already filled to the top.", ch, in, NULL, TO_CHAR );
		return;
	}

	amount = UMIN ( out->value[1], in->value[0] - in->value[1] );

	in->value[1] += amount;
	out->value[1] -= amount;
	in->value[2] = out->value[2];

	if ( vch == NULL ) {
		snprintf ( buf, sizeof ( buf ), "You pour %s from $p into $P.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, out, in, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "$n pours %s from $p into $P.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, out, in, TO_ROOM );
	} else {
		snprintf ( buf, sizeof ( buf ), "You pour some %s for $N.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, NULL, vch, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "$n pours you some %s.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, NULL, vch, TO_VICT );
		snprintf ( buf, sizeof ( buf ), "$n pours some %s for $N.",
				   liq_table[out->value[2]].liq_name );
		act ( buf, ch, NULL, vch, TO_NOTVICT );

	}
}

DefineCommand ( cmd_drink )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int amount;
	int liquid;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		for ( obj = ch->in_room->contents; obj; obj = obj->next_content ) {
			if ( obj->item_type == ITEM_FOUNTAIN )
			{ break; }
		}

		if ( obj == NULL ) {
			writeBuffer ( "Drink what?\n\r", ch );
			return;
		}
	} else {
		if ( ( obj = get_obj_here ( ch, arg ) ) == NULL ) {
			writeBuffer ( "You can't find it.\n\r", ch );
			return;
		}
	}

	if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 ) {
		writeBuffer ( "You fail to reach your mouth.  *Hic*\n\r", ch );
		return;
	}

	switch ( obj->item_type ) {
		default:
			writeBuffer ( "You can't drink from that.\n\r", ch );
			return;

		case ITEM_FOUNTAIN:
			if ( ( liquid = obj->value[2] )  < 0 ) {
				log_hd ( LOG_ERROR, Format ( "cmd_drink: bad liquid number %d.", liquid ) );
				liquid = obj->value[2] = 0;
			}
			amount = liq_table[liquid].liq_affect[4] * 3;
			break;

		case ITEM_DRINK_CON:
			if ( obj->value[1] <= 0 ) {
				writeBuffer ( "It is already empty.\n\r", ch );
				return;
			}

			if ( ( liquid = obj->value[2] )  < 0 ) {
				log_hd ( LOG_ERROR, Format ( "cmd_drink: bad liquid number %d.", liquid ) );
				liquid = obj->value[2] = 0;
			}

			amount = liq_table[liquid].liq_affect[4];
			amount = UMIN ( amount, obj->value[1] );
			break;
	}
	if ( !IS_NPC ( ch ) && !IsStaff ( ch )
			&&  ch->pcdata->condition[COND_FULL] > 45 ) {
		writeBuffer ( "You're too full to drink more.\n\r", ch );
		return;
	}

	act ( "$n drinks $T from $p.",
		  ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act ( "You drink $T from $p.",
		  ch, obj, liq_table[liquid].liq_name, TO_CHAR );

	gain_condition ( ch, COND_DRUNK,
					 amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
	gain_condition ( ch, COND_FULL,
					 amount * liq_table[liquid].liq_affect[COND_FULL] / 4 );
	gain_condition ( ch, COND_THIRST,
					 amount * liq_table[liquid].liq_affect[COND_THIRST] / 10 );
	gain_condition ( ch, COND_HUNGER,
					 amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );

	if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK]  > 10 )
	{ writeBuffer ( "You feel drunk.\n\r", ch ); }
	if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_FULL]   > 40 )
	{ writeBuffer ( "You are full.\n\r", ch ); }
	if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_THIRST] > 40 )
	{ writeBuffer ( "Your thirst is quenched.\n\r", ch ); }

	if ( obj->value[3] != 0 ) {
		/* The drink was poisoned ! */
		Affect af;

		act ( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
		writeBuffer ( "You choke and gag.\n\r", ch );
		af.where     = TO_AFFECTS;
		af.type      = gsn_poison;
		af.level	 = number_fuzzy ( amount );
		af.duration  = 3 * amount;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_POISON;
		affect_join ( ch, &af );
	}

	if ( obj->value[0] > 0 )
	{ obj->value[1] -= amount; }

	return;
}

DefineCommand ( cmd_eat )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;

	one_argument ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Eat what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	if ( !IsStaff ( ch ) ) {
		if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL ) {
			writeBuffer ( "That's not edible.\n\r", ch );
			return;
		}

		if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_FULL] > 40 ) {
			writeBuffer ( "You are too full to eat more.\n\r", ch );
			return;
		}
	}

	act ( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
	act ( "You eat $p.", ch, obj, NULL, TO_CHAR );

	switch ( obj->item_type ) {

		case ITEM_FOOD:
			if ( !IS_NPC ( ch ) ) {
				int condition;

				condition = ch->pcdata->condition[COND_HUNGER];
				gain_condition ( ch, COND_FULL, obj->value[0] );
				gain_condition ( ch, COND_HUNGER, obj->value[1] );
				if ( condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
				{ writeBuffer ( "You are no longer hungry.\n\r", ch ); }
				else if ( ch->pcdata->condition[COND_FULL] > 40 )
				{ writeBuffer ( "You are full.\n\r", ch ); }
			}

			if ( obj->value[3] != 0 ) {
				/* The food was poisoned! */
				Affect af;

				act ( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
				writeBuffer ( "You choke and gag.\n\r", ch );

				af.where	 = TO_AFFECTS;
				af.type      = gsn_poison;
				af.level 	 = number_fuzzy ( obj->value[0] );
				af.duration  = 2 * obj->value[0];
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = AFF_POISON;
				affect_join ( ch, &af );
			}
			break;

		case ITEM_PILL:
			obj_cast_spell ( obj->value[1], obj->value[0], ch, ch, NULL );
			obj_cast_spell ( obj->value[2], obj->value[0], ch, ch, NULL );
			obj_cast_spell ( obj->value[3], obj->value[0], ch, ch, NULL );
			break;
	}

	extract_obj ( obj );
	return;
}

/*
 * Remove an object.
 */
bool remove_obj ( Creature *ch, int iWear, bool fReplace )
{
	Item *obj;

	if ( ( obj = get_eq_char ( ch, iWear ) ) == NULL )
	{ return TRUE; }

	if ( !fReplace )
	{ return FALSE; }

	if ( IS_SET ( obj->extra_flags, ITEM_NOREMOVE ) ) {
		act ( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
		return FALSE;
	}

	unequip_char ( ch, obj );
	act ( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
	act ( "You stop using $p.", ch, obj, NULL, TO_CHAR );
	return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj ( Creature *ch, Item *obj, bool fReplace )
{
	char buf[MAX_STRING_LENGTH];

	if(!obj) { SUICIDE; }
	
	if (!IS_NPC(ch)) {    	
		if ((obj->requirements[SIZ_REQ] != SIZE_MAGIC) && (ch->size != obj->requirements[SIZ_REQ]))    	
		{			
			writeBuffer("The item does not even fit you.\r\n",ch);		    
			return;    	
		}    	
		if (get_curr_stat(ch,STAT_STR) < obj->requirements[STR_REQ])    	
		{
			writeBuffer("You do not have enough strength to wear this item.\r\n",ch);
			return;
		}
		if (get_curr_stat(ch,STAT_DEX) < obj->requirements[DEX_REQ])
		{
			writeBuffer("You do not have enough dexterity to wear this item.\r\n",ch);
			return;
		}
		if (get_curr_stat(ch,STAT_CON) < obj->requirements[CON_REQ])
		{
			writeBuffer("You do not have enough constitution to wear this item.\r\n",ch);
			return;
		}
		if (get_curr_stat(ch,STAT_INT) < obj->requirements[INT_REQ])
		{
			writeBuffer("You do not have enough Inteligence to wear this item.\r\n",ch);
			return;
		}
		if (get_curr_stat(ch,STAT_WIS) < obj->requirements[WIS_REQ])
		{
			writeBuffer("You do not have enough wisdom to wear this item.\r\n",ch);
			return;
		}
	} // -- end of IS_NPC check for stats	

	if ( ch->level < obj->level ) {
		sprintf ( buf, "You must be level %d to use this object.\n\r",
				  obj->level );
		writeBuffer ( buf, ch );
		act ( "$n tries to use $p, but is too inexperienced.",
			  ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( obj->item_type == ITEM_LIGHT ) {
		if ( !remove_obj ( ch, WEAR_LIGHT, fReplace ) )
		{ return; }
		act ( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
		act ( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_LIGHT );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_FINGER ) ) {
		if ( get_eq_char ( ch, WEAR_FINGER_L ) != NULL
				&&   get_eq_char ( ch, WEAR_FINGER_R ) != NULL
				&&   !remove_obj ( ch, WEAR_FINGER_L, fReplace )
				&&   !remove_obj ( ch, WEAR_FINGER_R, fReplace ) )
		{ return; }

		if ( get_eq_char ( ch, WEAR_FINGER_L ) == NULL ) {
			act ( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
			act ( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_FINGER_L );
			return;
		}

		if ( get_eq_char ( ch, WEAR_FINGER_R ) == NULL ) {
			act ( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
			act ( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_FINGER_R );
			return;
		}

		log_hd ( LOG_ERROR, "Wear_obj: no free finger." );
		writeBuffer ( "You already wear two rings.\n\r", ch );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_NECK ) ) {
		if ( get_eq_char ( ch, WEAR_NECK_1 ) != NULL
				&&   get_eq_char ( ch, WEAR_NECK_2 ) != NULL
				&&   !remove_obj ( ch, WEAR_NECK_1, fReplace )
				&&   !remove_obj ( ch, WEAR_NECK_2, fReplace ) )
		{ return; }

		if ( get_eq_char ( ch, WEAR_NECK_1 ) == NULL ) {
			act ( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
			act ( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_NECK_1 );
			return;
		}

		if ( get_eq_char ( ch, WEAR_NECK_2 ) == NULL ) {
			act ( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
			act ( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_NECK_2 );
			return;
		}

		log_hd ( LOG_ERROR, "Wear_obj: no free neck." );
		writeBuffer ( "You already wear two neck items.\n\r", ch );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_BODY ) ) {
		if ( !remove_obj ( ch, WEAR_BODY, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s torso.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your torso.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_BODY );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_HEAD ) ) {
		if ( !remove_obj ( ch, WEAR_HEAD, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_HEAD );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_LEGS ) ) {
		if ( !remove_obj ( ch, WEAR_LEGS, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_LEGS );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_FEET ) ) {
		if ( !remove_obj ( ch, WEAR_FEET, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_FEET );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_HANDS ) ) {
		if ( !remove_obj ( ch, WEAR_HANDS, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_HANDS );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_ARMS ) ) {
		if ( !remove_obj ( ch, WEAR_ARMS, fReplace ) )
		{ return; }
		act ( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_ARMS );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_ABOUT ) ) {
		if ( !remove_obj ( ch, WEAR_ABOUT, fReplace ) )
		{ return; }
		act ( "$n wears $p about $s torso.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p about your torso.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_ABOUT );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_WAIST ) ) {
		if ( !remove_obj ( ch, WEAR_WAIST, fReplace ) )
		{ return; }
		act ( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
		act ( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_WAIST );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_WRIST ) ) {
		if ( get_eq_char ( ch, WEAR_WRIST_L ) != NULL
				&&   get_eq_char ( ch, WEAR_WRIST_R ) != NULL
				&&   !remove_obj ( ch, WEAR_WRIST_L, fReplace )
				&&   !remove_obj ( ch, WEAR_WRIST_R, fReplace ) )
		{ return; }

		if ( get_eq_char ( ch, WEAR_WRIST_L ) == NULL ) {
			act ( "$n wears $p around $s left wrist.",
				  ch, obj, NULL, TO_ROOM );
			act ( "You wear $p around your left wrist.",
				  ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_WRIST_L );
			return;
		}

		if ( get_eq_char ( ch, WEAR_WRIST_R ) == NULL ) {
			act ( "$n wears $p around $s right wrist.",
				  ch, obj, NULL, TO_ROOM );
			act ( "You wear $p around your right wrist.",
				  ch, obj, NULL, TO_CHAR );
			equip_char ( ch, obj, WEAR_WRIST_R );
			return;
		}

		log_hd ( LOG_ERROR, "Wear_obj: no free wrist." );
		writeBuffer ( "You already wear two wrist items.\n\r", ch );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_SHIELD ) ) {
		Item *weapon;

		if ( !remove_obj ( ch, WEAR_SHIELD, fReplace ) )
		{ return; }

		weapon = get_eq_char ( ch, WEAR_WIELD );
		if ( weapon != NULL && ch->size < SIZE_LARGE
				&&  IS_WEAPON_STAT ( weapon, WEAPON_TWO_HANDS ) ) {
			writeBuffer ( "Your hands are tied up with your weapon!\n\r", ch );
			return;
		}

		act ( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
		act ( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_SHIELD );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WIELD ) ) {
		int sn, skill;

		if ( !remove_obj ( ch, WEAR_WIELD, fReplace ) )
		{ return; }

		if ( !IS_NPC ( ch )
				&& get_obj_weight ( obj ) > ( str_app[get_curr_stat ( ch, STAT_STR )].wield
											  * 10 ) ) {
			writeBuffer ( "It is too heavy for you to wield.\n\r", ch );
			return;
		}

		if ( !IS_NPC ( ch ) && ch->size < SIZE_LARGE
				&&  IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS )
				&&  get_eq_char ( ch, WEAR_SHIELD ) != NULL ) {
			writeBuffer ( "You need two hands free for that weapon.\n\r", ch );
			return;
		}

		act ( "$n wields $p.", ch, obj, NULL, TO_ROOM );
		act ( "You wield $p.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_WIELD );

		sn = get_weapon_sn ( ch );

		if ( sn == gsn_hand_to_hand )
		{ return; }

		skill = get_weapon_skill ( ch, sn );

		if ( skill >= 100 )
		{ act ( "$p feels like a part of you!", ch, obj, NULL, TO_CHAR ); }
		else if ( skill > 85 )
		{ act ( "You feel quite confident with $p.", ch, obj, NULL, TO_CHAR ); }
		else if ( skill > 70 )
		{ act ( "You are skilled with $p.", ch, obj, NULL, TO_CHAR ); }
		else if ( skill > 50 )
		{ act ( "Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR ); }
		else if ( skill > 25 )
		{ act ( "$p feels a little clumsy in your hands.", ch, obj, NULL, TO_CHAR ); }
		else if ( skill > 1 )
		{ act ( "You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR ); }
		else
			act ( "You don't even know which end is up on $p.",
				  ch, obj, NULL, TO_CHAR );

		return;
	}

	if ( CAN_WEAR ( obj, ITEM_HOLD ) ) {
		if ( !remove_obj ( ch, WEAR_HOLD, fReplace ) )
		{ return; }
		act ( "$n holds $p in $s hand.",   ch, obj, NULL, TO_ROOM );
		act ( "You hold $p in your hand.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_HOLD );
		return;
	}

	if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) ) {
		if ( !remove_obj ( ch, WEAR_FLOAT, fReplace ) )
		{ return; }
		act ( "$n releases $p to float next to $m.", ch, obj, NULL, TO_ROOM );
		act ( "You release $p and it floats next to you.", ch, obj, NULL, TO_CHAR );
		equip_char ( ch, obj, WEAR_FLOAT );
		return;
	}

	if ( fReplace )
	{ writeBuffer ( "You can't wear, wield, or hold that.\n\r", ch ); }

	return;
}

DefineCommand ( cmd_wear )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Wear, wield, or hold what?\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "all" ) ) {
		Item *obj_next;

		for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( obj->wear_loc == WEAR_NONE && can_see_obj ( ch, obj ) )
			{ wear_obj ( ch, obj, FALSE ); }
		}
		return;
	} else {
		if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
			writeBuffer ( "You do not have that item.\n\r", ch );
			return;
		}

		wear_obj ( ch, obj, TRUE );
	}

	return;
}



DefineCommand ( cmd_remove )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Remove what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_wear ( ch, arg ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	remove_obj ( ch, obj->wear_loc, TRUE );
	return;
}



DefineCommand ( cmd_sacrifice )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Item *obj;
	int silver;

	/* variables for AUTOSPLIT */
	Creature *gch;
	int members;
	char buffer[100];


	one_argument ( argument, arg );

	if ( arg[0] == '\0' || !str_cmp ( arg, ch->name ) ) {
		act ( "$n offers $mself to Mota, who graciously declines.",
			  ch, NULL, NULL, TO_ROOM );
		writeBuffer (
			"Mota appreciates your offer and may accept it later.\n\r", ch );
		return;
	}

	obj = get_obj_list ( ch, arg, ch->in_room->contents );
	if ( obj == NULL ) {
		writeBuffer ( "You can't find it.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_CORPSE_PC ) {
		if ( obj->contains ) {
			writeBuffer (
				"Mota wouldn't like that.\n\r", ch );
			return;
		}
	}


	if ( !CAN_WEAR ( obj, ITEM_TAKE ) || CAN_WEAR ( obj, ITEM_NO_SAC ) ) {
		act ( "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
		return;
	}

	if ( obj->in_room != NULL ) {
		for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room )
			if ( gch->on == obj ) {
				act ( "$N appears to be using $p.",
					  ch, obj, gch, TO_CHAR );
				return;
			}
	}

	silver = UMAX ( 1, obj->level * 3 );

	if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
	{ silver = UMIN ( silver, obj->cost ); }

	if ( silver == 1 )
		writeBuffer (
			"Mota gives you one silver coin for your sacrifice.\n\r", ch );
	else {
		snprintf ( buf, sizeof ( buf ), "Mota gives you %d silver coins for your sacrifice.\n\r",
				   silver );
		writeBuffer ( buf, ch );
	}

	ch->silver += silver;

	if ( IS_SET ( ch->act, PLR_AUTOSPLIT ) ) {
		/* AUTOSPLIT code */
		members = 0;
		for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) {
			if ( is_same_group ( gch, ch ) )
			{ members++; }
		}

		if ( members > 1 && silver > 1 ) {
			sprintf ( buffer, "%d", silver );
			cmd_function ( ch, &cmd_split, buffer );
		}
	}

	act ( "$n sacrifices $p to Mota.", ch, obj, NULL, TO_ROOM );
	wiznet ( "$N sends up $p as a burnt offering.",
			 ch, obj, WIZ_SACCING, 0, 0 );
	extract_obj ( obj );
	return;
}



DefineCommand ( cmd_quaff )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Quaff what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that potion.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_POTION ) {
		writeBuffer ( "You can quaff only potions.\n\r", ch );
		return;
	}

	if ( ch->level < obj->level ) {
		writeBuffer ( "This liquid is too powerful for you to drink.\n\r", ch );
		return;
	}

	act ( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
	act ( "You quaff $p.", ch, obj, NULL , TO_CHAR );

	obj_cast_spell ( obj->value[1], obj->value[0], ch, ch, NULL );
	obj_cast_spell ( obj->value[2], obj->value[0], ch, ch, NULL );
	obj_cast_spell ( obj->value[3], obj->value[0], ch, ch, NULL );

	extract_obj ( obj );
	return;
}

DefineCommand ( cmd_recite )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Creature *victim;
	Item *scroll;
	Item *obj;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );

	if ( ( scroll = get_obj_carry ( ch, arg1, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that scroll.\n\r", ch );
		return;
	}

	if ( scroll->item_type != ITEM_SCROLL ) {
		writeBuffer ( "You can recite only scrolls.\n\r", ch );
		return;
	}

	if ( ch->level < scroll->level ) {
		writeBuffer (
			"This scroll is too complex for you to comprehend.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg2[0] == '\0' ) {
		victim = ch;
	} else {
		if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
				&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL ) {
			writeBuffer ( "You can't find it.\n\r", ch );
			return;
		}
	}

	act ( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
	act ( "You recite $p.", ch, scroll, NULL, TO_CHAR );

	if ( number_percent() >= 20 + get_skill ( ch, gsn_scrolls ) * 4 / 5 ) {
		writeBuffer ( "You mispronounce a syllable.\n\r", ch );
		check_improve ( ch, gsn_scrolls, FALSE, 2 );
	}

	else {
		obj_cast_spell ( scroll->value[1], scroll->value[0], ch, victim, obj );
		obj_cast_spell ( scroll->value[2], scroll->value[0], ch, victim, obj );
		obj_cast_spell ( scroll->value[3], scroll->value[0], ch, victim, obj );
		check_improve ( ch, gsn_scrolls, TRUE, 2 );
	}

	extract_obj ( scroll );
	return;
}

DefineCommand ( cmd_brandish )
{
	Creature *vch;
	Creature *vch_next;
	Item *staff;
	int sn;

	if ( ( staff = get_eq_char ( ch, WEAR_HOLD ) ) == NULL ) {
		writeBuffer ( "You hold nothing in your hand.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_STAFF ) {
		writeBuffer ( "You can brandish only with a staff.\n\r", ch );
		return;
	}

	if ( ( sn = staff->value[3] ) < 0
			||   sn >= MAX_SKILL
			||   skill_table[sn].spell_fun == 0 ) {
		log_hd ( LOG_ERROR, Format ( "Do_brandish: bad sn %d.", sn ) );
		return;
	}

	WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );

	if ( staff->value[2] > 0 ) {
		act ( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
		act ( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
		if ( ch->level < staff->level
				||   number_percent() >= 20 + get_skill ( ch, gsn_staves ) * 4 / 5 ) {
			act ( "You fail to invoke $p.", ch, staff, NULL, TO_CHAR );
			act ( "...and nothing happens.", ch, NULL, NULL, TO_ROOM );
			check_improve ( ch, gsn_staves, FALSE, 2 );
		}

		else for ( vch = ch->in_room->people; vch; vch = vch_next ) {
				vch_next	= vch->next_in_room;

				switch ( skill_table[sn].target ) {
					default:
						log_hd ( LOG_ERROR, Format ( "cmd_brandish: bad target for sn %d.", sn ) );
						return;

					case TAR_IGNORE:
						if ( vch != ch )
						{ continue; }
						break;

					case TAR_CHAR_OFFENSIVE:
						if ( IS_NPC ( ch ) ? IS_NPC ( vch ) : !IS_NPC ( vch ) )
						{ continue; }
						break;

					case TAR_CHAR_DEFENSIVE:
						if ( IS_NPC ( ch ) ? !IS_NPC ( vch ) : IS_NPC ( vch ) )
						{ continue; }
						break;

					case TAR_CHAR_SELF:
						if ( vch != ch )
						{ continue; }
						break;
				}

				obj_cast_spell ( staff->value[3], staff->value[0], ch, vch, NULL );
				check_improve ( ch, gsn_staves, TRUE, 2 );
			}
	}

	if ( --staff->value[2] <= 0 ) {
		act ( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
		act ( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
		extract_obj ( staff );
	}

	return;
}

DefineCommand ( cmd_zap )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	Item *wand;
	Item *obj;

	one_argument ( argument, arg );
	if ( arg[0] == '\0' && ch->fighting == NULL ) {
		writeBuffer ( "Zap whom or what?\n\r", ch );
		return;
	}

	if ( ( wand = get_eq_char ( ch, WEAR_HOLD ) ) == NULL ) {
		writeBuffer ( "You hold nothing in your hand.\n\r", ch );
		return;
	}

	if ( wand->item_type != ITEM_WAND ) {
		writeBuffer ( "You can zap only with a wand.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg[0] == '\0' ) {
		if ( ch->fighting != NULL ) {
			victim = ch->fighting;
		} else {
			writeBuffer ( "Zap whom or what?\n\r", ch );
			return;
		}
	} else {
		if ( ( victim = get_char_room ( ch, arg ) ) == NULL
				&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL ) {
			writeBuffer ( "You can't find it.\n\r", ch );
			return;
		}
	}

	WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );

	if ( wand->value[2] > 0 ) {
		if ( victim != NULL ) {
			act ( "$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT );
			act ( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
			act ( "$n zaps you with $p.", ch, wand, victim, TO_VICT );
		} else {
			act ( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
			act ( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
		}

		if ( ch->level < wand->level
				||  number_percent() >= 20 + get_skill ( ch, gsn_wands ) * 4 / 5 ) {
			act ( "Your efforts with $p produce only smoke and sparks.",
				  ch, wand, NULL, TO_CHAR );
			act ( "$n's efforts with $p produce only smoke and sparks.",
				  ch, wand, NULL, TO_ROOM );
			check_improve ( ch, gsn_wands, FALSE, 2 );
		} else {
			obj_cast_spell ( wand->value[3], wand->value[0], ch, victim, obj );
			check_improve ( ch, gsn_wands, TRUE, 2 );
		}
	}

	if ( --wand->value[2] <= 0 ) {
		act ( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
		act ( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
		extract_obj ( wand );
	}

	return;
}

DefineCommand ( cmd_steal )
{
	char buf  [MAX_STRING_LENGTH];
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	Creature *victim;
	Item *obj;
	int percent;

	argument = one_argument ( argument, arg1 );
	argument = one_argument ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Steal what from whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "That's pointless.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim )
			&& victim->position == POS_FIGHTING ) {
		writeBuffer (  "Kill stealing is not permitted.\n\r"
					   "You'd better not -- you might get hit.\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, skill_table[gsn_steal].beats );
	percent  = number_percent();

	if ( !IS_AWAKE ( victim ) )
	{ percent -= 10; }
	else if ( !can_see ( victim, ch ) )
	{ percent += 25; }
	else
	{ percent += 50; }

	if ( ( ( ch->level + 7 < victim->level || ch->level - 7 > victim->level )
			&& !IS_NPC ( victim ) && !IS_NPC ( ch ) )
			|| ( !IS_NPC ( ch ) && percent > get_skill ( ch, gsn_steal ) )
			|| ( !IS_NPC ( ch ) && !is_clan ( ch ) ) ) {
		/*
		 * Failure.
		 */
		writeBuffer ( "Oops.\n\r", ch );
		affect_strip ( ch, gsn_sneak );
		REMOVE_BIT ( ch->affected_by, AFF_SNEAK );

		act ( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
		act ( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
		switch ( number_range ( 0, 3 ) ) {
			case 0 :
				sprintf ( buf, "%s is a lousy thief!", ch->name );
				break;
			case 1 :
				sprintf ( buf, "%s couldn't rob %s way out of a paper bag!",
						  ch->name, ( ch->sex == 2 ) ? "her" : "his" );
				break;
			case 2 :
				sprintf ( buf, "%s tried to rob me!", ch->name );
				break;
			case 3 :
				snprintf ( buf, sizeof ( buf ), "Keep your hands out of there, %s!", ch->name );
				break;
		}
		if ( !IS_AWAKE ( victim ) )
		{ cmd_function ( victim, &cmd_wake, "" ); }
		if ( IS_AWAKE ( victim ) )
		{ cmd_function ( victim, &cmd_yell, buf ); }
		if ( !IS_NPC ( ch ) ) {
			if ( IS_NPC ( victim ) ) {
				check_improve ( ch, gsn_steal, FALSE, 2 );
				multi_hit ( victim, ch, TYPE_UNDEFINED );
			} else {
				snprintf ( buf, sizeof ( buf ), "$N tried to steal from %s.", victim->name );
				wiznet ( buf, ch, NULL, WIZ_FLAGS, 0, 0 );
				if ( !IS_SET ( ch->act, PLR_THIEF ) ) {
					SET_BIT ( ch->act, PLR_THIEF );
					writeBuffer ( "*** You are now a THIEF!! ***\n\r", ch );
					save_char_obj ( ch );
				}
			}
		}

		return;
	}

	if ( !str_cmp ( arg1, "coin"  )
			||   !str_cmp ( arg1, "coins" )
			||   !str_cmp ( arg1, "gold"  )
			||	 !str_cmp ( arg1, "silver" ) ) {
		int gold, silver;

		gold = victim->gold * number_range ( 1, ch->level ) / MAX_LEVEL;
		silver = victim->silver * number_range ( 1, ch->level ) / MAX_LEVEL;
		if ( gold <= 0 && silver <= 0 ) {
			writeBuffer ( "You couldn't get any coins.\n\r", ch );
			return;
		}

		ch->gold     	+= gold;
		ch->silver   	+= silver;
		victim->silver 	-= silver;
		victim->gold 	-= gold;
		if ( silver <= 0 )
		{ sprintf ( buf, "Bingo!  You got %d gold coins.\n\r", gold ); }
		else if ( gold <= 0 )
		{ sprintf ( buf, "Bingo!  You got %d silver coins.\n\r", silver ); }
		else
			snprintf ( buf, sizeof ( buf ), "Bingo!  You got %d silver and %d gold coins.\n\r",
					   silver, gold );

		writeBuffer ( buf, ch );
		check_improve ( ch, gsn_steal, TRUE, 2 );
		return;
	}

	if ( ( obj = get_obj_carry ( victim, arg1, ch ) ) == NULL ) {
		writeBuffer ( "You can't find it.\n\r", ch );
		return;
	}

	if ( !can_drop_obj ( ch, obj )
			||   IS_SET ( obj->extra_flags, ITEM_INVENTORY )
			||   obj->level > ch->level ) {
		writeBuffer ( "You can't pry it away.\n\r", ch );
		return;
	}

	if ( ch->carry_number + get_obj_number ( obj ) > can_carry_n ( ch ) ) {
		writeBuffer ( "You have your hands full.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + get_obj_weight ( obj ) > can_carry_w ( ch ) ) {
		writeBuffer ( "You can't carry that much weight.\n\r", ch );
		return;
	}

	obj_from_char ( obj );
	obj_to_char ( obj, ch );
	act ( "You pocket $p.", ch, obj, NULL, TO_CHAR );
	check_improve ( ch, gsn_steal, TRUE, 2 );
	writeBuffer ( "Got it!\n\r", ch );
	return;
}

/*
 * Shopping commands.
 */
Creature *find_keeper ( Creature *ch )
{
	/*char buf[MAX_STRING_LENGTH];*/
	Creature *keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room ) {
		if ( IS_NPC ( keeper ) && ( pShop = keeper->pIndexData->pShop ) != NULL )
		{ break; }
	}

	if ( pShop == NULL ) {
		writeBuffer ( "You can't do that here.\n\r", ch );
		return NULL;
	}

	/*
	 * Undesirables.
	 *
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_KILLER) )
	{
	cmd_function(keeper, &cmd_say, "Killers are not welcome!");
	snprintf(buf,sizeof(buf), "%s the KILLER is over here!\n\r", ch->name);
	cmd_function(keeper, &cmd_yell, buf );
	return NULL;
	}

	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_THIEF) )
	{
	cmd_function(keeper, &cmd_say, "Thieves are not welcome!");
	snprintf(buf,sizeof(buf), "%s the THIEF is over here!\n\r", ch->name);
	cmd_function(keeper, &cmd_yell, buf );
	return NULL;
	}
	*/
	/*
	 * Shop hours.
	 */
	if ( time_info.hour < pShop->open_hour ) {
		cmd_function ( keeper, &cmd_say, "Sorry, I am closed. Come back later." );
		return NULL;
	}

	if ( time_info.hour > pShop->close_hour ) {
		cmd_function ( keeper, &cmd_say, "Sorry, I am closed. Come back tomorrow." );
		return NULL;
	}

	/*
	 * Invisible or hidden people.
	 */
	if ( !can_see ( keeper, ch ) ) {
		cmd_function ( keeper, &cmd_say, "I don't trade with folks I can't see." );
		return NULL;
	}

	return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper ( Item *obj, Creature *ch )
{
	Item *t_obj, *t_obj_next;

	/* see if any duplicates are found */
	for ( t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next ) {
		t_obj_next = t_obj->next_content;

		if ( obj->pIndexData == t_obj->pIndexData
				&&  !str_cmp ( obj->short_descr, t_obj->short_descr ) ) {
			/* if this is an unlimited item, destroy the new one */
			if ( IS_OBJ_STAT ( t_obj, ITEM_INVENTORY ) ) {
				extract_obj ( obj );
				return;
			}
			obj->cost = t_obj->cost; /* keep it standard */
			break;
		}
	}

	if ( t_obj == NULL ) {
		obj->next_content = ch->carrying;
		ch->carrying = obj;
	} else {
		obj->next_content = t_obj->next_content;
		t_obj->next_content = obj;
	}

	obj->carried_by      = ch;
	obj->in_room         = NULL;
	obj->in_obj          = NULL;
	ch->carry_number    += get_obj_number ( obj );
	ch->carry_weight    += get_obj_weight ( obj );
}

/* get an object from a shopkeeper's list */
Item *get_obj_keeper ( Creature *ch, Creature *keeper, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int number;
	int count;

	number = number_argument ( argument, arg );
	count  = 0;
	for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc == WEAR_NONE
				&&  can_see_obj ( keeper, obj )
				&&  can_see_obj ( ch, obj )
				&&  is_name ( arg, obj->name ) ) {
			if ( ++count == number )
			{ return obj; }

			/* skip other objects of the same name */
			while ( obj->next_content != NULL
					&& obj->pIndexData == obj->next_content->pIndexData
					&& !str_cmp ( obj->short_descr, obj->next_content->short_descr ) )
			{ obj = obj->next_content; }
		}
	}

	return NULL;
}

int get_cost ( Creature *keeper, Item *obj, bool fBuy )
{
	SHOP_DATA *pShop;
	int cost;

	if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	{ return 0; }

	if ( fBuy ) {
		cost = obj->cost * pShop->profit_buy  / 100;
	} else {
		Item *obj2;
		int itype;

		cost = 0;
		for ( itype = 0; itype < MAX_TRADE; itype++ ) {
			if ( obj->item_type == pShop->buy_type[itype] ) {
				cost = obj->cost * pShop->profit_sell / 100;
				break;
			}
		}

		if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) ) {
			for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content ) {
				if ( obj->pIndexData == obj2->pIndexData
						&&   !str_cmp ( obj->short_descr, obj2->short_descr ) ) {
					if ( IS_OBJ_STAT ( obj2, ITEM_INVENTORY ) )
					{ cost /= 2; }
					else
					{ cost = cost * 3 / 4; }
				}
			}
		}
	}

	if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND ) {
		if ( obj->value[1] == 0 )
		{ cost /= 4; }
		else
		{ cost = cost * obj->value[2] / obj->value[1]; }
	}

	return cost;
}

DefineCommand ( cmd_buy )
{
	char buf[MAX_STRING_LENGTH];
	int cost, roll;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Buy what?\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->in_room->room_flags, ROOM_PET_SHOP ) ) {
		char arg[MAX_INPUT_LENGTH];
		char buf[MAX_STRING_LENGTH];
		Creature *pet;
		RoomData *pRoomIndexNext;
		RoomData *in_room;

		smash_tilde ( argument );

		if ( IS_NPC ( ch ) )
		{ return; }

		argument = one_argument ( argument, arg );

		/* hack to make new thalos pets work */
		if ( ch->in_room->vnum == 9621 )
		{ pRoomIndexNext = get_room_index ( 9706 ); }
		else
		{ pRoomIndexNext = get_room_index ( ch->in_room->vnum + 1 ); }
		if ( pRoomIndexNext == NULL ) {
			log_hd ( LOG_ERROR, Format ( "cmd_buy: bad pet shop at vnum %d.", ch->in_room->vnum ) );
			writeBuffer ( "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		in_room     = ch->in_room;
		ch->in_room = pRoomIndexNext;
		pet         = get_char_room ( ch, arg );
		ch->in_room = in_room;

		if ( pet == NULL || !IS_SET ( pet->act, ACT_PET ) ) {
			writeBuffer ( "Sorry, you can't buy that here.\n\r", ch );
			return;
		}

		if ( ch->pet != NULL ) {
			writeBuffer ( "You already own a pet.\n\r", ch );
			return;
		}

		cost = 10 * pet->level * pet->level;

		if ( ( ch->silver + 100 * ch->gold ) < cost ) {
			writeBuffer ( "You can't afford it.\n\r", ch );
			return;
		}

		if ( ch->level < pet->level ) {
			writeBuffer (
				"You're not powerful enough to master this pet.\n\r", ch );
			return;
		}

		/* haggle */
		roll = number_percent();
		if ( roll < get_skill ( ch, gsn_haggle ) ) {
			cost -= cost / 2 * roll / 100;
			snprintf ( buf, sizeof ( buf ), "You haggle the price down to %d coins.\n\r", cost );
			writeBuffer ( buf, ch );
			check_improve ( ch, gsn_haggle, TRUE, 4 );

		}

		deduct_cost ( ch, cost );
		pet			= create_mobile ( pet->pIndexData );
		SET_BIT ( pet->act, ACT_PET );
		SET_BIT ( pet->affected_by, AFF_CHARM );
		pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;

		argument = one_argument ( argument, arg );
		if ( arg[0] != '\0' ) {
			sprintf ( buf, "%s %s", pet->name, arg );
			PURGE_DATA ( pet->name );
			pet->name = assign_string ( buf );
		}

		sprintf ( buf, "%sA neck tag says 'I belong to %s'.\n\r",
				  pet->description, ch->name );
		PURGE_DATA ( pet->description );
		pet->description = assign_string ( buf );

		char_to_room ( pet, ch->in_room );
		add_follower ( pet, ch );
		pet->leader = ch;
		ch->pet = pet;
		writeBuffer ( "Enjoy your pet.\n\r", ch );
		act ( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
		return;
	} else {
		Creature *keeper;
		Item *obj, *t_obj;
		char arg[MAX_INPUT_LENGTH];
		int number, count = 1;

		if ( ( keeper = find_keeper ( ch ) ) == NULL )
		{ return; }

		number = mult_argument ( argument, arg );
		obj  = get_obj_keeper ( ch, keeper, arg );
		cost = get_cost ( keeper, obj, TRUE );

		if ( number < 1 || number > 99 ) {
			act ( "$n tells you 'Get real!", keeper, NULL, ch, TO_VICT );
			return;
		}

		if ( cost <= 0 || !can_see_obj ( ch, obj ) ) {
			act ( "$n tells you 'I don't sell that -- try 'list''.",
				  keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( !IS_OBJ_STAT ( obj, ITEM_INVENTORY ) ) {
			for ( t_obj = obj->next_content;
					count < number && t_obj != NULL;
					t_obj = t_obj->next_content ) {
				if ( t_obj->pIndexData == obj->pIndexData
						&&  !str_cmp ( t_obj->short_descr, obj->short_descr ) )
				{ count++; }
				else
				{ break; }
			}

			if ( count < number ) {
				act ( "$n tells you 'I don't have that many in stock.",
					  keeper, NULL, ch, TO_VICT );
				ch->reply = keeper;
				return;
			}
		}

		if ( ( ch->silver + ch->gold * 100 ) < cost * number ) {
			if ( number > 1 )
				act ( "$n tells you 'You can't afford to buy that many.",
					  keeper, obj, ch, TO_VICT );
			else
				act ( "$n tells you 'You can't afford to buy $p'.",
					  keeper, obj, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( obj->level > ch->level ) {
			act ( "$n tells you 'You can't use $p yet'.",
				  keeper, obj, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}

		if ( ch->carry_number +  number * get_obj_number ( obj ) > can_carry_n ( ch ) ) {
			writeBuffer ( "You can't carry that many items.\n\r", ch );
			return;
		}

		if ( ch->carry_weight + number * get_obj_weight ( obj ) > can_carry_w ( ch ) ) {
			writeBuffer ( "You can't carry that much weight.\n\r", ch );
			return;
		}

		/* haggle */
		roll = number_percent();
		if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT )
				&& roll < get_skill ( ch, gsn_haggle ) ) {
			cost -= obj->cost / 2 * roll / 100;
			act ( "You haggle with $N.", ch, NULL, keeper, TO_CHAR );
			check_improve ( ch, gsn_haggle, TRUE, 4 );
		}

		if ( number > 1 ) {
			snprintf ( buf, sizeof ( buf ), "$n buys $p[%d].", number );
			act ( buf, ch, obj, NULL, TO_ROOM );
			snprintf ( buf, sizeof ( buf ), "You buy $p[%d] for %d silver.", number, cost * number );
			act ( buf, ch, obj, NULL, TO_CHAR );
		} else {
			act ( "$n buys $p.", ch, obj, NULL, TO_ROOM );
			snprintf ( buf, sizeof ( buf ), "You buy $p for %d silver.", cost );
			act ( buf, ch, obj, NULL, TO_CHAR );
		}
		deduct_cost ( ch, cost * number );
		keeper->gold += cost * number / 100;
		keeper->silver += cost * number - ( cost * number / 100 ) * 100;

		for ( count = 0; count < number; count++ ) {
			if ( IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
			{ t_obj = create_object ( obj->pIndexData, obj->level ); }
			else {
				t_obj = obj;
				obj = obj->next_content;
				obj_from_char ( t_obj );
			}

			if ( t_obj->timer > 0 && !IS_OBJ_STAT ( t_obj, ITEM_HAD_TIMER ) )
			{ t_obj->timer = 0; }
			REMOVE_BIT ( t_obj->extra_flags, ITEM_HAD_TIMER );
			obj_to_char ( t_obj, ch );
			if ( cost < t_obj->cost )
			{ t_obj->cost = cost; }
		}
	}
}

DefineCommand ( cmd_list )
{
	char buf[MAX_STRING_LENGTH];

	if ( IS_SET ( ch->in_room->room_flags, ROOM_PET_SHOP ) ) {
		RoomData *pRoomIndexNext;
		Creature *pet;
		bool found;

		/* hack to make new thalos pets work */
		{ pRoomIndexNext = get_room_index ( ch->in_room->vnum + 1 ); }

		if ( pRoomIndexNext == NULL ) {
			log_hd ( LOG_ERROR, Format ( "cmd_list: bad pet shop at vnum %d.", ch->in_room->vnum ) );
			writeBuffer ( "You can't do that here.\n\r", ch );
			return;
		}

		found = FALSE;
		for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room ) {
			if ( IS_SET ( pet->act, ACT_PET ) ) {
				if ( !found ) {
					found = TRUE;
					writeBuffer ( "Pets for sale:\n\r", ch );
				}
				sprintf ( buf, "[%2d] %8d - %s\n\r",
						  pet->level,
						  10 * pet->level * pet->level,
						  pet->short_descr );
				writeBuffer ( buf, ch );
			}
		}
		if ( !found )
		{ writeBuffer ( "Sorry, we're out of pets right now.\n\r", ch ); }
		return;
	} else {
		Creature *keeper;
		Item *obj;
		int cost, count;
		bool found;
		char arg[MAX_INPUT_LENGTH];

		if ( ( keeper = find_keeper ( ch ) ) == NULL )
		{ return; }
		one_argument ( argument, arg );

		found = FALSE;
		for ( obj = keeper->carrying; obj; obj = obj->next_content ) {
			if ( obj->wear_loc == WEAR_NONE
					&&   can_see_obj ( ch, obj )
					&&   ( cost = get_cost ( keeper, obj, TRUE ) ) > 0
					&&   ( arg[0] == '\0'
						   ||  is_name ( arg, obj->name ) ) ) {
				if ( !found ) {
					found = TRUE;
					writeBuffer ( "[Lv Price Qty] [str dex int wis con size  ]   Item\n\r", ch );
				}

				if ( IS_OBJ_STAT ( obj, ITEM_INVENTORY ) )
					snprintf ( buf, sizeof ( buf ), "[%2d %5d -- ] [%3d %3d %3d %3d %3d %5s] %s\n\r",
							   obj->level, cost, 
							   obj->requirements[STR_REQ], obj->requirements[DEX_REQ], obj->requirements[INT_REQ],
							   obj->requirements[WIZ_REQ], obj->requirements[CON_REQ], size_table[obj->requirements[SIZ_REQ]],
							   obj->short_descr);
				else {
					count = 1;

					while ( obj->next_content != NULL
							&& obj->pIndexData == obj->next_content->pIndexData
							&& !str_cmp ( obj->short_descr, obj->next_content->short_descr ) ) {
						obj = obj->next_content;
						count++;
					}
					snprintf ( buf, sizeof ( buf ), "[%2d %5d %2d ]  [%3d %3d %3d %3d %3d %5s] %s\n\r",
							   obj->level, cost, count, 
							   obj->requirements[STR_REQ], obj->requirements[DEX_REQ], obj->requirements[INT_REQ],
							   obj->requirements[WIZ_REQ], obj->requirements[CON_REQ], size_table[obj->requirements[SIZ_REQ]],
							   obj->short_descr );
				}
				writeBuffer ( buf, ch );
			}
		}

		if ( !found )
		{ writeBuffer ( "You can't buy anything here.\n\r", ch ); }
		return;
	}
}

DefineCommand ( cmd_sell )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *keeper;
	Item *obj;
	int cost, roll;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Sell what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper ( ch ) ) == NULL )
	{ return; }

	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		act ( "$n tells you 'You don't have that item'.",
			  keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj ( ch, obj ) ) {
		writeBuffer ( "You can't let go of it.\n\r", ch );
		return;
	}

	if ( !can_see_obj ( keeper, obj ) ) {
		act ( "$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( ( cost = get_cost ( keeper, obj, FALSE ) ) <= 0 ) {
		act ( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}
	if ( cost > ( keeper-> silver + 100 * keeper->gold ) ) {
		act ( "$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
			  keeper, obj, ch, TO_VICT );
		return;
	}

	act ( "$n sells $p.", ch, obj, NULL, TO_ROOM );
	/* haggle */
	roll = number_percent();
	if ( !IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) && roll < get_skill ( ch, gsn_haggle ) ) {
		writeBuffer ( "You haggle with the shopkeeper.\n\r", ch );
		cost += obj->cost / 2 * roll / 100;
		cost = UMIN ( cost, 95 * get_cost ( keeper, obj, TRUE ) / 100 );
		cost = UMIN ( cost, ( keeper->silver + 100 * keeper->gold ) );
		check_improve ( ch, gsn_haggle, TRUE, 4 );
	}
	sprintf ( buf, "You sell $p for %d silver and %d gold piece%s.",
			  cost - ( cost / 100 ) * 100, cost / 100, cost == 1 ? "" : "s" );
	act ( buf, ch, obj, NULL, TO_CHAR );
	ch->gold     += cost / 100;
	ch->silver 	 += cost - ( cost / 100 ) * 100;
	deduct_cost ( keeper, cost );
	if ( keeper->gold < 0 )
	{ keeper->gold = 0; }
	if ( keeper->silver < 0 )
	{ keeper->silver = 0; }

	if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT ( obj, ITEM_SELL_EXTRACT ) ) {
		extract_obj ( obj );
	} else {
		obj_from_char ( obj );
		if ( obj->timer )
		{ SET_BIT ( obj->extra_flags, ITEM_HAD_TIMER ); }
		else
		{ obj->timer = number_range ( 50, 100 ); }
		obj_to_keeper ( obj, keeper );
	}

	return;
}

DefineCommand ( cmd_value )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *keeper;
	Item *obj;
	int cost;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Value what?\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper ( ch ) ) == NULL )
	{ return; }

	if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL ) {
		act ( "$n tells you 'You don't have that item'.",
			  keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_see_obj ( keeper, obj ) ) {
		act ( "$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( !can_drop_obj ( ch, obj ) ) {
		writeBuffer ( "You can't let go of it.\n\r", ch );
		return;
	}

	if ( ( cost = get_cost ( keeper, obj, FALSE ) ) <= 0 ) {
		act ( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
		return;
	}

	sprintf ( buf,
			  "$n tells you 'I'll give you %d silver and %d gold coins for $p'.",
			  cost - ( cost / 100 ) * 100, cost / 100 );
	act ( buf, keeper, obj, ch, TO_VICT );
	ch->reply = keeper;

	return;
}

// -- EOF

