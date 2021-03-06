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

void acid_effect ( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_ROOM ) { /* nail objects on the floor */
		RoomData *room = ( RoomData * ) vo;
		Item *obj, *obj_next;

		for ( obj = room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			acid_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_CHAR ) { /* do the effect on a victim */
		Creature *victim = ( Creature * ) vo;
		Item *obj, *obj_next;

		/* let's toast some gear */
		for ( obj = victim->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			acid_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_OBJ ) { /* toast an object */
		Item *obj = ( Item * ) vo;
		Item *t_obj, *n_obj;
		int chance;
		const char *msg;

		if ( IS_OBJ_STAT ( obj, ITEM_BURN_PROOF )
				||  IS_OBJ_STAT ( obj, ITEM_NOPURGE )
				||  Math::instance().range ( 0, 4 ) == 0 )
		{ return; }

		chance = level / 4 + dam / 10;

		if ( chance > 25 )
		{ chance = ( chance - 25 ) / 2 + 25; }
		if ( chance > 50 )
		{ chance = ( chance - 50 ) / 2 + 50; }

		if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) )
		{ chance -= 5; }

		chance -= obj->level * 2;

		switch ( obj->item_type ) {
			default:
				return;
			case ITEM_CONTAINER:
			case ITEM_CORPSE_PC:
			case ITEM_CORPSE_NPC:
				msg = "$p fumes and dissolves.";
				break;
			case ITEM_ARMOR:
				msg = "$p is pitted and etched.";
				break;
			case ITEM_CLOTHING:
				msg = "$p is corroded into scrap.";
				break;
			case ITEM_STAFF:
			case ITEM_WAND:
				chance -= 10;
				msg = "$p corrodes and breaks.";
				break;
			case ITEM_SCROLL:
				chance += 10;
				msg = "$p is burned into waste.";
				break;
		}

		chance = URANGE ( 5, chance, 95 );

		if ( Math::instance().percent() > chance )
		{ return; }

		if ( CARRIED_BY ( obj ) != NULL )
		{ act ( msg, CARRIED_BY ( obj ), obj, NULL, TO_ALL ); }
		else if ( obj->in_room != NULL && obj->in_room->people != NULL )
		{ act ( msg, obj->in_room->people, obj, NULL, TO_ALL ); }

		if ( obj->item_type == ITEM_ARMOR ) { /* etch it */
			Affect *paf;
			bool af_found = FALSE;
			int i;

			affect_enchant ( obj );

			for ( paf = obj->affected; paf != NULL; paf = paf->next ) {
				if ( paf->location == APPLY_AC ) {
					af_found = TRUE;
					paf->type = -1;
					paf->modifier += 1;
					paf->level = UMAX ( paf->level, level );
					break;
				}
			}

			if ( !af_found )
				/* needs a new affect */
			{
				paf = new_affect();

				paf->type       = -1;
				paf->level      = level;
				paf->duration   = -1;
				paf->location   = APPLY_AC;
				paf->modifier   =  1;
				paf->bitvector  = 0;
				paf->next       = obj->affected;
				obj->affected   = paf;
			}

			if ( CARRIED_BY ( obj ) != NULL && obj->wear_loc != WEAR_NONE )
				for ( i = 0; i < 4; i++ )
				{ CARRIED_BY ( obj )->armor[i] += 1; }
			return;
		}

		/* get rid of the object */
		if ( obj->contains ) { /* dump contents */
			for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj ) {
				n_obj = t_obj->next_content;
				obj_from_obj ( t_obj );
				if ( obj->in_room != NULL )
				{ obj_to_room ( t_obj, obj->in_room ); }
				else if ( CARRIED_BY ( obj ) != NULL )
				{ obj_to_room ( t_obj, CARRIED_BY ( obj )->in_room ); }
				else {
					extract_obj ( t_obj );
					continue;
				}

				acid_effect ( t_obj, level / 2, dam / 2, TARGET_OBJ );
			}
		}

		extract_obj ( obj );
		return;
	}
}


void cold_effect ( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_ROOM ) { /* nail objects on the floor */
		RoomData *room = ( RoomData * ) vo;
		Item *obj, *obj_next;

		for ( obj = room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			cold_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_CHAR ) { /* whack a character */
		Creature *victim = ( Creature * ) vo;
		Item *obj, *obj_next;

		/* chill touch effect */
		if ( !saves_spell ( level / 4 + dam / 20, victim, DAM_COLD ) ) {
			Affect af;

			act ( "$n turns blue and shivers.", victim, NULL, NULL, TO_ROOM );
			act ( "A chill sinks deep into your bones.", victim, NULL, NULL, TO_CHAR );
			af.where     = TO_AFFECTS;
			af.type      = skill_lookup ( "chill touch" );
			af.level     = level;
			af.duration  = 6;
			af.location  = APPLY_STR;
			af.modifier  = -1;
			af.bitvector = 0;
			affect_join ( victim, &af );
		}

		/* hunger! (warmth sucked out */
		if ( !IS_NPC ( victim ) )
		{ gain_condition ( victim, COND_HUNGER, dam / 20 ); }

		/* let's toast some gear */
		for ( obj = victim->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			cold_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_OBJ ) { /* toast an object */
		Item *obj = ( Item * ) vo;
		int chance;
		const char *msg;

		if ( IS_OBJ_STAT ( obj, ITEM_BURN_PROOF )
				||  IS_OBJ_STAT ( obj, ITEM_NOPURGE )
				||  Math::instance().range ( 0, 4 ) == 0 )
		{ return; }

		chance = level / 4 + dam / 10;

		if ( chance > 25 )
		{ chance = ( chance - 25 ) / 2 + 25; }
		if ( chance > 50 )
		{ chance = ( chance - 50 ) / 2 + 50; }

		if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) )
		{ chance -= 5; }

		chance -= obj->level * 2;

		switch ( obj->item_type ) {
			default:
				return;
			case ITEM_POTION:
				msg = "$p freezes and shatters!";
				chance += 25;
				break;
			case ITEM_DRINK_CON:
				msg = "$p freezes and shatters!";
				chance += 5;
				break;
		}

		chance = URANGE ( 5, chance, 95 );

		if ( Math::instance().percent() > chance )
		{ return; }

		if ( CARRIED_BY ( obj ) != NULL )
		{ act ( msg, CARRIED_BY ( obj ), obj, NULL, TO_ALL ); }
		else if ( obj->in_room != NULL && obj->in_room->people != NULL )
		{ act ( msg, obj->in_room->people, obj, NULL, TO_ALL ); }

		extract_obj ( obj );
		return;
	}
}



void fire_effect ( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_ROOM ) { /* nail objects on the floor */
		RoomData *room = ( RoomData * ) vo;
		Item *obj, *obj_next;

		for ( obj = room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			fire_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_CHAR ) { /* do the effect on a victim */
		Creature *victim = ( Creature * ) vo;
		Item *obj, *obj_next;

		/* chance of blindness */
		if ( !IS_AFFECTED ( victim, AFF_BLIND )
				&&  !saves_spell ( level / 4 + dam / 20, victim, DAM_FIRE ) ) {
			Affect af;
			act ( "$n is blinded by smoke!", victim, NULL, NULL, TO_ROOM );
			act ( "Your eyes tear up from smoke...you can't see a thing!",
				  victim, NULL, NULL, TO_CHAR );

			af.where        = TO_AFFECTS;
			af.type         = skill_lookup ( "fire breath" );
			af.level        = level;
			af.duration     = Math::instance().range ( 0, level / 10 );
			af.location     = APPLY_HITROLL;
			af.modifier     = -4;
			af.bitvector    = AFF_BLIND;

			affect_to_char ( victim, &af );
		}

		/* getting thirsty */
		if ( !IS_NPC ( victim ) )
		{ gain_condition ( victim, COND_THIRST, dam / 20 ); }

		/* let's toast some gear! */
		for ( obj = victim->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;

			fire_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_OBJ ) { /* toast an object */
		Item *obj = ( Item * ) vo;
		Item *t_obj, *n_obj;
		int chance;
		const char *msg;

		if ( IS_OBJ_STAT ( obj, ITEM_BURN_PROOF )
				||  IS_OBJ_STAT ( obj, ITEM_NOPURGE )
				||  Math::instance().range ( 0, 4 ) == 0 )
		{ return; }

		chance = level / 4 + dam / 10;

		if ( chance > 25 )
		{ chance = ( chance - 25 ) / 2 + 25; }
		if ( chance > 50 )
		{ chance = ( chance - 50 ) / 2 + 50; }

		if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) )
		{ chance -= 5; }
		chance -= obj->level * 2;

		switch ( obj->item_type ) {
			default:
				return;
			case ITEM_CONTAINER:
				msg = "$p ignites and burns!";
				break;
			case ITEM_POTION:
				chance += 25;
				msg = "$p bubbles and boils!";
				break;
			case ITEM_SCROLL:
				chance += 50;
				msg = "$p crackles and burns!";
				break;
			case ITEM_STAFF:
				chance += 10;
				msg = "$p smokes and chars!";
				break;
			case ITEM_WAND:
				msg = "$p sparks and sputters!";
				break;
			case ITEM_FOOD:
				msg = "$p blackens and crisps!";
				break;
			case ITEM_PILL:
				msg = "$p melts and drips!";
				break;
		}

		chance = URANGE ( 5, chance, 95 );

		if ( Math::instance().percent() > chance )
		{ return; }

		if ( CARRIED_BY ( obj ) != NULL )
		{ act ( msg, CARRIED_BY ( obj ), obj, NULL, TO_ALL ); }
		else if ( obj->in_room != NULL && obj->in_room->people != NULL )
		{ act ( msg, obj->in_room->people, obj, NULL, TO_ALL ); }

		if ( obj->contains ) {
			/* dump the contents */

			for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj ) {
				n_obj = t_obj->next_content;
				obj_from_obj ( t_obj );
				if ( obj->in_room != NULL )
				{ obj_to_room ( t_obj, obj->in_room ); }
				else if ( CARRIED_BY ( obj ) != NULL )
				{ obj_to_room ( t_obj, CARRIED_BY ( obj )->in_room ); }
				else {
					extract_obj ( t_obj );
					continue;
				}
				fire_effect ( t_obj, level / 2, dam / 2, TARGET_OBJ );
			}
		}

		extract_obj ( obj );
		return;
	}
}

void poison_effect ( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_ROOM ) { /* nail objects on the floor */
		RoomData *room = ( RoomData * ) vo;
		Item *obj, *obj_next;

		for ( obj = room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			poison_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_CHAR ) { /* do the effect on a victim */
		Creature *victim = ( Creature * ) vo;
		Item *obj, *obj_next;

		/* chance of poisoning */
		if ( !saves_spell ( level / 4 + dam / 20, victim, DAM_POISON ) ) {
			Affect af;

			writeBuffer ( "You feel poison coursing through your veins.\n\r",
						  victim );
			act ( "$n looks very ill.", victim, NULL, NULL, TO_ROOM );

			af.where     = TO_AFFECTS;
			af.type      = gsn_poison;
			af.level     = level;
			af.duration  = level / 2;
			af.location  = APPLY_STR;
			af.modifier  = -1;
			af.bitvector = AFF_POISON;
			affect_join ( victim, &af );
		}

		/* equipment */
		for ( obj = victim->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			poison_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_OBJ ) { /* do some poisoning */
		Item *obj = ( Item * ) vo;
		int chance;


		if ( IS_OBJ_STAT ( obj, ITEM_BURN_PROOF )
				||  IS_OBJ_STAT ( obj, ITEM_BLESS )
				||  Math::instance().range ( 0, 4 ) == 0 )
		{ return; }

		chance = level / 4 + dam / 10;
		if ( chance > 25 )
		{ chance = ( chance - 25 ) / 2 + 25; }
		if ( chance > 50 )
		{ chance = ( chance - 50 ) / 2 + 50; }

		chance -= obj->level * 2;

		switch ( obj->item_type ) {
			default:
				return;
			case ITEM_FOOD:
				break;
			case ITEM_DRINK_CON:
				if ( obj->value[0] == obj->value[1] )
				{ return; }
				break;
		}

		chance = URANGE ( 5, chance, 95 );

		if ( Math::instance().percent() > chance )
		{ return; }

		obj->value[3] = 1;
		return;
	}
}


void shock_effect ( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_ROOM ) {
		RoomData *room = ( RoomData * ) vo;
		Item *obj, *obj_next;

		for ( obj = room->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			shock_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_CHAR ) {
		Creature *victim = ( Creature * ) vo;
		Item *obj, *obj_next;

		/* daze and confused? */
		if ( !saves_spell ( level / 4 + dam / 20, victim, DAM_LIGHTNING ) ) {
			writeBuffer ( "Your muscles stop responding.\n\r", victim );
			DAZE_STATE ( victim, UMAX ( 12, level / 4 + dam / 20 ) );
		}

		/* toast some gear */
		for ( obj = victim->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			shock_effect ( obj, level, dam, TARGET_OBJ );
		}
		return;
	}

	if ( target == TARGET_OBJ ) {
		Item *obj = ( Item * ) vo;
		int chance;
		const char *msg;

		if ( IS_OBJ_STAT ( obj, ITEM_BURN_PROOF )
				||  IS_OBJ_STAT ( obj, ITEM_NOPURGE )
				||  Math::instance().range ( 0, 4 ) == 0 )
		{ return; }

		chance = level / 4 + dam / 10;

		if ( chance > 25 )
		{ chance = ( chance - 25 ) / 2 + 25; }
		if ( chance > 50 )
		{ chance = ( chance - 50 ) / 2 + 50; }

		if ( IS_OBJ_STAT ( obj, ITEM_BLESS ) )
		{ chance -= 5; }

		chance -= obj->level * 2;

		switch ( obj->item_type ) {
			default:
				return;
			case ITEM_WAND:
			case ITEM_STAFF:
				chance += 10;
				msg = "$p overloads and explodes!";
				break;
			case ITEM_JEWELRY:
				chance -= 10;
				msg = "$p is fused into a worthless lump.";
		}

		chance = URANGE ( 5, chance, 95 );

		if ( Math::instance().percent() > chance )
		{ return; }

		if ( CARRIED_BY ( obj ) != NULL )
		{ act ( msg, CARRIED_BY ( obj ), obj, NULL, TO_ALL ); }
		else if ( obj->in_room != NULL && obj->in_room->people != NULL )
		{ act ( msg, obj->in_room->people, obj, NULL, TO_ALL ); }

		extract_obj ( obj );
		return;
	}
}

