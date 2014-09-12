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
int	hit_gain	args ( ( Creature *ch ) );
int	mana_gain	args ( ( Creature *ch ) );
int	move_gain	args ( ( Creature *ch ) );
void	mobile_update	args ( ( void ) );
void	weather_update	args ( ( void ) );
void	char_update	args ( ( void ) );
void	obj_update	args ( ( void ) );
void	aggr_update	args ( ( void ) );

/* used for saving */

int	save_number = 0;



/*
 * Advancement stuff.
 */
void advance_level ( Creature *ch, bool hide )
{
	int add_hp;
	int add_mana;
	int add_move;
	int add_prac;

	ch->pcdata->last_level =
		( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600;

	add_hp	= con_app[get_curr_stat ( ch, STAT_CON )].hitp + Math::instance().range (
				  archetype_table[ch->archetype].hp_min,
				  archetype_table[ch->archetype].hp_max );
	add_mana 	= Math::instance().range ( 2, ( 2 * get_curr_stat ( ch, STAT_INT )
										   + get_curr_stat ( ch, STAT_WIS ) ) / 5 );
	if ( !archetype_table[ch->archetype].fMana )
	{ add_mana /= 2; }
	add_move	= Math::instance().range ( 1, ( get_curr_stat ( ch, STAT_CON )
										   + get_curr_stat ( ch, STAT_DEX ) ) / 6 );
	add_prac	= wis_app[get_curr_stat ( ch, STAT_WIS )].practice;

	add_hp = add_hp * 9 / 10;
	add_mana = add_mana * 9 / 10;
	add_move = add_move * 9 / 10;

	add_hp	= UMAX (  2, add_hp   );
	add_mana	= UMAX (  2, add_mana );
	add_move	= UMAX (  6, add_move );

	ch->max_hit 	+= add_hp;
	ch->max_mana	+= add_mana;
	ch->max_move	+= add_move;
	ch->practice	+= add_prac;
	ch->train		+= 1;

	ch->pcdata->perm_hit	+= add_hp;
	ch->pcdata->perm_mana	+= add_mana;
	ch->pcdata->perm_move	+= add_move;

	if ( !hide ) {
		writeBuffer(Format(
				   "You gain %d hit point%s, %d mana, %d move, and %d practice%s.\n\r",
				   add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
				   add_prac, add_prac == 1 ? "" : "s" ), ch);
	}
	return;
}

void gain_exp ( Creature *ch, int gain )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ch->level == MAX_LEVEL ) { return; }

	// -- detail if they have lost experience.
	log_hd ( LOG_DEBUG, Format ( "%s has %s %d experience.", ch->name, gain > 0 ? "gained" : "lossed", gain ) );

	ch->exp_pool += gain;
	if ( ch->exp_pool > 100000 ) {
		ch->exp_pool = 100000;
	}

	return;
}

/*
 * Regeneration stuff.
 */
int hit_gain ( Creature *ch )
{
	int gain;
	int number;

	if ( ch->in_room == NULL )
	{ return 0; }

	if ( IS_NPC ( ch ) ) {
		gain =  5 + ch->level;
		if ( IS_AFFECTED ( ch, AFF_REGENERATION ) )
		{ gain *= 2; }

		switch ( ch->position ) {
			default :
				gain /= 2;
				break;
			case POS_SLEEPING:
				gain = 3 * gain / 2;
				break;
			case POS_RESTING:
				break;
			case POS_FIGHTING:
				gain /= 3;
				break;
		}


	} else {
		gain = UMAX ( 3, get_curr_stat ( ch, STAT_CON ) - 3 + ch->level / 2 );
		gain += archetype_table[ch->archetype].hp_max - 10;
		number = Math::instance().percent();
		if ( number < get_skill ( ch, gsn_fast_healing ) ) {
			gain += number * gain / 100;
			if ( ch->hit < ch->max_hit )
			{ check_improve ( ch, gsn_fast_healing, TRUE, 8 ); }
		}

		switch ( ch->position ) {
			default:
				gain /= 4;
				break;
			case POS_SLEEPING:
				break;
			case POS_RESTING:
				gain /= 2;
				break;
			case POS_FIGHTING:
				gain /= 6;
				break;
		}

		if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
		{ gain /= 2; }

		if ( ch->pcdata->condition[COND_THIRST] == 0 )
		{ gain /= 2; }

	}

	gain = gain * ch->in_room->heal_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
	{ gain = gain * ch->on->value[3] / 100; }

	if ( IS_AFFECTED ( ch, AFF_POISON ) )
	{ gain /= 4; }

	if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
	{ gain /= 8; }

	if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
	{ gain /= 2 ; }

	return UMIN ( gain, ch->max_hit - ch->hit );
}



int mana_gain ( Creature *ch )
{
	int gain;
	int number;

	if ( ch->in_room == NULL )
	{ return 0; }

	if ( IS_NPC ( ch ) ) {
		gain = 5 + ch->level;
		switch ( ch->position ) {
			default:
				gain /= 2;
				break;
			case POS_SLEEPING:
				gain = 3 * gain / 2;
				break;
			case POS_RESTING:
				break;
			case POS_FIGHTING:
				gain /= 3;
				break;
		}
	} else {
		gain = ( get_curr_stat ( ch, STAT_WIS )
				 + get_curr_stat ( ch, STAT_INT ) + ch->level ) / 2;
		number = Math::instance().percent();
		if ( number < get_skill ( ch, gsn_meditation ) ) {
			gain += number * gain / 100;
			if ( ch->mana < ch->max_mana )
			{ check_improve ( ch, gsn_meditation, TRUE, 8 ); }
		}
		if ( !archetype_table[ch->archetype].fMana )
		{ gain /= 2; }

		switch ( ch->position ) {
			default:
				gain /= 4;
				break;
			case POS_SLEEPING:
				break;
			case POS_RESTING:
				gain /= 2;
				break;
			case POS_FIGHTING:
				gain /= 6;
				break;
		}

		if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
		{ gain /= 2; }

		if ( ch->pcdata->condition[COND_THIRST] == 0 )
		{ gain /= 2; }

	}

	gain = gain * ch->in_room->mana_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
	{ gain = gain * ch->on->value[4] / 100; }

	if ( IS_AFFECTED ( ch, AFF_POISON ) )
	{ gain /= 4; }

	if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
	{ gain /= 8; }

	if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
	{ gain /= 2 ; }

	return UMIN ( gain, ch->max_mana - ch->mana );
}



int move_gain ( Creature *ch )
{
	int gain;

	if ( ch->in_room == NULL )
	{ return 0; }

	if ( IS_NPC ( ch ) ) {
		gain = ch->level;
	} else {
		gain = UMAX ( 15, ch->level );

		switch ( ch->position ) {
			case POS_SLEEPING:
				gain += get_curr_stat ( ch, STAT_DEX );
				break;
			case POS_RESTING:
				gain += get_curr_stat ( ch, STAT_DEX ) / 2;
				break;
		}

		if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
		{ gain /= 2; }

		if ( ch->pcdata->condition[COND_THIRST] == 0 )
		{ gain /= 2; }
	}

	gain = gain * ch->in_room->heal_rate / 100;

	if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
	{ gain = gain * ch->on->value[3] / 100; }

	if ( IS_AFFECTED ( ch, AFF_POISON ) )
	{ gain /= 4; }

	if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
	{ gain /= 8; }

	if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
	{ gain /= 2 ; }

	return UMIN ( gain, ch->max_move - ch->move );
}

void gain_condition ( Creature *ch, int iCond, int value )
{
	int condition;

	if ( value == 0 || IS_NPC ( ch ) || ch->level >= LEVEL_IMMORTAL )
	{ return; }

	condition				= ch->pcdata->condition[iCond];
	if ( condition == -1 )
	{ return; }
	ch->pcdata->condition[iCond]	= URANGE ( 0, condition + value, 48 );

	if ( ch->pcdata->condition[iCond] == 0 ) {
		switch ( iCond ) {
			case COND_HUNGER:
				writeBuffer ( "You are hungry.\n\r",  ch );
				break;

			case COND_THIRST:
				writeBuffer ( "You are thirsty.\n\r", ch );
				break;

			case COND_DRUNK:
				if ( condition != 0 )
				{ writeBuffer ( "You are sober.\n\r", ch ); }
				break;
		}
	}

	return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update ( void )
{
	Creature *ch;
	Creature *ch_next;
	Exit *pexit;
	int door;
	static int mLastDay = 0;
	
	time_t cd = time(0);
	
	/* Examine all mobs. */
	for ( ch = char_list; ch != NULL; ch = ch_next ) {
		ch_next = ch->next;

		if ( !IS_NPC ( ch ) || ch->in_room == NULL || IS_AFFECTED ( ch, AFF_CHARM ) )
		{ continue; }

		if ( ch->in_room->area->empty && !IS_SET ( ch->act, ACT_UPDATE_ALWAYS ) )
		{ continue; }

		/* Examine call for special procedure */
		if ( ch->spec_fun != 0 ) {
			if ( ( *ch->spec_fun ) ( ch ) )
			{ continue; }
		}

		if ( ch->pIndexData->pShop != NULL ) { /* give him some gold */
			if ( ( ch->gold * 100 + ch->silver ) < ch->pIndexData->wealth ) {
				log_hd ( LOG_DEBUG, Format ( "Restoring SHOP wealth's for shop owner: %s.", ch->name ) );
				if(mLastDay != cd->tm_yday) {
					mLastDay = cd->tm_yday; // -- assign this now so it doesn't effect every single NPC
								// -- to follow.
								
					// -- notify our twitter that we have replenished our stocks.
					tweetStatement(Format("Vendors have replenished their stock!"));
					// -- insert check here to add random items to certain shops and strip old ones
					Creature *ps, *ps_n;
					
					log_hd(LOG_BASIC, "Replenishing shops with Random Gear");
					// -- search for shops to update and outfit with new random items
					// -- if so inclined.
					for(ps = char_list; ps; ps = ps_n) {
						ps_n = ps->next;
						if(IS_NPC(ps)) {
							if(ps->pMobIndex && ps->pMobIndex->pShop) {
								Item *i, *in;
								for(i = ps->carrying; i; i = in) {
									in = i->next_content;
									
									// -- remove old random items.
									if(i->pIndexData && (i->pIndexData->vnum == OBJ_VNUM_RANDOM_LIGHT ||
									i->pIndexData->vnum == OBJ_VNUM_RANDOM_ARMOR ||
									i->pIndexData->vnum == OBJ_VNUM_RANDOM_WEAPON)) {
										obj_from_char(i);
										extract_obj(i);
									}
								}
								// -- generate new random items for this NPC
								random_shop(ps);
							} // -- end shop-check on ps
						} // -- end npc check on ps
					} // -- end ps for-loop
				} // -- end mLastDay check
				ch->gold += ch->pIndexData->wealth * Math::instance().range ( 1, 20 ) / 5000000;
				ch->silver += ch->pIndexData->wealth * Math::instance().range ( 1, 20 ) / 50000;
			}
		}

		/*
		 * Check triggers only if mobile still in default position
		 */
		if ( ch->position == ch->pIndexData->default_pos ) {
			/* Delay */
			if ( HAS_TRIGGER ( ch, TRIG_DELAY )
					&&   ch->mprog_delay > 0 ) {
				if ( --ch->mprog_delay <= 0 ) {
					mp_percent_trigger ( ch, NULL, NULL, NULL, TRIG_DELAY );
					continue;
				}
			}
			if ( HAS_TRIGGER ( ch, TRIG_RANDOM ) ) {
				if ( mp_percent_trigger ( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
				{ continue; }
			}
		}

		/* That's all for sleeping / busy monster, and empty zones */
		if ( ch->position != POS_STANDING )
		{ continue; }

		/* Scavenge */
		if ( IS_SET ( ch->act, ACT_SCAVENGER )
				&&   ch->in_room->contents != NULL
				&&   Math::instance().bits ( 6 ) == 0 ) {
			Item *obj;
			Item *obj_best;
			int max;

			max         = 1;
			obj_best    = 0;
			for ( obj = ch->in_room->contents; obj; obj = obj->next_content ) {
				if ( CAN_WEAR ( obj, ITEM_TAKE ) && can_loot ( ch, obj )
						&& obj->cost > max  && obj->cost > 0 ) {
					obj_best    = obj;
					max         = obj->cost;
				}
			}

			if ( obj_best ) {
				obj_from_room ( obj_best );
				obj_to_char ( obj_best, ch );
				act ( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
			}
		}

		/* Wander */
		if ( !IS_SET ( ch->act, ACT_SENTINEL )
				&& Math::instance().bits ( 3 ) == 0
				&& ( door = Math::instance().bits ( 5 ) ) <= 5
				&& ( pexit = ch->in_room->exit[door] ) != NULL
				&&   pexit->u1.to_room != NULL
				&&   !IS_SET ( pexit->exit_info, EX_CLOSED )
				&&   !IS_SET ( pexit->u1.to_room->room_flags, ROOM_NO_MOB )
				&& ( !IS_SET ( ch->act, ACT_STAY_AREA )
					 ||   pexit->u1.to_room->area == ch->in_room->area )
				&& ( !IS_SET ( ch->act, ACT_OUTDOORS )
					 ||   !IS_SET ( pexit->u1.to_room->room_flags, ROOM_INDOORS ) )
				&& ( !IS_SET ( ch->act, ACT_INDOORS )
					 ||   IS_SET ( pexit->u1.to_room->room_flags, ROOM_INDOORS ) ) ) {
			move_char ( ch, door, FALSE );
		}
	}

	return;
}

void announceNightFall() {
	switch(Math::instance().range(0,3)) {
		default:
		case 0:
			tweetStatement("Night has fallen on the Infected City");
			break;
		case 1:
			tweetStatement("Night Fall: Shriekers have been seen within the Infected City");
			break;
		case 2:
			tweetStatement("Darkness has fallen upon The Infected City");
			break;
		case 3:
			tweetStatement("#warning night creatures have begun to stir!");
			break;
	}
	// -- remove day-only mobs
	
	// -- place night mobs
}

void announceDayLight() {
	switch(Math::instance().range(0,3)) {
		default:
		case 0:
			tweetStatement(Format("The day has begun a new within The Infected City once more"));
			break;
		case 1:
			tweetStatement("Day light: Shriekers have retreated to the Darkness");
			break;
		case 2:
			tweetStatement("Day break has occurred, night creatures have gone to rest");
			break;
		case 3:
			tweetStatement("#fearnot the night creatures have returned home");
			break;
	}
	// -- remove night mobs
	
	// -- place day-only mobs
}

/*
 * Update the weather.
 */
void weather_update ( void )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;
	int diff;
	buf[0] = '\0';

	switch ( ++time_info.hour ) {
		case  5:
			weather_info.sunlight = SUN_LIGHT;
			strcat ( buf, "The day has begun.\n\r" );
			announceDayLight();
			break;

		case  6:
			weather_info.sunlight = SUN_RISE;
			strcat ( buf, "The sun rises in the east.\n\r" );
			break;

		case 19:
			weather_info.sunlight = SUN_SET;
			strcat ( buf, "The sun slowly disappears in the west.\n\r" );
			break;

		case 20:
			weather_info.sunlight = SUN_DARK;
			strcat ( buf, "The night has begun.\n\r" );
			announceNightFall();
			break;

		case 24:
			time_info.hour = 0;
			time_info.day++;
			break;
	}

	if ( time_info.day   >= 35 ) {
		time_info.day = 0;
		time_info.month++;
	}

	if ( time_info.month >= 17 ) {
		time_info.month = 0;
		time_info.year++;
	}

	/*
	 * Weather change.
	 */
	if ( time_info.month >= 9 && time_info.month <= 16 )
	{ diff = weather_info.mmhg >  985 ? -2 : 2; }
	else
	{ diff = weather_info.mmhg > 1015 ? -2 : 2; }

	weather_info.change   += diff * Math::instance().dice ( 1, 4 ) + Math::instance().dice ( 2, 6 ) - Math::instance().dice ( 2, 6 );
	weather_info.change    = UMAX ( weather_info.change, -12 );
	weather_info.change    = UMIN ( weather_info.change,  12 );

	weather_info.mmhg += weather_info.change;
	weather_info.mmhg  = UMAX ( weather_info.mmhg,  960 );
	weather_info.mmhg  = UMIN ( weather_info.mmhg, 1040 );

	switch ( weather_info.sky ) {
		default:
			log_hd ( LOG_ERROR, Format ( "Weather_update: bad sky %d.", weather_info.sky ) );
			weather_info.sky = SKY_CLOUDLESS;
			break;

		case SKY_CLOUDLESS:
			if ( weather_info.mmhg <  990
					|| ( weather_info.mmhg < 1010 && Math::instance().bits ( 2 ) == 0 ) ) {
				strcat ( buf, "The sky is getting cloudy.\n\r" );
				weather_info.sky = SKY_CLOUDY;
			}
			break;

		case SKY_CLOUDY:
			if ( weather_info.mmhg <  970
					|| ( weather_info.mmhg <  990 && Math::instance().bits ( 2 ) == 0 ) ) {
				strcat ( buf, "It starts to rain.\n\r" );
				weather_info.sky = SKY_RAINING;
			}

			if ( weather_info.mmhg > 1030 && Math::instance().bits ( 2 ) == 0 ) {
				strcat ( buf, "The clouds disappear.\n\r" );
				weather_info.sky = SKY_CLOUDLESS;
			}
			break;

		case SKY_RAINING:
			if ( weather_info.mmhg <  970 && Math::instance().bits ( 2 ) == 0 ) {
				strcat ( buf, "Lightning flashes in the sky.\n\r" );
				weather_info.sky = SKY_LIGHTNING;
			}

			if ( weather_info.mmhg > 1030
					|| ( weather_info.mmhg > 1010 && Math::instance().bits ( 2 ) == 0 ) ) {
				strcat ( buf, "The rain stopped.\n\r" );
				weather_info.sky = SKY_CLOUDY;
			}
			break;

		case SKY_LIGHTNING:
			if ( weather_info.mmhg > 1010
					|| ( weather_info.mmhg >  990 && Math::instance().bits ( 2 ) == 0 ) ) {
				strcat ( buf, "The lightning has stopped.\n\r" );
				weather_info.sky = SKY_RAINING;
				break;
			}
			break;
	}

	if ( buf[0] != '\0' ) {
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->connected == CON_PLAYING
					&&   IS_OUTSIDE ( d->character )
					&&   IS_AWAKE ( d->character ) )
			{ writeBuffer ( buf, d->character ); }
		}
	}

	return;
}



/*
 * Update all chars, including mobs.
*/
void char_update ( void )
{
	Creature *ch;
	Creature *ch_next;
	Creature *ch_quit;

	ch_quit	= NULL;

	/* update save counter */
	save_number++;

	if ( save_number > 29 )
	{ save_number = 0; }

	for ( ch = char_list; ch != NULL; ch = ch_next ) {
		Affect *paf;
		Affect *paf_next;

		ch_next = ch->next;

		if ( ch->timer > 30 )
		{ ch_quit = ch; }

		if ( ch->position >= POS_STUNNED ) {
			/* check to see if we need to go home */
			if ( IS_NPC ( ch ) && ch->zone != NULL && ch->zone != ch->in_room->area
					&& ch->desc == NULL &&  ch->fighting == NULL
					&& !IS_AFFECTED ( ch, AFF_CHARM ) && Math::instance().percent() < 5 ) {
				act ( "$n has wandered home.", ch, NULL, NULL, TO_ROOM );
				log_hd ( LOG_DEBUG, Format ( "NPC: %s added to extraction queue, (away from home too long)", ch->name ) );
				extract_char ( ch, TRUE );
				continue;
			}

			if ( ch->hit  < ch->max_hit )
			{ ch->hit  += hit_gain ( ch ); }
			else
			{ ch->hit = ch->max_hit; }

			if ( ch->mana < ch->max_mana )
			{ ch->mana += mana_gain ( ch ); }
			else
			{ ch->mana = ch->max_mana; }

			if ( ch->move < ch->max_move )
			{ ch->move += move_gain ( ch ); }
			else
			{ ch->move = ch->max_move; }
		}

		if ( ch->position == POS_STUNNED )
		{ update_pos ( ch ); }

		if ( !IS_NPC ( ch ) && ch->level < LEVEL_IMMORTAL ) {
			Item *obj;

			if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL
					&&   obj->item_type == ITEM_LIGHT
					&&   obj->value[2] > 0 ) {
				if ( --obj->value[2] == 0 && ch->in_room != NULL ) {
					--ch->in_room->light;
					act ( "$p goes out.", ch, obj, NULL, TO_ROOM );
					act ( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
					log_hd ( LOG_DEBUG, Format ( "ITEM: %s has flickered out into object extraction queue.", obj->name ) );
					extract_obj ( obj );
				} else if ( obj->value[2] <= 5 && ch->in_room != NULL )
				{ act ( "$p flickers.", ch, obj, NULL, TO_CHAR ); }
			}

			if ( IsStaff ( ch ) )
			{ ch->timer = 0; }

			if ( ++ch->timer >= 12 ) {
				if ( ch->was_in_room == NULL && ch->in_room != NULL ) {
					ch->was_in_room = ch->in_room;
					if ( ch->fighting != NULL )
					{ stop_fighting ( ch, TRUE ); }
					act ( "$n crumbles into dust.",
						  ch, NULL, NULL, TO_ROOM );
					writeBuffer ( "You crumble into dust.\n\r", ch );
					if ( ch->level > 1 )
					{ save_char_obj ( ch ); }
					log_hd ( LOG_SECURITY, Format ( "PLAYER: %s has idled and has been moved to safety in room %d", ch->name, ROOM_VNUM_LIMBO ) );
					char_from_room ( ch );
					char_to_room ( ch, get_room_index ( ROOM_VNUM_LIMBO ) );
				}
			}

			gain_condition ( ch, COND_DRUNK,  -1 );
			gain_condition ( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
			gain_condition ( ch, COND_THIRST, -1 );
			gain_condition ( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1 );
		}

		for ( paf = ch->affected; paf != NULL; paf = paf_next ) {
			paf_next	= paf->next;
			if ( paf->duration > 0 ) {
				paf->duration--;
				if ( Math::instance().range ( 0, 4 ) == 0 && paf->level > 0 )
				{ paf->level--; }  /* spell strength fades with time */
			} else if ( paf->duration < 0 )
				;
			else {
				if ( paf_next == NULL
						||   paf_next->type != paf->type
						||   paf_next->duration > 0 ) {
					if ( paf->type > 0 && skill_table[paf->type].msg_off ) {
						writeBuffer ( skill_table[paf->type].msg_off, ch );
						writeBuffer ( "\n\r", ch );
					}
				}

				affect_remove ( ch, paf );
			}
		}

		/*
		 * Careful with the damages here,
		 *   MUST NOT refer to ch after damage taken,
		 *   as it may be lethal damage (on NPC).
		 */

		if ( is_affected ( ch, gsn_plague ) && ch != NULL ) {
			Affect *af, plague;
			Creature *vch;
			int dam;

			if ( ch->in_room == NULL )
			{ continue; }

			act ( "$n writhes in agony as plague sores erupt from $s skin.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You writhe in agony from the plague.\n\r", ch );
			for ( af = ch->affected; af != NULL; af = af->next ) {
				if ( af->type == gsn_plague )
				{ break; }
			}

			if ( af == NULL ) {
				REMOVE_BIT ( ch->affected_by, AFF_PLAGUE );
				continue;
			}

			if ( af->level == 1 )
			{ continue; }

			plague.where		= TO_AFFECTS;
			plague.type 		= gsn_plague;
			plague.level 		= af->level - 1;
			plague.duration 	= Math::instance().range ( 1, 2 * plague.level );
			plague.location		= APPLY_STR;
			plague.modifier 	= -5;
			plague.bitvector 	= AFF_PLAGUE;

			for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room ) {
				if ( !saves_spell ( plague.level - 2, vch, DAM_DISEASE )
						&&  !IsStaff ( vch )
						&&  !IS_AFFECTED ( vch, AFF_PLAGUE ) && Math::instance().bits ( 4 ) == 0 ) {
					writeBuffer ( "You feel hot and feverish.\n\r", vch );
					act ( "$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM );
					log_hd ( LOG_DEBUG, Format ( "AFFECT: Plague has been spread from %s to %s.", ch->name, vch->name ) );
					affect_join ( vch, &plague );
				}
			}

			dam = UMIN ( ch->level, af->level / 5 + 1 );
			ch->mana -= dam;
			ch->move -= dam;
			damage ( ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE );
		} else if ( IS_AFFECTED ( ch, AFF_POISON ) && ch != NULL
					&&   !IS_AFFECTED ( ch, AFF_SLOW ) )

		{
			Affect *poison;

			poison = affect_find ( ch->affected, gsn_poison );

			if ( poison != NULL ) {
				act ( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
				writeBuffer ( "You shiver and suffer.\n\r", ch );
				log_hd ( LOG_DEBUG, Format ( "AFFECT: %s has taken damage from poison.", ch->name ) );
				damage ( ch, ch, poison->level / 10 + 1, gsn_poison, DAM_POISON, FALSE );
			}
		}

		else if ( ch->position == POS_INCAP && Math::instance().range ( 0, 1 ) == 0 ) {
			log_hd ( LOG_DEBUG, Format ( "POSITION: %s has taken damage from being incapacitated.", ch->name ) );
			damage ( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE );
		} else if ( ch->position == POS_MORTAL ) {
			log_hd ( LOG_DEBUG, Format ( "POSITION: %s has taken damage from being mortally wounded.", ch->name ) );
			damage ( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE );
		}
	}

	/*
	 * Autosave and autoquit.
	 * Check that these chars still exist.
	 */
	for ( ch = char_list; ch != NULL; ch = ch_next ) {
		ch_next = ch->next;

		if ( ch->desc != NULL && ch->desc->descriptor % 30 == save_number ) {
			log_hd ( LOG_DEBUG, Format ( "AutoSave: Autosaving %s to disk!", ch->name ) );
			save_char_obj ( ch );
		}

		if ( ch == ch_quit ) {
			log_hd ( LOG_DEBUG, Format ( "AutoQuit: %s has been autoquit.", ch->name ) );
			cmd_function ( ch, &cmd_quit, "" );
		}
	}

	return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update ( void )
{
	Item *obj;
	Item *obj_next;
	Affect *paf, *paf_next;

	for ( obj = object_list; obj != NULL; obj = obj_next ) {
		Creature *rch;
		const char *message;

		obj_next = obj->next;

		/* go through affects and decrement */
		for ( paf = obj->affected; paf != NULL; paf = paf_next ) {
			paf_next    = paf->next;
			if ( paf->duration > 0 ) {
				paf->duration--;
				if ( Math::instance().range ( 0, 4 ) == 0 && paf->level > 0 )
				{ paf->level--; }  /* spell strength fades with time */
			} else if ( paf->duration < 0 )
				;
			else {
				if ( paf_next == NULL
						||   paf_next->type != paf->type
						||   paf_next->duration > 0 ) {
					if ( paf->type > 0 && skill_table[paf->type].msg_obj ) {
						if ( obj->carried_by != NULL ) {
							rch = obj->carried_by;
							act ( skill_table[paf->type].msg_obj,
								  rch, obj, NULL, TO_CHAR );
						}
						if ( obj->in_room != NULL
								&& obj->in_room->people != NULL ) {
							rch = obj->in_room->people;
							act ( skill_table[paf->type].msg_obj,
								  rch, obj, NULL, TO_ALL );
						}
					}
				}

				affect_remove_obj ( obj, paf );
			}
		}


		if ( obj->timer <= 0 || --obj->timer > 0 )
		{ continue; }

		switch ( obj->item_type ) {
			default:
				message = "$p crumbles into dust.";
				break;
			case ITEM_FOUNTAIN:
				message = "$p dries up.";
				break;
			case ITEM_CORPSE_NPC:
				message = "$p decays into dust.";
				break;
			case ITEM_CORPSE_PC:
				message = "$p decays into dust.";
				break;
			case ITEM_FOOD:
				message = "$p decomposes.";
				break;
			case ITEM_POTION:
				message = "$p has evaporated from disuse.";
				break;
			case ITEM_PORTAL:
				message = "$p fades out of existence.";
				break;
			case ITEM_CONTAINER:
				if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) )
					if ( obj->contains )
						message =
							"$p flickers and vanishes, spilling its contents on the floor.";
					else
					{ message = "$p flickers and vanishes."; }
				else
				{ message = "$p crumbles into dust."; }
				break;
		}

		if ( obj->carried_by != NULL ) {
			if ( IS_NPC ( obj->carried_by )
					&&  obj->carried_by->pIndexData->pShop != NULL )
			{ obj->carried_by->silver += obj->cost / 5; }
			else {

				act ( message, obj->carried_by, obj, NULL, TO_CHAR );
				if ( obj->wear_loc == WEAR_FLOAT )
				{ act ( message, obj->carried_by, obj, NULL, TO_ROOM ); }
			}
		} else if ( obj->in_room != NULL
					&&      ( rch = obj->in_room->people ) != NULL ) {
			if ( ! ( obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
					 && !CAN_WEAR ( obj->in_obj, ITEM_TAKE ) ) ) {
				act ( message, rch, obj, NULL, TO_ROOM );
				act ( message, rch, obj, NULL, TO_CHAR );
			}
		}

		if ( ( obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT )
				&&  obj->contains ) {
			/* save the contents */
			Item *t_obj, *next_obj;

			for ( t_obj = obj->contains; t_obj != NULL; t_obj = next_obj ) {
				next_obj = t_obj->next_content;
				obj_from_obj ( t_obj );

				if ( obj->in_obj ) /* in another object */
				{ obj_to_obj ( t_obj, obj->in_obj ); }

				else if ( obj->carried_by ) /* carried */
					if ( obj->wear_loc == WEAR_FLOAT )
						if ( obj->carried_by->in_room == NULL )
						{ extract_obj ( t_obj ); }
						else
						{ obj_to_room ( t_obj, obj->carried_by->in_room ); }
					else
					{ obj_to_char ( t_obj, obj->carried_by ); }

				else if ( obj->in_room == NULL ) /* destroy it */
				{ extract_obj ( t_obj ); }

				else /* to a room */
				{ obj_to_room ( t_obj, obj->in_room ); }
			}
		}

		log_hd ( LOG_DEBUG, Format ( "ITEM: %s has entered into the extraction queue", obj->name ) );
		extract_obj ( obj );
	}

	return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update ( void )
{
	Creature *wch, *wch_next;
	Creature *ch, *ch_next;
	Creature *vch, *vch_next;
	Creature *victim;

	for ( wch = char_list; wch != NULL; wch = wch_next ) {
		wch_next = wch->next;
		if ( IS_NPC ( wch )
				||   wch->level >= LEVEL_IMMORTAL
				||   wch->in_room == NULL
				||   wch->in_room->area->empty )
		{ continue; }

		for ( ch = wch->in_room->people; ch != NULL; ch = ch_next ) {
			int count;

			ch_next	= ch->next_in_room;

			if ( !IS_NPC ( ch )
					||   !IS_SET ( ch->act, ACT_AGGRESSIVE )
					||   IS_SET ( ch->in_room->room_flags, ROOM_SAFE )
					||   IS_AFFECTED ( ch, AFF_CALM )
					||   ch->fighting != NULL
					||   IS_AFFECTED ( ch, AFF_CHARM )
					||   !IS_AWAKE ( ch )
					||   ( IS_SET ( ch->act, ACT_WIMPY ) && IS_AWAKE ( wch ) )
					||   !can_see ( ch, wch )
					||   Math::instance().bits ( 1 ) == 0 )
			{ continue; }

			/*
			 * Ok we have a 'wch' player character and a 'ch' npc aggressor.
			 * Now make the aggressor fight a RANDOM pc victim in the room,
			 *   giving each 'vch' an equal chance of selection.
			 */
			count	= 0;
			victim	= NULL;
			for ( vch = wch->in_room->people; vch != NULL; vch = vch_next ) {
				vch_next = vch->next_in_room;

				if ( !IS_NPC ( vch )
						&&   vch->level < LEVEL_IMMORTAL
						&&   ch->level >= vch->level - 5
						&&   ( !IS_SET ( ch->act, ACT_WIMPY ) || !IS_AWAKE ( vch ) )
						&&   can_see ( ch, vch ) ) {
					if ( Math::instance().range ( 0, count ) == 0 )
					{ victim = vch; }
					count++;
				}
			}

			if ( victim == NULL )
			{ continue; }

			log_hd ( LOG_DEBUG, Format ( "AGGRESSION: %s has attacked %s", ch->name, victim->name ) );
			multi_hit ( ch, victim, TYPE_UNDEFINED );
		}
	}

	return;
}

void msdp_update ( void )
{
	Socket *d;
	int PlayerCount = 0;

	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->character && d->connected == CON_PLAYING && !IS_NPC ( d->character ) ) {
			char buf[MAX_STRING_LENGTH];
			Creature *pOpponent = d->character->fighting;
			RoomData *pRoom = d->character->in_room;
			Affect *paf;

			++PlayerCount;

			MSDPSetString ( d, eMSDP_CHARACTER_NAME, d->character->name );
			MSDPSetNumber ( d, eMSDP_ALIGNMENT, d->character->alignment );
			MSDPSetNumber ( d, eMSDP_EXPERIENCE, d->character->exp );
			MSDPSetNumber ( d, eMSDP_EXPERIENCE_MAX, exp_per_level ( d->character,
							d->character->pcdata->points )  );
			MSDPSetNumber ( d, eMSDP_EXPERIENCE_TNL, ( ( d->character->level + 1 ) *
							exp_per_level ( d->character, d->character->pcdata->points ) -
							d->character->exp ) );

			MSDPSetNumber ( d, eMSDP_HEALTH, d->character->hit );
			MSDPSetNumber ( d, eMSDP_HEALTH_MAX, d->character->max_hit );
			MSDPSetNumber ( d, eMSDP_LEVEL, d->character->level );
			/*
			            MSDPSetNumber( d, eMSDP_RACE, TBD );
			            MSDPSetNumber( d, eMSDP_CLASS, TBD );
			*/
			MSDPSetNumber ( d, eMSDP_MANA, d->character->mana );
			MSDPSetNumber ( d, eMSDP_MANA_MAX, d->character->max_mana );
			MSDPSetNumber ( d, eMSDP_WIMPY, d->character->wimpy );
			MSDPSetNumber ( d, eMSDP_PRACTICE, d->character->practice );
			MSDPSetNumber ( d, eMSDP_MONEY, d->character->gold );
			MSDPSetNumber ( d, eMSDP_MOVEMENT, d->character->move );
			MSDPSetNumber ( d, eMSDP_MOVEMENT_MAX, d->character->max_move );
			MSDPSetNumber ( d, eMSDP_HITROLL, GET_HITROLL ( d->character ) );
			MSDPSetNumber ( d, eMSDP_DAMROLL, GET_DAMROLL ( d->character ) );
			//            MSDPSetNumber( d, eMSDP_AC, GET_AC(d->character) );
			MSDPSetNumber ( d, eMSDP_STR, get_curr_stat ( d->character, STAT_STR ) );
			MSDPSetNumber ( d, eMSDP_INT, get_curr_stat ( d->character, STAT_INT ) );
			MSDPSetNumber ( d, eMSDP_WIS, get_curr_stat ( d->character, STAT_WIS ) );
			MSDPSetNumber ( d, eMSDP_DEX, get_curr_stat ( d->character, STAT_DEX ) );
			MSDPSetNumber ( d, eMSDP_CON, get_curr_stat ( d->character, STAT_CON ) );
			/*
			            MSDPSetNumber( d, eMSDP_STR_PERM, d->character->pcdata->perm_str );
			            MSDPSetNumber( d, eMSDP_INT_PERM, d->character->pcdata->perm_int );
			            MSDPSetNumber( d, eMSDP_WIS_PERM, d->character->pcdata->perm_wis );
			            MSDPSetNumber( d, eMSDP_DEX_PERM, d->character->pcdata->perm_dex );
			            MSDPSetNumber( d, eMSDP_CON_PERM, d->character->pcdata->perm_con );
			*/
			/* This would be better moved elsewhere */
			if ( pOpponent != NULL ) {
				int hit_points = ( pOpponent->hit * 100 ) / pOpponent->max_hit;
				MSDPSetNumber ( d, eMSDP_OPPONENT_HEALTH, hit_points );
				MSDPSetNumber ( d, eMSDP_OPPONENT_HEALTH_MAX, 100 );
				MSDPSetNumber ( d, eMSDP_OPPONENT_LEVEL, pOpponent->level );
				MSDPSetString ( d, eMSDP_OPPONENT_NAME, pOpponent->name );
			} else { /* Clear the values */
				MSDPSetNumber ( d, eMSDP_OPPONENT_HEALTH, 0 );
				MSDPSetNumber ( d, eMSDP_OPPONENT_LEVEL, 0 );
				MSDPSetString ( d, eMSDP_OPPONENT_NAME, "" );
			}

			/* Only update room stuff if they've changed room */
			if ( pRoom && pRoom->vnum != d->pProtocol->pVariables[eMSDP_ROOM_VNUM]->ValueInt ) {
				int i; /* Loop counter */
				buf[0] = '\0';

				for ( i = DIR_NORTH; i < MAX_DIR; ++i ) {
					if ( pRoom->exit[i] != NULL ) {
						const char MsdpVar[] = { ( char ) MSDP_VAR, '\0' };
						const char MsdpVal[] = { ( char ) MSDP_VAL, '\0' };

						strcat ( buf, MsdpVar );
						strcat ( buf, dir_name[i] );
						strcat ( buf, MsdpVal );

						if ( IS_SET ( pRoom->exit[i]->exit_info, EX_CLOSED ) )
						{ strcat ( buf, "C" ); }
						else /* The exit is open */
						{ strcat ( buf, "O" ); }
					}
				}

				if ( pRoom->area != NULL )
				{ MSDPSetString ( d, eMSDP_AREA_NAME, pRoom->area->name ); }

				MSDPSetString ( d, eMSDP_ROOM_NAME, pRoom->name );
				MSDPSetTable ( d, eMSDP_ROOM_EXITS, buf );
				MSDPSetNumber ( d, eMSDP_ROOM_VNUM, pRoom->vnum );
			}
			/*
			            MSDPSetNumber( d, eMSDP_WORLD_TIME, d->character-> );
			*/

			buf[0] = '\0';
			for ( paf = d->character->affected; paf; paf = paf->next ) {
				char skill_buf[MAX_STRING_LENGTH];
				sprintf ( skill_buf, "%c%s%c%d",
						  ( char ) MSDP_VAR, skill_table[paf->type].name,
						  ( char ) MSDP_VAL, paf->duration );
				strcat ( buf, skill_buf );
			}
			MSDPSetTable ( d, eMSDP_AFFECTS, buf );

			MSDPUpdate ( d );
		}
	}

	/* Ideally this should be called once at startup, and again whenever
	 * someone leaves or joins the mud.  But this works, and it keeps the
	 * snippet simple.  Optimise as you see fit.
	 */
	MSSPSetPlayers ( PlayerCount );
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler ( void )
{
	static  int     pulse_area;
	static  int     pulse_mobile;
	static  int     pulse_violence;
	static  int     pulse_point;
	static  int	pulse_music;
	static  int     pulse_msdp;
	static  int     pulse_weather;
	
	if ( --pulse_msdp <= 0 ) {
		// log_hd(LOG_DEBUG, "MSDP update"); // -- disabled due to spam
		pulse_msdp      = PULSE_PER_SECOND;
		msdp_update();
	}

	if ( --pulse_area     <= 0 ) {
		log_hd ( LOG_DEBUG, Format ( "pulse_area = %d; area_update will be initiated.", PULSE_AREA ) );
		pulse_area	= PULSE_AREA;
		/* Math::instance().range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
		area_update	( );
	}

	if ( --pulse_music	  <= 0 ) {
		log_hd ( LOG_DEBUG, Format ( "pulse_music = %d; song_update will be initiated.", PULSE_MUSIC ) );
		pulse_music	= PULSE_MUSIC;
		song_update();
	}

	if ( --pulse_mobile   <= 0 ) {
		log_hd ( LOG_DEBUG, Format ( "pulse_mobile = %d; mobile_update will be initiated.", PULSE_MOBILE ) );
		pulse_mobile	= PULSE_MOBILE;
		mobile_update	( );
	}

	if ( --pulse_violence <= 0 ) {
		// log_hd(LOG_DEBUG, Format("pulse_violence = %d; violence_update will be initiated.", PULSE_VIOLENCE));
		pulse_violence	= PULSE_VIOLENCE;
		violence_update	( );
	}

	/// -- temporary solution until we create an event to handle this.
	if( --pulse_weather <= 0) {
		log_hd(LOG_DEBUG, Format("pulse_weather = %d; weather_update will be initiated.", PULSE_TICK*2));
		pulse_weather = PULSE_TICK*2;
		weather_update();
	}

	if ( --pulse_point    <= 0 ) {
		log_hd ( LOG_DEBUG, Format ( "pulse_tick = %d;  char_update, obj_update will be initiated", PULSE_TICK ) );
		wiznet ( "TICK!", NULL, NULL, WIZ_TICKS, 0, 0 );
		pulse_point     = PULSE_TICK;
		/* Math::instance().range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 ); */
		char_update	( );
		obj_update	( );
	}

	aggr_update( );
	tail_chain( );
	return;
}
