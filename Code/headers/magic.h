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

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN (	spell_null		);
DECLARE_SPELL_FUN (	spell_acid_blast	);
DECLARE_SPELL_FUN (	spell_armor		);
DECLARE_SPELL_FUN (	spell_bless		);
DECLARE_SPELL_FUN (	spell_blindness		);
DECLARE_SPELL_FUN (	spell_burning_hands	);
DECLARE_SPELL_FUN (	spell_call_lightning	);
DECLARE_SPELL_FUN (      spell_calm		);
DECLARE_SPELL_FUN (      spell_cancellation	);
DECLARE_SPELL_FUN (	spell_cause_critical	);
DECLARE_SPELL_FUN (	spell_cause_light	);
DECLARE_SPELL_FUN (	spell_cause_serious	);
DECLARE_SPELL_FUN (	spell_change_sex	);
DECLARE_SPELL_FUN (      spell_chain_lightning   );
DECLARE_SPELL_FUN (	spell_charm_person	);
DECLARE_SPELL_FUN (	spell_chill_touch	);
DECLARE_SPELL_FUN (	spell_colour_spray	);
DECLARE_SPELL_FUN (	spell_continual_light	);
DECLARE_SPELL_FUN (	spell_control_weather	);
DECLARE_SPELL_FUN (	spell_create_food	);
DECLARE_SPELL_FUN (	spell_create_rose	);
DECLARE_SPELL_FUN (	spell_create_spring	);
DECLARE_SPELL_FUN (	spell_create_water	);
DECLARE_SPELL_FUN (	spell_cure_blindness	);
DECLARE_SPELL_FUN (	spell_cure_critical	);
DECLARE_SPELL_FUN (      spell_cure_disease	);
DECLARE_SPELL_FUN (	spell_cure_light	);
DECLARE_SPELL_FUN (	spell_cure_poison	);
DECLARE_SPELL_FUN (	spell_cure_serious	);
DECLARE_SPELL_FUN (	spell_curse		);
DECLARE_SPELL_FUN (      spell_demonfire		);
DECLARE_SPELL_FUN (	spell_detect_evil	);
DECLARE_SPELL_FUN (	spell_detect_good	);
DECLARE_SPELL_FUN (	spell_detect_hidden	);
DECLARE_SPELL_FUN (	spell_detect_invis	);
DECLARE_SPELL_FUN (	spell_detect_magic	);
DECLARE_SPELL_FUN (	spell_detect_poison	);
DECLARE_SPELL_FUN (	spell_dispel_evil	);
DECLARE_SPELL_FUN (      spell_dispel_good       );
DECLARE_SPELL_FUN (	spell_dispel_magic	);
DECLARE_SPELL_FUN (	spell_earthquake	);
DECLARE_SPELL_FUN (	spell_enchant_armor	);
DECLARE_SPELL_FUN (	spell_enchant_weapon	);
DECLARE_SPELL_FUN (	spell_energy_drain	);
DECLARE_SPELL_FUN (	spell_faerie_fire	);
DECLARE_SPELL_FUN (	spell_faerie_fog	);
DECLARE_SPELL_FUN (	spell_farsight		);
DECLARE_SPELL_FUN (	spell_fireball		);
DECLARE_SPELL_FUN (	spell_fireproof		);
DECLARE_SPELL_FUN (	spell_flamestrike	);
DECLARE_SPELL_FUN (	spell_floating_disc	);
DECLARE_SPELL_FUN (	spell_fly		);
DECLARE_SPELL_FUN (      spell_frenzy		);
DECLARE_SPELL_FUN (	spell_gate		);
DECLARE_SPELL_FUN (	spell_giant_strength	);
DECLARE_SPELL_FUN (	spell_harm		);
DECLARE_SPELL_FUN (      spell_haste		);
DECLARE_SPELL_FUN (	spell_heal		);
DECLARE_SPELL_FUN (	spell_heat_metal	);
DECLARE_SPELL_FUN (      spell_holy_word		);
DECLARE_SPELL_FUN (	spell_identify		);
DECLARE_SPELL_FUN (	spell_infravision	);
DECLARE_SPELL_FUN (	spell_invis		);
DECLARE_SPELL_FUN (	spell_know_alignment	);
DECLARE_SPELL_FUN (	spell_lightning_bolt	);
DECLARE_SPELL_FUN (	spell_locate_object	);
DECLARE_SPELL_FUN (	spell_magic_missile	);
DECLARE_SPELL_FUN (      spell_mass_healing	);
DECLARE_SPELL_FUN (	spell_mass_invis	);
DECLARE_SPELL_FUN (	spell_nexus		);
DECLARE_SPELL_FUN (	spell_pass_door		);
DECLARE_SPELL_FUN (      spell_plague		);
DECLARE_SPELL_FUN (	spell_poison		);
DECLARE_SPELL_FUN (	spell_portal		);
DECLARE_SPELL_FUN (	spell_protection_evil	);
DECLARE_SPELL_FUN (	spell_protection_good	);
DECLARE_SPELL_FUN (	spell_ray_of_truth	);
DECLARE_SPELL_FUN (	spell_recharge		);
DECLARE_SPELL_FUN (	spell_refresh		);
DECLARE_SPELL_FUN (	spell_remove_curse	);
DECLARE_SPELL_FUN (	spell_sanctuary		);
DECLARE_SPELL_FUN (	spell_shocking_grasp	);
DECLARE_SPELL_FUN (	spell_shield		);
DECLARE_SPELL_FUN (	spell_sleep		);
DECLARE_SPELL_FUN (	spell_slow		);
DECLARE_SPELL_FUN (	spell_stone_skin	);
DECLARE_SPELL_FUN (	spell_summon		);
DECLARE_SPELL_FUN (	spell_teleport		);
DECLARE_SPELL_FUN (	spell_ventriloquate	);
DECLARE_SPELL_FUN (	spell_weaken		);
DECLARE_SPELL_FUN (	spell_word_of_recall	);
DECLARE_SPELL_FUN (	spell_acid_breath	);
DECLARE_SPELL_FUN (	spell_fire_breath	);
DECLARE_SPELL_FUN (	spell_frost_breath	);
DECLARE_SPELL_FUN (	spell_gas_breath	);
DECLARE_SPELL_FUN (	spell_lightning_breath	);
DECLARE_SPELL_FUN (	spell_general_purpose	);
DECLARE_SPELL_FUN (	spell_high_explosive	);
