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
void	check_assist	args ( ( Creature *ch, Creature *victim ) );
bool	check_dodge	args ( ( Creature *ch, Creature *victim ) );
void	check_killer	args ( ( Creature *ch, Creature *victim ) );
bool	check_parry	args ( ( Creature *ch, Creature *victim ) );
bool    check_shield_block     args ( ( Creature *ch, Creature *victim ) );
void    dam_message 	args ( ( Creature *ch, Creature *victim, int dam,
								 int dt, bool immune, int where ) );
void	death_cry	args ( ( Creature *ch ) );
void	group_gain	args ( ( Creature *ch, Creature *victim ) );
int	xp_compute	args ( ( Creature *gch, Creature *victim,
						 int total_levels ) );
bool	is_safe		args ( ( Creature *ch, Creature *victim ) );
void	make_corpse	args ( ( Creature *ch ) );
void	one_hit		args ( ( Creature *ch, Creature *victim, int dt ) );
void    mob_hit		args ( ( Creature *ch, Creature *victim, int dt ) );
void	raw_kill	args ( ( Creature *victim ) );
void	set_fighting	args ( ( Creature *ch, Creature *victim ) );
void	disarm		args ( ( Creature *ch, Creature *victim ) );
bool new_damage ( Creature *ch, Creature *victim, int dam, int dt, int dam_type, bool show, int where );

// -- we will add more races here later on when we
// -- are done rebuilding our race_table
bool is_human ( Creature *ch )
{
	if ( !IS_NPC ( ch ) ) { return FALSE; }

	if ( ch->race == race_lookup ( "human" ) )
	{ return TRUE; }

	return FALSE;
}


const struct wear_data wear_info[] = {
	//      partname	ispart	%hit	wear flag	item flag 		required part	AC?	superceded by
	//   { "none",		FALSE,	0,	WEAR_NONE,	0,			0,		FALSE,	0		},
	{ "light",		FALSE,	0,	WEAR_LIGHT,	ITEM_HOLD,		PART_HANDS,	FALSE,	0 		},
	{ "left finger", 	TRUE,	3,	WEAR_FINGER_L, 	ITEM_WEAR_FINGER,	PART_FINGERS,	FALSE,	WEAR_HANDS  	},
	{ "right finger",	TRUE,	4,	WEAR_FINGER_R, 	ITEM_WEAR_FINGER,	PART_FINGERS,	FALSE,	WEAR_HANDS  	},
	{ "neck",		TRUE,	3,	WEAR_NECK_1, 	ITEM_WEAR_NECK,		PART_HEAD,	TRUE,	WEAR_NECK_2 	},
	{ "neck",		TRUE,	0,	WEAR_NECK_2,	ITEM_WEAR_NECK,		PART_HEAD,	TRUE,	0	 	},
	{ "body",		TRUE,	20,	WEAR_BODY,	ITEM_WEAR_BODY,		0,		TRUE,	WEAR_ABOUT 	},
	{ "head",	        TRUE,	10,	WEAR_HEAD,	ITEM_WEAR_HEAD,		PART_HEAD,	TRUE,	0 		},
	{ "legs",		TRUE,	5,	WEAR_LEGS, 	ITEM_WEAR_LEGS,		PART_LEGS,	TRUE,	WEAR_ABOUT 	},
	{ "feet",		TRUE,	2,	WEAR_FEET,	ITEM_WEAR_FEET,		PART_FEET,	TRUE,	0 		},
	{ "hands",		TRUE,	5,	WEAR_HANDS,	ITEM_WEAR_HANDS, 	PART_HANDS,	TRUE,	0 		},
	{ "arms",		TRUE,	25,	WEAR_ARMS,	ITEM_WEAR_ARMS, 	PART_ARMS,	TRUE,	WEAR_ABOUT 	},
	{ "shield",	FALSE,	0,	WEAR_SHIELD,	ITEM_WEAR_SHIELD,	PART_ARMS,	FALSE,	0 		},
	{ "about body",	FALSE,	0,	WEAR_ABOUT,	ITEM_WEAR_ABOUT,	0,		TRUE,	0 		},
	{ "waist",		TRUE,	5,	WEAR_WAIST,	ITEM_WEAR_WAIST,	0,	TRUE,	WEAR_ABOUT 	},
	{ "left wrist",    TRUE,	5,	WEAR_WRIST_L,	ITEM_WEAR_WRIST,	PART_HANDS,	TRUE,	WEAR_HANDS 	},
	{ "right wrist",	TRUE,	5,	WEAR_WRIST_R,	ITEM_WEAR_WRIST,	PART_HANDS,	TRUE,	WEAR_HANDS 	},
	{ "wielded",	FALSE,	0,	WEAR_WIELD,	ITEM_WIELD,		PART_HANDS,	FALSE,  0 		},
	{ "held",		FALSE,	0,	WEAR_HOLD,	ITEM_HOLD,		PART_HANDS,	FALSE,	0 		},
	/*     { "dual wield",	FALSE,	0,	WEAR_WIELD2,	ITEM_WIELD,		PART_HANDS,	FALSE,	0 		},
	     { "pride",		FALSE,	0,	WEAR_PRIDE,	ITEM_WEAR_PRIDE,	0,		FALSE,	0 		},
	     { "face",		TRUE,	5,	WEAR_FACE,	ITEM_WEAR_FACE,		PART_FACE,	TRUE,	0 		},
	     { "ears",		TRUE,	3,	WEAR_EARS,	ITEM_WEAR_EARS,		PART_EAR,	FALSE,	0 		}, */
	{ "float",		FALSE,	0,	WEAR_FLOAT,	ITEM_WEAR_FLOAT,	0,		FALSE,	0 		},
	// Insert New Entries Here and update WEAR_ flags in merc.h
	{ "max wear",	FALSE,	0,	MAX_WEAR,	0,			0,		FALSE,	0		}
};

/*
 * Return a body part to hit - Lotherius
 * For this to work correctly, all the hitpct values in wear_info must add up to 100.
 */
int hpart ( Creature *ch, Creature *vict )
{
	int  i, b, c, count;

	c = 0;
	count = 0;

	while ( 1 ) {
		// -- institute a 5 times chance for a part to be hit.
		// -- ie, we try 5 times to see if something got hit.
		// -- if not, we default to -1, and just say they got hit.
		if ( count > ( MAX_WEAR * 5 ) ) {
			return -1;
		}
		b = Math::instance().percent ( );				// Get a roll
		for ( i = 0; i < MAX_WEAR; i++ ) {
			if ( wear_info[i].ispart == FALSE )			// If it isn't a part, skip
			{ continue; }
			if ( ( wear_info[i].part_req > 0 )			// If the victim doesn't have that part, skip
					&& ( !IS_SET ( vict->parts, wear_info[i].part_req ) ) )
			{ continue; }
			if ( wear_info[i].hitpct == 0 )				// If can't hit this part, skip
			{ continue; }
			c += wear_info[i].hitpct;					// add the percent for this item.
			if ( c >= b )
			{ return i; }
			count++;
		}
	}

	return i;
}


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update ( void )
{
	Creature *ch;
	Creature *ch_next;
	Creature *victim;

	for ( ch = char_list; ch != NULL; ch = ch_next ) {
		ch_next	= ch->next;

		if ( ( victim = FIGHTING ( ch ) ) == NULL || IN_ROOM ( ch ) == NULL )
		{ continue; }

		if ( mPeacefulMode ) {
			stop_fighting ( ch, false );
		}

		if ( IS_AWAKE ( ch ) && IN_ROOM ( ch ) == IN_ROOM ( victim ) )
		{ multi_hit ( ch, victim, TYPE_UNDEFINED ); }
		else
		{ stop_fighting ( ch, FALSE ); }

		if ( ( victim = FIGHTING ( ch ) ) == NULL )
		{ continue; }

		/*
		 * Fun for the whole family!
		 */
		check_assist ( ch, victim );

		if ( IS_NPC ( ch ) ) {
			if ( HAS_TRIGGER ( ch, TRIG_FIGHT ) )
			{ mp_percent_trigger ( ch, victim, NULL, NULL, TRIG_FIGHT ); }
			if ( HAS_TRIGGER ( ch, TRIG_HPCNT ) )
			{ mp_hprct_trigger ( ch, victim ); }
		}
	}

	return;
}

/* for auto assisting */
void check_assist ( Creature *ch, Creature *victim )
{
	Creature *rch, *rch_next;

	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch_next ) {
		rch_next = rch->next_in_room;

		if ( IS_AWAKE ( rch ) && FIGHTING ( rch ) == NULL ) {

			/* quick check for ASSIST_PLAYER */
			if ( !IS_NPC ( ch ) && IS_NPC ( rch )
					&& IS_SET ( rch->off_flags, ASSIST_PLAYERS )
					&&  rch->level + 6 > victim->level ) {
				log_hd ( LOG_DEBUG, Format ( "NPC assisted player: %s has aided %s.", rch->name, ch->name ) );
				cmd_function ( rch, &cmd_emote, "screams and attacks!" );
				multi_hit ( rch, victim, TYPE_UNDEFINED );
				continue;
			}

			/* PCs next */
			if ( !IS_NPC ( ch ) || IS_AFFECTED ( ch, AFF_CHARM ) ) {
				if ( ( ( !IS_NPC ( rch ) && IS_SET ( rch->act, PLR_AUTOASSIST ) )
						||     IS_AFFECTED ( rch, AFF_CHARM ) )
						&&   is_same_group ( ch, rch )
						&&   !is_safe ( rch, victim ) ) {
					log_hd ( LOG_DEBUG, Format ( "PC assisted same group: %s has aided %s", rch->name, ch->name ) );
					multi_hit ( rch, victim, TYPE_UNDEFINED );
				}
				continue;
			}

			/* now check the NPC cases */

			if ( IS_NPC ( ch ) && !IS_AFFECTED ( ch, AFF_CHARM ) ) {
				if ( ( IS_NPC ( rch ) && IS_SET ( rch->off_flags, ASSIST_ALL ) )
						||   ( IS_NPC ( rch ) && rch->group && rch->group == ch->group )
						||   ( IS_NPC ( rch ) && rch->race == ch->race
							   && IS_SET ( rch->off_flags, ASSIST_RACE ) )
						||   ( IS_NPC ( rch ) && IS_SET ( rch->off_flags, ASSIST_ALIGN )
							   &&   ( ( IS_GOOD ( rch )    && IS_GOOD ( ch ) )
									  ||  ( IS_EVIL ( rch )    && IS_EVIL ( ch ) )
									  ||  ( IS_NEUTRAL ( rch ) && IS_NEUTRAL ( ch ) ) ) )
						||   ( rch->pIndexData == ch->pIndexData
							   && IS_SET ( rch->off_flags, ASSIST_VNUM ) ) ) {
					Creature *vch;
					Creature *target;
					int number;

					if ( Math::instance().bits ( 1 ) == 0 )
					{ continue; }

					target = NULL;
					number = 0;
					for ( vch = IN_ROOM ( ch )->people; vch; vch = vch->next ) {
						if ( can_see ( rch, vch )
								&&  is_same_group ( vch, victim )
								&&  Math::instance().range ( 0, number ) == 0 ) {
							target = vch;
							number++;
						}
					}

					if ( target != NULL ) {
						log_hd ( LOG_DEBUG, Format ( "NPC aided NPC: %s has aided %s", rch->name, ch->name ) );
						cmd_function ( rch, &cmd_emote, "screams and attacks!" );
						multi_hit ( rch, target, TYPE_UNDEFINED );
					}
				}
			}
		}
	}
}


/*
 * Do one group of attacks.
 */
void multi_hit ( Creature *ch, Creature *victim, int dt )
{
	int     chance;

	if ( mPeacefulMode ) {
		stop_fighting ( ch, false );
		stop_fighting ( victim, false );
	}

	/* decrement the wait */
	if ( ch->desc == NULL )
	{ ch->wait = UMAX ( 0, ch->wait - PULSE_VIOLENCE ); }

	if ( ch->desc == NULL )
	{ ch->daze = UMAX ( 0, ch->daze - PULSE_VIOLENCE ); }


	/* no attacks for stunnies -- just a check */
	if ( ch->position < POS_RESTING )
	{ return; }

	if ( IS_NPC ( ch ) ) {
		mob_hit ( ch, victim, dt );
		return;
	}

	one_hit ( ch, victim, dt );

	if ( FIGHTING ( ch ) != victim )
	{ return; }

	// -- dazing has some lasting effects.
	if ( ch->daze ) {
		// -- we are dazed, can we make another attack or not?
		if ( Math::instance().range ( 0, 1 ) == Math::instance().range ( 0, 2 ) ) {
			return;
		}
	}

	if ( IS_AFFECTED ( ch, AFF_HASTE ) )
	{ one_hit ( ch, victim, dt ); }

	if ( FIGHTING ( ch ) != victim || dt == gsn_backstab )
	{ return; }

	chance = get_skill ( ch, gsn_second_attack ) / 2;

	if ( IS_AFFECTED ( ch, AFF_SLOW ) )
	{ chance /= 2; }

	if ( Math::instance().percent( ) < chance ) {
		one_hit ( ch, victim, dt );
		check_improve ( ch, gsn_second_attack, TRUE, 5 );
		if ( FIGHTING ( ch ) != victim )
		{ return; }
	}

	chance = get_skill ( ch, gsn_third_attack ) / 4;

	if ( IS_AFFECTED ( ch, AFF_SLOW ) )
	{ chance = 0; };

	if ( Math::instance().percent( ) < chance ) {
		one_hit ( ch, victim, dt );
		check_improve ( ch, gsn_third_attack, TRUE, 6 );
		if ( FIGHTING ( ch ) != victim )
		{ return; }
	}

	return;
}

/* procedure for all mobile attacks */
void mob_hit ( Creature *ch, Creature *victim, int dt )
{
	int chance, number;
	Creature *vch, *vch_next;

	if ( mPeacefulMode ) {
		stop_fighting ( ch, false );
		stop_fighting ( victim, false );
	}


	one_hit ( ch, victim, dt );

	if ( FIGHTING ( ch ) != victim )
	{ return; }

	/* Area attack -- BALLS nasty! */

	if ( IS_SET ( ch->off_flags, OFF_AREA_ATTACK ) ) {
		for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;
			if ( ( vch != victim && FIGHTING ( vch ) == ch ) )
			{ one_hit ( ch, vch, dt ); }
		}
	}

	if ( IS_AFFECTED ( ch, AFF_HASTE )
			||  ( IS_SET ( ch->off_flags, OFF_FAST ) && !IS_AFFECTED ( ch, AFF_SLOW ) ) )
	{ one_hit ( ch, victim, dt ); }

	if ( FIGHTING ( ch ) != victim || dt == gsn_backstab )
	{ return; }

	chance = get_skill ( ch, gsn_second_attack ) / 2;

	if ( IS_AFFECTED ( ch, AFF_SLOW ) && !IS_SET ( ch->off_flags, OFF_FAST ) )
	{ chance /= 2; }

	if ( Math::instance().percent() < chance ) {
		one_hit ( ch, victim, dt );
		if ( FIGHTING ( ch ) != victim )
		{ return; }
	}

	chance = get_skill ( ch, gsn_third_attack ) / 4;

	if ( IS_AFFECTED ( ch, AFF_SLOW ) && !IS_SET ( ch->off_flags, OFF_FAST ) )
	{ chance = 0; }

	if ( Math::instance().percent() < chance ) {
		one_hit ( ch, victim, dt );
		if ( FIGHTING ( ch ) != victim )
		{ return; }
	}

	/* oh boy!  Fun stuff! */

	if ( ch->wait > 0 )
	{ return; }

	number = Math::instance().range ( 0, 2 );

	if ( number == 1 && IS_SET ( ch->act, ACT_MAGE ) ) {
		/*  { mob_cast_mage(ch,victim); return; } */ ;
	}

	if ( number == 2 && IS_SET ( ch->act, ACT_CLERIC ) ) {
		/* { mob_cast_cleric(ch,victim); return; } */ ;
	}

	/* now for the skills */

	number = Math::instance().range ( 0, 8 );

	switch ( number ) {
		case ( 0 ) :
			if ( IS_SET ( ch->off_flags, OFF_BASH ) )
			{ cmd_function ( ch, &cmd_bash, "" ); }
			break;

		case ( 1 ) :
			if ( IS_SET ( ch->off_flags, OFF_BERSERK ) && !IS_AFFECTED ( ch, AFF_BERSERK ) )
			{ cmd_function ( ch, &cmd_berserk, "" ); }
			break;


		case ( 2 ) :
			if ( IS_SET ( ch->off_flags, OFF_DISARM )
					|| ( get_weapon_sn ( ch ) != gsn_hand_to_hand
						 && ( IS_SET ( ch->act, ACT_WARRIOR )
							  ||  IS_SET ( ch->act, ACT_THIEF ) ) ) )
			{ cmd_function ( ch, &cmd_disarm, "" ); }
			break;

		case ( 3 ) :
			if ( IS_SET ( ch->off_flags, OFF_KICK ) )
			{ cmd_function ( ch, &cmd_kick, "" ); }
			break;

		case ( 4 ) :
			if ( IS_SET ( ch->off_flags, OFF_KICK_DIRT ) )
			{ cmd_function ( ch, &cmd_dirt, "" ); }
			break;

		case ( 5 ) :
			if ( IS_SET ( ch->off_flags, OFF_TAIL ) ) {
				/* cmd_function(ch, &cmd_tail, "") */ ;
			}
			break;

		case ( 6 ) :
			if ( IS_SET ( ch->off_flags, OFF_TRIP ) )
			{ cmd_function ( ch, &cmd_trip, "" ); }
			break;

		case ( 7 ) :
			if ( IS_SET ( ch->off_flags, OFF_CRUSH ) ) {
				/* cmd_function(ch, &cmd_crush, "") */ ;
			}
			break;
		case ( 8 ) :
			if ( IS_SET ( ch->off_flags, OFF_BACKSTAB ) ) {
				cmd_function ( ch, &cmd_backstab, "" );
			}
	}
}


/*
 * Hit one guy once.
 */
void one_hit ( Creature *ch, Creature *victim, int dt )
{
	Item *wield;
	int victim_ac;
	int thac0;
	int thac0_00;
	int thac0_32;
	int dam;
	int diceroll;
	int sn, skill;
	int dam_type;
	bool result;

	sn = -1;


	/* just in case */
	if ( victim == ch || ch == NULL || victim == NULL )
	{ return; }

	/*
	 * Can't beat a dead char!
	 * Guard against weird room-leavings.
	 */
	if ( victim->position == POS_DEAD || IN_ROOM ( ch ) != IN_ROOM ( victim ) )
	{ return; }

	/*
	 * Figure out the type of damage message.
	 */
	wield = get_eq_char ( ch, WEAR_WIELD );

	if ( dt == TYPE_UNDEFINED ) {
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
		{ dt += wield->value[3]; }
		else
		{ dt += ch->dam_type; }
	}

	if ( dt < TYPE_HIT ) {
		if ( wield != NULL )
		{ dam_type = attack_table[wield->value[3]].damage; }
		else
		{ dam_type = attack_table[ch->dam_type].damage; }
	} else
	{ dam_type = attack_table[dt - TYPE_HIT].damage; }

	if ( dam_type == -1 )
	{ dam_type = DAM_BASH; }

	/* get the weapon skill */
	sn = get_weapon_sn ( ch );
	skill = 20 + get_weapon_skill ( ch, sn );

	/*
	 * Calculate to-hit-armor-archetype-0 versus armor.
	 */
	if ( IS_NPC ( ch ) ) {
		thac0_00 = 20;
		thac0_32 = -4;   /* as good as a thief */
		if ( IS_SET ( ch->act, ACT_WARRIOR ) )
		{ thac0_32 = -10; }
		else if ( IS_SET ( ch->act, ACT_THIEF ) )
		{ thac0_32 = -4; }
		else if ( IS_SET ( ch->act, ACT_CLERIC ) )
		{ thac0_32 = 2; }
		else if ( IS_SET ( ch->act, ACT_MAGE ) )
		{ thac0_32 = 6; }
	} else {
		thac0_00 = archetype_table[ch->archetype].thac0_00;
		thac0_32 = archetype_table[ch->archetype].thac0_32;
	}
	thac0  = Math::instance().interpolate ( ch->level, thac0_00, thac0_32 );

	if ( thac0 < 0 )
	{ thac0 = thac0 / 2; }

	if ( thac0 < -5 )
	{ thac0 = -5 + ( thac0 + 5 ) / 2; }

	thac0 -= GET_HITROLL ( ch ) * skill / 100;
	thac0 += 5 * ( 100 - skill ) / 100;

	if ( dt == gsn_backstab )
	{ thac0 -= 10 * ( 100 - get_skill ( ch, gsn_backstab ) ); }

	switch ( dam_type ) {
		case ( DAM_PIERCE ) :
			victim_ac = GET_AC ( victim, AC_PIERCE ) / 10;
			break;
		case ( DAM_BASH ) :
			victim_ac = GET_AC ( victim, AC_BASH ) / 10;
			break;
		case ( DAM_SLASH ) :
			victim_ac = GET_AC ( victim, AC_SLASH ) / 10;
			break;
		default:
			victim_ac = GET_AC ( victim, AC_EXOTIC ) / 10;
			break;
	};

	if ( victim_ac < -15 )
	{ victim_ac = ( victim_ac + 15 ) / 5 - 15; }

	if ( !can_see ( ch, victim ) )
	{ victim_ac -= 4; }

	if ( victim->position < POS_FIGHTING )
	{ victim_ac += 4; }

	if ( victim->position < POS_RESTING )
	{ victim_ac += 6; }

	/*
	 * The moment of excitement!
	 */
	while ( ( diceroll = Math::instance().bits ( 5 ) ) >= 20 )
		;

	if ( diceroll == 0
			|| ( diceroll != 19 && diceroll < thac0 - victim_ac ) ) {
		/* Miss. */
		damage ( ch, victim, 0, dt, dam_type, TRUE );
		tail_chain( );
		return;
	}

	/*
	 * Hit.
	 * Calc damage.
	 */
	if ( IS_NPC ( ch ) && ( !ch->pIndexData->new_format || wield == NULL ) ) {
		if ( !ch->pIndexData->new_format ) {
			dam = Math::instance().range ( ch->level / 2, ch->level * 3 / 2 );
			if ( wield != NULL )
			{ dam += dam / 2; }
		} else
		{ dam = Math::instance().dice ( ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE] ); }
	} else {
		if ( sn != -1 )
		{ check_improve ( ch, sn, TRUE, 5 ); }
		if ( wield != NULL ) {
			if ( wield->pIndexData->new_format )
			{ dam = Math::instance().dice ( wield->value[1], wield->value[2] ) * skill / 100; }
			else
				dam = Math::instance().range ( wield->value[1] * skill / 100,
											   wield->value[2] * skill / 100 );

			if ( get_eq_char ( ch, WEAR_SHIELD ) == NULL ) /* no shield = more */
			{ dam = dam * 11 / 10; }

			/* sharpness! */
			if ( IS_WEAPON_STAT ( wield, WEAPON_SHARP ) ) {
				int percent;

				if ( ( percent = Math::instance().percent() ) <= ( skill / 8 ) )
				{ dam = 2 * dam + ( dam * 2 * percent / 100 ); }
			}
		} else
		{ dam = Math::instance().range ( 1 + 4 * skill / 100, 2 * ch->level / 3 * skill / 100 ); }
	}

	/*
	 * Bonuses.
	 */
	if ( get_skill ( ch, gsn_enhanced_damage ) > 0 ) {
		diceroll = Math::instance().percent();
		if ( diceroll <= get_skill ( ch, gsn_enhanced_damage ) ) {
			check_improve ( ch, gsn_enhanced_damage, TRUE, 6 );
			dam += 2 * ( dam * diceroll / 300 );
		}
	}

	if ( !IS_AWAKE ( victim ) )
	{ dam *= 2; }
	else if ( victim->position < POS_FIGHTING )
	{ dam = dam * 3 / 2; }

	if ( dt == gsn_backstab && wield != NULL ) {
		if ( wield->value[0] != 2 )
		{ dam *= 2 + ( ch->level / 10 ); }
		else
		{ dam *= 2 + ( ch->level / 8 ); }
	}
	dam += GET_DAMROLL ( ch ) * UMIN ( 100, skill ) / 100;

	if ( dam <= 0 )
	{ dam = 1; }


	// -- what part did we hit?
	result = new_damage ( ch, victim, dam, dt, dam_type, TRUE, hpart ( ch, victim ) );
	if ( Math::instance().range ( 0, 10 ) == Math::instance().range ( 3, 12 ) && !IS_NPC ( ch ) ) {
		gain_exp ( ch, Math::instance().range ( 1, Math::instance().range ( 2, 4 ) ) );
	}

	/* but do we have a funky weapon? */
	if ( result && wield != NULL ) {
		int dam;

		if ( FIGHTING ( ch ) == victim && IS_WEAPON_STAT ( wield, WEAPON_POISON ) ) {
			int level;
			Affect *poison, af;

			if ( ( poison = affect_find ( wield->affected, gsn_poison ) ) == NULL )
			{ level = wield->level; }
			else
			{ level = poison->level; }

			if ( !saves_spell ( level / 2, victim, DAM_POISON ) ) {
				writeBuffer ( "You feel poison coursing through your veins.",
							  victim );
				act ( "$n is poisoned by the venom on $p.",
					  victim, wield, NULL, TO_ROOM );

				af.where     = TO_AFFECTS;
				af.type      = gsn_poison;
				af.level     = level * 3 / 4;
				af.duration  = level / 2;
				af.location  = APPLY_STR;
				af.modifier  = -1;
				af.bitvector = AFF_POISON;
				affect_join ( victim, &af );
			}

			/* weaken the poison if it's temporary */
			if ( poison != NULL ) {
				poison->level = UMAX ( 0, poison->level - 2 );
				poison->duration = UMAX ( 0, poison->duration - 1 );

				if ( poison->level == 0 || poison->duration == 0 )
				{ act ( "The poison on $p has worn off.", ch, wield, NULL, TO_CHAR ); }
			}
		}


		if ( FIGHTING ( ch ) == victim && IS_WEAPON_STAT ( wield, WEAPON_VAMPIRIC ) ) {
			dam = Math::instance().range ( 1, wield->level / 5 + 1 );
			act ( "$p draws life from $n.", victim, wield, NULL, TO_ROOM );
			act ( "You feel $p drawing your life away.",
				  victim, wield, NULL, TO_CHAR );
			damage ( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			ch->alignment = UMAX ( -1000, ch->alignment - 1 );
			ch->hit += dam / 2;
		}

		if ( FIGHTING ( ch ) == victim && IS_WEAPON_STAT ( wield, WEAPON_FLAMING ) ) {
			dam = Math::instance().range ( 1, wield->level / 4 + 1 );
			act ( "$n is burned by $p.", victim, wield, NULL, TO_ROOM );
			act ( "$p sears your flesh.", victim, wield, NULL, TO_CHAR );
			fire_effect ( ( void * ) victim, wield->level / 2, dam, TARGET_CHAR );
			damage ( ch, victim, dam, 0, DAM_FIRE, FALSE );
		}

		if ( FIGHTING ( ch ) == victim && IS_WEAPON_STAT ( wield, WEAPON_FROST ) ) {
			dam = Math::instance().range ( 1, wield->level / 6 + 2 );
			act ( "$p freezes $n.", victim, wield, NULL, TO_ROOM );
			act ( "The cold touch of $p surrounds you with ice.",
				  victim, wield, NULL, TO_CHAR );
			cold_effect ( victim, wield->level / 2, dam, TARGET_CHAR );
			damage ( ch, victim, dam, 0, DAM_COLD, FALSE );
		}

		if ( FIGHTING ( ch ) == victim && IS_WEAPON_STAT ( wield, WEAPON_SHOCKING ) ) {
			dam = Math::instance().range ( 1, wield->level / 5 + 2 );
			act ( "$n is struck by lightning from $p.", victim, wield, NULL, TO_ROOM );
			act ( "You are shocked by $p.", victim, wield, NULL, TO_CHAR );
			shock_effect ( victim, wield->level / 2, dam, TARGET_CHAR );
			damage ( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
		}
	}
	tail_chain( );
	return;
}


// -- Relic code, we preserved this old damage call to ease the transformation to our
// -- new system.  Essentially, we do not require this, but we want it to ensure we work properly.
// -- also spells do not attack body parts.
bool damage ( Creature *ch, Creature *victim, int dam, int dt, int dam_type, bool show )
{
	return new_damage ( ch, victim, dam, dt, dam_type, show, -1 );
}

bool new_damage ( Creature *ch, Creature *victim, int dam, int dt, int dam_type, bool show, int where )
{
	Item *corpse;
	bool immune;

	if ( victim->position == POS_DEAD )
	{ return FALSE; }

	/*
	 * Stop up any residual loopholes.
	 */
	if ( dam > 1200 && dt >= TYPE_HIT ) {
		log_hd ( LOG_ERROR, Format ( "Damage: %d: more than 1200 points!", dam ) );
		dam = 1200;
		if ( !IsStaff ( ch ) ) {
			Item *obj;
			obj = get_eq_char ( ch, WEAR_WIELD );
			writeBuffer ( "You really shouldn't cheat.\n\r", ch );
			if ( obj != NULL )
			{ extract_obj ( obj ); }
		}
	}


	/* damage reduction */
	if ( dam > 35 )
	{ dam = ( dam - 35 ) / 2 + 35; }
	if ( dam > 80 )
	{ dam = ( dam - 80 ) / 2 + 80; }




	if ( victim != ch ) {
		/*
		 * Certain attacks are forbidden.
		 * Most other attacks are returned.
		 */
		if ( is_safe ( ch, victim ) )
		{ return FALSE; }
		check_killer ( ch, victim );

		if ( victim->position > POS_STUNNED ) {
			if ( FIGHTING ( victim ) == NULL ) {
				set_fighting ( victim, ch );
				if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_KILL ) )
				{ mp_percent_trigger ( victim, ch, NULL, NULL, TRIG_KILL ); }
			}
			if ( victim->timer <= 4 )
			{ victim->position = POS_FIGHTING; }
		}

		if ( victim->position > POS_STUNNED ) {
			if ( FIGHTING ( ch ) == NULL )
			{ set_fighting ( ch, victim ); }
		}

		/*
		 * More charm stuff.
		 */
		if ( victim->master == ch )
		{ stop_follower ( victim ); }
	}

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED ( ch, AFF_INVISIBLE ) ) {
		affect_strip ( ch, gsn_invis );
		affect_strip ( ch, gsn_mass_invis );
		REMOVE_BIT ( ch->affected_by, AFF_INVISIBLE );
		act ( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
	}

	/*
	 * Damage modifiers.
	 */

	if ( dam > 1 && !IS_NPC ( victim )
			&&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	{ dam = 9 * dam / 10; }

	if ( dam > 1 && IS_AFFECTED ( victim, AFF_SANCTUARY ) )
	{ dam /= 2; }

	if ( dam > 1 && ( ( IS_AFFECTED ( victim, AFF_PROTECT_EVIL ) && IS_EVIL ( ch ) )
					  ||		     ( IS_AFFECTED ( victim, AFF_PROTECT_GOOD ) && IS_GOOD ( ch ) ) ) )
	{ dam -= dam / 4; }

	immune = FALSE;


	/*
	 * Check for parry, and dodge.
	 */
	if ( dt >= TYPE_HIT && ch != victim ) {
		if ( check_parry ( ch, victim ) )
		{ return FALSE; }
		if ( check_dodge ( ch, victim ) )
		{ return FALSE; }
		if ( check_shield_block ( ch, victim ) )
		{ return FALSE; }

	}

	switch ( check_immune ( victim, dam_type ) ) {
		case ( IS_IMMUNE ) :
			immune = TRUE;
			dam = 0;
			break;
		case ( IS_RESISTANT ) :
			dam -= dam / 3;
			break;
		case ( IS_VULNERABLE ) :
			dam += dam / 2;
			break;
	}

	if ( show )
	{ dam_message ( ch, victim, dam, dt, immune, where ); }

	if ( dam == 0 )
	{ return FALSE; }

	/*
	 * Hurt the victim.
	 * Inform the victim of his new state.
	 */
	victim->hit -= dam;
	if ( !IS_NPC ( victim )
			&&   victim->level >= LEVEL_IMMORTAL
			&&   victim->hit < 1 )
	{ victim->hit = 1; }
	update_pos ( victim );

	switch ( victim->position ) {
		case POS_MORTAL:
			act ( "$n is mortally wounded, and will die soon, if not aided.",
				  victim, NULL, NULL, TO_ROOM );
			writeBuffer (
				"You are mortally wounded, and will die soon, if not aided.\n\r",
				victim );
			break;

		case POS_INCAP:
			act ( "$n is incapacitated and will slowly die, if not aided.",
				  victim, NULL, NULL, TO_ROOM );
			writeBuffer (
				"You are incapacitated and will slowly die, if not aided.\n\r",
				victim );
			break;

		case POS_STUNNED:
			act ( "$n is stunned, but will probably recover.",
				  victim, NULL, NULL, TO_ROOM );
			writeBuffer ( "You are stunned, but will probably recover.\n\r",
						  victim );
			break;

		case POS_DEAD:
			act ( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
			writeBuffer ( "You have been KILLED!!\n\r\n\r", victim );
			break;

		default:
			if ( dam > victim->max_hit / 4 )
			{ writeBuffer ( "That really did HURT!\n\r", victim ); }
			if ( victim->hit < victim->max_hit / 4 )
			{ writeBuffer ( "You sure are BLEEDING!\n\r", victim ); }
			break;
	}

	/*
	 * Sleep spells and extremely wounded folks.
	 */
	if ( !IS_AWAKE ( victim ) )
	{ stop_fighting ( victim, FALSE ); }

	/*
	 * Payoff for killing things.
	 */
	if ( victim->position == POS_DEAD ) {
		group_gain ( ch, victim );

		if ( !IS_NPC ( victim ) ) {
			log_hd ( LOG_BASIC | LOG_SECURITY, Format ( "%s killed by %s at %d",
					 victim->name,
					 ( IS_NPC ( ch ) ? ch->short_descr : ch->name ),
					 IN_ROOM ( ch )->vnum ) );

			// -- how embarrassing!  Twitter knows!  Oh thats right, twitter knows all!
			tweetStatement ( Format ( "%s%s has been %s by %s.", !IS_NPC ( ch ) ? "[PK]:" : "", victim->name, !IS_NPC ( ch ) ? "murdered" : "slain", !IS_NPC ( ch ) ? ch->name : ch->short_descr ) );

			/*
			 * Dying penalty:
			 * 2/3 way back to previous level.
			 */
			if ( victim->exp > exp_per_level ( victim, victim->pcdata->points ) * victim->level )
				gain_exp ( victim, ( 2 * ( exp_per_level ( victim, victim->pcdata->points )
										   * victim->level - victim->exp ) / 3 ) + 50 );
		}

		if ( IS_NPC ( victim ) )
			wiznet ( Format ( "%s got toasted by %s at %s [room %d]",
							  ( IS_NPC ( victim ) ? victim->short_descr : victim->name ),
							  ( IS_NPC ( ch ) ? ch->short_descr : ch->name ),
							  IN_ROOM ( ch )->name, IN_ROOM ( ch )->vnum ),
					 NULL, NULL, WIZ_MOBDEATHS, 0, 0 );
		else
			wiznet ( Format ( "%s got toasted by %s at %s [room %d]",
							  ( IS_NPC ( victim ) ? victim->short_descr : victim->name ),
							  ( IS_NPC ( ch ) ? ch->short_descr : ch->name ),
							  IN_ROOM ( ch )->name, IN_ROOM ( ch )->vnum )
					 , NULL, NULL, WIZ_DEATHS, 0, 0 );

		/*
		 * Death trigger
		 */
		if ( IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_DEATH ) ) {
			victim->position = POS_STANDING;
			mp_percent_trigger ( victim, ch, NULL, NULL, TRIG_DEATH );
		}

		raw_kill ( victim );
		int chance = Math::instance().percent();
		int chance_diff = Math::instance().range ( 1, victim->level ) / 2;
		if ( chance < chance_diff ) {
			if ( is_human ( victim ) && !IS_NPC ( ch ) ) {
				make_treasure_chest ( ch, victim, victim->level );
			}
		}

		/* dump the flags */
		if ( ch != victim && !IS_NPC ( ch ) && !is_same_clan ( ch, victim ) ) {
			if ( IS_SET ( victim->act, PLR_KILLER ) )
			{ REMOVE_BIT ( victim->act, PLR_KILLER ); }
			else
			{ REMOVE_BIT ( victim->act, PLR_THIEF ); }
		}

		/* RT new auto commands */

		if ( !IS_NPC ( ch )
				&&  ( corpse = get_obj_list ( ch, "corpse", IN_ROOM ( ch )->contents ) ) != NULL
				&&  corpse->item_type == ITEM_CORPSE_NPC && can_see_obj ( ch, corpse ) ) {
			Item *coins;

			corpse = get_obj_list ( ch, "corpse", IN_ROOM ( ch )->contents );

			if ( IS_SET ( ch->act, PLR_AUTOLOOT ) &&
					corpse && corpse->contains ) { /* exists and not empty */
				cmd_function ( ch, &cmd_get, "all corpse" );
			}

			if ( IS_SET ( ch->act, PLR_AUTOGOLD ) &&
					corpse && corpse->contains  && /* exists and not empty */
					!IS_SET ( ch->act, PLR_AUTOLOOT ) ) {
				if ( ( coins = get_obj_list ( ch, "gcash", corpse->contains ) )
						!= NULL ) {
					cmd_function ( ch, &cmd_get, "all.gcash corpse" );
				}
			}

			if ( IS_SET ( ch->act, PLR_AUTOSAC ) ) {
				if ( IS_SET ( ch->act, PLR_AUTOLOOT ) && corpse && corpse->contains ) {
					return TRUE;  /* leave if corpse has treasure */
				} else {
					cmd_function ( ch, &cmd_sacrifice, "corpse" );
				}
			}
		}

		return TRUE;
	}

	if ( victim == ch )
	{ return TRUE; }

	/*
	 * Take care of link dead people.
	 */
	if ( !IS_NPC ( victim ) && victim->desc == NULL ) {
		if ( Math::instance().range ( 0, victim->wait ) == 0 ) {
			cmd_function ( victim, &cmd_recall, "" );
			return TRUE;
		}
	}

	/*
	 * Wimp out?
	 */
	if ( IS_NPC ( victim ) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2 ) {
		if ( ( IS_SET ( victim->act, ACT_WIMPY ) && Math::instance().bits ( 2 ) == 0
				&&   victim->hit < victim->max_hit / 5 )
				||   ( IS_AFFECTED ( victim, AFF_CHARM ) && victim->master != NULL
					   &&     victim->master->in_room != IN_ROOM ( victim ) ) ) {
			cmd_function ( victim, &cmd_flee, "" );
		}
	}

	if ( !IS_NPC ( victim )
			&&   victim->hit > 0
			&&   victim->hit <= victim->wimpy
			&&   victim->wait < PULSE_VIOLENCE / 2 ) {
		cmd_function ( victim, &cmd_flee, "" );
	}

	tail_chain( );
	return TRUE;
}

bool is_safe ( Creature *ch, Creature *victim )
{
	if ( IN_ROOM ( victim ) == NULL || IN_ROOM ( ch ) == NULL )
	{ return TRUE; }

	if ( FIGHTING ( victim ) == ch || victim == ch )
	{ return FALSE; }

	if ( IsStaff ( ch ) && ch->level > LEVEL_IMMORTAL )
	{ return FALSE; }

	/* killing mobiles */
	if ( IS_NPC ( victim ) ) {

		/* safe room? */
		if ( IS_SET ( IN_ROOM ( victim )->room_flags, ROOM_SAFE ) ) {
			writeBuffer ( "Not in this room.\n\r", ch );
			return TRUE;
		}

		if ( victim->pIndexData->pShop != NULL ) {
			writeBuffer ( "The shopkeeper wouldn't like that.\n\r", ch );
			return TRUE;
		}

		/* no killing healers, trainers, etc */
		if ( IS_SET ( victim->act, ACT_TRAIN )
				||  IS_SET ( victim->act, ACT_PRACTICE )
				||  IS_SET ( victim->act, ACT_IS_HEALER )
				||  IS_SET ( victim->act, ACT_IS_CHANGER ) ) {
			writeBuffer ( "I don't think Mota would approve.\n\r", ch );
			return TRUE;
		}

		if ( !IS_NPC ( ch ) ) {
			/* no pets */
			if ( IS_SET ( victim->act, ACT_PET ) ) {
				act ( "But $N looks so cute and cuddly...",
					  ch, NULL, victim, TO_CHAR );
				return TRUE;
			}

			/* no charmed creatures unless owner */
			if ( IS_AFFECTED ( victim, AFF_CHARM ) && ch != victim->master ) {
				writeBuffer ( "You don't own that monster.\n\r", ch );
				return TRUE;
			}
		}
	}
	/* killing players */
	else {
		/* NPC doing the killing */
		if ( IS_NPC ( ch ) ) {
			/* safe room check */
			if ( IS_SET ( IN_ROOM ( victim )->room_flags, ROOM_SAFE ) ) {
				writeBuffer ( "Not in this room.\n\r", ch );
				return TRUE;
			}

			/* charmed mobs and pets cannot attack players while owned */
			if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL
					&&  ch->master->fighting != victim ) {
				writeBuffer ( "Players are your friends!\n\r", ch );
				return TRUE;
			}
		}
		/* player doing the killing */
		else {
			if ( !is_clan ( ch ) ) {
				writeBuffer ( "Join a clan if you want to kill players.\n\r", ch );
				return TRUE;
			}

			if ( IS_SET ( victim->act, PLR_KILLER ) || IS_SET ( victim->act, PLR_THIEF ) )
			{ return FALSE; }

			if ( !is_clan ( victim ) ) {
				writeBuffer ( "They aren't in a clan, leave them alone.\n\r", ch );
				return TRUE;
			}

			if ( ch->level > victim->level + 8 ) {
				writeBuffer ( "Pick on someone your own size.\n\r", ch );
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool is_safe_spell ( Creature *ch, Creature *victim, bool area )
{
	if ( IN_ROOM ( victim ) == NULL || IN_ROOM ( ch ) == NULL )
	{ return TRUE; }

	if ( victim == ch && area )
	{ return TRUE; }

	if ( FIGHTING ( victim ) == ch || victim == ch )
	{ return FALSE; }

	if ( IsStaff ( ch ) && ch->level > LEVEL_IMMORTAL && !area )
	{ return FALSE; }

	/* killing mobiles */
	if ( IS_NPC ( victim ) ) {
		/* safe room? */
		if ( IS_SET ( IN_ROOM ( victim )->room_flags, ROOM_SAFE ) )
		{ return TRUE; }

		if ( victim->pIndexData->pShop != NULL )
		{ return TRUE; }

		/* no killing healers, trainers, etc */
		if ( IS_SET ( victim->act, ACT_TRAIN )
				||  IS_SET ( victim->act, ACT_PRACTICE )
				||  IS_SET ( victim->act, ACT_IS_HEALER )
				||  IS_SET ( victim->act, ACT_IS_CHANGER ) )
		{ return TRUE; }

		if ( !IS_NPC ( ch ) ) {
			/* no pets */
			if ( IS_SET ( victim->act, ACT_PET ) )
			{ return TRUE; }

			/* no charmed creatures unless owner */
			if ( IS_AFFECTED ( victim, AFF_CHARM ) && ( area || ch != victim->master ) )
			{ return TRUE; }

			/* legal kill? -- cannot hit mob fighting non-group member */
			if ( FIGHTING ( victim ) != NULL && !is_same_group ( ch, FIGHTING ( victim ) ) )
			{ return TRUE; }
		} else {
			/* area effect spells do not hit other mobs */
			if ( area && !is_same_group ( victim, FIGHTING ( ch ) ) )
			{ return TRUE; }
		}
	}
	/* killing players */
	else {
		if ( area && IsStaff ( victim ) && victim->level > LEVEL_IMMORTAL )
		{ return TRUE; }

		/* NPC doing the killing */
		if ( IS_NPC ( ch ) ) {
			/* charmed mobs and pets cannot attack players while owned */
			if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL
					&&  ch->master->fighting != victim )
			{ return TRUE; }

			/* safe room? */
			if ( IS_SET ( IN_ROOM ( victim )->room_flags, ROOM_SAFE ) )
			{ return TRUE; }

			/* legal kill? -- mobs only hit players grouped with opponent*/
			if ( FIGHTING ( ch ) != NULL && !is_same_group ( FIGHTING ( ch ), victim ) )
			{ return TRUE; }
		}

		/* player doing the killing */
		else {
			if ( !is_clan ( ch ) )
			{ return TRUE; }

			if ( IS_SET ( victim->act, PLR_KILLER ) || IS_SET ( victim->act, PLR_THIEF ) )
			{ return FALSE; }

			if ( !is_clan ( victim ) )
			{ return TRUE; }

			if ( ch->level > victim->level + 8 )
			{ return TRUE; }
		}

	}
	return FALSE;
}
/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer ( Creature *ch, Creature *victim )
{
	char buf[MAX_STRING_LENGTH];
	/*
	 * Follow charm thread to responsible character.
	 * Attacking someone's charmed char is hostile!
	 */
	while ( IS_AFFECTED ( victim, AFF_CHARM ) && victim->master != NULL )
	{ victim = victim->master; }

	/*
	 * NPC's are fair game.
	 * So are killers and thieves.
	 */
	if ( IS_NPC ( victim )
			||   IS_SET ( victim->act, PLR_KILLER )
			||   IS_SET ( victim->act, PLR_THIEF ) )
	{ return; }

	/*
	 * Charm-o-rama.
	 */
	if ( IS_SET ( ch->affected_by, AFF_CHARM ) ) {
		if ( ch->master == NULL ) {
			log_hd ( LOG_ERROR, Format ( "Check_killer: %s bad AFF_CHARM",
										 IS_NPC ( ch ) ? ch->short_descr : ch->name ) );
			affect_strip ( ch, gsn_charm_person );
			REMOVE_BIT ( ch->affected_by, AFF_CHARM );
			return;
		}
		/*
			writeBuffer( "*** You are now a KILLER!! ***\n\r", ch->master );
		  	SET_BIT(ch->master->act, PLR_KILLER);
		*/

		stop_follower ( ch );
		return;
	}

	/*
	 * NPC's are cool of course (as long as not charmed).
	 * Hitting yourself is cool too (bleeding).
	 * So is being immortal (Alander's idea).
	 * And current killers stay as they are.
	 */
	if ( IS_NPC ( ch )
			||   ch == victim
			||   ch->level >= LEVEL_IMMORTAL
			||   !is_clan ( ch )
			||   IS_SET ( ch->act, PLR_KILLER )
			||	 FIGHTING ( ch )  == victim )
	{ return; }

	writeBuffer ( "*** You are now a KILLER!! ***\n\r", ch );
	SET_BIT ( ch->act, PLR_KILLER );
	snprintf ( buf, sizeof ( buf ), "$N is attempting to murder %s", victim->name );
	wiznet ( buf, ch, NULL, WIZ_FLAGS, 0, 0 );
	save_char_obj ( ch );
	return;
}



/*
 * Check for parry.
 */
bool check_parry ( Creature *ch, Creature *victim )
{
	int chance;

	if ( !IS_AWAKE ( victim ) )
	{ return FALSE; }

	chance = get_skill ( victim, gsn_parry ) / 2;

	if ( get_eq_char ( victim, WEAR_WIELD ) == NULL ) {
		if ( IS_NPC ( victim ) )
		{ chance /= 2; }
		else
		{ return FALSE; }
	}

	if ( !can_see ( ch, victim ) )
	{ chance /= 2; }

	if ( Math::instance().percent( ) >= chance + victim->level - ch->level )
	{ return FALSE; }

	act ( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
	act ( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
	check_improve ( victim, gsn_parry, TRUE, 6 );
	return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block ( Creature *ch, Creature *victim )
{
	int chance;

	if ( !IS_AWAKE ( victim ) )
	{ return FALSE; }


	chance = get_skill ( victim, gsn_shield_block ) / 5 + 3;


	if ( get_eq_char ( victim, WEAR_SHIELD ) == NULL )
	{ return FALSE; }

	if ( Math::instance().percent( ) >= chance + victim->level - ch->level )
	{ return FALSE; }

	act ( "You block $n's attack with your shield.",  ch, NULL, victim, TO_VICT    );
	act ( "$N blocks your attack with a shield.", ch, NULL, victim, TO_CHAR    );
	check_improve ( victim, gsn_shield_block, TRUE, 6 );
	return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge ( Creature *ch, Creature *victim )
{
	int chance;

	if ( !IS_AWAKE ( victim ) )
	{ return FALSE; }

	chance = get_skill ( victim, gsn_dodge ) / 2;

	if ( !can_see ( victim, ch ) )
	{ chance /= 2; }

	if ( Math::instance().percent( ) >= chance + victim->level - ch->level )
	{ return FALSE; }

	act ( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
	act ( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
	check_improve ( victim, gsn_dodge, TRUE, 6 );
	return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos ( Creature *victim )
{
	if ( victim->hit > 0 ) {
		if ( victim->position <= POS_STUNNED )
		{ victim->position = POS_STANDING; }
		return;
	}

	if ( IS_NPC ( victim ) && victim->hit < 1 ) {
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->hit <= -11 ) {
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->hit <= -6 ) { victim->position = POS_MORTAL; }
	else if ( victim->hit <= -3 ) { victim->position = POS_INCAP; }
	else                          { victim->position = POS_STUNNED; }

	return;
}

/*
 * Start fights.
 */
void set_fighting ( Creature *ch, Creature *victim )
{
	if ( FIGHTING ( ch ) != NULL ) {
		log_hd ( LOG_ERROR, "Set_fighting: already fighting" );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
	{ affect_strip ( ch, gsn_sleep ); }

	FIGHTING ( ch ) = victim;
	ch->position = POS_FIGHTING;

	return;
}



/*
 * Stop fights.
 */
void stop_fighting ( Creature *ch, bool fBoth )
{
	Creature *fch;

	for ( fch = char_list; fch != NULL; fch = fch->next ) {
		if ( fch == ch || ( fBoth && FIGHTING ( fch ) == ch ) ) {
			FIGHTING ( fch )	= NULL;
			fch->position	= IS_NPC ( fch ) ? fch->default_pos : POS_STANDING;
			update_pos ( fch );
		}
	}

	return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse ( Creature *ch )
{
	char buf[MAX_STRING_LENGTH];
	Item *corpse;
	Item *obj;
	Item *obj_next;
	char *name;

	if ( IS_NPC ( ch ) ) {
		name		= ch->short_descr;
		corpse		= create_object ( get_obj_index ( OBJ_VNUM_CORPSE_NPC ), 0 );
		corpse->timer	= Math::instance().range ( 3, 6 );
		if ( ch->gold > 0 ) {
			obj_to_obj ( create_money ( ch->gold, ch->silver ), corpse );
			ch->gold = 0;
			ch->silver = 0;
		}
		corpse->cost = 0;
	} else {
		name		= ch->name;
		corpse		= create_object ( get_obj_index ( OBJ_VNUM_CORPSE_PC ), 0 );
		corpse->timer	= Math::instance().range ( 25, 40 );
		REMOVE_BIT ( ch->act, PLR_CANLOOT );
		if ( !is_clan ( ch ) )
		{ corpse->owner = assign_string ( ch->name ); }
		else {
			corpse->owner = NULL;
			if ( ch->gold > 1 || ch->silver > 1 ) {
				obj_to_obj ( create_money ( ch->gold / 2, ch->silver / 2 ), corpse );
				ch->gold -= ch->gold / 2;
				ch->silver -= ch->silver / 2;
			}
		}

		corpse->cost = 0;
	}

	corpse->level = ch->level;

	sprintf ( buf, corpse->short_descr, name );
	PURGE_DATA ( corpse->short_descr );
	corpse->short_descr = assign_string ( buf );

	sprintf ( buf, corpse->description, name );
	PURGE_DATA ( corpse->description );
	corpse->description = assign_string ( buf );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
		bool floating = FALSE;

		obj_next = obj->next_content;
		if ( obj->wear_loc == WEAR_FLOAT )
		{ floating = TRUE; }
		obj_from_char ( obj );
		if ( obj->item_type == ITEM_POTION )
		{ obj->timer = Math::instance().range ( 500, 1000 ); }
		if ( obj->item_type == ITEM_SCROLL )
		{ obj->timer = Math::instance().range ( 1000, 2500 ); }
		if ( IS_SET ( obj->extra_flags, ITEM_ROT_DEATH ) && !floating ) {
			obj->timer = Math::instance().range ( 5, 10 );
			REMOVE_BIT ( obj->extra_flags, ITEM_ROT_DEATH );
		}
		REMOVE_BIT ( obj->extra_flags, ITEM_VIS_DEATH );

		if ( IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
		{ extract_obj ( obj ); }
		else if ( floating ) {
			if ( IS_OBJ_STAT ( obj, ITEM_ROT_DEATH ) ) { /* get rid of it! */
				if ( obj->contains != NULL ) {
					Item *in, *in_next;

					act ( "$p evaporates,scattering its contents.",
						  ch, obj, NULL, TO_ROOM );
					for ( in = obj->contains; in != NULL; in = in_next ) {
						in_next = in->next_content;
						obj_from_obj ( in );
						obj_to_room ( in, IN_ROOM ( ch ) );
					}
				} else
					act ( "$p evaporates.",
						  ch, obj, NULL, TO_ROOM );
				extract_obj ( obj );
			} else {
				act ( "$p falls to the floor.", ch, obj, NULL, TO_ROOM );
				obj_to_room ( obj, IN_ROOM ( ch ) );
			}
		} else
		{ obj_to_obj ( obj, corpse ); }
	}

	obj_to_room ( corpse, IN_ROOM ( ch ) );
	return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry ( Creature *ch )
{
	RoomData *was_in_room;
	const char *msg;
	int door;
	int vnum;

	vnum = 0;
	msg = "You hear $n's death cry.";

	switch ( Math::instance().bits ( 4 ) ) {
		case  0:
			msg  = "$n hits the ground ... DEAD.";
			break;
		case  1:
			// -- material
			if ( IS_SET ( ch->material_flags, MAT_FOOD ) ) {
				msg  = "$n splatters blood on your armor.";
				break;
			}
		case  2:
			if ( IS_SET ( ch->parts, PART_GUTS ) ) {
				msg = "$n spills $s guts all over the floor.";
				vnum = OBJ_VNUM_GUTS;
			}
			break;
		case  3:
			if ( IS_SET ( ch->parts, PART_HEAD ) ) {
				msg  = "$n's severed head plops on the ground.";
				vnum = OBJ_VNUM_SEVERED_HEAD;
			}
			break;
		case  4:
			if ( IS_SET ( ch->parts, PART_HEART ) ) {
				msg  = "$n's heart is torn from $s chest.";
				vnum = OBJ_VNUM_TORN_HEART;
			}
			break;
		case  5:
			if ( IS_SET ( ch->parts, PART_ARMS ) ) {
				msg  = "$n's arm is sliced from $s dead body.";
				vnum = OBJ_VNUM_SLICED_ARM;
			}
			break;
		case  6:
			if ( IS_SET ( ch->parts, PART_LEGS ) ) {
				msg  = "$n's leg is sliced from $s dead body.";
				vnum = OBJ_VNUM_SLICED_LEG;
			}
			break;
		case 7:
			if ( IS_SET ( ch->parts, PART_BRAINS ) ) {
				msg = "$n's head is shattered, and $s brains splash all over you.";
				vnum = OBJ_VNUM_BRAINS;
			}
			break;
	}

	act ( msg, ch, NULL, NULL, TO_ROOM );

	if ( vnum != 0 ) {
		char buf[MAX_STRING_LENGTH];
		Item *obj;
		char *name;

		name		= IS_NPC ( ch ) ? ch->short_descr : ch->name;
		obj		= create_object ( get_obj_index ( vnum ), 0 );
		obj->timer	= Math::instance().range ( 4, 7 );

		sprintf ( buf, obj->short_descr, name );
		PURGE_DATA ( obj->short_descr );
		obj->short_descr = assign_string ( buf );

		sprintf ( buf, obj->description, name );
		PURGE_DATA ( obj->description );
		obj->description = assign_string ( buf );

		if ( obj->item_type == ITEM_FOOD ) {
			if ( IS_SET ( ch->form, FORM_POISON ) )
			{ obj->value[3] = 1; }
			else if ( !IS_SET ( ch->form, FORM_EDIBLE ) )
			{ obj->item_type = ITEM_TRASH; }
		}

		obj_to_room ( obj, IN_ROOM ( ch ) );
	}

	if ( IS_NPC ( ch ) )
	{ msg = "You hear something's death cry."; }
	else
	{ msg = "You hear someone's death cry."; }

	was_in_room = IN_ROOM ( ch );
	for ( door = 0; door <= 5; door++ ) {
		Exit *pexit;

		if ( ( pexit = was_in_room->exit[door] ) != NULL
				&&   pexit->u1.to_room != NULL
				&&   pexit->u1.to_room != was_in_room ) {
			IN_ROOM ( ch ) = pexit->u1.to_room;
			act ( msg, ch, NULL, NULL, TO_ROOM );
		}
	}
	IN_ROOM ( ch ) = was_in_room;

	return;
}

void raw_kill ( Creature *victim )
{
	int i;

	stop_fighting ( victim, TRUE );
	death_cry ( victim );
	make_corpse ( victim );

	// -- yank out our events that are attached to the life of the victim
	EventManager::instance().extractOnDeath ( victim );

	if ( IS_NPC ( victim ) ) {
		victim->pIndexData->killed++;
		kill_table[URANGE ( 0, victim->level, MAX_LEVEL - 1 )].killed++;
		extract_char ( victim, TRUE );
		return;
	}

	// -- This is where pfile deletion will be called
	// -- and everything below will be removed

	extract_char ( victim, FALSE );
	while ( victim->affected )
	{ affect_remove ( victim, victim->affected ); }
	victim->affected_by	= race_table[victim->race].aff;
	for ( i = 0; i < 4; i++ )
	{ victim->armor[i] = 100; }
	victim->position	= POS_RESTING;
	victim->hit		= UMAX ( 1, victim->hit  );
	victim->mana	= UMAX ( 1, victim->mana );
	victim->move	= UMAX ( 1, victim->move );
	save_char_obj ( victim ); // -- saving is important
	return;
}

void group_gain ( Creature *ch, Creature *victim )
{
	Creature *gch, *lch;
	int members;
	int group_levels;

	/*
	 * Monsters don't get kill xp's or alignment changes.
	 * P-killing doesn't help either.
	 * Dying of mortal wounds or poison doesn't give xp to anyone!
	 */
	if ( victim == ch )
	{ return; }

	members = 0;
	group_levels = 0;
	for ( gch = IN_ROOM ( ch )->people; gch != NULL; gch = gch->next_in_room ) {
		if ( is_same_group ( gch, ch ) ) {
			members++;
			group_levels += IS_NPC ( gch ) ? gch->level / 2 : gch->level;
		}
	}

	if ( members == 0 ) {
		log_hd ( LOG_ERROR, Format ( "Group_gain: members: %d", members ) );
		members = 1;
		group_levels = ch->level ;
	}

	lch = ( ch->leader != NULL ) ? ch->leader : ch;

	for ( gch = IN_ROOM ( ch )->people; gch != NULL; gch = gch->next_in_room ) {
		Item *obj;
		Item *obj_next;

		if ( !is_same_group ( gch, ch ) || IS_NPC ( gch ) )
		{ continue; }

		/*	Taken out, add it back if you want it */
		if ( gch->level - lch->level >= 5 ) {
			writeBuffer ( "You are too high for this group.\n\r", gch );
			continue;
		}

		if ( gch->level - lch->level <= -5 ) {
			writeBuffer ( "You are too low for this group.\n\r", gch );
			continue;
		}

		// -- gain some experience possibly.
		if ( Math::instance().range ( 0, 1 ) == Math::instance().range ( 0, 2 ) ) {
			gain_exp ( gch, Math::instance().range ( 0, ( xp_compute ( gch, victim, group_levels ) + 1 ) ) );
		}
		for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( obj->wear_loc == WEAR_NONE )
			{ continue; }

			if ( ( IS_OBJ_STAT ( obj, ITEM_ANTI_EVIL )    && IS_EVIL ( ch )    )
					||   ( IS_OBJ_STAT ( obj, ITEM_ANTI_GOOD )    && IS_GOOD ( ch )    )
					||   ( IS_OBJ_STAT ( obj, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL ( ch ) ) ) {
				act ( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
				act ( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
				obj_from_char ( obj );
				obj_to_room ( obj, IN_ROOM ( ch ) );
			}
		}
	}

	return;
}

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute ( Creature *gch, Creature *victim, int total_levels )
{
	int xp = 0, base_exp;
	int level_range;

	int time_per_level;

	level_range = victim->level - gch->level;

	/* compute the base exp */
	switch ( level_range ) {
		default :
			base_exp =   0;
			break;
		case -9 :
			base_exp =   1;
			break;
		case -8 :
			base_exp =   2;
			break;
		case -7 :
			base_exp =   3;
			break;
		case -6 :
			base_exp =   4;
			break;
		case -5 :
			base_exp =  5;
			break;
		case -4 :
			base_exp =  6;
			break;
		case -3 :
			base_exp =  7;
			break;
		case -2 :
			base_exp =  8;
			break;
		case -1 :
			base_exp =  9;
			break;
		case  0 :
			base_exp =  10;
			break;
		case  1 :
			base_exp =  11;
			break;
		case  2 :
			base_exp = 12;
			break;
		case  3 :
			base_exp = 14;
			break;
		case  4 :
			base_exp = 16;
			break;
	}

	if ( level_range > 4 )
	{ base_exp = 16 + 5 * ( level_range - 4.5 ); }

	if ( IS_NPC ( victim ) ) {
		if ( is_affected ( victim, skill_lookup ( "sanctuary" ) ) )
		{ base_exp = ( base_exp * 20 ) / 3; }
		if ( is_affected ( victim, skill_lookup ( "haste" ) ) )
		{ base_exp = ( base_exp * 20 ) / 3; }
		if ( IS_SET ( victim->off_flags, OFF_AREA_ATTACK ) )
		{ base_exp = ( base_exp * 2 ) / 3; }
		if ( IS_SET ( victim->off_flags, OFF_BACKSTAB ) )
		{ base_exp = ( base_exp * 2 ) / 3; }
		if ( IS_SET ( victim->off_flags, OFF_FAST ) )
		{ base_exp = ( base_exp * 2 ) / 3; }
		if ( IS_SET ( victim->off_flags, OFF_DODGE ) )
		{ base_exp = ( base_exp * 2 ) / 3; }
		if ( IS_SET ( victim->off_flags, OFF_PARRY ) )
		{ base_exp = ( base_exp * 2 ) / 3; }

		if ( victim->spec_fun != 0 ) {
			if (   SameString ( spec_name ( victim->spec_fun ), "spec_breath_any" )
					|| SameString ( spec_name ( victim->spec_fun ), "spec_breath_acid" )
					|| SameString ( spec_name ( victim->spec_fun ), "spec_breath_fire" )
					|| SameString ( spec_name ( victim->spec_fun ), "spec_breath_frost" )
					|| SameString ( spec_name ( victim->spec_fun ), "spec_breath_gas" )
					|| SameString ( spec_name ( victim->spec_fun ), "spec_breath_lightning" )
			   )
			{ base_exp = ( base_exp * 20 ) / 10; }

			else if (   SameString ( spec_name ( victim->spec_fun ), "spec_cast_cleric" )
						|| SameString ( spec_name ( victim->spec_fun ), "spec_cast_mage" )
						|| SameString ( spec_name ( victim->spec_fun ), "spec_cast_undead" )
					)
			{ base_exp = ( base_exp * 20 ) / 10; }

			else if ( SameString ( spec_name ( victim->spec_fun ), "spec_poison" ) )
			{ base_exp = ( base_exp * 20 ) / 10; }
		}
	}

	if ( gch->level < 6 )                       	/* more exp at the low levels */
	{ xp = 10 * xp / ( gch->level + 4 ); }

	if ( gch->level > 35 )                        /* less at high */
	{ xp =  15 * xp / ( gch->level - 25 ); }

	/* reduce for playing time */
	{
		/* compute quarter-hours per level */
		time_per_level = 4 * ( gch->played + ( int ) ( current_time - gch->logon ) ) / 3600 / gch->level;
		time_per_level = URANGE ( 2, time_per_level, 12 );
		if ( gch->level < 10 ) 											/* make it a curve */
		{ time_per_level = UMAX ( time_per_level, ( 10 - gch->level ) ); }
		xp = xp * time_per_level / 12;
	}

	/* randomize the rewards */
	xp = Math::instance().range ( xp * 3 / 4, xp * 5 / 4 );

	/* adjust for grouping */
	xp = xp * gch->level / ( UMAX ( 1, total_levels - 1 ) );

	// -- prevent crazy amounts of experience.
	if ( xp > 10 ) { xp = ( xp / 3.5 ); }

	return xp;
}

void dam_message ( Creature *ch, Creature *victim, int dam, int dt, bool immune, int where )
{
	char buf1[256], buf2[256], buf3[256];
	const char *attack;
	char punct;

	if ( ch == NULL || victim == NULL )
	{ return; }


	struct dam_struct {
		const char *vs;
		const char *vp;
	};

	const dam_struct dam_mess[] = {
		{ "miss",	"misses",	},  // 0 damage
		{ "hurt", "hurts" }, 		// 5%
		{ "scratch", "scratches" }, // 10%
		{ "bruise", "bruises" },	// 15%
		{ "graze", "grazes" }, 		// 20%
		{ "hit", "hits" },			// 30%
		{ "wound", "wounds" },		// 40%
		{ "do a number on", "does a number on" }, // 45%
		{ "ravage", "ravages" },	// 50%
		{ "tear open", "tears open" },
		{ "kill", "kills" },		// 100% or greater!
		{ "murder", "murders" },	// should be impossible to reach
		{ "slaughter", "slaughters" },
		{ NULL, NULL }
	};

	int cnt = 0;

	for ( cnt = 0; dam_mess[cnt].vs != NULL; cnt++ )
		;

	// damage messages based on % of damage done!
	int pct = ( dam * 100 ) / victim->hit; // not based on max health, but remaining health! (so strikes seem more vital!)
	int pct_max = ( dam * 100 ) / victim->max_hit; // total damage was more than the max health
	int x = 0;

	// -- disabled until we put in our leaderboard system
	// --	update_board ( ch, dam, BOARD_DAMAGE );

	if ( dam == 0 ) { x = 0; where = -1;}
	else {

		if ( pct <= 5 ) { x = 1; }
		if ( pct <= 10 && pct > 5 ) { x = 2; }
		if ( pct <= 15 && pct > 10 ) { x = 3; }
		if ( pct <= 20 && pct > 15 ) { x = 4; }
		if ( pct <= 30 && pct > 20 ) { x = 5; }
		if ( pct <= 40 && pct > 30 ) { x = 6; }
		if ( pct <= 45 && pct > 40 ) { x = 7; }
		if ( pct <  50 && pct > 40 ) { x = 8; }
		if ( pct >= 50 && pct < 100 ) { x = 9; }

		if ( pct >= 100 ) { x = 10; where = -1;}		// over 100% damage (kill him!
		if ( pct >= 150 ) { x = 11; where = -1; }		// murders


		// in-case I done fucked up.
		if ( x == 0 )
		{ x = 1; }

		// Make them a little more random!
		if ( x != 1 && x < 10 )
		{	x = Math::instance().range ( 1, x ); }
	}

	// -- change it up so we are not always hitting a body part.
	if ( Math::instance().percent() > 97 ) {
		where = -1;
	}

	// -- become a slaughterer because you did more damage then they had health ;)
	if ( pct_max >= 200 )
	{ x = 12; where = -1;}

	// -- should never happen, but if for some odd reason it does, we correct it!
	if ( x > cnt )
	{ x = cnt; }

	// -- this should NEVER happen, but we want to make sure we don't bung up the mud.
	if ( IS_NULLSTR ( dam_mess[x].vp ) || IS_NULLSTR ( dam_mess[x].vs ) ) {
		x = Math::instance().range ( 1, 5 );
	}

	// -- we should never encounter this problem again, however, we will maintain this here just incase.
	if ( IS_NULLSTR ( dam_mess[x].vp ) || IS_NULLSTR ( dam_mess[x].vs ) ) {
		log_hd ( LOG_DEBUG | LOG_ERROR, Format ( "Message for Damage is NULL *AFTER* corrections.  Unknown error! (cnt:%d) (x:%d)", cnt, x ) );
	}

	punct   = ( dam <= 24 ) ? '.' : '!';

	if ( dt == TYPE_HIT ) {
		if ( ch  == victim ) {
			sprintf ( buf1, "$n %s $melf%c", dam_mess[x].vp, punct );
			sprintf ( buf2, "You %s yourself%c", dam_mess[x].vs, punct );
		} else {
			sprintf ( buf1, "$n %s $N%c",  dam_mess[x].vp, punct );
			sprintf ( buf2, "You %s $N%c", dam_mess[x].vs, punct );
			sprintf ( buf3, "$n %s you%c", dam_mess[x].vp, punct );
		}
	} else {
		if ( dt >= 0 && dt < MAX_SKILL )
		{ attack	= skill_table[dt].noun_damage; }
		else if ( dt >= TYPE_HIT
				  && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE )
		{ attack	= attack_table[dt - TYPE_HIT].noun; }
		else {
			log_hd ( LOG_ERROR, Format ( "Dam_message: bad dt %d.", dt ) );
			dt  = TYPE_HIT;
			attack  = attack_table[0].name;
		}

		if ( immune ) {
			if ( ch == victim ) {
				sprintf ( buf1, "$n is unaffected by $s own %s.", attack );
				sprintf ( buf2, "Luckily, you are immune to that." );
			} else {
				sprintf ( buf1, "$N is unaffected by $n's %s!", attack );
				sprintf ( buf2, "$N is unaffected by your %s!", attack );
				sprintf ( buf3, "$n's %s is powerless against you.", attack );
			}
		} else {
			if ( ch == victim ) {
				if ( where > 0 ) {
					sprintf ( buf1, "$n's %s %s $s own %s%c", attack, dam_mess[x].vp, wear_info[where].name, punct );
					sprintf ( buf2, "Your %s %s your %s%c \ab{\aR%d\ab}", attack, dam_mess[x].vp, wear_info[where].name, punct, dam );
				} else {
					sprintf ( buf1, "$n's %s %s $m%c", attack, dam_mess[x].vp, punct );
					sprintf ( buf2, "Your %s %s you%c \ab{\aR%d\ab}", attack, dam_mess[x].vp, punct, dam );
				}
			} else {
				if ( where > 0 ) {
					sprintf ( buf1, "$n's %s %s $N's %s%c", attack, dam_mess[x].vp, wear_info[where].name, punct );
					sprintf ( buf2, "Your %s %s $N's %s%c \ab{\aR%d\ab}", attack, dam_mess[x].vp, wear_info[where].name, punct, dam );
					sprintf ( buf3, "$n's %s %s your %s%c \ab{\aR%d\ab}", attack, dam_mess[x].vp, wear_info[where].name, punct, dam );
				} else {
					sprintf ( buf1, "$n's %s %s $N%c",  attack, dam_mess[x].vp, punct );
					sprintf ( buf2, "Your %s %s $N%c \ab{\aR%d\ab}",  attack, dam_mess[x].vp, punct, dam );
					sprintf ( buf3, "$n's %s %s you%c \ab{\aR%d\ab}", attack, dam_mess[x].vp, punct, dam );
				}
			}

		}
	}

	if ( ch == victim ) {
		act ( buf1, ch, NULL, NULL, TO_ROOM );
		act ( buf2, ch, NULL, NULL, TO_CHAR );
	} else {
		act ( buf1, ch, NULL, victim, TO_NOTVICT );
		act ( buf2, ch, NULL, victim, TO_CHAR );
		act ( buf3, ch, NULL, victim, TO_VICT );
	}

	return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm ( Creature *ch, Creature *victim )
{
	Item *obj;

	if ( ( obj = get_eq_char ( victim, WEAR_WIELD ) ) == NULL )
	{ return; }

	if ( IS_OBJ_STAT ( obj, ITEM_NOREMOVE ) ) {
		act ( "$S weapon won't budge!", ch, NULL, victim, TO_CHAR );
		act ( "$n tries to disarm you, but your weapon won't budge!",
			  ch, NULL, victim, TO_VICT );
		act ( "$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	act ( "$n DISARMS you and sends your weapon flying!",
		  ch, NULL, victim, TO_VICT    );
	act ( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
	act ( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

	obj_from_char ( obj );
	if ( IS_OBJ_STAT ( obj, ITEM_NODROP ) || IS_OBJ_STAT ( obj, ITEM_INVENTORY ) )
	{ obj_to_char ( obj, victim ); }
	else {
		obj_to_room ( obj, IN_ROOM ( victim ) );
		if ( IS_NPC ( victim ) && victim->wait == 0 && can_see_obj ( victim, obj ) )
		{ get_obj ( victim, obj, NULL ); }
	}

	return;
}

DefineCommand ( cmd_berserk )
{
	int chance, hp_percent;

	if ( ( chance = get_skill ( ch, gsn_berserk ) ) == 0
			||  ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_BERSERK ) )
			||  ( !IS_NPC ( ch )
				  &&   ch->level < skill_table[gsn_berserk].skill_level[ch->archetype] ) ) {
		writeBuffer ( "You turn red in the face, but nothing happens.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_BERSERK ) || is_affected ( ch, gsn_berserk )
			||  is_affected ( ch, skill_lookup ( "frenzy" ) ) ) {
		writeBuffer ( "You get a little madder.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CALM ) ) {
		writeBuffer ( "You're feeling to mellow to berserk.\n\r", ch );
		return;
	}

	if ( ch->mana < 50 ) {
		writeBuffer ( "You can't get up enough energy.\n\r", ch );
		return;
	}

	/* modifiers */

	/* fighting */
	if ( ch->position == POS_FIGHTING )
	{ chance += 10; }

	/* damage -- below 50% of hp helps, above hurts */
	hp_percent = 100 * ch->hit / ch->max_hit;
	chance += 25 - hp_percent / 2;

	if ( Math::instance().percent() < chance ) {
		Affect af;

		WAIT_STATE ( ch, PULSE_VIOLENCE );
		ch->mana -= 50;
		ch->move /= 2;

		/* heal a little damage */
		ch->hit += ch->level * 2;
		ch->hit = UMIN ( ch->hit, ch->max_hit );

		writeBuffer ( "Your pulse races as you are consumed by rage!\n\r", ch );
		act ( "$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		check_improve ( ch, gsn_berserk, TRUE, 2 );

		af.where	= TO_AFFECTS;
		af.type		= gsn_berserk;
		af.level	= ch->level;
		af.duration	= Math::instance().fuzzy ( ch->level / 8 );
		af.modifier	= UMAX ( 1, ch->level / 5 );
		af.bitvector 	= AFF_BERSERK;

		af.location	= APPLY_HITROLL;
		affect_to_char ( ch, &af );

		af.location	= APPLY_DAMROLL;
		affect_to_char ( ch, &af );

		af.modifier	= UMAX ( 10, 10 * ( ch->level / 5 ) );
		af.location	= APPLY_AC;
		affect_to_char ( ch, &af );
	}

	else {
		WAIT_STATE ( ch, 3 * PULSE_VIOLENCE );
		ch->mana -= 25;
		ch->move /= 2;

		writeBuffer ( "Your pulse speeds up, but nothing happens.\n\r", ch );
		check_improve ( ch, gsn_berserk, FALSE, 2 );
	}
}

DefineCommand ( cmd_bash )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	int chance;

	ChopC ( argument, arg );

	if ( ( chance = get_skill ( ch, gsn_bash ) ) == 0
			||	 ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_BASH ) )
			||	 ( !IS_NPC ( ch )
				   &&	  ch->level < skill_table[gsn_bash].skill_level[ch->archetype] ) ) {
		writeBuffer ( "Bashing? What's that?\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' ) {
		victim = FIGHTING ( ch );
		if ( victim == NULL ) {
			writeBuffer ( "But you aren't fighting anyone!\n\r", ch );
			return;
		}
	}

	else if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->position < POS_FIGHTING ) {
		act ( "You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "You try to bash your brains out, but fail.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim ) &&
			FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		act ( "But $N is your friend!", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* modifiers */

	/* size  and weight */
	chance += ch->carry_weight / 250;
	chance -= victim->carry_weight / 200;

	if ( ch->size < victim->size )
	{ chance += ( ch->size - victim->size ) * 15; }
	else
	{ chance += ( ch->size - victim->size ) * 10; }


	/* stats */
	chance += get_curr_stat ( ch, STAT_STR );
	chance -= ( get_curr_stat ( victim, STAT_DEX ) * 4 ) / 3;
	chance -= GET_AC ( victim, AC_BASH ) / 25;
	/* speed */
	if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
	{ chance += 10; }
	if ( IS_SET ( victim->off_flags, OFF_FAST ) || IS_AFFECTED ( victim, AFF_HASTE ) )
	{ chance -= 30; }

	/* level */
	chance += ( ch->level - victim->level );

	if ( !IS_NPC ( victim )
			&& chance < get_skill ( victim, gsn_dodge ) ) {
		/*
		act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
		act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
		WAIT_STATE(ch,skill_table[gsn_bash].beats);
		return;*/
		chance -= 3 * ( get_skill ( victim, gsn_dodge ) - chance );
	}

	/* now the attack */
	if ( Math::instance().percent() < chance ) {

		act ( "$n sends you sprawling with a powerful bash!",
			  ch, NULL, victim, TO_VICT );
		act ( "You slam into $N, and send $M flying!", ch, NULL, victim, TO_CHAR );
		act ( "$n sends $N sprawling with a powerful bash.",
			  ch, NULL, victim, TO_NOTVICT );
		check_improve ( ch, gsn_bash, TRUE, 1 );

		DAZE_STATE ( victim, 3 * PULSE_VIOLENCE );
		WAIT_STATE ( ch, skill_table[gsn_bash].beats );
		victim->position = POS_RESTING;
		damage ( ch, victim, Math::instance().range ( 2, 2 + 2 * ch->size + chance / 20 ), gsn_bash,
				 DAM_BASH, FALSE );

	} else {
		damage ( ch, victim, 0, gsn_bash, DAM_BASH, FALSE );
		act ( "You fall flat on your face!",
			  ch, NULL, victim, TO_CHAR );
		act ( "$n falls flat on $s face.",
			  ch, NULL, victim, TO_NOTVICT );
		act ( "You evade $n's bash, causing $m to fall flat on $s face.",
			  ch, NULL, victim, TO_VICT );
		check_improve ( ch, gsn_bash, FALSE, 1 );
		ch->position = POS_RESTING;
		WAIT_STATE ( ch, skill_table[gsn_bash].beats * 3 / 2 );
	}
	check_killer ( ch, victim );
}

DefineCommand ( cmd_dirt )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	int chance;

	ChopC ( argument, arg );

	if ( ( chance = get_skill ( ch, gsn_dirt ) ) == 0
			||   ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_KICK_DIRT ) )
			||   ( !IS_NPC ( ch )
				   &&    ch->level < skill_table[gsn_dirt].skill_level[ch->archetype] ) ) {
		writeBuffer ( "You get your feet dirty.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' ) {
		victim = FIGHTING ( ch );
		if ( victim == NULL ) {
			writeBuffer ( "But you aren't in combat!\n\r", ch );
			return;
		}
	}

	else if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( victim, AFF_BLIND ) ) {
		act ( "$E's already been blinded.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Very funny.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim ) &&
			FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		act ( "But $N is such a good friend!", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* modifiers */

	/* dexterity */
	chance += get_curr_stat ( ch, STAT_DEX );
	chance -= 2 * get_curr_stat ( victim, STAT_DEX );

	/* speed  */
	if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
	{ chance += 10; }
	if ( IS_SET ( victim->off_flags, OFF_FAST ) || IS_AFFECTED ( victim, AFF_HASTE ) )
	{ chance -= 25; }

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* sloppy hack to prevent false zeroes */
	if ( chance % 5 == 0 )
	{ chance += 1; }

	/* terrain */

	switch ( IN_ROOM ( ch )->sector_type ) {
		case ( SECT_INSIDE ) :
			chance -= 20;
			break;
		case ( SECT_CITY ) :
			chance -= 10;
			break;
		case ( SECT_FIELD ) :
			chance +=  5;
			break;
		case ( SECT_FOREST ) :
			break;
		case ( SECT_HILLS ) :
			break;
		case ( SECT_MOUNTAIN ) :
			chance -= 10;
			break;
		case ( SECT_WATER_SWIM ) :
			chance  =  0;
			break;
		case ( SECT_WATER_NOSWIM ) :
			chance  =  0;
			break;
		case ( SECT_AIR ) :
			chance  =  0;
			break;
		case ( SECT_DESERT ) :
			chance += 10;
			break;
	}

	if ( chance == 0 ) {
		writeBuffer ( "There isn't any dirt to kick.\n\r", ch );
		return;
	}

	/* now the attack */
	if ( Math::instance().percent() < chance ) {
		Affect af;
		act ( "$n is blinded by the dirt in $s eyes!", victim, NULL, NULL, TO_ROOM );
		act ( "$n kicks dirt in your eyes!", ch, NULL, victim, TO_VICT );
		damage ( ch, victim, Math::instance().range ( 2, 5 ), gsn_dirt, DAM_NONE, FALSE );
		writeBuffer ( "You can't see a thing!\n\r", victim );
		check_improve ( ch, gsn_dirt, TRUE, 2 );
		WAIT_STATE ( ch, skill_table[gsn_dirt].beats );

		af.where	= TO_AFFECTS;
		af.type 	= gsn_dirt;
		af.level 	= ch->level;
		af.duration	= 0;
		af.location	= APPLY_HITROLL;
		af.modifier	= -4;
		af.bitvector 	= AFF_BLIND;

		affect_to_char ( victim, &af );
	} else {
		damage ( ch, victim, 0, gsn_dirt, DAM_NONE, TRUE );
		check_improve ( ch, gsn_dirt, FALSE, 2 );
		WAIT_STATE ( ch, skill_table[gsn_dirt].beats );
	}
	check_killer ( ch, victim );
}

DefineCommand ( cmd_trip )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	int chance;

	ChopC ( argument, arg );

	if ( ( chance = get_skill ( ch, gsn_trip ) ) == 0
			||   ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_TRIP ) )
			||   ( !IS_NPC ( ch )
				   && ch->level < skill_table[gsn_trip].skill_level[ch->archetype] ) ) {
		writeBuffer ( "Tripping?  What's that?\n\r", ch );
		return;
	}


	if ( arg[0] == '\0' ) {
		victim = FIGHTING ( ch );
		if ( victim == NULL ) {
			writeBuffer ( "But you aren't fighting anyone!\n\r", ch );
			return;
		}
	}

	else if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim ) &&
			FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( victim, AFF_FLYING ) ) {
		act ( "$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->position < POS_FIGHTING ) {
		act ( "$N is already down.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "You fall flat on your face!\n\r", ch );
		WAIT_STATE ( ch, 2 * skill_table[gsn_trip].beats );
		act ( "$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* modifiers */

	/* size */
	if ( ch->size < victim->size )
	{ chance += ( ch->size - victim->size ) * 10; } /* bigger = harder to trip */

	/* dex */
	chance += get_curr_stat ( ch, STAT_DEX );
	chance -= get_curr_stat ( victim, STAT_DEX ) * 3 / 2;

	/* speed */
	if ( IS_SET ( ch->off_flags, OFF_FAST ) || IS_AFFECTED ( ch, AFF_HASTE ) )
	{ chance += 10; }
	if ( IS_SET ( victim->off_flags, OFF_FAST ) || IS_AFFECTED ( victim, AFF_HASTE ) )
	{ chance -= 20; }

	/* level */
	chance += ( ch->level - victim->level ) * 2;


	/* now the attack */
	if ( Math::instance().percent() < chance ) {
		act ( "$n trips you and you go down!", ch, NULL, victim, TO_VICT );
		act ( "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR );
		act ( "$n trips $N, sending $M to the ground.", ch, NULL, victim, TO_NOTVICT );
		check_improve ( ch, gsn_trip, TRUE, 1 );

		DAZE_STATE ( victim, 2 * PULSE_VIOLENCE );
		WAIT_STATE ( ch, skill_table[gsn_trip].beats );
		victim->position = POS_RESTING;
		damage ( ch, victim, Math::instance().range ( 2, 2 +  2 * victim->size ), gsn_trip,
				 DAM_BASH, TRUE );
	} else {
		damage ( ch, victim, 0, gsn_trip, DAM_BASH, TRUE );
		WAIT_STATE ( ch, skill_table[gsn_trip].beats * 2 / 3 );
		check_improve ( ch, gsn_trip, FALSE, 1 );
	}
	check_killer ( ch, victim );
}



DefineCommand ( cmd_kill )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Kill whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}
	/*  Allow player killing
	    if ( !IS_NPC(victim) )
	    {
	        if ( !IS_SET(victim->act, PLR_KILLER)
	        &&   !IS_SET(victim->act, PLR_THIEF) )
	        {
	            writeBuffer( "You must MURDER a player.\n\r", ch );
	            return;
	        }
	    }
	*/
	if ( victim == ch ) {
		writeBuffer ( "You hit yourself.  Ouch!\n\r", ch );
		multi_hit ( ch, ch, TYPE_UNDEFINED );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch->position == POS_FIGHTING ) {
		writeBuffer ( "You do the best you can!\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, 1 * PULSE_VIOLENCE );
	check_killer ( ch, victim );
	multi_hit ( ch, victim, TYPE_UNDEFINED );
	return;
}

DefineCommand ( cmd_murde )
{
	writeBuffer ( "If you want to MURDER, spell it out.\n\r", ch );
	return;
}

DefineCommand ( cmd_murder )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Murder whom?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) || ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_PET ) ) )
	{ return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Suicide is a mortal sin.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim ) &&
			FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		act ( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch->position == POS_FIGHTING ) {
		writeBuffer ( "You do the best you can!\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, 1 * PULSE_VIOLENCE );
	if ( IS_NPC ( ch ) )
	{ snprintf ( buf, sizeof ( buf ), "Help! I am being attacked by %s!", ch->short_descr ); }
	else
	{ sprintf ( buf, "Help!  I am being attacked by %s!", ch->name ); }
	cmd_function ( victim, &cmd_yell, buf );
	check_killer ( ch, victim );
	multi_hit ( ch, victim, TYPE_UNDEFINED );
	return;
}



DefineCommand ( cmd_backstab )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	Item *obj;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Backstab whom?\n\r", ch );
		return;
	}

	if ( FIGHTING ( ch ) != NULL ) {
		writeBuffer ( "You're facing the wrong end.\n\r", ch );
		return;
	}

	else if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "How can you sneak up on yourself?\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) )
	{ return; }

	if ( IS_NPC ( victim ) &&
			FIGHTING ( victim ) != NULL &&
			!is_same_group ( ch, FIGHTING ( victim ) ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL ) {
		writeBuffer ( "You need to wield a weapon to backstab.\n\r", ch );
		return;
	}

	if ( victim->hit < victim->max_hit / 3 ) {
		act ( "$N is hurt and suspicious ... you can't sneak up.",
			  ch, NULL, victim, TO_CHAR );
		return;
	}

	check_killer ( ch, victim );
	WAIT_STATE ( ch, skill_table[gsn_backstab].beats );
	if ( Math::instance().percent( ) < get_skill ( ch, gsn_backstab )
			|| ( get_skill ( ch, gsn_backstab ) >= 2 && !IS_AWAKE ( victim ) ) ) {
		check_improve ( ch, gsn_backstab, TRUE, 1 );
		multi_hit ( ch, victim, gsn_backstab );
	} else {
		check_improve ( ch, gsn_backstab, FALSE, 1 );
		damage ( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE );
	}

	return;
}



DefineCommand ( cmd_flee )
{
	RoomData *was_in;
	RoomData *now_in;
	Creature *victim;
	int attempt;

	if ( ( victim = FIGHTING ( ch ) ) == NULL ) {
		if ( ch->position == POS_FIGHTING )
		{ ch->position = POS_STANDING; }
		writeBuffer ( "You aren't fighting anyone.\n\r", ch );
		return;
	}

	was_in = IN_ROOM ( ch );
	for ( attempt = 0; attempt < 6; attempt++ ) {
		Exit *pexit;
		int door;

		door = Math::instance().door( );
		if ( ( pexit = was_in->exit[door] ) == 0
				||   pexit->u1.to_room == NULL
				||   IS_SET ( pexit->exit_info, EX_CLOSED )
				||   Math::instance().range ( 0, ch->daze ) != 0
				|| ( IS_NPC ( ch )
					 &&   IS_SET ( pexit->u1.to_room->room_flags, ROOM_NO_MOB ) ) )
		{ continue; }

		move_char ( ch, door, FALSE );
		if ( ( now_in = IN_ROOM ( ch ) ) == was_in )
		{ continue; }

		IN_ROOM ( ch ) = was_in;
		act ( "$n has fled!", ch, NULL, NULL, TO_ROOM );
		IN_ROOM ( ch ) = now_in;

		if ( !IS_NPC ( ch ) ) {
			writeBuffer ( "You flee from combat!\n\r", ch );
			if ( ( ch->archetype == 2 )
					&& ( Math::instance().percent() < 3 * ( ch->level / 2 ) ) )
			{ writeBuffer ( "You snuck away safely.\n\r", ch ); }
			else {
				writeBuffer ( "You lost 10 exp.\n\r", ch );
				gain_exp ( ch, -10 );
			}
		}

		stop_fighting ( ch, TRUE );
		return;
	}

	writeBuffer ( "PANIC! You couldn't escape!\n\r", ch );
	return;
}



DefineCommand ( cmd_rescue )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	Creature *fch;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Rescue whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "What about fleeing instead?\n\r", ch );
		return;
	}

	if ( !IS_NPC ( ch ) && IS_NPC ( victim ) ) {
		writeBuffer ( "Doesn't need your help!\n\r", ch );
		return;
	}

	if ( FIGHTING ( ch ) == victim ) {
		writeBuffer ( "Too late.\n\r", ch );
		return;
	}

	if ( ( fch = FIGHTING ( victim ) ) == NULL ) {
		writeBuffer ( "That person is not fighting right now.\n\r", ch );
		return;
	}

	if ( IS_NPC ( fch ) && !is_same_group ( ch, victim ) ) {
		writeBuffer ( "Kill stealing is not permitted.\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, skill_table[gsn_rescue].beats );
	if ( Math::instance().percent( ) > get_skill ( ch, gsn_rescue ) ) {
		writeBuffer ( "You fail the rescue.\n\r", ch );
		check_improve ( ch, gsn_rescue, FALSE, 1 );
		return;
	}

	act ( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
	act ( "$n rescues you!", ch, NULL, victim, TO_VICT    );
	act ( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
	check_improve ( ch, gsn_rescue, TRUE, 1 );

	stop_fighting ( fch, FALSE );
	stop_fighting ( victim, FALSE );

	check_killer ( ch, fch );
	set_fighting ( ch, fch );
	set_fighting ( fch, ch );
	return;
}



DefineCommand ( cmd_kick )
{
	Creature *victim;

	if ( !IS_NPC ( ch )
			&&   ch->level < skill_table[gsn_kick].skill_level[ch->archetype] ) {
		writeBuffer (
			"You better leave the martial arts to fighters.\n\r", ch );
		return;
	}

	if ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_KICK ) )
	{ return; }

	if ( ( victim = FIGHTING ( ch ) ) == NULL ) {
		writeBuffer ( "You aren't fighting anyone.\n\r", ch );
		return;
	}

	WAIT_STATE ( ch, skill_table[gsn_kick].beats );
	if ( get_skill ( ch, gsn_kick ) > Math::instance().percent() ) {
		damage ( ch, victim, Math::instance().range ( 1, ch->level ), gsn_kick, DAM_BASH, TRUE );
		check_improve ( ch, gsn_kick, TRUE, 1 );
	} else {
		damage ( ch, victim, 0, gsn_kick, DAM_BASH, TRUE );
		check_improve ( ch, gsn_kick, FALSE, 1 );
	}
	check_killer ( ch, victim );
	return;
}

DefineCommand ( cmd_disarm )
{
	Creature *victim;
	Item *obj;
	int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;

	hth = 0;

	if ( ( chance = get_skill ( ch, gsn_disarm ) ) == 0 ) {
		writeBuffer ( "You don't know how to disarm opponents.\n\r", ch );
		return;
	}

	if ( get_eq_char ( ch, WEAR_WIELD ) == NULL
			&&   ( ( hth = get_skill ( ch, gsn_hand_to_hand ) ) == 0
				   ||    ( IS_NPC ( ch ) && !IS_SET ( ch->off_flags, OFF_DISARM ) ) ) ) {
		writeBuffer ( "You must wield a weapon to disarm.\n\r", ch );
		return;
	}

	if ( ( victim = FIGHTING ( ch ) ) == NULL ) {
		writeBuffer ( "You aren't fighting anyone.\n\r", ch );
		return;
	}

	if ( ( obj = get_eq_char ( victim, WEAR_WIELD ) ) == NULL ) {
		writeBuffer ( "Your opponent is not wielding a weapon.\n\r", ch );
		return;
	}

	/* find weapon skills */
	ch_weapon = get_weapon_skill ( ch, get_weapon_sn ( ch ) );
	vict_weapon = get_weapon_skill ( victim, get_weapon_sn ( victim ) );
	ch_vict_weapon = get_weapon_skill ( ch, get_weapon_sn ( victim ) );

	/* modifiers */

	/* skill */
	if ( get_eq_char ( ch, WEAR_WIELD ) == NULL )
	{ chance = chance * hth / 150; }
	else
	{ chance = chance * ch_weapon / 100; }

	chance += ( ch_vict_weapon / 2 - vict_weapon ) / 2;

	/* dex vs. strength */
	chance += get_curr_stat ( ch, STAT_DEX );
	chance -= 2 * get_curr_stat ( victim, STAT_STR );

	/* level */
	chance += ( ch->level - victim->level ) * 2;

	/* and now the attack */
	if ( Math::instance().percent() < chance ) {
		WAIT_STATE ( ch, skill_table[gsn_disarm].beats );
		disarm ( ch, victim );
		check_improve ( ch, gsn_disarm, TRUE, 1 );
	} else {
		WAIT_STATE ( ch, skill_table[gsn_disarm].beats );
		act ( "You fail to disarm $N.", ch, NULL, victim, TO_CHAR );
		act ( "$n tries to disarm you, but fails.", ch, NULL, victim, TO_VICT );
		act ( "$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT );
		check_improve ( ch, gsn_disarm, FALSE, 1 );
	}
	check_killer ( ch, victim );
	return;
}

DefineCommand ( cmd_surrender )
{
	Creature *mob;
	if ( ( mob = FIGHTING ( ch ) ) == NULL ) {
		writeBuffer ( "But you're not fighting!\n\r", ch );
		return;
	}
	act ( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
	act ( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
	act ( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
	stop_fighting ( ch, TRUE );

	if ( !IS_NPC ( ch ) && IS_NPC ( mob )
			&&   ( !HAS_TRIGGER ( mob, TRIG_SURR )
				   || !mp_percent_trigger ( mob, ch, NULL, NULL, TRIG_SURR ) ) ) {
		act ( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
		multi_hit ( mob, ch, TYPE_UNDEFINED );
	}
}

DefineCommand ( cmd_sla )
{
	writeBuffer ( "If you want to SLAY, spell it out.\n\r", ch );
	return;
}



DefineCommand ( cmd_slay )
{
	Creature *victim;
	char arg[MAX_INPUT_LENGTH];

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Slay whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( ch == victim ) {
		writeBuffer ( "Suicide is a mortal sin.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) && victim->level >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	log_hd ( LOG_SECURITY, Format ( "%s has slayed %s in cold blood.", ch->name, victim->name ) );
	act ( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
	act ( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
	act ( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
	raw_kill ( victim );
	return;
}
