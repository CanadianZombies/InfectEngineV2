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

#ifndef _Vnums_H
#define _Vnums_H

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_FIDO		      1
#define MOB_VNUM_CITYGUARD	      2
#define MOB_VNUM_VAMPIRE	      3

#define MOB_VNUM_PATROLMAN	      4
#define GROUP_VNUM_TROLLS	   2100	// -- will be replaced with other shinanagins.
#define GROUP_VNUM_OGRES	   2101 // -- Yes, it will be replaced!


/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE	      1
#define OBJ_VNUM_GOLD_ONE	      2
#define OBJ_VNUM_GOLD_SOME	      3
#define OBJ_VNUM_SILVER_SOME	      4
#define OBJ_VNUM_COINS		      5

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_GUTS		     16
#define OBJ_VNUM_BRAINS		     17

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_DISC		     23
#define OBJ_VNUM_PORTAL		     25
#define OBJ_VNUM_DUMMY	30
#define OBJ_VNUM_ROSE		     31

#define OBJ_VNUM_PIT		     32

#define OBJ_VNUM_SCHOOL_MACE	     33
#define OBJ_VNUM_SCHOOL_DAGGER	     34
#define OBJ_VNUM_SCHOOL_SWORD	     35
#define OBJ_VNUM_SCHOOL_SPEAR	     39
#define OBJ_VNUM_SCHOOL_STAFF	     40
#define OBJ_VNUM_SCHOOL_AXE	     41
#define OBJ_VNUM_SCHOOL_FLAIL	     42
#define OBJ_VNUM_SCHOOL_WHIP	     43
#define OBJ_VNUM_SCHOOL_POLEARM      44

#define OBJ_VNUM_SCHOOL_VEST	   36
#define OBJ_VNUM_SCHOOL_SHIELD	   37
#define OBJ_VNUM_SCHOOL_BANNER     38

#define OBJ_VNUM_MAP		   45
#define OBJ_VNUM_WHISTLE	   46


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		      2
#define ROOM_VNUM_TEMPLE	      2
#define ROOM_VNUM_ALTAR		      2
#define ROOM_VNUM_SCHOOL	      2


#endif