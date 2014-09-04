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

DefineCommand ( cmd_heal )
{
	Creature *mob;
	char arg[MAX_INPUT_LENGTH];
	int cost, sn;
	SPELL_FUN *spell;
	const char *words;

	/* check for healer */
	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room ) {
		if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_IS_HEALER ) )
		{ break; }
	}

	if ( mob == NULL ) {
		writeBuffer ( "You can't do that here.\n\r", ch );
		return;
	}

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		/* display price list */
		act ( "$N says 'I offer the following spells:'", ch, NULL, mob, TO_CHAR );
		writeBuffer ( "  light: cure light wounds      10 gold\n\r", ch );
		writeBuffer ( "  serious: cure serious wounds  15 gold\n\r", ch );
		writeBuffer ( "  critic: cure critical wounds  25 gold\n\r", ch );
		writeBuffer ( "  heal: healing spell	      50 gold\n\r", ch );
		writeBuffer ( "  blind: cure blindness         20 gold\n\r", ch );
		writeBuffer ( "  disease: cure disease         15 gold\n\r", ch );
		writeBuffer ( "  poison:  cure poison	      25 gold\n\r", ch );
		writeBuffer ( "  uncurse: remove curse	      50 gold\n\r", ch );
		writeBuffer ( "  refresh: restore movement      5 gold\n\r", ch );
		writeBuffer ( "  mana:  restore mana	      10 gold\n\r", ch );
		writeBuffer ( " Type heal <type> to be healed.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "light" ) ) {
		spell = spell_cure_light;
		sn    = skill_lookup ( "cure light" );
		words = "judicandus dies";
		cost  = 1000;
	}

	else if ( !str_prefix ( arg, "serious" ) ) {
		spell = spell_cure_serious;
		sn    = skill_lookup ( "cure serious" );
		words = "judicandus gzfuajg";
		cost  = 1600;
	}

	else if ( !str_prefix ( arg, "critical" ) ) {
		spell = spell_cure_critical;
		sn    = skill_lookup ( "cure critical" );
		words = "judicandus qfuhuqar";
		cost  = 2500;
	}

	else if ( !str_prefix ( arg, "heal" ) ) {
		spell = spell_heal;
		sn = skill_lookup ( "heal" );
		words = "pzar";
		cost  = 5000;
	}

	else if ( !str_prefix ( arg, "blindness" ) ) {
		spell = spell_cure_blindness;
		sn    = skill_lookup ( "cure blindness" );
		words = "judicandus noselacri";
		cost  = 2000;
	}

	else if ( !str_prefix ( arg, "disease" ) ) {
		spell = spell_cure_disease;
		sn    = skill_lookup ( "cure disease" );
		words = "judicandus eugzagz";
		cost = 1500;
	}

	else if ( !str_prefix ( arg, "poison" ) ) {
		spell = spell_cure_poison;
		sn    = skill_lookup ( "cure poison" );
		words = "judicandus sausabru";
		cost  = 2500;
	}

	else if ( !str_prefix ( arg, "uncurse" ) || !str_prefix ( arg, "curse" ) ) {
		spell = spell_remove_curse;
		sn    = skill_lookup ( "remove curse" );
		words = "candussido judifgz";
		cost  = 5000;
	}

	else if ( !str_prefix ( arg, "mana" ) || !str_prefix ( arg, "energize" ) ) {
		spell = NULL;
		sn = -1;
		words = "energizer";
		cost = 1000;
	}


	else if ( !str_prefix ( arg, "refresh" ) || !str_prefix ( arg, "moves" ) ) {
		spell =  spell_refresh;
		sn    = skill_lookup ( "refresh" );
		words = "candusima";
		cost  = 500;
	}

	else {
		act ( "$N says 'Type 'heal' for a list of spells.'",
			  ch, NULL, mob, TO_CHAR );
		return;
	}

	if ( cost > ( ch->gold * 100 + ch->silver ) ) {
		act ( "$N says 'You do not have enough gold for my services.'",
			  ch, NULL, mob, TO_CHAR );
		return;
	}

	WAIT_STATE ( ch, PULSE_VIOLENCE );

	deduct_cost ( ch, cost );
	mob->gold += cost / 100;
	mob->silver += cost % 100;
	act ( "$n utters the words '$T'.", mob, NULL, words, TO_ROOM );

	if ( spell == NULL ) { /* restore mana trap...kinda hackish */
		ch->mana += Math::instance().dice ( 2, 8 ) + mob->level / 3;
		ch->mana = UMIN ( ch->mana, ch->max_mana );
		writeBuffer ( "A warm glow passes through you.\n\r", ch );
		return;
	}

	if ( sn == -1 )
	{ return; }

	spell ( sn, mob->level, mob, ch, TARGET_CHAR );
}
