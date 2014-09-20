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

/* Treasure code by Dalsor for AWW. */

/* This code generates a treasure chest, populates it with
 * coins, gemstones, and possibly a random item. The gems
 * and coins come from this file, the items from random.c
 * Chests require lockpicks to open, which is a ITEM_LOCKPICK
 * kinda thing with a v0 value ranging from 1-100.
 *
 * Dalsor. http://aww-mud.org 6500
 * AWW - As the Wheel Weaves
 */


Item *	make_treasure_item	args ( ( int level ) );
void make_gem ( int level, Item *box )
{
	Item *gem;

	char gem_short[MSL];
	char gem_long[MSL];
	char gem_quality[MSL];
	char gem_size[MSL];
	char gem_name[MSL];
	const char *gem_quality_table[] = {"flawed", "chipped", "shiny", "sparkling", "flawless"};
	const char *gem_size_table[]    =	{"tiny", "small", "medium", "large"};
	const char *gem_name_table[]    =	{"cat's eye gem-stone", "round bloodstone", "clear-white topaz",
										 "angel's skin coral", " blue spinel", " clear quartz",
										 "fire-colored agate", "yellow-gold crysoberyl", "black moonstone",
										 "bright-green peridot", "amber gemstone", "yellow citrine gemstone",
										 "yellow garnet", "smokey-colored quartz", "milky-white pearl",
										 "reddish-pink tourmaline", "rock-crystal", "fire opal",
										 "blue sapphire", "diamond", "ruby-red alexandrite",
										 "yellowish-orange jade stone", "gray-white jade stone",
										 "vibrant purple amethyst", "rose-colored quartz", "bluish-colored quartz",
										 "milky-white topaz", "angel's skin coral", "greenish-blue aquamarine",
										 "pale-blue turquoise", "pink sapphire", "green emerald", "rock-crystal",
										 "yellowed ivory", "bright green beryl stone", "blue turquoise",
										 "light red morganite stone", "black pearl", "black opal", "cat's-eye beryl gemstone",
										 "slab of pure white ivory", "silver nugget", "gold nugget",
										 "black onyx stone", "golden colored sapphire", "white pearl",
										 "red ruby", "light pink conch pearl", "reddish-pink alexandrite",
										 "demantoid garnet stone", "rose-colored quartz", "green beryl stone",
										 "lapis lazuli stone", "opaque colored jasper", "deep blue tigereye stone",
										 "reddish-brown sardonyx", "sea-green aquamarine gem", "milk-and-honey colored cat's-eye chrysoberyl",
										 "star-diopside", "cat's-eye diopside", "reddish-cast sunstone",
										 "silver bar", "gold bar", "jet stone", "snowflake obsidian", "imperial topaz",
										 "star ruby", "turquoise stone", "demantoid garnet stone"
									  };

	const char *gem_name_table2[] = {"small bloodstone, sculpted into a tiny female figurine",
									 "small carving of petrified wood, in the form of a woodsman",
									 "rose crafted from seafolk porcelain",
									 "small ruby lion, reaching forward with one paw",
									 "rectangular piece of ivory, etched with a glowing sun",
									 "emerald replica of an avendesora leaf",
									 "small pony, crafted from seafolk porcelain",
									 "silverpike, fashioned from crystal edged with silver flecks",
									 "miniature seafolk raker, crafted from polished petrified wood",
									 "white quartz seagull",
									 "oyster shell, made from mother-of-pearl",
									 "silver medallion etched with a swooping falcon",
									 "gold medallion, etched with a running wolf",
									 "miniature jade wagon, etched in intricate detail",
									 "small handful of brilliant red fire drops",
									 "small moonstone obelisk",
									 "miniature copper horn, encrusted with several tiny pearls",
									 "perfect replica of a golden crane",
									 "perfect onyx hawk"
									};


	int i, value = 1, quality = 0, size = 0;
	int gem_name_int = 0, gem_name2_int = 0;

	gem_short[0]    = '\0';
	gem_long[0]     = '\0';
	gem_name[0]     = '\0';
	gem_size[0]     = '\0';
	gem_quality[0]  = '\0';


	if ( level < 1 ) { level = 1; }
	if ( box == NULL ) { return; }

	gem = create_object ( get_obj_index ( OBJ_VNUM_GEM ), 0 );

	i = Math::instance().range ( 1, level );
	i = i * 9 / 11;
	i = Math::instance().range ( 1, i );

	if ( i > 220 ) {
		i = Math::instance().range ( 1, level );
		i = i * 9 / 11;
		i = Math::instance().range ( 1, i );
	}

	quality       = Math::instance().range ( 1, 5 );
	size          = Math::instance().range ( 1, 4 );
	gem_name_int  = Math::instance().range ( 1, 69 );
	gem_name2_int =	Math::instance().range ( 1, 19 );

	/* Tried to use strcpy and strcat in here, but it didn't work.
	 * should have, used it the same way in other functions, and
	 * it worked.... oh well.
	 */

	/*PURGE_DATA(gem_quality);*/
	/*strcpy(gem_quality,gem_quality_table[quality - 1]);*/
	strcpy ( gem_quality, gem_quality_table[quality - 1] );

	/*PURGE_DATA(gem_size);*/
	strcpy ( gem_size, gem_size_table[size - 1] );

	if ( i < 200 ) {
		/*PURGE_DATA(gem_name);*/
		sprintf ( gem_name, "%s", gem_name_table[gem_name_int - 1] );
	} else {
		/*PURGE_DATA(gem_name);*/
		sprintf ( gem_name, "%s", gem_name_table2[gem_name2_int - 1] );
	}
	value = Math::instance().range ( i, i + 50 + ( quality * 10 ) );
	if ( value < 1 ) { value = 1; }

	// set the buffer we will use for the short desc
	/*PURGE_DATA(gem_short);*/
	sprintf ( gem_short, "a %s %s %s", gem_size, gem_quality, gem_name );

	// set the buffer we will use for the long desc
	sprintf ( gem_long, "Here lies a %s.", gem_short );

	/*PURGE_DATA(gem->short_descr);*/
	gem->short_descr = assign_string ( gem_short );
	/*PURGE_DATA(gem->description);*/
	gem->description = assign_string ( gem_long );
	/*PURGE_DATA(gem->name);*/
	gem->name = assign_string ( gem_name );

	gem->wear_flags     = ITEM_TAKE;
	//	gem->guild          = assign_string("Loner");
	gem->condition      = 100;
	gem->weight         = 1;
	gem->cost           = value;

	obj_to_obj ( gem, box );


}

void make_treasure_chest ( Creature *ch, Creature *victim, int level )
{
	Item *box;

	char box_type[MSL];
	char box_keywd[MSL];
	char box_desc[MSL];
	char buf_name[MSL];
	char buf_short[MSL];
	char buf_long[MSL];
	const char *box_desc_store[] = {"iron", "wood", "brass"};
	const char *box_keywd_store[] = {"rusted", "sturdy", "heavy"};
	const char *box_keywd_storeWB[] = {"rotting", "sturdy", "heavy"};
	const char *box_type_store[] = {"chest", "trunk", "coffer", "coinbox", "strongbox", "safebox"};
	int desc_num  = 0;
	int coins  = 0;
	int chance = 0;

	box_type[0]  = '\0';
	box_keywd[0] = '\0';
	box_desc[0]  = '\0';
	buf_name[0]  = '\0';
	buf_short[0] = '\0';
	buf_long[0]  = '\0';

	/* Begin Creating Treasure Box
	   Treasure Box:  chest, trunk, coffer, coinbox, strongbox, safebox
	   Use pointer arrays to fill values for variables
	   box_type
	   box_desc
	   box_keywd */

	strcat ( box_type, box_type_store[Math::instance().range ( 1, 6 ) - 1] );
	desc_num = Math::instance().range ( 1, 3 );
	strcat ( box_desc, box_desc_store[desc_num - 1] );
	if ( desc_num == 2 ) { /*if its a wood box to this*/
		strcat ( box_keywd , box_keywd_storeWB[Math::instance().range ( 1, 3 ) - 1] );
	} else { /*if its a metal box do this*/
		strcat ( box_keywd , box_keywd_store[Math::instance().range ( 1, 3 ) - 1] );
	}
	/*End Creating Treasure Box */

	sprintf ( buf_name, "%s %s %s", box_keywd, box_desc, box_type );
	sprintf ( buf_short, "a %s %s %s", box_keywd, box_desc, box_type );
	sprintf ( buf_long, "Here lies a %s %s %s.", box_keywd, box_desc, box_type );

	box = create_object ( get_obj_index ( OBJ_VNUM_TREASURE_CHEST ), 0 );

	if ( desc_num == 2 ) { /*if its a wood box to this*/
		SET_BIT ( box->material_flags, MAT_WOOD );
	} else { /*if its a metal box do this*/
		SET_BIT ( box->material_flags, MAT_IRON );
	}

	PURGE_DATA ( box->short_descr );
	box->short_descr = assign_string ( buf_short );
	PURGE_DATA ( box->description );
	box->description = assign_string ( buf_long );
	PURGE_DATA ( box->name );
	box->name = assign_string ( buf_name );

	if ( box->extra_flags )
	{ box->extra_flags = 0; }

	box->wear_flags     = ITEM_TAKE;
	box->item_type      = ITEM_TREASURECHEST;
	if ( !strcmp ( box_keywd, "iron" ) )
	{ box->weight     = 50; }
	else if ( !strcmp ( box_keywd, "brass" ) )
	{ box->weight     = 40; }
	else if ( !strcmp ( box_keywd, "wood" ) )
	{ box->weight     = 30; }
	box->timer          = Math::instance().range ( level * 8 / 12, level * 9 / 11 );
	box->value[0]       = Math::instance().range ( 1, level ); /* silver coins */
	box->value[1]       = CONT_CLOSEABLE; /* container-type flags */
	box->value[1]       += CONT_CLOSED; /* container-type flags */
	box->value[1]       += CONT_LOCKED; /* container-type flags */
	box->value[2]       = Math::instance().range ( level * 8 / 12, level * 9 / 11 ); /* box lock difficulty */
	switch ( Math::instance().range ( 1, 10 ) ) {
		case 1:
			box->value[3]   = TRAP_BOOMER;
			break;
		case 2:
			box->value[3]   = TRAP_GASSER;
			break;
		case 3:
			box->value[3]   = TRAP_BLADE;
			break;
		case 4:
			box->value[3]   = TRAP_NEEDLE;
			break;
		case 5:
			box->value[3]   = TRAP_MANA;
			break;
		case 6:
			box->value[3]   = TRAP_FIRE;
			break;
		case 7:
			box->value[3]   = TRAP_ACID;
			break;
		default:
			box->value[3]   = TRAP_NEEDLE;
			break;
	}
	box->value[4]       = level;
	//box->guild          = assign_string ( "Loner" );
	box->condition      = 100;
	box->cost           = 1;

	/* put some coins in the box */
	coins = box->value[0];
	if ( coins < 20 ) { coins = 20; }
	obj_to_obj ( create_money ( coins, coins / 3 ), box );

	/* see if the box has any gems */
	chance = Math::instance().percent();
	if ( chance < Math::instance().percent() ) {
		switch ( Math::instance().range ( 1, 6 ) ) {
			case 1:
				make_gem ( level, box );
				make_gem ( level, box );
				break;
			case 2:
				make_gem ( level, box );
				make_gem ( level, box );
				break;
			case 3:
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				break;
			case 4:
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				break;
			case 5:
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				break;
			case 6:
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				make_gem ( level, box );
				obj_to_obj ( make_treasure_item ( level ), box );
				break;
		}
	}
	act ( "$N carried $t, which falls to the ground.", ch, capitalize ( box->short_descr ), victim, TO_ROOM );
	act ( "$N carried $t, which falls to the ground.", ch, capitalize ( box->short_descr ), victim, TO_CHAR );
	obj_to_room ( box, IN_ROOM ( ch ) );
	return;
}
