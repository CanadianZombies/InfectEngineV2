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

#ifndef _FixSkillNumber_H
#define _FixSkillNumber_H
#if defined(Database_Cpp)
	#define SkillNumber(gsn) sh_int gsn;
#else
	#define SkillNumber(gsn) extern sh_int gsn;
#endif

SkillNumber(gsn_backstab);
SkillNumber(gsn_dodge);
SkillNumber(gsn_envenom);
SkillNumber(gsn_hide);
SkillNumber(gsn_peek);
SkillNumber(gsn_pick_lock);
SkillNumber(gsn_sneak);
SkillNumber(gsn_steal)
SkillNumber(gsn_disarm);
SkillNumber(gsn_enhanced_damage);
SkillNumber(gsn_kick);
SkillNumber(gsn_parry);
SkillNumber(gsn_rescue);
SkillNumber(gsn_second_attack);
SkillNumber(gsn_third_attack);
SkillNumber(gsn_blindness);
SkillNumber(gsn_charm_person);
SkillNumber(gsn_curse);
SkillNumber(gsn_invis);
SkillNumber(gsn_mass_invis);
SkillNumber(gsn_plague);
SkillNumber(gsn_poison);
SkillNumber(gsn_sleep);
SkillNumber(gsn_fly);
SkillNumber(gsn_sanctuary);
SkillNumber(gsn_axe);
SkillNumber(gsn_dagger);
SkillNumber(gsn_flail);
SkillNumber(gsn_mace);
SkillNumber(gsn_polearm);
SkillNumber(gsn_shield_block);
SkillNumber(gsn_spear);
SkillNumber(gsn_sword);
SkillNumber(gsn_whip);
SkillNumber(gsn_bash);
SkillNumber(gsn_berserk);
SkillNumber(gsn_dirt);
SkillNumber(gsn_hand_to_hand);
SkillNumber(gsn_trip);
SkillNumber(gsn_fast_healing);
SkillNumber(gsn_haggle);
SkillNumber(gsn_lore);
SkillNumber(gsn_meditation);
SkillNumber(gsn_scrolls);
SkillNumber(gsn_staves);
SkillNumber(gsn_wands);
SkillNumber(gsn_recall);

#endif
