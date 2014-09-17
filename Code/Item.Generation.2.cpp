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

/***************************************************************************

                      _____  __      __  __      __
                     /  _  \/  \    /  \/  \    /  \
                    /  /_\  \   \/\/   /\   \/\/   /
                   /    |    \        /  \        /
                   \____|__  /\__/\  /    \__/\  /
                           \/      \/          \/

    As the Wheel Weaves based on ROM 2.4. Original code by Dalsor.
    See changes.log for a list of changes from the original ROM code.
    Credits for code created by other authors have been left
 	intact at the head of each function.

 ***************************************************************************/

/* Random items - not really - code by Dalsor for AWW. */

/*
	else if ( chance chance <= 95 ) {
		obj->item_type = ITEM_NOTEPAPER;
	}

	else if ( chance <= 99 ) {
		obj->item_type = ITEM_DRINK_CON;
	}

}
*/
Item *rand_item ( Item *obj, int level )
{
	char *l_item_name;
	char mat_short[MSL];
	char buf_name[MSL];
	char buf_short[MSL];
	char buf_long[MSL];
	int chance = Math::instance().percent();
	int item_chance = Math::instance().percent();
	int weap_chance = Math::instance().percent();
	int wname_chance = Math::instance().percent();
	int weight = level * 5;
	mat_short[0] = '\0';
	buf_name[0] = '\0';
	buf_short[0] = '\0';
	buf_long[0] = '\0';

	if ( weight > 50 ) { weight = 50; }

	/* determine the item type and include the name if not a weapon */
	if ( item_chance <= 10 ) {
		obj->item_type = ITEM_LOCKPICK;
		strcpy ( l_item_name, "lockpick" );
		obj->wear_flags   = ITEM_TAKE;
		obj->wear_flags   += ITEM_HOLD;
	} else if ( item_chance <= 20 ) {
		obj->item_type = ITEM_PILL;
		strcpy ( l_item_name, "pill" );
		obj->wear_flags   = ITEM_TAKE;
	} else if ( item_chance <= 30 ) {
		obj->item_type = ITEM_SCROLL;
		strcpy ( l_item_name, "scroll" );
		obj->wear_flags   = ITEM_TAKE;
		obj->wear_flags   += ITEM_HOLD;
	} else if ( item_chance <= 40 ) {
		obj->item_type = ITEM_POTION;
		strcpy ( l_item_name, "potion" );
	} else if ( item_chance <= 70 ) {
		obj->item_type = ITEM_ARMOR;
	} else if ( item_chance <= 90 ) {
		obj->item_type = ITEM_WEAPON;
		obj->wear_flags   = ITEM_TAKE;
		obj->wear_flags   += ITEM_WIELD;
	} else if ( item_chance <= 93 ) {
		obj->item_type = ITEM_LIGHT;
		strcat ( l_item_name, "lamp" );
		obj->value[2] = Math::instance().range ( 1, 999 );
	} else if ( item_chance <= 94 ) {
		obj->item_type = ITEM_FOOD;
		strcat ( l_item_name, "meal" );
		obj->value[0] = Math::instance().range ( 1, 99 );
		obj->value[1] = Math::instance().range ( 1, 99 );
		obj->value[2] = 0;
	} else {
		obj->item_type = ITEM_WEAPON;
		obj->wear_flags   = ITEM_TAKE;
		obj->wear_flags   += ITEM_WIELD;
	}

	/* if the type is a weapon, give it a name */
	if ( obj->item_type == ITEM_WEAPON ) {
		if ( weap_chance <= 10 ) {
			obj->value[0] = WEAPON_FLAIL;
			if ( wname_chance <= 25 ) {
				strcpy ( l_item_name, "horseman's flail" );
			} else if ( wname_chance <= 50 ) {
				strcpy ( l_item_name, "footman's flail" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "morning-star" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "flail" );
			}
		} else if ( weap_chance <= 15 ) {
			obj->value[0] = WEAPON_POLEARM;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "staff" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "bo-staff" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "quarter-staff" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "bludgeon" );
			} else if ( wname_chance <= 60 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "cane" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "rod" );
			} else if ( wname_chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "cudgel" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "stick" );
			}
		} else if ( weap_chance <= 20 ) {
			obj->value[0] = WEAPON_WHIP;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "whip" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "scourge" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "blackjack" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "cat-o' nine tails" );
			} else if ( wname_chance <= 60 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "lash" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "flog" );
			} else if ( wname_chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "quirt" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "whip" );
			}
		} else if ( weap_chance <= 25 ) {
			obj->value[0] = WEAPON_MACE;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "mace" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "club" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "hammer" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "warhammer" );
			} else if ( wname_chance <= 60 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "horseman's mace" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "footman's mace" );
			} else if ( wname_chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "beating rod" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "bludgeon" );
			}
		} else if ( weap_chance <= 40 ) {
			obj->value[0] = WEAPON_SPEAR;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "spear" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "pike" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "lance" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "javelin" );
			} else if ( wname_chance <= 70 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "spear" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "spontoon" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "spear" );
			}
		} else if ( weap_chance <= 80 ) {
			obj->value[0] = WEAPON_DAGGER;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "dagger" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "knife" );
			} else if ( wname_chance <= 35 ) {
				strcat ( l_item_name, "dirk" );
				l_item_name[0] = '\0';
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "dagger" );
			}
		} else if ( weap_chance <= 90 ) {
			obj->value[0] = WEAPON_SWORD;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "sword" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "cutlass" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "sabre" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "sword" );
			} else if ( wname_chance <= 70 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "blade" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "falchion" );
			} else if ( wname_chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "broadsword" );
			} else if ( wname_chance <= 95 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "shortsword" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "sword" );
			}
		} else if ( weap_chance >= 80 ) {
			obj->value[0] = WEAPON_AXE;
			if ( wname_chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "axe" );
			} else if ( wname_chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "hand-axe" );
			} else if ( wname_chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "battle-axe" );
			} else if ( wname_chance <= 50 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "war-axe" );
			} else if ( wname_chance <= 70 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "axe" );
			} else if ( wname_chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "woodsman's axe" );
			} else if ( wname_chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "broad-axe" );
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "axe" );
			}
		}
		/* now we assign an attack noun to the weapon */
		switch ( obj->value[0] ) {
			default :
				obj->value[3] = attack_lookup ( "pound" );
				break;
			case ( WEAPON_SWORD ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "slice" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "slash" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "stab" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pierce" ); break; }
				else { obj->value[3] = attack_lookup ( "slash" );	break; }
			case ( WEAPON_DAGGER ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "jab" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "slice" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pierce" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "thrust" ); break; }
				else { obj->value[3] = attack_lookup ( "jab" ); break; }
			case ( WEAPON_SPEAR ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "jab" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "slice" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pierce" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "thrust" ); break; }
				else { obj->value[3] = attack_lookup ( "pierce" ); break; }
			case ( WEAPON_POLEARM ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pound" );	break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "thwack" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "beating" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crunch" ); break; }
				else { obj->value[3] = attack_lookup ( "whack" ); break; }
			case ( WEAPON_MACE ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pound" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crush" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "beating" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crunch" ); break; }
				else { obj->value[3] = attack_lookup ( "pound" ); break; }
			case ( WEAPON_AXE ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "cleave" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crunch" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "slash" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "chop" ); break; }
				else { obj->value[3] = attack_lookup ( "cleave" ); break; }
			case ( WEAPON_FLAIL ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "pound" );	break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crush" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "beating" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "crunch" ); break; }
				else { obj->value[3] = attack_lookup ( "pound" ); break; }
			case ( WEAPON_WHIP ) :
				if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "whip" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "flog" ); break; }
				else if ( Math::instance().percent() < 10 ) { obj->value[3] = attack_lookup ( "sting" ); break; }
				else { obj->value[3] = attack_lookup ( "whip" ); break; }
		}

	}
	/* now let's find a valid material */
	switch ( obj->item_type ) {
		default:
			obj->material_flags = MAT_IRON;
			weight += 10;
			break;
		case ITEM_FOOD:
			obj->material_flags = MAT_FOOD;
			weight += 1;
			break;
		case ITEM_LOCKPICK:
			if ( chance <= 10 ) {
				obj->material_flags = MAT_IRON;
				weight += 10;
				break;
			}
			if ( chance <= 60 ) {
				obj->material_flags = MAT_STEEL;
				weight += 10;
				break;
			}
			if ( chance <= 75 ) {
				obj->material_flags = MAT_WOOD;
				weight += 5;
				break;
			}
			if ( chance <= 90 ) {
				obj->material_flags = MAT_BONE;
				weight += 2;
				break;
			}
			if ( chance <= 10 ) {
				obj->material_flags = MAT_STEEL;
				weight += 5;
				break;
			} else {
				obj->material_flags = MAT_IRON;
				weight += 5;
				break;
			}
		case ITEM_PILL:
			obj->material_flags = MAT_FOOD;
			weight -= 10;
			break;
		case ITEM_SCROLL:
			obj->material_flags = MAT_IVORY;
			weight += 20;
			break;
		case ITEM_POTION:
			obj->material_flags = MAT_GLASS;
			weight += 1;
			break;
		case ITEM_ARMOR:
			if ( chance <= 10 ) {
				obj->material_flags = MAT_BRONZE;
				weight += 8;
				break;
			}
			if ( chance <= 25 ) {
				obj->material_flags = MAT_LEATHER;
				weight += 1;
				break;
			}
			if ( chance <= 60 ) {
				obj->material_flags = MAT_STEEL;
				weight += 10;
				break;
			}
			if ( chance <= 75 ) {
				obj->material_flags = MAT_MITHRIL;
				weight += 3;
				break;
			}
			if ( chance <= 80 ) {
				obj->material_flags = MAT_EBONY;
				weight += 7;
				break;
			}
			if ( chance <= 90 ) {
				obj->material_flags = MAT_DRAGONSCALE;
				weight += 4;
				break;
			}
			if ( chance <= 95 ) {
				obj->material_flags = MAT_DOUBLE_PLATED;
				weight += 15;
				break;
			}
			if ( chance <= 10 ) {
				obj->material_flags = MAT_STEEL;
				weight += 9;
				break;
			} else {
				obj->material_flags = MAT_IRON;
				weight += 9;
				break;
			}
			break;
		case ITEM_WEAPON:
			if ( chance <= 10 ) {
				obj->material_flags = MAT_BRONZE;
				weight += 8;
				break;
			}
			if ( chance <= 25 ) {
				obj->material_flags = MAT_LEATHER;
				weight += 1;
				break;
			}
			if ( chance <= 60 ) {
				obj->material_flags = MAT_STEEL;
				weight += 10;
				break;
			}
			if ( chance <= 75 ) {
				obj->material_flags = MAT_MITHRIL;
				weight += 3;
				break;
			}
			if ( chance <= 80 ) {
				obj->material_flags = MAT_EBONY;
				weight += 7;
				break;
			}
			if ( chance <= 90 ) {
				obj->material_flags = MAT_DRAGONSCALE;
				weight += 4;
				break;
			}
			if ( chance <= 95 ) {
				obj->material_flags = MAT_DOUBLE_PLATED;
				weight += 15;
				break;
			}
			if ( chance <= 10 ) {
				obj->material_flags = MAT_STEEL;
				weight += 9;
				break;
			} else {
				obj->material_flags = MAT_IRON;
				weight += 9;
				break;
			}
			break;
	}
	/* we now have a material, assign a color type corresponding to the material */
	if ( IS_SET ( obj->material_flags, MAT_IRON ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "cold-gray" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "hard-gray" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "dull-gray" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "bright-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "misty-gray" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_STEEL ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "cold-gray" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "hard-gray" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "dull-gray" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "bright-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "misty-gray" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_WOOD ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "light-brown" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "dark-brown" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "nearly black" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "reddish-brown" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "dark red" ); }
		else { strcat ( mat_short, "dark brown" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_BONE ) ) {
		mat_short[0] = '\0';
		if ( chance <= 25 )	     { strcat ( mat_short, "dark-gray" ); }
		else if ( chance <= 50 ) { strcat ( mat_short, "pale white" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "bleached white" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "cold-gray" ); }
		else if ( chance <= 25 ) { strcat ( mat_short, "cobalt-gray" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "hard-gray" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "blue-gray" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "bright-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "silvery-gray" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_FOOD ) ) {
		mat_short[0] = '\0';
		strcat ( mat_short, "plain" );
	} else if ( IS_SET ( obj->material_flags, MAT_LEATHER ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "pale brown" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "light brown" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "dark brown" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "brownish-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "reddish-brown" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else if ( IS_SET ( obj->material_flags, MAT_BRONZE ) ) {
		mat_short[0] = '\0';
		strcat ( mat_short, "plain" );
	} else if ( IS_SET ( obj->material_flags, MAT_DRAGONSCALE ) ) {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "pale brown" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "light brown" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "dark brown" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "brownish-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "reddish-brown" ); }
		else { strcat ( mat_short, "gray-white" ); }
	} else {
		mat_short[0] = '\0';
		if ( chance <= 15 )	     { strcat ( mat_short, "cold-gray" ); }
		else if ( chance <= 35 ) { strcat ( mat_short, "hard-gray" ); }
		else if ( chance <= 55 ) { strcat ( mat_short, "dull-gray" ); }
		else if ( chance <= 75 ) { strcat ( mat_short, "bright-gray" ); }
		else if ( chance <= 85 ) { strcat ( mat_short, "misty-gray" ); }
		else { strcat ( mat_short, "gray-white" ); }
	}

	switch ( obj->item_type ) {
		default:
			break;
		case ITEM_ARMOR:
			if ( chance <= 5 )	     {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "thigh-guard" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_LEGS;
				break;
			} else if ( chance <= 10 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "thigh-plate" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_LEGS;
				break;
			} else if ( chance <= 15 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "greave" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_LEGS;
				break;
			} else if ( chance <= 20 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "belt" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_WAIST;
				break;
			} else if ( chance <= 25 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "shin-guard" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_LEGS;
				break;
			} else if ( chance <= 30 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "bracer" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_WRIST;
				break;
			} else if ( chance <= 35 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "wrist-guard" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_WRIST;
				break;
			} else if ( chance <= 45 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "helm" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_HEAD;
				break;
			} else if ( chance <= 55 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "set of arm-guards" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_ARMS;
				break;
			} else if ( chance <= 65 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "backplate" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_BODY;
				break;
			} else if ( chance <= 70 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "breastplate" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_BODY;
				break;
			} else if ( chance <= 75 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "pair of gauntlets" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_HANDS;
				break;
			} else if ( chance <= 85 ) {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "helmet" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_HEAD;
				break;
			} else {
				l_item_name[0] = '\0';
				strcat ( l_item_name, "helmet" );
				obj->wear_flags   = ITEM_TAKE;
				obj->wear_flags   += ITEM_WEAR_HEAD;
				break;
			}
	}

	obj->weight     = weight;
	obj->condition  = Math::instance().range ( weight * 9, weight * 11 );
	if ( obj->condition > 100 )
	{ obj->condition = 100; }
	obj->cost       = Math::instance().range ( weight * 9, weight * 11 ) * 2;
	//obj->guild      = assign_string ( "Loner" );

	sprintf ( buf_name, "%s %s %s", mat_short, flag_string ( material_flags, obj->material_flags ), l_item_name );
	PURGE_DATA ( obj->name );
	obj->name = assign_string ( buf_name );

	sprintf ( buf_short, "a %s %s %s", mat_short, flag_string ( material_flags, obj->material_flags ), l_item_name );
	PURGE_DATA ( obj->short_descr );
	obj->short_descr = assign_string ( buf_short );

	sprintf ( buf_long, "Here lies a %s %s %s.", mat_short, flag_string ( material_flags, obj->material_flags ), l_item_name );
	PURGE_DATA ( obj->description );
	obj->description = assign_string ( buf_long );

	return obj;
}

int rand_spell( )
{
	if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure blinding" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure critical" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure disease" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure serious" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure light" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "cure poison" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "heal" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "defense" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "vigor" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "shield" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "fire shield" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "large" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "glow" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "see shadow" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "see light" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "seeing" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "see hidden" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "see invis" ); }
	else if ( Math::instance().percent() < 10 ) { return skill_lookup ( "smell poison" ); }
	else { return skill_lookup ( "invert" ); }
}
Item *make_rand_special ( Item *fObj )
{
	int chance = Math::instance().range ( 1, 12 );
	Affect *af = new_affect();
	switch ( chance ) {
		case 1:
			af->location   =   APPLY_STR;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 2:
			af->location   =   APPLY_DEX;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 3:
			af->location   =   APPLY_CON;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 4:
			af->location   =   APPLY_WIS;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 5:
			af->location   =   APPLY_INT;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 6:
			af->location   =   APPLY_WIS;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 7:
			af->location   =   APPLY_STR;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 8:
			af->location   =   APPLY_CON;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 9:
			af->location   =   APPLY_HITROLL;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 10:
			af->location   =   APPLY_DAMROLL;
			af->modifier   =   Math::instance().range ( 1, 20 );
			break;
		case 11:
			af->location   =   APPLY_HIT;
			af->modifier   =   ( Math::instance().range ( 1, 10 ) * 5 );
			break;
		case 12:
			af->location   =   APPLY_MANA;
			af->modifier   =   ( Math::instance().range ( 1, 10 ) * 5 );
			break;
		case 13:
			af->location   =   APPLY_MOVE;
			af->modifier   =   ( Math::instance().range ( 1, 10 ) * 5 );
			break;
		case 14:
			af->location   =   APPLY_AC;
			af->modifier   =   - ( Math::instance().range ( 1, 10 ) * 5 );
			break;
	}
	af->where	   =   TO_OBJECT;
	af->type       =   -1;
	af->duration   =   -1;
	af->bitvector  =   0;
	af->level      =   250;
	af->next       =   fObj->affected;
	fObj->affected =   af;
	fObj->cost += ( af->modifier * 20 );
	return ( fObj );
}

Item *make_treasure_item ( int level )
{
	Item *obj;

	bool remort = FALSE;
	int oWeight;

	if ( level < 1 ) { level = 1; }
	/*if ( Math::instance().percent() / 2 > Math::instance().percent() ) remort = TRUE;*/

	obj = create_object ( get_obj_index ( OBJ_VNUM_GEM ), 0 );

	rand_item ( obj, level );
	oWeight = Math::instance().range ( obj->weight / 4, obj->weight / 3 );
	obj->level = 1;

	switch ( obj->item_type ) {

		case ITEM_WEAPON:
			if ( obj->item_type != ITEM_WEAPON )
			{ break; } /* what the fuck happened... boom */
			if ( !CAN_WEAR ( obj, ITEM_TAKE ) ) /* set take if not set */
			{ obj->wear_flags   = ITEM_TAKE; }
			obj->value[1]     = remort ? 2 : 1; /* if the obj is remort, times 2 */
			obj->value[2]     = Math::instance().range ( oWeight * 1.75, oWeight * 2.5 ); /* sick */
			break;

		case ITEM_ARMOR:
			obj->value[0]     = remort ? Math::instance().range ( oWeight, oWeight * 2 ) /* more sick */
								: Math::instance().range ( oWeight * 2, oWeight * 3 );
			obj->value[1]     = remort ? Math::instance().range ( oWeight, oWeight * 2 )
								: Math::instance().range ( oWeight * 2, oWeight * 3 );
			obj->value[2]     = remort ? Math::instance().range ( oWeight, oWeight * 2 )
								: Math::instance().range ( oWeight * 2, oWeight * 3 );
			obj->value[3]     = remort ? Math::instance().range ( oWeight, oWeight * 2 )
								: Math::instance().range ( oWeight * 2, oWeight * 3 );
			break;

		case ITEM_LOCKPICK:
			obj->condition    = level / 2;
			obj->value[0]     = level / 3 + obj->condition; /* might be too high */
			break;

		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			obj->value[0]     = level / 2;
			obj->value[1]     = rand_spell();
			obj->value[2]     = rand_spell();
			obj->value[3]     = rand_spell();
			obj->value[4]     = rand_spell();
			break;
	}
	if ( obj->condition > 100 )
	{ obj->condition = 100; }
	make_rand_special ( obj );
	return ( obj );
}
