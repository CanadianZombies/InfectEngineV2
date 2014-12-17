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
void	affect_modify	args ( ( Creature *ch, Affect *paf, bool fAdd ) );


/* friend stuff -- for NPC's mostly */
bool is_friend ( Creature *ch, Creature *victim )
{
	if ( is_same_group ( ch, victim ) )
	{ return TRUE; }


	if ( !IS_NPC ( ch ) )
	{ return FALSE; }

	if ( !IS_NPC ( victim ) ) {
		if ( IS_SET ( ch->off_flags, ASSIST_PLAYERS ) )
		{ return TRUE; }
		else
		{ return FALSE; }
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) )
	{ return FALSE; }

	if ( IS_SET ( ch->off_flags, ASSIST_ALL ) )
	{ return TRUE; }

	if ( ch->group && ch->group == victim->group )
	{ return TRUE; }

	if ( IS_SET ( ch->off_flags, ASSIST_VNUM )
			&&  ch->pIndexData == victim->pIndexData )
	{ return TRUE; }

	if ( IS_SET ( ch->off_flags, ASSIST_RACE ) && ch->race == victim->race )
	{ return TRUE; }

	if ( IS_SET ( ch->off_flags, ASSIST_ALIGN )
			&&  !IS_SET ( ch->act, ACT_NOALIGN ) && !IS_SET ( victim->act, ACT_NOALIGN )
			&&  ( ( IS_GOOD ( ch ) && IS_GOOD ( victim ) )
				  ||	 ( IS_EVIL ( ch ) && IS_EVIL ( victim ) )
				  ||   ( IS_NEUTRAL ( ch ) && IS_NEUTRAL ( victim ) ) ) )
	{ return TRUE; }

	return FALSE;
}

/* returns number of people on an object */
int count_users ( Item *obj )
{
	Creature *fch;
	int count = 0;

	if ( obj->in_room == NULL )
	{ return 0; }

	for ( fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room )
		if ( fch->on == obj )
		{ count++; }

	return count;
}

/* returns material number */
int material_lookup ( const char *name )
{
	return 0;
}

const char *material_bit_name ( int bit )
{
	static char buf[512];

	buf[0] = '\0';
	if ( bit & MAT_PRACTICE ) { strcat ( buf, " practice" ); }
	if ( bit & MAT_FOOD ) { strcat ( buf, " food" ); }
	if ( bit & MAT_LIQUID ) { strcat ( buf, " drink" ); }
	if ( bit & MAT_GLASS ) { strcat ( buf, " glass" ); }

	if ( bit & MAT_LEATHER ) { strcat ( buf, " leather" ); }
	if ( bit & MAT_WOOD ) { strcat ( buf, " wood" ); }
	if ( bit & MAT_BONE ) { strcat ( buf, " bone" ); }
	if ( bit & MAT_EBONY ) { strcat ( buf, " ebony" ); }
	if ( bit & MAT_IVORY ) { strcat ( buf, " ivory" ); }
	if ( bit & MAT_DRAGONSCALE ) { strcat ( buf, " dragonscale" ); }
	if ( bit & MAT_COPPER ) { strcat ( buf, " copper" ); }
	if ( bit & MAT_BRASS ) { strcat ( buf, " brass" ); }
	if ( bit & MAT_BRONZE ) { strcat ( buf, " bronze" ); }
	if ( bit & MAT_IRON ) { strcat ( buf, " iron" ); }
	if ( bit & MAT_STEEL ) { strcat ( buf, " steel" ); }
	if ( bit & MAT_TAINTED_MITHRIL ) { strcat ( buf, " tainted mithril" ); }
	if ( bit & MAT_MITHRIL ) { strcat ( buf, " mithril" ); }
	if ( bit & MAT_OBSIDIAN ) { strcat ( buf, " obsidian" ); }
	if ( bit & MAT_ONYX ) { strcat ( buf, " onyx" ); }
	if ( bit & MAT_OPAL ) { strcat ( buf, " opal" ); }
	if ( bit & MAT_DOUBLE_PLATED ) { strcat ( buf, " double plated" ); }
	if ( bit & MAT_SILVER ) { strcat ( buf, " silver" ); }
	if ( bit & MAT_GOLD ) { strcat ( buf, " gold" ); }
	if ( bit & MAT_PLATINUM ) { strcat ( buf, " platinum" ); }
	if ( bit & MAT_TITANIUM ) { strcat ( buf, " titanium" ); }
	if ( bit & MAT_ADAMANTANIUM ) { strcat ( buf, " adamantanium" ); }
	if ( bit & MAT_QUARTZ ) { strcat ( buf, " quartz" ); }
	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

int weapon_lookup ( const char *name )
{
	int type;

	for ( type = 0; weapon_table[type].name != NULL; type++ ) {
		if ( LOWER ( name[0] ) == LOWER ( weapon_table[type].name[0] )
				&&  !str_prefix ( name, weapon_table[type].name ) )
		{ return type; }
	}

	return -1;
}

int weapon_type ( const char *name )
{
	int type;

	for ( type = 0; weapon_table[type].name != NULL; type++ ) {
		if ( LOWER ( name[0] ) == LOWER ( weapon_table[type].name[0] )
				&&  !str_prefix ( name, weapon_table[type].name ) )
		{ return weapon_table[type].type; }
	}

	return WEAPON_EXOTIC;
}

const char *item_name ( int item_type )
{
	int type;

	for ( type = 0; item_table[type].name != NULL; type++ ) {
		if ( item_type == item_table[type].type )
		{ return item_table[type].name; }
	}
	return "none";
}

const char *weapon_name ( int weapon_type )
{
	int type;

	for ( type = 0; weapon_table[type].name != NULL; type++ )
		if ( weapon_type == weapon_table[type].type )
		{ return weapon_table[type].name; }
	return "exotic";
}

int attack_lookup  ( const char *name )
{
	int att;

	for ( att = 0; attack_table[att].name != NULL; att++ ) {
		if ( LOWER ( name[0] ) == LOWER ( attack_table[att].name[0] )
				&&  !str_prefix ( name, attack_table[att].name ) )
		{ return att; }
	}

	return 0;
}

/* returns a flag for wiznet */
long wiznet_lookup ( const char *name )
{
	int flag;

	for ( flag = 0; wiznet_table[flag].name != NULL; flag++ ) {
		if ( LOWER ( name[0] ) == LOWER ( wiznet_table[flag].name[0] )
				&& !str_prefix ( name, wiznet_table[flag].name ) )
		{ return flag; }
	}

	return -1;
}

/* returns archetype number */
int archetype_lookup ( const char *name )
{
	int archetype;

	for ( archetype = 0; archetype < MAX_CLASS; archetype++ ) {
		if ( LOWER ( name[0] ) == LOWER ( archetype_table[archetype].name[0] )
				&&  !str_prefix ( name, archetype_table[archetype].name ) )
		{ return archetype; }
	}

	return -1;
}

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */

int check_immune ( Creature *ch, int dam_type )
{
	int immune, def;
	int bit;

	immune = -1;
	def = IS_NORMAL;

	if ( dam_type == DAM_NONE )
	{ return immune; }

	if ( dam_type <= 3 ) {
		if ( IS_SET ( ch->imm_flags, IMM_WEAPON ) )
		{ def = IS_IMMUNE; }
		else if ( IS_SET ( ch->res_flags, RES_WEAPON ) )
		{ def = IS_RESISTANT; }
		else if ( IS_SET ( ch->vuln_flags, VULN_WEAPON ) )
		{ def = IS_VULNERABLE; }
	} else { /* magical attack */
		if ( IS_SET ( ch->imm_flags, IMM_MAGIC ) )
		{ def = IS_IMMUNE; }
		else if ( IS_SET ( ch->res_flags, RES_MAGIC ) )
		{ def = IS_RESISTANT; }
		else if ( IS_SET ( ch->vuln_flags, VULN_MAGIC ) )
		{ def = IS_VULNERABLE; }
	}

	/* set bits to check -- VULN etc. must ALL be the same or this will fail */
	switch ( dam_type ) {
		case ( DAM_BASH ) :
			bit = IMM_BASH;
			break;
		case ( DAM_PIERCE ) :
			bit = IMM_PIERCE;
			break;
		case ( DAM_SLASH ) :
			bit = IMM_SLASH;
			break;
		case ( DAM_FIRE ) :
			bit = IMM_FIRE;
			break;
		case ( DAM_COLD ) :
			bit = IMM_COLD;
			break;
		case ( DAM_LIGHTNING ) :
			bit = IMM_LIGHTNING;
			break;
		case ( DAM_ACID ) :
			bit = IMM_ACID;
			break;
		case ( DAM_POISON ) :
			bit = IMM_POISON;
			break;
		case ( DAM_NEGATIVE ) :
			bit = IMM_NEGATIVE;
			break;
		case ( DAM_HOLY ) :
			bit = IMM_HOLY;
			break;
		case ( DAM_ENERGY ) :
			bit = IMM_ENERGY;
			break;
		case ( DAM_MENTAL ) :
			bit = IMM_MENTAL;
			break;
		case ( DAM_DISEASE ) :
			bit = IMM_DISEASE;
			break;
		case ( DAM_DROWNING ) :
			bit = IMM_DROWNING;
			break;
		case ( DAM_LIGHT ) :
			bit = IMM_LIGHT;
			break;
		case ( DAM_CHARM ) :
			bit = IMM_CHARM;
			break;
		case ( DAM_SOUND ) :
			bit = IMM_SOUND;
			break;
		default:
			return def;
	}

	if ( IS_SET ( ch->imm_flags, bit ) )
	{ immune = IS_IMMUNE; }
	else if ( IS_SET ( ch->res_flags, bit ) && immune != IS_IMMUNE )
	{ immune = IS_RESISTANT; }
	else if ( IS_SET ( ch->vuln_flags, bit ) ) {
		if ( immune == IS_IMMUNE )
		{ immune = IS_RESISTANT; }
		else if ( immune == IS_RESISTANT )
		{ immune = IS_NORMAL; }
		else
		{ immune = IS_VULNERABLE; }
	}

	if ( immune == -1 )
	{ return def; }
	else
	{ return immune; }
}

bool is_clan ( Creature *ch )
{
	return ch->clan;
}

bool is_same_clan ( Creature *ch, Creature *victim )
{
	if ( clan_table[ch->clan].independent )
	{ return FALSE; }
	else
	{ return ( ch->clan == victim->clan ); }
}

/* checks mob format */
bool is_old_mob ( Creature *ch )
{
	if ( ch->pIndexData == NULL )
	{ return FALSE; }
	else if ( ch->pIndexData->new_format )
	{ return FALSE; }
	return TRUE;
}

/* for returning skill information */
int get_skill ( Creature *ch, int sn )
{
	int skill;

	if ( sn == -1 ) { /* shorthand for level based skills */
		skill = ch->level * 5 / 2;
	}

	else if ( sn < -1 || sn > MAX_SKILL ) {
		log_hd ( LOG_ERROR, Format ( "Bad sn %d in get_skill.", sn ) );
		skill = 0;
	}

	else if ( !IS_NPC ( ch ) ) {
		if ( ch->level < skill_table[sn].skill_level[ch->archetype] )
		{ skill = 0; }
		else
		{ skill = ch->pcdata->learned[sn]; }
	}

	else { /* mobiles */


		if ( skill_table[sn].spell_fun != spell_null )
		{ skill = 40 + 2 * ch->level; }

		else if ( sn == gsn_sneak || sn == gsn_hide )
		{ skill = ch->level * 2 + 20; }

		else if ( ( sn == gsn_dodge && IS_SET ( ch->off_flags, OFF_DODGE ) )
				  ||       ( sn == gsn_parry && IS_SET ( ch->off_flags, OFF_PARRY ) ) )
		{ skill = ch->level * 2; }

		else if ( sn == gsn_shield_block )
		{ skill = 10 + 2 * ch->level; }

		else if ( sn == gsn_second_attack
				  && ( IS_SET ( ch->act, ACT_WARRIOR ) || IS_SET ( ch->act, ACT_THIEF ) ) )
		{ skill = 10 + 3 * ch->level; }

		else if ( sn == gsn_third_attack && IS_SET ( ch->act, ACT_WARRIOR ) )
		{ skill = 4 * ch->level - 40; }

		else if ( sn == gsn_hand_to_hand )
		{ skill = 40 + 2 * ch->level; }

		else if ( sn == gsn_trip && IS_SET ( ch->off_flags, OFF_TRIP ) )
		{ skill = 10 + 3 * ch->level; }

		else if ( sn == gsn_bash && IS_SET ( ch->off_flags, OFF_BASH ) )
		{ skill = 10 + 3 * ch->level; }

		else if ( sn == gsn_disarm
				  &&  ( IS_SET ( ch->off_flags, OFF_DISARM )
						||   IS_SET ( ch->act, ACT_WARRIOR )
						||	  IS_SET ( ch->act, ACT_THIEF ) ) )
		{ skill = 20 + 3 * ch->level; }

		else if ( sn == gsn_berserk && IS_SET ( ch->off_flags, OFF_BERSERK ) )
		{ skill = 3 * ch->level; }

		else if ( sn == gsn_kick )
		{ skill = 10 + 3 * ch->level; }

		else if ( sn == gsn_backstab && IS_SET ( ch->act, ACT_THIEF ) )
		{ skill = 20 + 2 * ch->level; }

		else if ( sn == gsn_rescue )
		{ skill = 40 + ch->level; }

		else if ( sn == gsn_recall )
		{ skill = 40 + ch->level; }

		else if ( sn == gsn_sword
				  ||  sn == gsn_dagger
				  ||  sn == gsn_spear
				  ||  sn == gsn_mace
				  ||  sn == gsn_axe
				  ||  sn == gsn_flail
				  ||  sn == gsn_whip
				  ||  sn == gsn_polearm )
		{ skill = 40 + 5 * ch->level / 2; }

		else
		{ skill = 0; }
	}

	if ( ch->daze > 0 ) {
		if ( skill_table[sn].spell_fun != spell_null )
		{ skill /= 2; }
		else
		{ skill = 2 * skill / 3; }
	}

	if ( !IS_NPC ( ch ) && ch->pcdata->condition[COND_DRUNK]  > 10 )
	{ skill = 9 * skill / 10; }

	return URANGE ( 0, skill, 100 );
}

/* for returning weapon information */
int get_weapon_sn ( Creature *ch )
{
	Item *wield;
	int sn;

	wield = get_eq_char ( ch, WEAR_WIELD );
	if ( wield == NULL || wield->item_type != ITEM_WEAPON )
	{ sn = gsn_hand_to_hand; }
	else switch ( wield->value[0] ) {
			default :
				sn = -1;
				break;
			case ( WEAPON_SWORD ) :
				sn = gsn_sword;
				break;
			case ( WEAPON_DAGGER ) :
				sn = gsn_dagger;
				break;
			case ( WEAPON_SPEAR ) :
				sn = gsn_spear;
				break;
			case ( WEAPON_MACE ) :
				sn = gsn_mace;
				break;
			case ( WEAPON_AXE ) :
				sn = gsn_axe;
				break;
			case ( WEAPON_FLAIL ) :
				sn = gsn_flail;
				break;
			case ( WEAPON_WHIP ) :
				sn = gsn_whip;
				break;
			case ( WEAPON_POLEARM ) :
				sn = gsn_polearm;
				break;
		}
	return sn;
}

int get_weapon_skill ( Creature *ch, int sn )
{
	int skill;

	/* -1 is exotic */
	if ( IS_NPC ( ch ) ) {
		if ( sn == -1 )
		{ skill = 3 * ch->level; }
		else if ( sn == gsn_hand_to_hand )
		{ skill = 40 + 2 * ch->level; }
		else
		{ skill = 40 + 5 * ch->level / 2; }
	}

	else {
		if ( sn == -1 )
		{ skill = 3 * ch->level; }
		else
		{ skill = ch->pcdata->learned[sn]; }
	}

	return URANGE ( 0, skill, 100 );
}


/* used to de-screw characters */
void reset_char ( Creature *ch )
{
	int loc, mod, stat;
	Item *obj;
	Affect *af;
	int i;

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ch->pcdata->perm_hit == 0
			||	ch->pcdata->perm_mana == 0
			||  ch->pcdata->perm_move == 0
			||	ch->pcdata->last_level == 0 ) {
		/* do a FULL reset */
		for ( loc = 0; loc < MAX_WEAR; loc++ ) {
			obj = get_eq_char ( ch, loc );
			if ( obj == NULL )
			{ continue; }
			if ( !obj->enchanted )
				for ( af = obj->pIndexData->affected; af != NULL; af = af->next ) {
					mod = af->modifier;
					switch ( af->location ) {
						case APPLY_SEX:
							ch->sex		-= mod;
							if ( ch->sex < 0 || ch->sex > 2 )
								ch->sex = IS_NPC ( ch ) ?
										  0 :
										  ch->pcdata->true_sex;
							break;
						case APPLY_MANA:
							ch->max_mana	-= mod;
							break;
						case APPLY_HIT:
							ch->max_hit	-= mod;
							break;
						case APPLY_MOVE:
							ch->max_move	-= mod;
							break;
					}
				}

			for ( af = obj->affected; af != NULL; af = af->next ) {
				mod = af->modifier;
				switch ( af->location ) {
					case APPLY_SEX:
						ch->sex         -= mod;
						break;
					case APPLY_MANA:
						ch->max_mana    -= mod;
						break;
					case APPLY_HIT:
						ch->max_hit     -= mod;
						break;
					case APPLY_MOVE:
						ch->max_move    -= mod;
						break;
				}
			}
		}
		/* now reset the permanent stats */
		ch->pcdata->perm_hit 	= ch->max_hit;
		ch->pcdata->perm_mana 	= ch->max_mana;
		ch->pcdata->perm_move	= ch->max_move;
		ch->pcdata->last_level	= ch->played / 3600;
		if ( ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2 ) {
			if ( ch->sex > 0 && ch->sex < 3 )
			{ ch->pcdata->true_sex	= ch->sex; }
			else
			{ ch->pcdata->true_sex 	= 0; }
		}
	}

	/* now restore the character to his/her true condition */
	for ( stat = 0; stat < MAX_STATS; stat++ )
	{ ch->mod_stat[stat] = 0; }

	if ( ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2 )
	{ ch->pcdata->true_sex = 0; }
	ch->sex		= ch->pcdata->true_sex;
	ch->max_hit 	= ch->pcdata->perm_hit;
	ch->max_mana	= ch->pcdata->perm_mana;
	ch->max_move	= ch->pcdata->perm_move;

	for ( i = 0; i < 4; i++ )
	{ ch->armor[i]	= 100; }

	ch->hitroll		= 0;
	ch->damroll		= 0;
	ch->saving_throw	= 0;

	/* now start adding back the effects */
	for ( loc = 0; loc < MAX_WEAR; loc++ ) {
		obj = get_eq_char ( ch, loc );
		if ( obj == NULL )
		{ continue; }
		for ( i = 0; i < 4; i++ )
		{ ch->armor[i] -= apply_ac ( obj, loc, i ); }

		if ( !obj->enchanted )
			for ( af = obj->pIndexData->affected; af != NULL; af = af->next ) {
				mod = af->modifier;
				switch ( af->location ) {
					case APPLY_STR:
						ch->mod_stat[STAT_STR]	+= mod;
						break;
					case APPLY_DEX:
						ch->mod_stat[STAT_DEX]	+= mod;
						break;
					case APPLY_INT:
						ch->mod_stat[STAT_INT]	+= mod;
						break;
					case APPLY_WIS:
						ch->mod_stat[STAT_WIS]	+= mod;
						break;
					case APPLY_CON:
						ch->mod_stat[STAT_CON]	+= mod;
						break;

					case APPLY_SEX:
						ch->sex			+= mod;
						break;
					case APPLY_MANA:
						ch->max_mana		+= mod;
						break;
					case APPLY_HIT:
						ch->max_hit		+= mod;
						break;
					case APPLY_MOVE:
						ch->max_move		+= mod;
						break;

					case APPLY_AC:
						for ( i = 0; i < 4; i ++ )
						{ ch->armor[i] += mod; }
						break;
					case APPLY_HITROLL:
						ch->hitroll		+= mod;
						break;
					case APPLY_DAMROLL:
						ch->damroll		+= mod;
						break;

					case APPLY_SAVES:
						ch->saving_throw += mod;
						break;
					case APPLY_SAVING_ROD:
						ch->saving_throw += mod;
						break;
					case APPLY_SAVING_PETRI:
						ch->saving_throw += mod;
						break;
					case APPLY_SAVING_BREATH:
						ch->saving_throw += mod;
						break;
					case APPLY_SAVING_SPELL:
						ch->saving_throw += mod;
						break;
				}
			}

		for ( af = obj->affected; af != NULL; af = af->next ) {
			mod = af->modifier;
			switch ( af->location ) {
				case APPLY_STR:
					ch->mod_stat[STAT_STR]  += mod;
					break;
				case APPLY_DEX:
					ch->mod_stat[STAT_DEX]  += mod;
					break;
				case APPLY_INT:
					ch->mod_stat[STAT_INT]  += mod;
					break;
				case APPLY_WIS:
					ch->mod_stat[STAT_WIS]  += mod;
					break;
				case APPLY_CON:
					ch->mod_stat[STAT_CON]  += mod;
					break;

				case APPLY_SEX:
					ch->sex                 += mod;
					break;
				case APPLY_MANA:
					ch->max_mana            += mod;
					break;
				case APPLY_HIT:
					ch->max_hit             += mod;
					break;
				case APPLY_MOVE:
					ch->max_move            += mod;
					break;

				case APPLY_AC:
					for ( i = 0; i < 4; i ++ )
					{ ch->armor[i] += mod; }
					break;
				case APPLY_HITROLL:
					ch->hitroll             += mod;
					break;
				case APPLY_DAMROLL:
					ch->damroll             += mod;
					break;

				case APPLY_SAVES:
					ch->saving_throw += mod;
					break;
				case APPLY_SAVING_ROD:
					ch->saving_throw += mod;
					break;
				case APPLY_SAVING_PETRI:
					ch->saving_throw += mod;
					break;
				case APPLY_SAVING_BREATH:
					ch->saving_throw += mod;
					break;
				case APPLY_SAVING_SPELL:
					ch->saving_throw += mod;
					break;
			}
		}
	}

	/* now add back spell effects */
	for ( af = ch->affected; af != NULL; af = af->next ) {
		mod = af->modifier;
		switch ( af->location ) {
			case APPLY_STR:
				ch->mod_stat[STAT_STR]  += mod;
				break;
			case APPLY_DEX:
				ch->mod_stat[STAT_DEX]  += mod;
				break;
			case APPLY_INT:
				ch->mod_stat[STAT_INT]  += mod;
				break;
			case APPLY_WIS:
				ch->mod_stat[STAT_WIS]  += mod;
				break;
			case APPLY_CON:
				ch->mod_stat[STAT_CON]  += mod;
				break;

			case APPLY_SEX:
				ch->sex                 += mod;
				break;
			case APPLY_MANA:
				ch->max_mana            += mod;
				break;
			case APPLY_HIT:
				ch->max_hit             += mod;
				break;
			case APPLY_MOVE:
				ch->max_move            += mod;
				break;

			case APPLY_AC:
				for ( i = 0; i < 4; i ++ )
				{ ch->armor[i] += mod; }
				break;
			case APPLY_HITROLL:
				ch->hitroll             += mod;
				break;
			case APPLY_DAMROLL:
				ch->damroll             += mod;
				break;

			case APPLY_SAVES:
				ch->saving_throw += mod;
				break;
			case APPLY_SAVING_ROD:
				ch->saving_throw += mod;
				break;
			case APPLY_SAVING_PETRI:
				ch->saving_throw += mod;
				break;
			case APPLY_SAVING_BREATH:
				ch->saving_throw += mod;
				break;
			case APPLY_SAVING_SPELL:
				ch->saving_throw += mod;
				break;
		}
	}

	/* make sure sex is RIGHT!!!! */
	if ( ch->sex < 0 || ch->sex > 2 )
	{ ch->sex = ch->pcdata->true_sex; }
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust ( Creature *ch )
{
	if ( ch->desc != NULL && ch->desc->original != NULL )
	{ ch = ch->desc->original; }

	if ( ch->trust )
	{ return ch->trust; }

	if ( IS_NPC ( ch ) && ch->level >= LEVEL_HERO )
	{ return LEVEL_HERO - 1; }
	else
	{ return ch->level; }
}


/*
 * Retrieve a character's age.
 */
int get_age ( Creature *ch )
{
	return 17 + ( ch->played + ( int ) ( current_time - ch->logon ) ) / 72000;
}

// -- Maximum awesomeness allowed!
int get_curr_stat ( Creature *ch, int stat )
{
	return URANGE ( 3, ch->perm_stat[stat] + ch->mod_stat[stat], MAX_ATTR );
}

// -- replacement for the tabled defensive stat for dex. (dex_app)
int get_defense ( Creature *ch )
{
	int dex = get_curr_stat ( ch, STAT_DEX );
	if ( dex <= 6 ) {
		int ret_stat = 0;
		for ( int x = dex; x >= 0; x-- ) {
			ret_stat += 10;
		}
		return ret_stat;
	}
	if ( dex >= 15 ) {
		int ret_stat = -5;
		for ( int x = dex; x >= 15; x-- ) {
			ret_stat = ( ret_stat - 5 );
			if ( dex > 20 ) { ret_stat = ( ret_stat - 5 ); } // if we have more then 20 dex, we get a bigger bonus
		}
		return ret_stat;
	}
	return 0;
}

/* command for returning max training score */
int get_max_train ( Creature *ch, int stat )
{
	int max;

	if ( IS_NPC ( ch ) || ch->level > LEVEL_IMMORTAL )
	{ return 25; }

	max = pc_race_table[ch->race].max_stats[stat];
	if ( archetype_table[ch->archetype].attr_prime == stat ) {
		if ( ch->race == race_lookup ( "human" ) )
		{ max += 3; }
		else
		{ max += 2; }
	}
	return UMIN ( max, 25 );
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n ( Creature *ch )
{
	if ( !IS_NPC ( ch ) && ch->level >= LEVEL_IMMORTAL )
	{ return 1000; }

	if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_PET ) )
	{ return 0; }

	return MAX_WEAR +  2 * get_curr_stat ( ch, STAT_DEX ) + ch->level;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w ( Creature *ch )
{
	if ( !IS_NPC ( ch ) && ch->level >= LEVEL_IMMORTAL )
	{ return 10000000; }

	if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_PET ) )
	{ return 0; }

	return str_app[get_curr_stat ( ch, STAT_STR )].carry * 10 + ch->level * 25;
}



/*
 * See if a string is one of the names of an object.
 */

bool is_name ( const char *str, char *namelist )
{
	char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
	char *list, *string;

	/* fix crash on NULL namelist */
	if ( namelist == NULL || namelist[0] == '\0' )
	{ return FALSE; }

	/* fixed to prevent is_name on "" returning TRUE */
	if ( str[0] == '\0' )
	{ return FALSE; }

	string = ( char * ) str;
	/* we need ALL parts of string to match part of namelist */
	for ( ; ; ) { /* start parsing string */
		str = ChopC ( str, part );

		if ( part[0] == '\0' )
		{ return TRUE; }

		/* check to see if this is part of namelist */
		list = namelist;
		for ( ; ; ) { /* start parsing namelist */
			list = ChopC ( list, name );
			if ( name[0] == '\0' ) /* this name was not found */
			{ return FALSE; }

			if ( !str_prefix ( string, name ) )
			{ return TRUE; } /* full pattern match */

			if ( !str_prefix ( part, name ) )
			{ break; }
		}
	}
}

bool is_exact_name ( const char *str, const char *namelist )
{
	char name[MAX_INPUT_LENGTH];

	if ( namelist == NULL )
	{ return FALSE; }

	for ( ; ; ) {
		namelist = ChopC ( namelist, name );
		if ( name[0] == '\0' )
		{ return FALSE; }
		if ( SameString ( str, name ) )
		{ return TRUE; }
	}
}

/* enchanted stuff for eq */
void affect_enchant ( Item *obj )
{
	/* okay, move all the old flags into new vectors if we have to */
	if ( !obj->enchanted ) {
		Affect *paf, *af_new;
		obj->enchanted = TRUE;

		for ( paf = obj->pIndexData->affected;
				paf != NULL; paf = paf->next ) {
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where	= paf->where;
			af_new->type        = UMAX ( 0, paf->type );
			af_new->level       = paf->level;
			af_new->duration    = paf->duration;
			af_new->location    = paf->location;
			af_new->modifier    = paf->modifier;
			af_new->bitvector   = paf->bitvector;
		}
	}
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify ( Creature *ch, Affect *paf, bool fAdd )
{
	Item *wield;
	int mod, i;

	mod = paf->modifier;

	if ( fAdd ) {
		switch ( paf->where ) {
			case TO_AFFECTS:
				SET_BIT ( ch->affected_by, paf->bitvector );
				break;
			case TO_IMMUNE:
				SET_BIT ( ch->imm_flags, paf->bitvector );
				break;
			case TO_RESIST:
				SET_BIT ( ch->res_flags, paf->bitvector );
				break;
			case TO_VULN:
				SET_BIT ( ch->vuln_flags, paf->bitvector );
				break;
		}
	} else {
		switch ( paf->where ) {
			case TO_AFFECTS:
				REMOVE_BIT ( ch->affected_by, paf->bitvector );
				break;
			case TO_IMMUNE:
				REMOVE_BIT ( ch->imm_flags, paf->bitvector );
				break;
			case TO_RESIST:
				REMOVE_BIT ( ch->res_flags, paf->bitvector );
				break;
			case TO_VULN:
				REMOVE_BIT ( ch->vuln_flags, paf->bitvector );
				break;
		}
		mod = 0 - mod;
	}

	switch ( paf->location ) {
		default:
			log_hd ( LOG_ERROR, Format ( "Affect_modify: unknown location %d.", paf->location ) );
			return;

		case APPLY_NONE:
			break;
		case APPLY_STR:
			ch->mod_stat[STAT_STR]	+= mod;
			break;
		case APPLY_DEX:
			ch->mod_stat[STAT_DEX]	+= mod;
			break;
		case APPLY_INT:
			ch->mod_stat[STAT_INT]	+= mod;
			break;
		case APPLY_WIS:
			ch->mod_stat[STAT_WIS]	+= mod;
			break;
		case APPLY_CON:
			ch->mod_stat[STAT_CON]	+= mod;
			break;
		case APPLY_SEX:
			ch->sex			+= mod;
			break;
		case APPLY_CLASS:
			break;
		case APPLY_LEVEL:
			break;
		case APPLY_AGE:
			break;
		case APPLY_HEIGHT:
			break;
		case APPLY_WEIGHT:
			break;
		case APPLY_MANA:
			ch->max_mana		+= mod;
			break;
		case APPLY_HIT:
			ch->max_hit		+= mod;
			break;
		case APPLY_MOVE:
			ch->max_move		+= mod;
			break;
		case APPLY_GOLD:
			break;
		case APPLY_EXP:
			break;
		case APPLY_AC:
			for ( i = 0; i < 4; i ++ )
			{ ch->armor[i] += mod; }
			break;
		case APPLY_HITROLL:
			ch->hitroll		+= mod;
			break;
		case APPLY_DAMROLL:
			ch->damroll		+= mod;
			break;
		case APPLY_SAVES:
			ch->saving_throw		+= mod;
			break;
		case APPLY_SAVING_ROD:
			ch->saving_throw		+= mod;
			break;
		case APPLY_SAVING_PETRI:
			ch->saving_throw		+= mod;
			break;
		case APPLY_SAVING_BREATH:
			ch->saving_throw		+= mod;
			break;
		case APPLY_SAVING_SPELL:
			ch->saving_throw		+= mod;
			break;
		case APPLY_SPELL_AFFECT:
			break;
	}

	/*
	 * Check for weapon wielding.
	 * Guard against recursion (for weapons with affects).
	 */
	if ( !IS_NPC ( ch ) && ( wield = get_eq_char ( ch, WEAR_WIELD ) ) != NULL
			&&   get_obj_weight ( wield ) > ( str_app[get_curr_stat ( ch, STAT_STR )].wield * 10 ) ) {
		static int depth;

		if ( depth == 0 ) {
			depth++;
			act ( "You drop $p.", ch, wield, NULL, TO_CHAR );
			act ( "$n drops $p.", ch, wield, NULL, TO_ROOM );
			obj_from_char ( wield );
			obj_to_room ( wield, IN_ROOM ( ch ) );
			depth--;
		}
	}

	return;
}


/* find an effect in an affect list */
Affect  *affect_find ( Affect *paf, int sn )
{
	Affect *paf_find;

	for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next ) {
		if ( paf_find->type == sn )
		{ return paf_find; }
	}

	return NULL;
}

/* fix object affects when removing one */
void affect_check ( Creature *ch, int where, int vector )
{
	Affect *paf;
	Item *obj;

	if ( where == TO_OBJECT || where == TO_WEAPON || vector == 0 )
	{ return; }

	for ( paf = ch->affected; paf != NULL; paf = paf->next )
		if ( paf->where == where && paf->bitvector == vector ) {
			switch ( where ) {
				case TO_AFFECTS:
					SET_BIT ( ch->affected_by, vector );
					break;
				case TO_IMMUNE:
					SET_BIT ( ch->imm_flags, vector );
					break;
				case TO_RESIST:
					SET_BIT ( ch->res_flags, vector );
					break;
				case TO_VULN:
					SET_BIT ( ch->vuln_flags, vector );
					break;
			}
			return;
		}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc == -1 )
		{ continue; }

		for ( paf = obj->affected; paf != NULL; paf = paf->next )
			if ( paf->where == where && paf->bitvector == vector ) {
				switch ( where ) {
					case TO_AFFECTS:
						SET_BIT ( ch->affected_by, vector );
						break;
					case TO_IMMUNE:
						SET_BIT ( ch->imm_flags, vector );
						break;
					case TO_RESIST:
						SET_BIT ( ch->res_flags, vector );
						break;
					case TO_VULN:
						SET_BIT ( ch->vuln_flags, vector );

				}
				return;
			}

		if ( obj->enchanted )
		{ continue; }

		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
			if ( paf->where == where && paf->bitvector == vector ) {
				switch ( where ) {
					case TO_AFFECTS:
						SET_BIT ( ch->affected_by, vector );
						break;
					case TO_IMMUNE:
						SET_BIT ( ch->imm_flags, vector );
						break;
					case TO_RESIST:
						SET_BIT ( ch->res_flags, vector );
						break;
					case TO_VULN:
						SET_BIT ( ch->vuln_flags, vector );
						break;
				}
				return;
			}
	}
}

/*
 * Give an affect to a char.
 */
void affect_to_char ( Creature *ch, Affect *paf )
{
	Affect *paf_new;

	paf_new = new_affect();

	*paf_new		= *paf;

	VALIDATE ( paf );	/* in case we missed it when we set up paf */
	paf_new->next	= ch->affected;
	ch->affected	= paf_new;

	affect_modify ( ch, paf_new, TRUE );
	return;
}

/* give an affect to an object */
void affect_to_obj ( Item *obj, Affect *paf )
{
	Affect *paf_new;

	paf_new = new_affect();

	*paf_new		= *paf;

	VALIDATE ( paf );	/* in case we missed it when we set up paf */
	paf_new->next	= obj->affected;
	obj->affected	= paf_new;

	/* apply any affect vectors to the object's extra_flags */
	if ( paf->bitvector )
		switch ( paf->where ) {
			case TO_OBJECT:
				SET_BIT ( obj->extra_flags, paf->bitvector );
				break;
			case TO_WEAPON:
				if ( obj->item_type == ITEM_WEAPON )
				{ SET_BIT ( obj->value[4], paf->bitvector ); }
				break;
		}


	return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove ( Creature *ch, Affect *paf )
{
	int where;
	int vector;

	if ( ch->affected == NULL ) {
		log_hd ( LOG_ERROR, "Affect_remove: no affect." );
		return;
	}

	affect_modify ( ch, paf, FALSE );
	where = paf->where;
	vector = paf->bitvector;

	if ( paf == ch->affected ) {
		ch->affected	= paf->next;
	} else {
		Affect *prev;

		for ( prev = ch->affected; prev != NULL; prev = prev->next ) {
			if ( prev->next == paf ) {
				prev->next = paf->next;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Affect_remove: cannot find paf." );
			return;
		}
	}

	recycle_affect ( paf );

	affect_check ( ch, where, vector );
	return;
}

void affect_remove_obj ( Item *obj, Affect *paf )
{
	int where, vector;
	if ( obj->affected == NULL ) {
		log_hd ( LOG_ERROR, "Affect_remove_object: no affect." );
		return;
	}

	if ( CARRIED_BY ( obj ) != NULL && obj->wear_loc != -1 )
	{ affect_modify ( CARRIED_BY ( obj ), paf, FALSE ); }

	where = paf->where;
	vector = paf->bitvector;

	/* remove flags from the object if needed */
	if ( paf->bitvector )
		switch ( paf->where ) {
			case TO_OBJECT:
				REMOVE_BIT ( obj->extra_flags, paf->bitvector );
				break;
			case TO_WEAPON:
				if ( obj->item_type == ITEM_WEAPON )
				{ REMOVE_BIT ( obj->value[4], paf->bitvector ); }
				break;
		}

	if ( paf == obj->affected ) {
		obj->affected    = paf->next;
	} else {
		Affect *prev;

		for ( prev = obj->affected; prev != NULL; prev = prev->next ) {
			if ( prev->next == paf ) {
				prev->next = paf->next;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Affect_remove_object: cannot find paf." );
			return;
		}
	}

	recycle_affect ( paf );

	if ( CARRIED_BY ( obj ) != NULL && obj->wear_loc != -1 )
	{ affect_check ( CARRIED_BY ( obj ), where, vector ); }
	return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip ( Creature *ch, int sn )
{
	Affect *paf;
	Affect *paf_next;

	for ( paf = ch->affected; paf != NULL; paf = paf_next ) {
		paf_next = paf->next;
		if ( paf->type == sn )
		{ affect_remove ( ch, paf ); }
	}

	return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected ( Creature *ch, int sn )
{
	Affect *paf;

	for ( paf = ch->affected; paf != NULL; paf = paf->next ) {
		if ( paf->type == sn )
		{ return TRUE; }
	}

	return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join ( Creature *ch, Affect *paf )
{
	Affect *paf_old;

	// -- debug this mofo!
	if ( !paf ) {
		log_hd ( LOG_ERROR | LOG_DEBUG, "affect_join: paf is NULL, committing suicide!" );
		SUICIDE;
	}

	// like a boss!
	for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next ) {
		if ( paf_old->type == paf->type ) {
			int ol = paf->level;
			paf->level = ( ( ol += paf_old->level ) / 2 );
			paf->duration += paf_old->duration;
			paf->modifier += paf_old->modifier;
			affect_remove ( ch, paf_old );
			break;
		}
	}

	affect_to_char ( ch, paf );
	return;
}



/*
 * Move a char out of a room.
 */
void char_from_room ( Creature *ch )
{
	Item *obj;

	if ( IN_ROOM ( ch ) == NULL ) {
		log_hd ( LOG_ERROR, "Char_from_room: NULL." );
		return;
	}

	if ( !IS_NPC ( ch ) )
	{ --IN_ROOM ( ch )->area->nplayer; }

	if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL
			&&   obj->item_type == ITEM_LIGHT
			&&   obj->value[2] != 0
			&&   IN_ROOM ( ch )->light > 0 )
	{ --IN_ROOM ( ch )->light; }

	if ( ch == IN_ROOM ( ch )->people ) {
		IN_ROOM ( ch )->people = ch->next_in_room;
	} else {
		Creature *prev;

		for ( prev = IN_ROOM ( ch )->people; prev; prev = prev->next_in_room ) {
			if ( prev->next_in_room == ch ) {
				prev->next_in_room = ch->next_in_room;
				break;
			}
		}

		if ( prev == NULL )
		{ log_hd ( LOG_ERROR, "Char_from_room: ch not found." ); }
	}

	IN_ROOM ( ch )      = NULL;
	ch->next_in_room = NULL;
	ch->on 	     = NULL;  /* sanity check! */
	return;
}



/*
 * Move a char into a room.
 */
void char_to_room ( Creature *ch, RoomData *pRoomIndex )
{
	Item *obj;

	if ( pRoomIndex == NULL ) {
		RoomData *room;

		log_hd ( LOG_ERROR, "Char_to_room: NULL." );

		if ( ( room = get_room_index ( ROOM_VNUM_TEMPLE ) ) != NULL )
		{ char_to_room ( ch, room ); }

		return;
	}

	IN_ROOM ( ch )		= pRoomIndex;
	ch->next_in_room	= pRoomIndex->people;
	pRoomIndex->people	= ch;

	if ( !IS_NPC ( ch ) ) {
		if ( IN_ROOM ( ch )->area->empty ) {
			IN_ROOM ( ch )->area->empty = FALSE;
			IN_ROOM ( ch )->area->age = 0;
		}
		++IN_ROOM ( ch )->area->nplayer;
	}

	if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL
			&&   obj->item_type == ITEM_LIGHT
			&&   obj->value[2] != 0 )
	{ ++IN_ROOM ( ch )->light; }

	if ( IS_AFFECTED ( ch, AFF_PLAGUE ) ) {
		Affect *af, plague;
		Creature *vch;

		for ( af = ch->affected; af != NULL; af = af->next ) {
			if ( af->type == gsn_plague )
			{ break; }
		}

		if ( af == NULL ) {
			REMOVE_BIT ( ch->affected_by, AFF_PLAGUE );
			return;
		}

		if ( af->level == 1 )
		{ return; }

		plague.where		= TO_AFFECTS;
		plague.type 		= gsn_plague;
		plague.level 		= af->level - 1;
		plague.duration 	= Math::instance().range ( 1, 2 * plague.level );
		plague.location		= APPLY_STR;
		plague.modifier 	= -5;
		plague.bitvector 	= AFF_PLAGUE;

		for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch->next_in_room ) {
			if ( !saves_spell ( plague.level - 2, vch, DAM_DISEASE )
					&&  !IsStaff ( vch ) &&
					!IS_AFFECTED ( vch, AFF_PLAGUE ) && Math::instance().bits ( 6 ) == 0 ) {
				writeBuffer ( "You feel hot and feverish.\n\r", vch );
				act ( "$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM );
				affect_join ( vch, &plague );
			}
		}
	}


	return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char ( Item *obj, Creature *ch )
{
	obj->next_content	 = ch->carrying;
	ch->carrying	 = obj;
	CARRIED_BY ( obj )	 = ch;
	obj->in_room	 = NULL;
	IN_OBJ ( obj )		 = NULL;
	ch->carry_number	+= get_obj_number ( obj );
	ch->carry_weight	+= get_obj_weight ( obj );
}



/*
 * Take an obj from its character.
 */
void obj_from_char ( Item *obj )
{
	Creature *ch;

	if ( ( ch = CARRIED_BY ( obj ) ) == NULL ) {
		log_hd ( LOG_ERROR, "Obj_from_char: null ch." );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
	{ unequip_char ( ch, obj ); }

	if ( ch->carrying == obj ) {
		ch->carrying = obj->next_content;
	} else {
		Item *prev;

		for ( prev = ch->carrying; prev != NULL; prev = prev->next_content ) {
			if ( prev->next_content == obj ) {
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( prev == NULL )
		{ log_hd ( LOG_ERROR, "Obj_from_char: obj not in list." ); }
	}

	CARRIED_BY ( obj )	 = NULL;
	obj->next_content	 = NULL;
	ch->carry_number	-= get_obj_number ( obj );
	ch->carry_weight	-= get_obj_weight ( obj );
	return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac ( Item *obj, int iWear, int type )
{
	if ( obj->item_type != ITEM_ARMOR )
	{ return 0; }

	switch ( iWear ) {
		case WEAR_BODY:
			return 3 * obj->value[type];
		case WEAR_HEAD:
			return 2 * obj->value[type];
		case WEAR_LEGS:
			return 2 * obj->value[type];
		case WEAR_FEET:
			return     obj->value[type];
		case WEAR_HANDS:
			return     obj->value[type];
		case WEAR_ARMS:
			return     obj->value[type];
		case WEAR_SHIELD:
			return     obj->value[type];
		case WEAR_NECK_1:
			return     obj->value[type];
		case WEAR_NECK_2:
			return     obj->value[type];
		case WEAR_ABOUT:
			return 2 * obj->value[type];
		case WEAR_WAIST:
			return     obj->value[type];
		case WEAR_WRIST_L:
			return     obj->value[type];
		case WEAR_WRIST_R:
			return     obj->value[type];
		case WEAR_HOLD:
			return     obj->value[type];
	}

	return 0;
}



/*
 * Find a piece of eq on a character.
 */
Item *get_eq_char ( Creature *ch, int iWear )
{
	Item *obj;

	if ( ch == NULL )
	{ return NULL; }

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc == iWear )
		{ return obj; }
	}

	return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char ( Creature *ch, Item *obj, int iWear )
{
	Affect *paf;
	int i;

	if ( get_eq_char ( ch, iWear ) != NULL ) {
		log_hd ( LOG_ERROR, Format ( "Equip_char: already equipped (%d).", iWear ) );
		return;
	}

	if ( ( IS_OBJ_STAT ( obj, ITEM_ANTI_EVIL )    && IS_EVIL ( ch )    )
			||   ( IS_OBJ_STAT ( obj, ITEM_ANTI_GOOD )    && IS_GOOD ( ch )    )
			||   ( IS_OBJ_STAT ( obj, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL ( ch ) ) ) {
		/*
		 * Thanks to Morgenes for the bug fix here!
		 */
		act ( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
		act ( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
		obj_from_char ( obj );
		obj_to_room ( obj, IN_ROOM ( ch ) );
		return;
	}

	for ( i = 0; i < 4; i++ )
	{ ch->armor[i]      	-= apply_ac ( obj, iWear, i ); }
	obj->wear_loc	 = iWear;

	if ( !obj->enchanted )
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
			if ( paf->location != APPLY_SPELL_AFFECT )
			{ affect_modify ( ch, paf, TRUE ); }
	for ( paf = obj->affected; paf != NULL; paf = paf->next )
		if ( paf->location == APPLY_SPELL_AFFECT )
		{ affect_to_char ( ch, paf ); }
		else
		{ affect_modify ( ch, paf, TRUE ); }

	if ( obj->item_type == ITEM_LIGHT
			&&   obj->value[2] != 0
			&&   IN_ROOM ( ch ) != NULL )
	{ ++IN_ROOM ( ch )->light; }

	return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char ( Creature *ch, Item *obj )
{
	Affect *paf = NULL;
	Affect *lpaf = NULL;
	Affect *lpaf_next = NULL;
	int i;

	if ( obj->wear_loc == WEAR_NONE ) {
		log_hd ( LOG_ERROR, "Unequip_char: already unequipped." );
		return;
	}

	for ( i = 0; i < 4; i++ )
	{ ch->armor[i]	+= apply_ac ( obj, obj->wear_loc, i ); }
	obj->wear_loc	 = -1;

	if ( !obj->enchanted ) {
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next ) {
			if ( paf->location == APPLY_SPELL_AFFECT ) {
				for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next ) {
					lpaf_next = lpaf->next;
					if ( ( lpaf->type == paf->type ) &&
							( lpaf->level == paf->level ) &&
							( lpaf->location == APPLY_SPELL_AFFECT ) ) {
						affect_remove ( ch, lpaf );
						lpaf_next = NULL;
					}
				}
			} else {
				affect_modify ( ch, paf, FALSE );
				affect_check ( ch, paf->where, paf->bitvector );
			}
		}
	}
	for ( paf = obj->affected; paf != NULL; paf = paf->next ) {
		if ( paf->location == APPLY_SPELL_AFFECT ) {
			log_hd ( LOG_ERROR, Format ( "norm-apply: %d", 0 ) );
			for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next ) {
				lpaf_next = lpaf->next;
				if ( ( lpaf->type == paf->type ) &&
						( lpaf->level == paf->level ) &&
						( lpaf->location == APPLY_SPELL_AFFECT ) ) {
					log_hd ( LOG_ERROR, Format ( "location = %d", lpaf->location ) );
					log_hd ( LOG_ERROR, Format ( "type = %d", lpaf->type ) );
					affect_remove ( ch, lpaf );
					lpaf_next = NULL;
				}
			}
		} else {
			affect_modify ( ch, paf, FALSE );
			affect_check ( ch, paf->where, paf->bitvector );
		}
	}

	if ( obj->item_type == ITEM_LIGHT
			&&   obj->value[2] != 0
			&&   IN_ROOM ( ch ) != NULL
			&&   IN_ROOM ( ch )->light > 0 )
	{ --IN_ROOM ( ch )->light; }

	return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list ( ItemData *pObjIndex, Item *list )
{
	Item *obj;
	int nMatch;

	nMatch = 0;
	for ( obj = list; obj != NULL; obj = obj->next_content ) {
		if ( obj->pIndexData == pObjIndex )
		{ nMatch++; }
	}

	return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room ( Item *obj )
{
	RoomData *in_room;
	Creature *ch;

	if ( ( in_room = obj->in_room ) == NULL ) {
		log_hd ( LOG_ERROR, "obj_from_room: NULL." );
		return;
	}

	for ( ch = in_room->people; ch != NULL; ch = ch->next_in_room )
		if ( ch->on == obj )
		{ ch->on = NULL; }

	if ( obj == in_room->contents ) {
		in_room->contents = obj->next_content;
	} else {
		Item *prev;

		for ( prev = in_room->contents; prev; prev = prev->next_content ) {
			if ( prev->next_content == obj ) {
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Obj_from_room: obj not found." );
			return;
		}
	}

	obj->in_room      = NULL;
	obj->next_content = NULL;
	return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room ( Item *obj, RoomData *pRoomIndex )
{
	obj->next_content		= pRoomIndex->contents;
	pRoomIndex->contents	= obj;
	obj->in_room		= pRoomIndex;
	CARRIED_BY ( obj )		= NULL;
	IN_OBJ ( obj )			= NULL;
	return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj ( Item *obj, Item *obj_to )
{
	obj->next_content		= obj_to->contains;
	obj_to->contains		= obj;
	IN_OBJ ( obj )			= obj_to;
	obj->in_room		= NULL;
	CARRIED_BY ( obj )		= NULL;
	if ( obj_to->pIndexData->vnum == OBJ_VNUM_PIT )
	{ obj->cost = 0; }

	for ( ; obj_to != NULL; obj_to = obj_to->in_obj ) {
		if ( obj_to->carried_by != NULL ) {
			obj_to->carried_by->carry_number += get_obj_number ( obj );
			obj_to->carried_by->carry_weight += get_obj_weight ( obj )
												* WEIGHT_MULT ( obj_to ) / 100;
		}
	}

	return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj ( Item *obj )
{
	Item *obj_from;

	if ( ( obj_from = IN_OBJ ( obj ) ) == NULL ) {
		log_hd ( LOG_ERROR, "Obj_from_obj: null obj_from." );
		return;
	}

	if ( obj == obj_from->contains ) {
		obj_from->contains = obj->next_content;
	} else {
		Item *prev;

		for ( prev = obj_from->contains; prev; prev = prev->next_content ) {
			if ( prev->next_content == obj ) {
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Obj_from_obj: obj not found." );
			return;
		}
	}

	obj->next_content = NULL;
	IN_OBJ ( obj )       = NULL;

	for ( ; obj_from != NULL; obj_from = obj_from->in_obj ) {
		if ( obj_from->carried_by != NULL ) {
			obj_from->carried_by->carry_number -= get_obj_number ( obj );
			obj_from->carried_by->carry_weight -= get_obj_weight ( obj )
												  * WEIGHT_MULT ( obj_from ) / 100;
		}
	}

	return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj ( Item *obj )
{
	Item *obj_content;
	Item *obj_next;

	if ( obj->in_room != NULL )
	{ obj_from_room ( obj ); }
	else if ( CARRIED_BY ( obj ) != NULL )
	{ obj_from_char ( obj ); }
	else if ( IN_OBJ ( obj ) != NULL )
	{ obj_from_obj ( obj ); }

	for ( obj_content = obj->contains; obj_content; obj_content = obj_next ) {
		obj_next = obj_content->next_content;
		extract_obj ( obj_content );
	}

	if ( object_list == obj ) {
		object_list = obj->next;
	} else {
		Item *prev;

		for ( prev = object_list; prev != NULL; prev = prev->next ) {
			if ( prev->next == obj ) {
				prev->next = obj->next;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, Format ( "Extract_obj: obj %d not found.", obj->pIndexData->vnum ) );
			return;
		}
	}

	--obj->pIndexData->count;
	recycle_obj ( obj );
	return;
}



/*
 * Extract a char from the world.
 */
void extract_char ( Creature *ch, bool fPull )
{
	Creature *wch;
	Item *obj;
	Item *obj_next;

	/* doesn't seem to be necessary
	if ( IN_ROOM(ch) == NULL )
	{
	log_hd(LOG_ERROR, "Extract_char: NULL." );
	return;
	}
	*/

	nuke_pets ( ch );
	ch->pet = NULL; /* just in case */

	if ( fPull )

	{ die_follower ( ch ); }

	stop_fighting ( ch, TRUE );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
		obj_next = obj->next_content;
		extract_obj ( obj );
	}

	if ( IN_ROOM ( ch ) != NULL )
	{ char_from_room ( ch ); }

	/* Death room is set in the clan tabe now */
	if ( !fPull ) {
		char_to_room ( ch, get_room_index ( clan_table[ch->clan].hall ) );
		return;
	}

	if ( IS_NPC ( ch ) )
	{ --ch->pIndexData->count; }

	if ( ch->desc != NULL && ch->desc->original != NULL ) {
		cmd_function ( ch, &cmd_return, "" );
		ch->desc = NULL;
	}

	for ( wch = char_list; wch != NULL; wch = wch->next ) {
		if ( wch->reply == ch )
		{ wch->reply = NULL; }
		if ( ch->mprog_target == wch )
		{ wch->mprog_target = NULL; }
	}

	if ( ch == char_list ) {
		char_list = ch->next;
	} else {
		Creature *prev;

		for ( prev = char_list; prev != NULL; prev = prev->next ) {
			if ( prev->next == ch ) {
				prev->next = ch->next;
				break;
			}
		}

		if ( prev == NULL ) {
			log_hd ( LOG_ERROR, "Extract_char: char not found." );
			return;
		}
	}

	if ( ch->desc != NULL )
	{ ch->desc->character = NULL; }
	recycle_char ( ch );
	return;
}



/*
 * Find a char in the room.
 */
Creature *get_char_room ( Creature *ch, const char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *rch;
	int number;
	int count;

	number = number_argument ( argument, arg );
	count  = 0;
	if ( SameString ( arg, "self" ) )
	{ return ch; }
	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( !can_see ( ch, rch ) || !is_name ( arg, rch->name ) )
		{ continue; }
		if ( ++count == number )
		{ return rch; }
	}

	return NULL;
}




/*
 * Find a char in the world.
 */
Creature *get_char_world ( Creature *ch, const char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *wch;
	int number;
	int count;

	if ( ( wch = get_char_room ( ch, argument ) ) != NULL )
	{ return wch; }

	number = number_argument ( argument, arg );
	count  = 0;
	for ( wch = char_list; wch != NULL ; wch = wch->next ) {
		if ( IN_ROOM ( wch ) == NULL || !can_see ( ch, wch )
				||   !is_name ( arg, wch->name ) )
		{ continue; }
		if ( ++count == number )
		{ return wch; }
	}

	return NULL;
}



/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
Item *get_obj_type ( ItemData *pObjIndex )
{
	Item *obj;

	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		if ( obj->pIndexData == pObjIndex )
		{ return obj; }
	}

	return NULL;
}


/*
 * Find an obj in a list.
 */
Item *get_obj_list ( Creature *ch, const char *argument, Item *list )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int number;
	int count;

	number = number_argument ( argument, arg );
	count  = 0;
	for ( obj = list; obj != NULL; obj = obj->next_content ) {
		if ( can_see_obj ( ch, obj ) && is_name ( arg, obj->name ) ) {
			if ( ++count == number )
			{ return obj; }
		}
	}

	return NULL;
}



/*
 * Find an obj in player's inventory.
 */
Item *get_obj_carry ( Creature *ch, const char *argument, Creature *viewer )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int number;
	int count;

	number = number_argument ( argument, arg );
	count  = 0;
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc == WEAR_NONE
				&&   ( can_see_obj ( viewer, obj ) )
				&&   is_name ( arg, obj->name ) ) {
			if ( ++count == number )
			{ return obj; }
		}
	}

	return NULL;
}



/*
 * Find an obj in player's equipment.
 */
Item *get_obj_wear ( Creature *ch, const char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int number;
	int count;

	number = number_argument ( argument, arg );
	count  = 0;
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc != WEAR_NONE
				&&   can_see_obj ( ch, obj )
				&&   is_name ( arg, obj->name ) ) {
			if ( ++count == number )
			{ return obj; }
		}
	}

	return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
Item *get_obj_here ( Creature *ch, const char *argument )
{
	Item *obj;

	obj = get_obj_list ( ch, argument, IN_ROOM ( ch )->contents );
	if ( obj != NULL )
	{ return obj; }

	if ( ( obj = get_obj_carry ( ch, argument, ch ) ) != NULL )
	{ return obj; }

	if ( ( obj = get_obj_wear ( ch, argument ) ) != NULL )
	{ return obj; }

	return NULL;
}



/*
 * Find an obj in the world.
 */
Item *get_obj_world ( Creature *ch, const char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Item *obj;
	int number;
	int count;

	if ( ( obj = get_obj_here ( ch, argument ) ) != NULL )
	{ return obj; }

	number = number_argument ( argument, arg );
	count  = 0;
	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		if ( can_see_obj ( ch, obj ) && is_name ( arg, obj->name ) ) {
			if ( ++count == number )
			{ return obj; }
		}
	}

	return NULL;
}

/* deduct cost from a character */

void deduct_cost ( Creature *ch, int cost )
{
	int silver = 0, gold = 0;

	silver = UMIN ( ch->silver, cost );

	if ( silver < cost ) {
		gold = ( ( cost - silver + 99 ) / 100 );
		silver = cost - 100 * gold;
	}

	ch->gold -= gold;
	ch->silver -= silver;

	if ( ch->gold < 0 ) {
		log_hd ( LOG_ERROR, Format ( "deduct costs: gold %d < 0", ch->gold ) );
		ch->gold = 0;
	}
	if ( ch->silver < 0 ) {
		log_hd ( LOG_ERROR, Format ( "deduct costs: silver %d < 0", ch->silver ) );
		ch->silver = 0;
	}
}
/*
 * Create a 'money' obj.
 */
Item *create_money ( int gold, int silver )
{
	char buf[MAX_STRING_LENGTH];
	Item *obj;

	if ( gold < 0 || silver < 0 || ( gold == 0 && silver == 0 ) ) {
		log_hd ( LOG_ERROR, Format ( "Create_money: zero or negative money.", UMIN ( gold, silver ) ) );
		gold = UMAX ( 1, gold );
		silver = UMAX ( 1, silver );
	}

	if ( gold == 0 && silver == 1 ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SILVER_ONE ), 0 );
	} else if ( gold == 1 && silver == 0 ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_GOLD_ONE ), 0 );
	} else if ( silver == 0 ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_GOLD_SOME ), 0 );
		sprintf ( buf, obj->short_descr, gold );
		PURGE_DATA ( obj->short_descr );
		obj->short_descr        = assign_string ( buf );
		obj->value[1]           = gold;
		obj->cost               = gold;
		obj->weight		= gold / 5;
	} else if ( gold == 0 ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SILVER_SOME ), 0 );
		sprintf ( buf, obj->short_descr, silver );
		PURGE_DATA ( obj->short_descr );
		obj->short_descr        = assign_string ( buf );
		obj->value[0]           = silver;
		obj->cost               = silver;
		obj->weight		= silver / 20;
	}

	else {
		obj = create_object ( get_obj_index ( OBJ_VNUM_COINS ), 0 );
		sprintf ( buf, obj->short_descr, silver, gold );
		PURGE_DATA ( obj->short_descr );
		obj->short_descr	= assign_string ( buf );
		obj->value[0]		= silver;
		obj->value[1]		= gold;
		obj->cost		= 100 * gold + silver;
		obj->weight		= gold / 5 + silver / 20;
	}

	return obj;
}



/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number ( Item *obj )
{
	int number;

	if ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY
			||  obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY )
	{ number = 0; }
	else
	{ number = 1; }

	for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
	{ number += get_obj_number ( obj ); }

	return number;
}


/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight ( Item *obj )
{
	int weight;
	Item *tobj;

	weight = obj->weight;
	for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
	{ weight += get_obj_weight ( tobj ) * WEIGHT_MULT ( obj ) / 100; }

	return weight;
}

int get_true_weight ( Item *obj )
{
	int weight;

	weight = obj->weight;
	for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
	{ weight += get_obj_weight ( obj ); }

	return weight;
}

/*
 * True if room is dark.
 */
bool room_is_dark ( RoomData *pRoomIndex )
{
	if ( pRoomIndex->light > 0 )
	{ return FALSE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_DARK ) )
	{ return TRUE; }

	if ( pRoomIndex->sector_type == SECT_INSIDE
			||   pRoomIndex->sector_type == SECT_CITY )
	{ return FALSE; }

	if ( weather_info.sunlight == SUN_SET
			||   weather_info.sunlight == SUN_DARK )
	{ return TRUE; }

	return FALSE;
}


bool is_room_owner ( Creature *ch, RoomData *room )
{
	if ( room->owner == NULL || room->owner[0] == '\0' )
	{ return FALSE; }

	return is_name ( ch->name, room->owner );
}

/*
 * True if room is private.
 */
bool room_is_private ( RoomData *pRoomIndex )
{
	Creature *rch;
	int count;


	if ( pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0' )
	{ return TRUE; }

	count = 0;
	for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
	{ count++; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_PRIVATE )  && count >= 2 )
	{ return TRUE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_SOLITARY ) && count >= 1 )
	{ return TRUE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_IMP_ONLY ) )
	{ return TRUE; }

	return FALSE;
}

/* visibility on a room -- for entering and exits */
bool can_see_room ( Creature *ch, RoomData *pRoomIndex )
{
	if ( IS_SET ( pRoomIndex->room_flags, ROOM_IMP_ONLY )
			&&  get_trust ( ch ) < MAX_LEVEL )
	{ return FALSE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_GODS_ONLY )
			&&  !IsStaff ( ch ) )
	{ return FALSE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_HEROES_ONLY )
			&&  !IsStaff ( ch ) )
	{ return FALSE; }

	if ( IS_SET ( pRoomIndex->room_flags, ROOM_NEWBIES_ONLY )
			&&  ch->level > 5 && !IsStaff ( ch ) )
	{ return FALSE; }

	if ( !IsStaff ( ch ) && pRoomIndex->clan && ch->clan != pRoomIndex->clan )
	{ return FALSE; }

	return TRUE;
}



/*
 * True if char can see victim.
 */
bool can_see ( Creature *ch, Creature *victim )
{
	/* RT changed so that WIZ_INVIS has levels */
	if ( ch == victim )
	{ return TRUE; }

	if ( get_trust ( ch ) < victim->invis_level )
	{ return FALSE; }


	if ( get_trust ( ch ) < victim->incog_level && IN_ROOM ( ch ) != IN_ROOM ( victim ) )
	{ return FALSE; }

	if ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) )
			||   ( IS_NPC ( ch ) && IsStaff ( ch ) ) )
	{ return TRUE; }

	if ( IS_AFFECTED ( ch, AFF_BLIND ) )
	{ return FALSE; }

	if ( room_is_dark ( IN_ROOM ( ch ) ) && !IS_AFFECTED ( ch, AFF_INFRARED ) )
	{ return FALSE; }

	if ( IS_AFFECTED ( victim, AFF_INVISIBLE )
			&&   !IS_AFFECTED ( ch, AFF_DETECT_INVIS ) )
	{ return FALSE; }

	/* sneaking */
	if ( IS_AFFECTED ( victim, AFF_SNEAK )
			&&   !IS_AFFECTED ( ch, AFF_DETECT_HIDDEN )
			&&   FIGHTING ( victim ) == NULL ) {
		int chance;
		chance = get_skill ( victim, gsn_sneak );
		chance += get_curr_stat ( victim, STAT_DEX ) * 3 / 2;
		chance -= get_curr_stat ( ch, STAT_INT ) * 2;
		chance -= ch->level - victim->level * 3 / 2;

		if ( Math::instance().percent() < chance )
		{ return FALSE; }
	}

	if ( IS_AFFECTED ( victim, AFF_HIDE )
			&&   !IS_AFFECTED ( ch, AFF_DETECT_HIDDEN )
			&&   FIGHTING ( victim ) == NULL )
	{ return FALSE; }

	return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj ( Creature *ch, Item *obj )
{
	if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) )
	{ return TRUE; }

	if ( IS_SET ( obj->extra_flags, ITEM_VIS_DEATH ) )
	{ return FALSE; }

	if ( IS_AFFECTED ( ch, AFF_BLIND ) && obj->item_type != ITEM_POTION )
	{ return FALSE; }

	if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	{ return TRUE; }

	if ( IS_SET ( obj->extra_flags, ITEM_INVIS )
			&&   !IS_AFFECTED ( ch, AFF_DETECT_INVIS ) )
	{ return FALSE; }

	if ( IS_OBJ_STAT ( obj, ITEM_GLOW ) )
	{ return TRUE; }

	if ( room_is_dark ( IN_ROOM ( ch ) ) && !IS_AFFECTED ( ch, AFF_DARK_VISION ) )
	{ return FALSE; }

	return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj ( Creature *ch, Item *obj )
{
	if ( !IS_SET ( obj->extra_flags, ITEM_NODROP ) )
	{ return TRUE; }

	if ( !IS_NPC ( ch ) && ch->level >= LEVEL_IMMORTAL )
	{ return TRUE; }

	return FALSE;
}


/*
 * Return ascii name of an affect location.
 */
const char *affect_loc_name ( int location )
{
	switch ( location ) {
		case APPLY_NONE:
			return "none";
		case APPLY_STR:
			return "strength";
		case APPLY_DEX:
			return "dexterity";
		case APPLY_INT:
			return "intelligence";
		case APPLY_WIS:
			return "wisdom";
		case APPLY_CON:
			return "constitution";
		case APPLY_SEX:
			return "sex";
		case APPLY_CLASS:
			return "archetype";
		case APPLY_LEVEL:
			return "level";
		case APPLY_AGE:
			return "age";
		case APPLY_MANA:
			return "mana";
		case APPLY_HIT:
			return "hp";
		case APPLY_MOVE:
			return "moves";
		case APPLY_GOLD:
			return "gold";
		case APPLY_EXP:
			return "experience";
		case APPLY_AC:
			return "armor archetype";
		case APPLY_HITROLL:
			return "hit roll";
		case APPLY_DAMROLL:
			return "damage roll";
		case APPLY_SAVES:
			return "saves";
		case APPLY_SAVING_ROD:
			return "save vs rod";
		case APPLY_SAVING_PETRI:
			return "save vs petrification";
		case APPLY_SAVING_BREATH:
			return "save vs breath";
		case APPLY_SAVING_SPELL:
			return "save vs spell";
		case APPLY_SPELL_AFFECT:
			return "none";
	}

	log_hd ( LOG_ERROR, Format ( "Affect_location_name: unknown location %d.", location ) );
	return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
const char *affect_bit_name ( int vector )
{
	static char buf[512];

	buf[0] = '\0';
	if ( vector & AFF_BLIND         ) { strcat ( buf, " blind"         ); }
	if ( vector & AFF_INVISIBLE     ) { strcat ( buf, " invisible"     ); }
	if ( vector & AFF_DETECT_EVIL   ) { strcat ( buf, " detect_evil"   ); }
	if ( vector & AFF_DETECT_GOOD   ) { strcat ( buf, " detect_good"   ); }
	if ( vector & AFF_DETECT_INVIS  ) { strcat ( buf, " detect_invis"  ); }
	if ( vector & AFF_DETECT_MAGIC  ) { strcat ( buf, " detect_magic"  ); }
	if ( vector & AFF_DETECT_HIDDEN ) { strcat ( buf, " detect_hidden" ); }
	if ( vector & AFF_SANCTUARY     ) { strcat ( buf, " sanctuary"     ); }
	if ( vector & AFF_FAERIE_FIRE   ) { strcat ( buf, " faerie_fire"   ); }
	if ( vector & AFF_INFRARED      ) { strcat ( buf, " infrared"      ); }
	if ( vector & AFF_CURSE         ) { strcat ( buf, " curse"         ); }
	if ( vector & AFF_POISON        ) { strcat ( buf, " poison"        ); }
	if ( vector & AFF_PROTECT_EVIL  ) { strcat ( buf, " prot_evil"     ); }
	if ( vector & AFF_PROTECT_GOOD  ) { strcat ( buf, " prot_good"     ); }
	if ( vector & AFF_SLEEP         ) { strcat ( buf, " sleep"         ); }
	if ( vector & AFF_SNEAK         ) { strcat ( buf, " sneak"         ); }
	if ( vector & AFF_HIDE          ) { strcat ( buf, " hide"          ); }
	if ( vector & AFF_CHARM         ) { strcat ( buf, " charm"         ); }
	if ( vector & AFF_FLYING        ) { strcat ( buf, " flying"        ); }
	if ( vector & AFF_PASS_DOOR     ) { strcat ( buf, " pass_door"     ); }
	if ( vector & AFF_BERSERK	    ) { strcat ( buf, " berserk"	    ); }
	if ( vector & AFF_CALM	    ) { strcat ( buf, " calm"	    ); }
	if ( vector & AFF_HASTE	    ) { strcat ( buf, " haste"	    ); }
	if ( vector & AFF_SLOW          ) { strcat ( buf, " slow"          ); }
	if ( vector & AFF_PLAGUE	    ) { strcat ( buf, " plague" 	    ); }
	if ( vector & AFF_DARK_VISION   ) { strcat ( buf, " dark_vision"   ); }
	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}



/*
 * Return ascii name of extra flags vector.
 */
const char *extra_bit_name ( int extra_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( extra_flags & ITEM_GLOW         ) { strcat ( buf, " glow"         ); }
	if ( extra_flags & ITEM_HUM          ) { strcat ( buf, " hum"          ); }
	if ( extra_flags & ITEM_DARK         ) { strcat ( buf, " dark"         ); }
	if ( extra_flags & ITEM_LOCK         ) { strcat ( buf, " lock"         ); }
	if ( extra_flags & ITEM_EVIL         ) { strcat ( buf, " evil"         ); }
	if ( extra_flags & ITEM_INVIS        ) { strcat ( buf, " invis"        ); }
	if ( extra_flags & ITEM_MAGIC        ) { strcat ( buf, " magic"        ); }
	if ( extra_flags & ITEM_NODROP       ) { strcat ( buf, " nodrop"       ); }
	if ( extra_flags & ITEM_BLESS        ) { strcat ( buf, " bless"        ); }
	if ( extra_flags & ITEM_ANTI_GOOD    ) { strcat ( buf, " anti-good"    ); }
	if ( extra_flags & ITEM_ANTI_EVIL    ) { strcat ( buf, " anti-evil"    ); }
	if ( extra_flags & ITEM_ANTI_NEUTRAL ) { strcat ( buf, " anti-neutral" ); }
	if ( extra_flags & ITEM_NOREMOVE     ) { strcat ( buf, " noremove"     ); }
	if ( extra_flags & ITEM_INVENTORY    ) { strcat ( buf, " inventory"    ); }
	if ( extra_flags & ITEM_NOPURGE	 ) { strcat ( buf, " nopurge"	); }
	if ( extra_flags & ITEM_VIS_DEATH	 ) { strcat ( buf, " vis_death"	); }
	if ( extra_flags & ITEM_ROT_DEATH	 ) { strcat ( buf, " rot_death"	); }
	if ( extra_flags & ITEM_NOLOCATE	 ) { strcat ( buf, " no_locate"	); }
	if ( extra_flags & ITEM_SELL_EXTRACT ) { strcat ( buf, " sell_extract" ); }
	if ( extra_flags & ITEM_BURN_PROOF	 ) { strcat ( buf, " burn_proof"	); }
	if ( extra_flags & ITEM_NOUNCURSE	 ) { strcat ( buf, " no_uncurse"	); }
	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

/* return ascii name of an act vector */
const char *act_bit_name ( int act_flags )
{
	static char buf[512];

	buf[0] = '\0';

	if ( IS_SET ( act_flags, ACT_IS_NPC ) ) {
		strcat ( buf, " npc" );
		if ( act_flags & ACT_SENTINEL 	) { strcat ( buf, " sentinel" ); }
		if ( act_flags & ACT_SCAVENGER	) { strcat ( buf, " scavenger" ); }
		if ( act_flags & ACT_AGGRESSIVE	) { strcat ( buf, " aggressive" ); }
		if ( act_flags & ACT_STAY_AREA	) { strcat ( buf, " stay_area" ); }
		if ( act_flags & ACT_WIMPY	) { strcat ( buf, " wimpy" ); }
		if ( act_flags & ACT_PET		) { strcat ( buf, " pet" ); }
		if ( act_flags & ACT_TRAIN	) { strcat ( buf, " train" ); }
		if ( act_flags & ACT_PRACTICE	) { strcat ( buf, " practice" ); }
		if ( act_flags & ACT_UNDEAD	) { strcat ( buf, " undead" ); }
		if ( act_flags & ACT_CLERIC	) { strcat ( buf, " cleric" ); }
		if ( act_flags & ACT_MAGE	) { strcat ( buf, " mage" ); }
		if ( act_flags & ACT_THIEF	) { strcat ( buf, " thief" ); }
		if ( act_flags & ACT_WARRIOR	) { strcat ( buf, " warrior" ); }
		if ( act_flags & ACT_NOALIGN	) { strcat ( buf, " no_align" ); }
		if ( act_flags & ACT_NOPURGE	) { strcat ( buf, " no_purge" ); }
		if ( act_flags & ACT_IS_HEALER	) { strcat ( buf, " healer" ); }
		if ( act_flags & ACT_IS_CHANGER  ) { strcat ( buf, " changer" ); }
		if ( act_flags & ACT_GAIN	) { strcat ( buf, " skill_train" ); }
		if ( act_flags & ACT_UPDATE_ALWAYS ) { strcat ( buf, " update_always" ); }
		if ( act_flags & ACT_RANDOM_EQ ) { strcat ( buf, " random_eq" ); }
	} else {
		strcat ( buf, " player" );
		if ( act_flags & PLR_AUTOASSIST	) { strcat ( buf, " autoassist" ); }
		if ( act_flags & PLR_AUTOEXIT	) { strcat ( buf, " autoexit" ); }
		if ( act_flags & PLR_AUTOLOOT	) { strcat ( buf, " autoloot" ); }
		if ( act_flags & PLR_AUTOSAC	) { strcat ( buf, " autosac" ); }
		if ( act_flags & PLR_AUTOGOLD	) { strcat ( buf, " autogold" ); }
		if ( act_flags & PLR_AUTOSPLIT	) { strcat ( buf, " autosplit" ); }
		if ( act_flags & PLR_HOLYLIGHT	) { strcat ( buf, " holy_light" ); }
		if ( act_flags & PLR_CANLOOT	) { strcat ( buf, " loot_corpse" ); }
		if ( act_flags & PLR_NOSUMMON	) { strcat ( buf, " no_summon" ); }
		if ( act_flags & PLR_NOFOLLOW	) { strcat ( buf, " no_follow" ); }
		if ( act_flags & PLR_FREEZE	) { strcat ( buf, " frozen" ); }
		if ( act_flags & PLR_THIEF	) { strcat ( buf, " thief" ); }
		if ( act_flags & PLR_KILLER	) { strcat ( buf, " killer" ); }
	}
	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *random_eq_bit_name ( int random_eq_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( random_eq_flags & RANDOM_HELM ) { strcat ( buf, " helm" ); }
	if ( random_eq_flags & RANDOM_TORSO ) { strcat ( buf, " torso" ); }
	if ( random_eq_flags & RANDOM_ARMS ) { strcat ( buf, " arms" ); }
	if ( random_eq_flags & RANDOM_HANDS ) { strcat ( buf, " hands" ); }
	if ( random_eq_flags & RANDOM_LEGS ) { strcat ( buf, " legs" ); }
	if ( random_eq_flags & RANDOM_FEET ) { strcat ( buf, " feet" ); }
	if ( random_eq_flags & RANDOM_WAIST ) { strcat ( buf, " waist" ); }
	if ( random_eq_flags & RANDOM_ABOUT ) { strcat ( buf, " about" ); }
	if ( random_eq_flags & RANDOM_WRIST ) { strcat ( buf, " wrist" ); }
	if ( random_eq_flags & RANDOM_NECK ) { strcat ( buf, " neck" ); }
	if ( random_eq_flags & RANDOM_LIGHT ) { strcat ( buf, " light" ); }
	if ( random_eq_flags & RANDOM_RING ) { strcat ( buf, " ring" ); }
	if ( random_eq_flags & RANDOM_TRINKET ) { strcat ( buf, " trinket" ); }
	if ( random_eq_flags & RANDOM_WEAPON ) { strcat ( buf, " weapon" ); }
	if ( random_eq_flags & RANDOM_SHIELD ) { strcat ( buf, " shield" ); }
	if ( random_eq_flags & RANDOM_WAND ) { strcat ( buf, " wand" ); }
	if ( random_eq_flags & RANDOM_STAFF ) { strcat ( buf, " staff" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *comm_bit_name ( int comm_flags )
{
	static char buf[512];

	buf[0] = '\0';

	if ( comm_flags & COMM_QUIET		) { strcat ( buf, " quiet" ); }
	if ( comm_flags & COMM_DEAF		) { strcat ( buf, " deaf" ); }
	if ( comm_flags & COMM_NOWIZ		) { strcat ( buf, " no_wiz" ); }
	if ( comm_flags & COMM_NOAUCTION	) { strcat ( buf, " no_auction" ); }
	if ( comm_flags & COMM_NOGOSSIP	) { strcat ( buf, " no_gossip" ); }
	if ( comm_flags & COMM_NOQUESTION	) { strcat ( buf, " no_question" ); }
	if ( comm_flags & COMM_NOMUSIC	) { strcat ( buf, " no_music" ); }
	if ( comm_flags & COMM_NOQUOTE	) { strcat ( buf, " no_quote" ); }
	if ( comm_flags & COMM_COMPACT	) { strcat ( buf, " compact" ); }
	if ( comm_flags & COMM_BRIEF		) { strcat ( buf, " brief" ); }
	if ( comm_flags & COMM_PROMPT	) { strcat ( buf, " prompt" ); }
	if ( comm_flags & COMM_COMBINE	) { strcat ( buf, " combine" ); }
	if ( comm_flags & COMM_NOEMOTE	) { strcat ( buf, " no_emote" ); }
	if ( comm_flags & COMM_NOSHOUT	) { strcat ( buf, " no_shout" ); }
	if ( comm_flags & COMM_NOTELL	) { strcat ( buf, " no_tell" ); }
	if ( comm_flags & COMM_NOCHANNELS	) { strcat ( buf, " no_channels" ); }


	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *imm_bit_name ( int imm_flags )
{
	static char buf[512];

	buf[0] = '\0';

	if ( imm_flags & IMM_SUMMON		) { strcat ( buf, " summon" ); }
	if ( imm_flags & IMM_CHARM		) { strcat ( buf, " charm" ); }
	if ( imm_flags & IMM_MAGIC		) { strcat ( buf, " magic" ); }
	if ( imm_flags & IMM_WEAPON		) { strcat ( buf, " weapon" ); }
	if ( imm_flags & IMM_BASH		) { strcat ( buf, " blunt" ); }
	if ( imm_flags & IMM_PIERCE		) { strcat ( buf, " piercing" ); }
	if ( imm_flags & IMM_SLASH		) { strcat ( buf, " slashing" ); }
	if ( imm_flags & IMM_FIRE		) { strcat ( buf, " fire" ); }
	if ( imm_flags & IMM_COLD		) { strcat ( buf, " cold" ); }
	if ( imm_flags & IMM_LIGHTNING	) { strcat ( buf, " lightning" ); }
	if ( imm_flags & IMM_ACID		) { strcat ( buf, " acid" ); }
	if ( imm_flags & IMM_POISON		) { strcat ( buf, " poison" ); }
	if ( imm_flags & IMM_NEGATIVE	) { strcat ( buf, " negative" ); }
	if ( imm_flags & IMM_HOLY		) { strcat ( buf, " holy" ); }
	if ( imm_flags & IMM_ENERGY		) { strcat ( buf, " energy" ); }
	if ( imm_flags & IMM_MENTAL		) { strcat ( buf, " mental" ); }
	if ( imm_flags & IMM_DISEASE	) { strcat ( buf, " disease" ); }
	if ( imm_flags & IMM_DROWNING	) { strcat ( buf, " drowning" ); }
	if ( imm_flags & IMM_LIGHT		) { strcat ( buf, " light" ); }
	if ( imm_flags & VULN_IRON		) { strcat ( buf, " iron" ); }
	if ( imm_flags & VULN_WOOD		) { strcat ( buf, " wood" ); }
	if ( imm_flags & VULN_SILVER	) { strcat ( buf, " silver" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *wear_bit_name ( int wear_flags )
{
	static char buf[512];

	buf [0] = '\0';
	if ( wear_flags & ITEM_TAKE		) { strcat ( buf, " take" ); }
	if ( wear_flags & ITEM_WEAR_FINGER	) { strcat ( buf, " finger" ); }
	if ( wear_flags & ITEM_WEAR_NECK	) { strcat ( buf, " neck" ); }
	if ( wear_flags & ITEM_WEAR_BODY	) { strcat ( buf, " torso" ); }
	if ( wear_flags & ITEM_WEAR_HEAD	) { strcat ( buf, " head" ); }
	if ( wear_flags & ITEM_WEAR_LEGS	) { strcat ( buf, " legs" ); }
	if ( wear_flags & ITEM_WEAR_FEET	) { strcat ( buf, " feet" ); }
	if ( wear_flags & ITEM_WEAR_HANDS	) { strcat ( buf, " hands" ); }
	if ( wear_flags & ITEM_WEAR_ARMS	) { strcat ( buf, " arms" ); }
	if ( wear_flags & ITEM_WEAR_SHIELD	) { strcat ( buf, " shield" ); }
	if ( wear_flags & ITEM_WEAR_ABOUT	) { strcat ( buf, " body" ); }
	if ( wear_flags & ITEM_WEAR_WAIST	) { strcat ( buf, " waist" ); }
	if ( wear_flags & ITEM_WEAR_WRIST	) { strcat ( buf, " wrist" ); }
	if ( wear_flags & ITEM_WIELD		) { strcat ( buf, " wield" ); }
	if ( wear_flags & ITEM_HOLD		) { strcat ( buf, " hold" ); }
	if ( wear_flags & ITEM_NO_SAC	) { strcat ( buf, " nosac" ); }
	if ( wear_flags & ITEM_WEAR_FLOAT	) { strcat ( buf, " float" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *form_bit_name ( int form_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( form_flags & FORM_POISON	) { strcat ( buf, " poison" ); }
	else if ( form_flags & FORM_EDIBLE	) { strcat ( buf, " edible" ); }
	if ( form_flags & FORM_MAGICAL	) { strcat ( buf, " magical" ); }
	if ( form_flags & FORM_INSTANT_DECAY	) { strcat ( buf, " instant_rot" ); }
	if ( form_flags & FORM_OTHER		) { strcat ( buf, " other" ); }
	if ( form_flags & FORM_ANIMAL	) { strcat ( buf, " animal" ); }
	if ( form_flags & FORM_SENTIENT	) { strcat ( buf, " sentient" ); }
	if ( form_flags & FORM_UNDEAD	) { strcat ( buf, " undead" ); }
	if ( form_flags & FORM_CONSTRUCT	) { strcat ( buf, " construct" ); }
	if ( form_flags & FORM_MIST		) { strcat ( buf, " mist" ); }
	if ( form_flags & FORM_INTANGIBLE	) { strcat ( buf, " intangible" ); }
	if ( form_flags & FORM_BIPED		) { strcat ( buf, " biped" ); }
	if ( form_flags & FORM_CENTAUR	) { strcat ( buf, " centaur" ); }
	if ( form_flags & FORM_INSECT	) { strcat ( buf, " insect" ); }
	if ( form_flags & FORM_SPIDER	) { strcat ( buf, " spider" ); }
	if ( form_flags & FORM_CRUSTACEAN	) { strcat ( buf, " crustacean" ); }
	if ( form_flags & FORM_WORM		) { strcat ( buf, " worm" ); }
	if ( form_flags & FORM_BLOB		) { strcat ( buf, " blob" ); }
	if ( form_flags & FORM_MAMMAL	) { strcat ( buf, " mammal" ); }
	if ( form_flags & FORM_BIRD		) { strcat ( buf, " bird" ); }
	if ( form_flags & FORM_REPTILE	) { strcat ( buf, " reptile" ); }
	if ( form_flags & FORM_SNAKE		) { strcat ( buf, " snake" ); }
	if ( form_flags & FORM_DRAGON	) { strcat ( buf, " dragon" ); }
	if ( form_flags & FORM_AMPHIBIAN	) { strcat ( buf, " amphibian" ); }
	if ( form_flags & FORM_FISH		) { strcat ( buf, " fish" ); }
	if ( form_flags & FORM_COLD_BLOOD 	) { strcat ( buf, " cold_blooded" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *part_bit_name ( int part_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( part_flags & PART_HEAD		) { strcat ( buf, " head" ); }
	if ( part_flags & PART_ARMS		) { strcat ( buf, " arms" ); }
	if ( part_flags & PART_LEGS		) { strcat ( buf, " legs" ); }
	if ( part_flags & PART_HEART		) { strcat ( buf, " heart" ); }
	if ( part_flags & PART_BRAINS	) { strcat ( buf, " brains" ); }
	if ( part_flags & PART_GUTS		) { strcat ( buf, " guts" ); }
	if ( part_flags & PART_HANDS		) { strcat ( buf, " hands" ); }
	if ( part_flags & PART_FEET		) { strcat ( buf, " feet" ); }
	if ( part_flags & PART_FINGERS	) { strcat ( buf, " fingers" ); }
	if ( part_flags & PART_EAR		) { strcat ( buf, " ears" ); }
	if ( part_flags & PART_EYE		) { strcat ( buf, " eyes" ); }
	if ( part_flags & PART_LONG_TONGUE	) { strcat ( buf, " long_tongue" ); }
	if ( part_flags & PART_EYESTALKS	) { strcat ( buf, " eyestalks" ); }
	if ( part_flags & PART_TENTACLES	) { strcat ( buf, " tentacles" ); }
	if ( part_flags & PART_FINS		) { strcat ( buf, " fins" ); }
	if ( part_flags & PART_WINGS		) { strcat ( buf, " wings" ); }
	if ( part_flags & PART_TAIL		) { strcat ( buf, " tail" ); }
	if ( part_flags & PART_CLAWS		) { strcat ( buf, " claws" ); }
	if ( part_flags & PART_FANGS		) { strcat ( buf, " fangs" ); }
	if ( part_flags & PART_HORNS		) { strcat ( buf, " horns" ); }
	if ( part_flags & PART_SCALES	) { strcat ( buf, " scales" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *weapon_bit_name ( int weapon_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( weapon_flags & WEAPON_FLAMING	) { strcat ( buf, " flaming" ); }
	if ( weapon_flags & WEAPON_FROST	) { strcat ( buf, " frost" ); }
	if ( weapon_flags & WEAPON_VAMPIRIC	) { strcat ( buf, " vampiric" ); }
	if ( weapon_flags & WEAPON_SHARP	) { strcat ( buf, " sharp" ); }
	if ( weapon_flags & WEAPON_VORPAL	) { strcat ( buf, " vorpal" ); }
	if ( weapon_flags & WEAPON_TWO_HANDS ) { strcat ( buf, " two-handed" ); }
	if ( weapon_flags & WEAPON_SHOCKING 	) { strcat ( buf, " shocking" ); }
	if ( weapon_flags & WEAPON_POISON	) { strcat ( buf, " poison" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const char *cont_bit_name ( int cont_flags )
{
	static char buf[512];

	buf[0] = '\0';

	if ( cont_flags & CONT_CLOSEABLE	) { strcat ( buf, " closable" ); }
	if ( cont_flags & CONT_PICKPROOF	) { strcat ( buf, " pickproof" ); }
	if ( cont_flags & CONT_CLOSED	) { strcat ( buf, " closed" ); }
	if ( cont_flags & CONT_LOCKED	) { strcat ( buf, " locked" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}


const char *off_bit_name ( int off_flags )
{
	static char buf[512];

	buf[0] = '\0';

	if ( off_flags & OFF_AREA_ATTACK	) { strcat ( buf, " area attack" ); }
	if ( off_flags & OFF_BACKSTAB	) { strcat ( buf, " backstab" ); }
	if ( off_flags & OFF_BASH		) { strcat ( buf, " bash" ); }
	if ( off_flags & OFF_BERSERK		) { strcat ( buf, " berserk" ); }
	if ( off_flags & OFF_DISARM		) { strcat ( buf, " disarm" ); }
	if ( off_flags & OFF_DODGE		) { strcat ( buf, " dodge" ); }
	if ( off_flags & OFF_FADE		) { strcat ( buf, " fade" ); }
	if ( off_flags & OFF_FAST		) { strcat ( buf, " fast" ); }
	if ( off_flags & OFF_KICK		) { strcat ( buf, " kick" ); }
	if ( off_flags & OFF_KICK_DIRT	) { strcat ( buf, " kick_dirt" ); }
	if ( off_flags & OFF_PARRY		) { strcat ( buf, " parry" ); }
	if ( off_flags & OFF_RESCUE		) { strcat ( buf, " rescue" ); }
	if ( off_flags & OFF_TAIL		) { strcat ( buf, " tail" ); }
	if ( off_flags & OFF_TRIP		) { strcat ( buf, " trip" ); }
	if ( off_flags & OFF_CRUSH		) { strcat ( buf, " crush" ); }
	if ( off_flags & ASSIST_ALL		) { strcat ( buf, " assist_all" ); }
	if ( off_flags & ASSIST_ALIGN	) { strcat ( buf, " assist_align" ); }
	if ( off_flags & ASSIST_RACE		) { strcat ( buf, " assist_race" ); }
	if ( off_flags & ASSIST_PLAYERS	) { strcat ( buf, " assist_players" ); }
	if ( off_flags & ASSIST_GUARD	) { strcat ( buf, " assist_guard" ); }
	if ( off_flags & ASSIST_VNUM		) { strcat ( buf, " assist_vnum" ); }

	return ( buf[0] != '\0' ) ? buf + 1 : "none";
}


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


