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

void set_material args ( ( Item *obj, int bit ) );
void rem_material args ( ( Item *obj, int bit ) );
void create_random ( Creature * mob, const char *argument );
void make_armor ( Item* obj, Creature* mob, int random_vnum, int item_type, int wear_flags, const char* wear_name );
#define MAX_VERBS 40
const char *verb_types[] = {
	"expensive looking",
	"inscrutable",
	"ugly",
	"old",
	"amazing",
	"uncommon",
	"ornate",
	"splendid",
	"ancient",
	"dusty",
	"scratched",
	"flawed",
	"burnt",
	"heavy",
	"gilded",
	"spooky",
	"plain",
	"dirty",
	"wrinkled",
	"shiny",
	"warped",
	"cheaply made",
	"flawed",
	"burnt",
	"slightly used",
	"well used",
	"slightly used",
	"sought after",
	"rare looking",
	"common",
	"unique looking",
	"breath taking",
	"colorful",
	"divinely carved",
	"dirty",
	"chipped",
	"blood stained",
	"dull looking",
	"tinted",
	"shiny",
	"fragile looking"
};

const char *some[] = {
	"some",
	"a pair of",
	"a set of",
	"two"
};

#define MAX_LIGHT 2
const char *light_types[] = {
	"lantern",
	"lamp",
	"light"
};


#define MAX_RING 1
const char *ring_types[] = {
	"ring",
	"band"
};

#define MAX_NECK 16
const char *neck_types[] = {
	"cloak",
	"cape",
	"gorget",
	"bevor",
	"aventail",
	"neckguard",
	"necklace",
	"pendant",
	"charm",
	"circlet",
	"choker",
	"collar",
	"gorget",
	"locket",
	"beads",
	"bangle",
	"brooch"
};

#define MAX_TRINKET 9
const char *trinket_types[] = {
	"bauble",
	"stone",
	"charm",
	"fetish",
	"bone",
	"trinket",
	"amulet",
	"juju",
	"periapt",
	"talisman"
};

#define MAX_WRIST 10
const char *wrist_types[] = {
	"bracer",
	"bracelet",
	"wristguard",
	"wristband",
	"band",
	"armlet",
	"circlet",
	"manacle",
	"arm band",
	"wristlet",
	"shackle"
};

#define MAX_FACE 5
const char *face_types[] = {
	"mask",
	"faceplate",
	"faceguard",
	"facemask",
	"pigface guard",
	"lip spike"
};

#define MAX_HEAD 12
const char *head_types[] = {
	"helm",
	"war helm",
	"battle helm",
	"viking war helmet",
	"cabasset",
	"visor",
	"coif",
	"great helm",
	"cervelliere",
	"sallet",
	"bascinet",
	"barbute",
	"close helm"
};

#define MAX_WAIST 4
const char *waist_types[] = {
	"belt",
	"girth",
	"girdle",
	"chastity belt",
	"sash"
};

#define MAX_SOCKET 1
const char *socket_types[] = {
	"gem",
	"jewel"
};

#define MAX_HAND 6
const char *hand_types[] = {
	"battle gloves",
	"gauntlets",
	"finger gauntlets",
	"mitten gauntlets",
	"cuffs",
	"mitts",
	"gloves"
};

#define MAX_FEET 6
const char *feet_types[] = {
	"boots",
	"shoes",
	"sandles",
	"clogs",
	"sollerets",
	"gaiters",
	"sabots"
};

#define MAX_LEGS 6
const char *legs_types[] = {
	"leggings",
	"leg plates",
	"cuisses",
	"chausses",
	"greaves",
	"tassets",
	"poleyn"
};

#define MAX_ARMS 4
const char *arms_types[] = {
	"sleeves",
	"vambraces",
	"bracers",
	"armbands",
	"arm plates"
};

#define MAX_SHIELD 20
const char *shield_types[] = {
	"shield",
	"buckler",
	"oval shield",
	"oval buckler",
	"continental shield",
	"continental buckler",
	"beveled shield",
	"beveled buckler",
	"ornate shield",
	"ornate buckler",
	"angled shield",
	"angled buckler",
	"edged shield",
	"edged buckler",
	"war shield",
	"battle shield",
	"tower shield",
	"kite shield",
	"aspis",
	"targe",
	"aegis"
};

#define MAX_BODY 4
const char *body_types[] = {
	"breastplate",
	"chestplate",
	"body armor",
	"torso",
	"vest"
};

#define MAX_ABOUT 1
const char *about_types[] = {
	"poncho",
	"pouldron"
};

void affect_strip_obj ( Item *obj )
{
	Affect *paf;
	Affect *paf_next;
	for ( paf = obj->affected; paf != NULL; paf = paf_next ) {
		paf_next = paf->next;
		if ( paf->type == gsn_reserved )
		{ affect_remove_obj ( obj, paf ); }
	}
	return;
}

/*I cant remember where I got this paf_set from, it only bugs out
when trying to add a bitvector. this code WONT make that call since I
add bits in a different place, but if you wanna use this to add bits
you will need to debug it*/
void paf_set ( Item *obj, int types, int modifier, int bitvector )
{
	Affect *paf = new_affect();
	paf->where = TO_OBJECT;
	paf->type = 0;
	paf->level = obj->level;
	paf->location = types;
	paf->duration = -1;
	paf->modifier = modifier;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;

	if ( bitvector ) {
		paf->where = TO_AFFECTS;
		paf->bitvector = bitvector;
	}
}

void paf_apply ( Item *obj )
{
	/*These are the mods im using, everything is based on a max level mob
	of 60, equipment wear slots, equipment material, equipment weight and item type*/
	static int stat_types[] = {
		APPLY_STR, APPLY_STR, APPLY_DEX, APPLY_INT, APPLY_WIS, APPLY_CON
	};
	static int save_types[] = {
		/*I DEFAULTED THIS TO SAVING SPELL, I HAVE 5 DIFFERENT SAVING
		SPELL TYPES, FEEL FREE TO ADD/CHANGE/MODIFY TO SUIT YOUR CODE:)*/
		APPLY_SAVING_SPELL,
		APPLY_SAVING_SPELL,
		APPLY_SAVING_SPELL,
		APPLY_SAVING_SPELL,
		APPLY_SAVING_SPELL
	};
	static int combat_types[] = {
		APPLY_HITROLL, APPLY_DAMROLL
	};
	static int gain_types[] = {
		APPLY_MANA, APPLY_HIT, APPLY_MOVE
	};
	static int ac_types[] = {
		APPLY_AC
	};

	/*i LEFT THESE HERE FOR YOU TO LOOK AT, I HAVE EXTENDED BITS IN MY
	CODE AND DOUBLE THE MATERIALS, I HAVE THEM BROKEN UP INTO GROUPS OF
	MATERIALS BASED AROUND ARMOR/WEAPONS, RINGS, WANDS/STAVES*/

	/*static char *defensive_spell_types[] =
	  {
	  "armor", "shield", "stone skin", "bless", "sanctuary"
	  };

	static char *offensive_spell_types[] =
	  {
	  "lightning bolt", "acid blast", "magic missile"
	  };

	static char *curitive_spell_types[] =
	  {
	  "create food", "create spring", "cure blindness", "cure critical", "cure disease",
	"cure poison", "remove curse", "refresh"
	  };

	static char *maledictive_spell_types[] =
	  {
	  "blindness", "curse", "plague", "poison", "sleep", "weaken"
	  };

	static char *mental_spell_types[] =
	  {
	"charm person", "dispel magic", "calm", "change sex", "floating disc"
	  };

	static char *misc_spell_types[] =
	  {
	"slow", "fly", "pass door", "frenzy", "giant strength",
	"haste", "word of recall"
	  };

	if(obj->item_type == ITEM_STAFF)
	{
	if (IS_SET(obj->material_flags, MAT_ORTHOCLASE))
	obj->value[3] = skill_lookup(defensive_spell_types[number_range(0, 4)]);
	  if (IS_SET(obj->material_flags, MAT_PLAGIOCLASE))
	obj->value[3] = skill_lookup(curitive_spell_types[number_range(0, 7)]);
	  if (IS_SET(obj->material_flags, MAT_BIOTITE))
	obj->value[3] = skill_lookup(misc_spell_types[number_range(0, 6)]);
	}
	else if(obj->item_type == ITEM_WAND)
	{
	  if (IS_SET(obj->material_flags, MAT_QUARTZ))
	obj->value[3] = skill_lookup(offensive_spell_types[number_range(0, 3)]);
	  if (IS_SET(obj->material_flags, MAT_PROXENE))
	obj->value[3] = skill_lookup(maledictive_spell_types[number_range(0, 5)]);
	  if (IS_SET(obj->material_flags, MAT_OLIVINE))
	obj->value[3] = skill_lookup(mental_spell_types[number_range(0, 4)]);
	}
	else if(obj->item_type == ITEM_JEWELRY )
	{
	//A ring is based on its stone, lets keep all rings
	//of the same stone all the same.
	//I HAD ABOUT 15 DIFF RING STONES
	if (IS_SET(obj->material_flags, MAT_GOLD))
	{
	paf_set(obj, stat_types[2], -2, 0);
	paf_set(obj, combat_types[0], 3, 0);
	}
	if (IS_SET(obj->material_flags, MAT_SILVER))
	{
	paf_set(obj, stat_types[2], -2, 0);
	paf_set(obj, combat_types[1], 3, 0);
	}
	}
	else */if ( obj->item_type == ITEM_ARMOR ) {

		/*Low lvl gets nothing unless its lucky, leather to steel*/
		if ( obj->level <= 23 && number_percent ( ) >= 10 )
		{ return; }

		/*Certon wear slots give different affects'ish, things like
		earrings dont give off -ac, ect.*/
		if ( IS_SET ( obj->wear_flags, ITEM_HOLD )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_FLOAT )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_FINGER ) ) {
			paf_set ( obj, save_types[number_range ( 0, 4 )], number_range ( -10, -1 ), 0 );
		}
		/*Some equipment is better than others in their own respects, I have
		if something sucks 1 way it should be better in another.*/
		else if ( IS_SET ( obj->wear_flags, ITEM_WEAR_NECK )
				  || IS_SET ( obj->wear_flags, ITEM_WEAR_WRIST ) ) {
			if ( IS_SET ( obj->material_flags, MAT_ONYX ) )
			{ paf_set ( obj, combat_types[0], 1, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_OPAL ) )
			{ paf_set ( obj, combat_types[1], 1, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED ) ) {
				paf_set ( obj, combat_types[1], 2, 0 );
				paf_set ( obj, combat_types[0], 1, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_OBSIDIAN ) ) {
				paf_set ( obj, combat_types[1], 2, 0 );
				paf_set ( obj, combat_types[0], 2, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_GOLD ) ) {
				paf_set ( obj, save_types[0], -2, 0 ); //gold is good against afflictive
				paf_set ( obj, combat_types[0], 2, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_SILVER ) ) {
				paf_set ( obj, save_types[1], -2, 0 ); //silver is good against mental
				paf_set ( obj, combat_types[1], 2, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_PLATINUM ) ) {
				paf_set ( obj, save_types[number_range ( 0, 4 )], -2, 0 ); //platinum is a mixed bag for a price
			} else if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL ) ) { //tainted is good against breath
				paf_set ( obj, save_types[0], number_range ( -5, -1 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_MITHRIL ) ) { // mithril is good against malidictions
				paf_set ( obj, save_types[2], number_range ( -5, -1 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_TITANIUM ) ) {
				paf_set ( obj, stat_types[0], 1, 0 ); //luck bonus
				paf_set ( obj, combat_types[1], 1, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) ) {
				paf_set ( obj, stat_types[0], 1, 0 ); //luck bonus
				paf_set ( obj, combat_types[0], 1, 0 );
			} else
				//stats for low level luckys
			{ paf_set ( obj, stat_types[number_range ( 0, 5 )], number_range ( 1, 3 ), 0 ); }

		} else if ( IS_SET ( obj->wear_flags, ITEM_WEAR_HEAD )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_WAIST )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_HANDS )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_FEET )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_LEGS )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_ARMS ) ) {
			if ( IS_SET ( obj->material_flags, MAT_ONYX ) )
			{ paf_set ( obj, combat_types[0], 2, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_OPAL ) )
			{ paf_set ( obj, combat_types[1], 2, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED ) ) {
				paf_set ( obj, combat_types[1], 2, 0 );
				paf_set ( obj, combat_types[0], 1, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_OBSIDIAN ) ) {
				paf_set ( obj, combat_types[1], 1, 0 );
				paf_set ( obj, combat_types[0], 1, 0 );
				paf_set ( obj, save_types[number_range ( 0, 4 )], -3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_GOLD ) ) {
				paf_set ( obj, save_types[0], -3, 0 ); //gold is good against afflictive
				paf_set ( obj, combat_types[0], 3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_SILVER ) ) {
				paf_set ( obj, save_types[1], -3, 0 ); //silver is good against mental
				paf_set ( obj, combat_types[1], 3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_PLATINUM ) ) {
				paf_set ( obj, save_types[number_range ( 0, 4 )], -5, 0 ); //platinum is a mixed bag for a price
			} else if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL ) ) { //tainted is good against breath
				paf_set ( obj, save_types[0], number_range ( -7, -3 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_MITHRIL ) ) { // mithril is good against malidictions
				paf_set ( obj, save_types[2], number_range ( -7, -3 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_TITANIUM ) ) {
				paf_set ( obj, stat_types[0], 1, 0 ); //luck bonus
				paf_set ( obj, combat_types[0], 1, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) ) {
				paf_set ( obj, stat_types[0], 1, 0 ); //luck bonus
				paf_set ( obj, combat_types[1], 1, 0 );
			} else
				//gains for low level luckys
			{ paf_set ( obj, gain_types[number_range ( 0, 2 )], number_range ( 1, obj->level / 2 ), 0 ); }
		} else if ( IS_SET ( obj->wear_flags, ITEM_WEAR_BODY )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_ABOUT )
					|| IS_SET ( obj->wear_flags, ITEM_WEAR_SHIELD ) ) {
			if ( IS_SET ( obj->material_flags, MAT_ONYX ) )
			{ paf_set ( obj, combat_types[0], 2, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_OPAL ) )
			{ paf_set ( obj, combat_types[1], 2, 0 ); }

			else if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED ) ) {
				paf_set ( obj, save_types[4], number_range ( 5, 10 ), 0 );
				paf_set ( obj, combat_types[1], 2, 0 );
				paf_set ( obj, combat_types[0], 3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_OBSIDIAN ) ) {
				paf_set ( obj, combat_types[1], 3, 0 );
				paf_set ( obj, combat_types[0], 3, 0 );
				paf_set ( obj, save_types[number_range ( 0, 4 )], -10, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_GOLD ) ) {
				paf_set ( obj, save_types[0], number_range ( -12, -5 ), 0 ); //gold is good against afflictive
				paf_set ( obj, combat_types[0], 3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_SILVER ) ) {
				paf_set ( obj, save_types[1], number_range ( -12, -5 ), 0 ); //silver is good against mental
				paf_set ( obj, combat_types[1], 3, 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_PLATINUM ) ) {
				paf_set ( obj, save_types[number_range ( 0, 4 )], number_range ( -12, -5 ), 0 ); //platinum is a mixed bag for a price
			} else if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL ) ) { //tainted is good against breath
				paf_set ( obj, save_types[0], number_range ( -12, -7 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_MITHRIL ) ) { // mithril is good against malidictions
				paf_set ( obj, save_types[2], number_range ( -12, -7 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_TITANIUM ) ) {
				paf_set ( obj, stat_types[1], 1, 0 ); //str bonus
				paf_set ( obj, combat_types[0], number_range ( 1, 3 ), 0 );
				paf_set ( obj, combat_types[1], number_range ( 1, 3 ), 0 );
			} else if ( IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) ) {
				paf_set ( obj, stat_types[5], 1, 0 ); //con bonus
				paf_set ( obj, stat_types[1], 1, 0 ); //str bonus
				paf_set ( obj, combat_types[0], number_range ( 1, 3 ), 0 );
				paf_set ( obj, combat_types[1], number_range ( 1, 3 ), 0 );
			} else
				//ac for low level luckys
			{ paf_set ( obj, ac_types[0], number_range ( -10, -5 ), 0 ); }
		}
	} else if ( obj->item_type == ITEM_LIGHT ) {
		switch ( number_range ( 0, 2 ) ) {
			case 0:
				paf_set ( obj, combat_types[number_range ( 0, 1 )], number_range ( 1, obj->level / 11 ), 0 );
				break;
			case 1:
				paf_set ( obj, save_types[number_range ( 0, 4 )], number_range ( -5, -1 ), 0 );
				break;
			case 2:
				paf_set ( obj, stat_types[number_range ( 0, 5 )], number_range ( 1, 3 ), 0 );
				break;
		}
	} else if ( obj->item_type == ITEM_WEAPON ) {
		switch ( number_range ( 0, 1 ) ) {
			case 0:
				paf_set ( obj, combat_types[number_range ( 0, 1 )], number_range ( 1, obj->level / 10 ), 0 );
				break;
			case 1:
				paf_set ( obj, combat_types[0], number_range ( 1, obj->level / 10 ), 0 );
				paf_set ( obj, combat_types[1], number_range ( 1, obj->level / 10 ), 0 );
				break;
		}
	} else {
		/*Default, lets give it some saves.*/
		paf_set ( obj, save_types[number_range ( 0, 4 )], number_range ( -10, -1 ), 0 );
	}

	/*Its modded, then its magic.*/
	if ( !IS_SET ( obj->extra_flags, ITEM_MAGIC ) ) {
		SET_BIT ( obj->extra_flags, ITEM_MAGIC );
		obj->enchanted = TRUE;
	}
}

void obj_cost ( Item *obj, Creature * mob )
{
	int cost;

	/*Cost is based on item type, weight AND material.*/
	if ( obj->item_type == ITEM_JEWELRY
			|| obj->item_type == ITEM_TREASURE )
	{ cost = obj->weight * 50; }
	else
	{ cost = obj->weight; }

	if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED )
			|| IS_SET ( obj->material_flags, MAT_PLATINUM ) )
	{ cost = cost * 150; }

	else if ( IS_SET ( obj->material_flags, MAT_TITANIUM )
			  || IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) )
	{ cost = cost * 250; }

	else if ( IS_SET ( obj->material_flags, MAT_SILVER ) )
	{ cost = cost * 350; }

	else if ( IS_SET ( obj->material_flags, MAT_GOLD ) )
	{ cost = cost * 450; }

	else
	{ cost = cost * obj->level; }

	obj->cost = cost;
}

void obj_condition ( Item *obj )
{
	int condition;

	/*Condition is based on level, low level eq sucks anyways:)*/
	if ( obj->level <= 5 )
	{ condition = number_range ( 1, 100 ); }
	else if ( obj->level <= 10 )
	{ condition = number_range ( 10, 100 ); }
	else if ( obj->level <= 15 )
	{ condition = number_range ( 20, 100 ); }
	else if ( obj->level <= 25 )
	{ condition = number_range ( 30, 100 ); }
	else if ( obj->level <= 30 )
	{ condition = number_range ( 40, 100 ); }
	else if ( obj->level <= 35 )
	{ condition = number_range ( 50, 100 ); }
	else if ( obj->level <= 40 )
	{ condition = number_range ( 60, 100 ); }
	else if ( obj->level <= 45 )
	{ condition = number_range ( 70, 100 ); }
	else if ( obj->level <= 50 )
	{ condition = number_range ( 80, 100 ); }
	else
	{ condition = number_range ( 90, 100 ); }

	if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL )
			|| IS_SET ( obj->material_flags, MAT_MITHRIL ) )
	{ condition = 100; }

	obj->condition = condition;
}

void set_material_based_flags ( Item *obj, Creature * mob )
{
	if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED ) ) {
		SET_BIT ( obj->extra_flags, ITEM_ANTI_EVIL );
		SET_BIT ( obj->extra_flags, ITEM_ANTI_NEUTRAL );
	}

	if ( IS_SET ( obj->material_flags, MAT_SILVER ) )
	{ SET_BIT ( obj->extra_flags, ITEM_HUM ); }

	if ( IS_SET ( obj->material_flags, MAT_GOLD ) )
	{ SET_BIT ( obj->extra_flags, ITEM_GLOW ); }

	if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL ) )
	{ SET_BIT ( obj->extra_flags, ITEM_ANTI_GOOD ); }

	if ( IS_SET ( obj->material_flags, MAT_MITHRIL ) )
	{ SET_BIT ( obj->extra_flags, ITEM_ANTI_EVIL ); }

	if ( IS_SET ( obj->material_flags, MAT_OPAL ) )
	{ SET_BIT ( obj->extra_flags, ITEM_ANTI_GOOD ); }

	if ( IS_SET ( obj->material_flags, MAT_ONYX ) )
	{ SET_BIT ( obj->extra_flags, ITEM_ANTI_EVIL ); }

	if ( IS_SET ( obj->material_flags, MAT_OBSIDIAN ) )
	{ SET_BIT ( obj->extra_flags, ITEM_INVIS ); }

	/*Lets add some gusto.*/
	if ( number_percent ( ) <= 10 ) {
		switch ( number_range ( 1, 2 ) ) {
			case 1:
				if ( !IS_SET ( obj->extra_flags, ITEM_GLOW ) )
				{ SET_BIT ( obj->extra_flags, ITEM_GLOW ); }
				break;
			case 2:
				if ( !IS_SET ( obj->extra_flags, ITEM_HUM ) )
				{ SET_BIT ( obj->extra_flags, ITEM_HUM ); }
				break;
		}
	}

	/*Go for the full gusto.*/
	if ( number_percent ( ) <= 5 ) {
		if ( !IS_SET ( obj->extra_flags, ITEM_NODROP ) )
		{ SET_BIT ( obj->extra_flags, ITEM_NODROP ); }

		if ( number_percent ( ) <= 15 ) {
			if ( !IS_SET ( obj->extra_flags, ITEM_NOREMOVE ) )
			{ SET_BIT ( obj->extra_flags, ITEM_NOREMOVE ); }
		}
	}

	/*Low lvl equipment sucks:)*/
	if ( obj->level <= 15 ) {
		if ( !IS_SET ( obj->extra_flags, ITEM_MELT_DROP ) )
		{ SET_BIT ( obj->extra_flags, ITEM_MELT_DROP ); }
	}
}

void obj_level ( Item *obj, Creature * mob )
{
	int level;

	/*Base that level on something.*/
	if ( mob->pIndexData->pShop )
	{ obj->level = number_range ( 1, 60 ); }
	else
	{ obj->level = mob->level; }

	/*I added this because I noticed decent 'looking' weapons
	at low levels doing really bad damage, and not worth even
	looking at. A level 10 weapon is good stuff at a low lvl.*/
	if ( obj->item_type == ITEM_WIELD && level < 10 )
	{ level = 10; }
}

void obj_weight ( Item *obj )
{
	int weight = 0;

	/*Weight is based on wear slot AND material. The best armor
	is the heaviest armor and will be the highest level, from
	leather to adamantanium*/
	if ( obj->item_type == ITEM_ARMOR ) {
		if ( IS_SET ( obj->wear_flags, ITEM_HOLD )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_FLOAT )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_FINGER ) )
		{ weight = number_range ( 10, 50 ); }

		if ( IS_SET ( obj->wear_flags, ITEM_WEAR_NECK )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_WRIST ) )
		{ weight = number_range ( 50, 100 ); }

		if ( IS_SET ( obj->wear_flags, ITEM_WEAR_HEAD )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_WAIST )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_HANDS )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_FEET )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_LEGS )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_ARMS )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_SHIELD ) )
		{ weight = number_range ( 100, 150 ); }

		if ( IS_SET ( obj->wear_flags, ITEM_WEAR_BODY )
				|| IS_SET ( obj->wear_flags, ITEM_WEAR_ABOUT ) )
		{ weight = number_range ( 150, 250 ); }

		if ( IS_SET ( obj->material_flags, MAT_LEATHER ) )
		{ weight = weight / 2; }

		if ( IS_SET ( obj->material_flags, MAT_WOOD )
				|| IS_SET ( obj->material_flags, MAT_BONE )
				|| IS_SET ( obj->material_flags, MAT_EBONY )
				|| IS_SET ( obj->material_flags, MAT_IVORY )
				|| IS_SET ( obj->material_flags, MAT_DRAGONSCALE ) )
		{ weight = weight; }
		/*Yay, bonus for the ages of discovery*/
		if ( IS_SET ( obj->material_flags, MAT_COPPER ) )
		{ weight += weight + 1; }
		if ( IS_SET ( obj->material_flags, MAT_BRASS ) )
		{ weight += weight + 2; }
		if ( IS_SET ( obj->material_flags, MAT_BRONZE ) )
		{ weight += weight + 3; }
		if ( IS_SET ( obj->material_flags, MAT_IRON ) )
		{ weight += weight + 4; }
		if ( IS_SET ( obj->material_flags, MAT_STEEL ) )
		{ weight += weight + 5; }
		/*Now we get into the magical stuff, where the materials
		are blessed and cursed so we dont do too much with weight
		unless its the BIG eq:)*/
		if ( IS_SET ( obj->material_flags, MAT_TAINTED_MITHRIL )
				|| IS_SET ( obj->material_flags, MAT_MITHRIL )
				|| IS_SET ( obj->material_flags, MAT_ONYX )
				|| IS_SET ( obj->material_flags, MAT_OPAL ) )
		{ weight = weight; }

		/*Its a little lighter like leather, but worth its weight, literilly*/
		if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED )
				|| IS_SET ( obj->material_flags, MAT_SILVER )
				|| IS_SET ( obj->material_flags, MAT_GOLD ) )
		{ weight = weight / 2; }

		if ( IS_SET ( obj->material_flags, MAT_PLATINUM )
				|| IS_SET ( obj->material_flags, MAT_OBSIDIAN ) )
		{ weight += weight * .5; }

		if ( IS_SET ( obj->material_flags, MAT_TITANIUM ) )
		{ weight += weight * 2; }
		if ( IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) )
		{ weight += weight * 3; }

	}

	if ( obj->item_type == ITEM_WEAPON ) {
		if ( obj->value[0] == WEAPON_EXOTIC )
		{ weight = number_range ( 10, 500 ); }
		else if ( obj->value[0] == WEAPON_SWORD )
		{ weight = number_range ( 100, 250 ); }
		else if ( obj->value[0] == WEAPON_DAGGER )
		{ weight = number_range ( 10, 100 ); }
		else if ( obj->value[0] == WEAPON_SPEAR )
		{ weight = number_range ( 100, 200 ); }
		else if ( obj->value[0] == WEAPON_MACE )
		{ weight = number_range ( 150, 350 ); }
		else if ( obj->value[0] == WEAPON_AXE )
		{ weight = number_range ( 100, 350 ); }
		else if ( obj->value[0] == WEAPON_FLAIL )
		{ weight = number_range ( 150, 350 ); }
		else if ( obj->value[0] == WEAPON_WHIP )
		{ weight = number_range ( 10, 100 ); }
		else if ( obj->value[0] == WEAPON_POLEARM )
		{ weight = number_range ( 250, 500 ); }

		/*Maximum weight for a weapon is 500, + 100
		for two handed, you will need max str for that but
		boy thats a strong weapon:)*/
		if ( IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
		{ weight += 100; }
	}
	if ( obj->item_type == ITEM_TREASURE ) {
		weight = number_range ( 200, 600 );
	}

	if ( obj->item_type == ITEM_JEWELRY ) {
		weight = number_range ( 10, 100 );
	}

	if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	{ weight = number_range ( 10, 200 ); }

	if ( weight <= 0 )
	{ weight = number_range ( 100, 200 ); }

	obj->weight = weight;
}

#define ANSI_ESCAPE "[0"

/*Created to cut down repitetion*/
void set_obj_stats ( Item *obj, Creature * mob, bool special )
{
	int number;
	int type;

	if ( special ) {
		if ( obj->level <= 29 )
		{ special = FALSE; }
	}

	/*Check item types and set stuff, when adding more item types
	to the random generator, you must add here.*/
	switch ( obj->item_type ) {
		default:
			log_hd ( LOG_ERROR, Format ( "set_obj_stats: vnum %d bad item_type.", obj->pIndexData->vnum ) );
			break;

		case ITEM_LIGHT:
			switch ( number_range ( 1, 3 ) ) {
				case 1:
					obj->value[2] = 50;
					break;
				case 2:
					obj->value[2] = 100;
					break;
				case 3:
					obj->value[2] = 500;
					break;
			}
			/*special lights last forever*/
			if ( special )
			{ obj->value[2] = -1; }
			break;
		case ITEM_WEAPON:
			/*The number is based on a max level weapon of 60 to keep
			weapons in check. 60 / 4 = 7.5*/
			number = obj->level / 8 + .5;

			if ( number <= 1 )
			{ number = 1; }
			/*the type is based on the weapon, smallest to biggest damage.*/
			/*A level 60 dagger/exotic/whip will do an average of 20 damage*/
			if ( obj->value[0] == WEAPON_DAGGER
					|| obj->value[0] == WEAPON_EXOTIC
					|| obj->value[0] == WEAPON_WHIP )
			{ type = 4; }

			/*A level 60 staff/axe/spear will do an average of 28 damage*/
			if ( obj->value[0] == WEAPON_AXE
					|| obj->value[0] == WEAPON_SPEAR )
			{ type = 6; }

			/*A level 60 sword/mace/flail/polearm will do an average of 36 damage*/
			if ( obj->value[0] == WEAPON_SWORD
					|| obj->value[0] == WEAPON_MACE
					|| obj->value[0] == WEAPON_FLAIL
					|| obj->value[0] == WEAPON_POLEARM )
			{ type = 8; }

			/*Add a bonus for 2handed weapon, a level 60 2handed sword crushes at 44 average damage*/
			if ( IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
			{ type += 2; }

			/*special weapons get strange damage noun and adds yet another boost*/
			if ( special ) {
				type += 1;
				obj->value[3] = number_range ( 4, 15 );
			}

			obj->value[1] = number;
			obj->value[2] = type;

			break;
		case ITEM_ARMOR:

			/*the heaviest armor will weigh 750 lbs, so the top 3 -ac slots
			will be based solely on armor weight giving a maximum bonus
			of -25 for an adamantanium torso, that is good shiz. And
			the worst armor of a leather torso giving a minimum bonus
			of -2.5.*/
			obj->value[0] = obj->weight / 30;

			/*I defaulted the worst case scenerio to 0, this doesnt mean
			the armor is crap, it could still have bonus's given to it magicly.*/
			if ( obj->value[0] <= 0 )
			{ obj->value[0] = 0; }

			obj->value[1] = obj->value[0];
			obj->value[2] = obj->value[0];

			/*Now we deal with the magical defense*/
			if ( IS_SET ( obj->material_flags, MAT_TITANIUM )
					|| IS_SET ( obj->material_flags, MAT_ADAMANTANIUM ) )
			{ obj->value[3] = obj->value[0] / 4; }

			else if ( IS_SET ( obj->material_flags, MAT_PLATINUM ) )
			{ obj->value[3] = obj->value[0] * 3; }

			else if ( IS_SET ( obj->material_flags, MAT_OBSIDIAN ) )
			{ obj->value[2] = obj->value[0] * 2; }

			else if ( IS_SET ( obj->material_flags, MAT_DOUBLE_PLATED ) )
			{ obj->value[3] = obj->value[0] * .5; }

			else
				/*Defaults have always been that this catagory is halfed*/
			{ obj->value[3] = obj->value[0] / 2; }

			/*special armor gets better defense based on its level.*/
			if ( special ) {
				obj->value[0] += obj->level / 10;
				obj->value[1] += obj->level / 10;
				obj->value[2] += obj->level / 10;
				obj->value[3] += obj->level / 10;
			}

			break;
		case ITEM_WAND:
		case ITEM_STAFF:
			if ( special )
			{ obj->value[1] = obj->level; }
			else
			{ obj->value[1] = obj->level / 2; } // max charges
			obj->value[2] = obj->value[1];// charges left
			obj->value[0] = obj->level; // level of charges
			break;

		/*I LEFT THIS STUFF HERE FOR EXPANSION IDEAS*/
		case ITEM_TREASURE:
		case ITEM_JEWELRY:

		case ITEM_POTION:
		case ITEM_PILL:
		case ITEM_SCROLL:

		case ITEM_JUKEBOX:
		case ITEM_FURNITURE:
		case ITEM_TRASH:
		case ITEM_CONTAINER:
		case ITEM_DRINK_CON:
		case ITEM_KEY:
		case ITEM_FOOD:
		case ITEM_BOAT:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
		case ITEM_FOUNTAIN:
		case ITEM_MAP:
		case ITEM_CLOTHING:
		case ITEM_PORTAL:
		case ITEM_WARP_STONE:
		case ITEM_ROOM_KEY:
		case ITEM_GEM:
		case ITEM_MONEY:
			break;
	}
}

void set_name ( Item *obj, const char* wear_slot )
{
	char buf[MSL];

	PURGE_DATA ( obj->name );
	if ( obj->item_type == ITEM_WEAPON )
	{ sprintf ( buf, "%s", wear_slot ); }
	else
	{ sprintf ( buf, "%s %s", flag_string ( material_flags, obj->material_flags ), wear_slot ); }
	obj->name = assign_string ( buf );
}

void set_short ( Item *obj )
{
	char buf[MSL];

	PURGE_DATA ( obj->short_descr );
	if ( obj->wear_flags == ITEM_WEAR_LEGS
			|| obj->wear_flags == ITEM_WEAR_ARMS
			|| obj->wear_flags == ITEM_WEAR_HANDS
			|| obj->wear_flags == ITEM_WEAR_FEET )
	{ sprintf ( buf, "%s %s", some[number_range ( 0, 2 )], obj->name ); }
	else
	{ sprintf ( buf, "%s %s",  "a", obj->name ); }


	obj->short_descr = assign_string ( buf );
}

void set_long ( Item *obj )
{
	char buf[MSL];

	PURGE_DATA ( obj->description );

	if ( obj->wear_flags == ITEM_WEAR_LEGS
			|| obj->wear_flags == ITEM_WEAR_ARMS
			|| obj->wear_flags == ITEM_WEAR_HANDS
			|| obj->wear_flags == ITEM_WEAR_FEET )
		switch ( number_range ( 0, 2 ) ) {
			case 0:
				sprintf ( buf, "%s have been dropped here.", obj->short_descr );
				break;
			case 1:
				sprintf ( buf, "%s have been left here.", obj->short_descr );
				break;
			case 2:
				sprintf ( buf, "%s are lying here.", obj->short_descr );
				break;
		}
	else
		switch ( number_range ( 0, 2 ) ) {
			case 0:
				sprintf ( buf, "%s has been dropped here.", obj->short_descr );
				break;
			case 1:
				sprintf ( buf, "%s has been left here.", obj->short_descr );
				break;
			case 2:
				sprintf ( buf, "%s is lying here.", obj->short_descr );
				break;
		}

	obj->description = assign_string ( buf );
	obj->description[0] = toupper ( ( int ) obj->description[0] );
}

void remove_material ( Item *obj, int bit )
{
	REMOVE_BIT ( obj->material_flags, bit );
}

void set_material ( Item *obj, int bit )
{
	static int armor_types[] = {
		MAT_PRACTICE, MAT_LEATHER, MAT_WOOD, MAT_BONE, MAT_EBONY,
		MAT_IVORY, MAT_DRAGONSCALE, MAT_COPPER, MAT_BRASS, MAT_BRONZE,
		MAT_IRON, MAT_STEEL, MAT_TAINTED_MITHRIL, MAT_MITHRIL, MAT_OBSIDIAN,
		MAT_ONYX, MAT_OPAL, MAT_DOUBLE_PLATED, MAT_SILVER, MAT_GOLD,
		MAT_PLATINUM, MAT_TITANIUM, MAT_ADAMANTANIUM
	};

	int n_armor;
	/*I ADDED THIS SO THAT I DIDNT GET LEATHER WEAPONS AT LOW LEVELS
	AND OR PRACTICE ARMORS, NEITHER MADE SENSE:)*/
	if ( obj->pIndexData->vnum == OBJ_VNUM_RANDOM_ARMOR )
	{ n_armor = URANGE ( 1, obj->level / 2.38, nelems ( armor_types ) - 1 ); }
	else
	{ n_armor = URANGE ( 0, obj->level / 2.38, nelems ( armor_types ) - 1 ); }

	if ( !bit ) {
		if ( obj->pIndexData->vnum == OBJ_VNUM_RANDOM_ARMOR
				|| obj->pIndexData->vnum == OBJ_VNUM_RANDOM_WEAPON ) {
			SET_BIT ( obj->material_flags, armor_types[n_armor] );
		}
	} else
	{ SET_BIT ( obj->material_flags, bit ); }
}

void string_object ( Item* obj, const char *buf )
{
	set_name ( obj, buf );
	set_short ( obj );
	set_long ( obj );
}

void make_armor ( Item* obj, Creature* mob, int random_vnum, int item_type, int wear_flags, const char* wear_name )
{
	/*create the object*/
	obj = create_object ( get_obj_index ( random_vnum ), 0 );
	/*give it to the mob*/
	obj_to_char ( obj, mob );
	/*what type of object is it*/
	obj->item_type = item_type;
	/*where do we wear:)*/
	obj->wear_flags = wear_flags;
	/*set the level based on the mobs level*/
	obj_level ( obj, mob );

	/*Material MUST be set AFTER level and
	BEFORE weight/paf/cost/stats/cost/condition and flags.
	Reason being is that the level determines the material and
	those latter calls all look for the material, so if the
	material isnt set you will not get proper stats set.*/
	set_material ( obj, 0 );
	/*set the weight based on material AND wear slot*/
	obj_weight ( obj );
	/*add stats to the objects based off its material*/
	paf_apply ( obj );
	/*set the name, short and long, these also make calls to the material*/
	string_object ( obj, wear_name );
	/*set the objects stats, based on material, wear slot AND weight*/
	set_obj_stats ( obj, mob, FALSE );
	/*set the objects condition*/
	obj_condition ( obj );
	/*set the cost, based off material, weight and wear slot*/
	obj_cost ( obj, mob );
	/*set any flags belonging to the material, ie'anti_neutral'*/
	set_material_based_flags ( obj, mob );
	/*a lights gotta radiate... light*/
	if ( obj->item_type == ITEM_LIGHT ) {
		SET_BIT ( obj->extra_flags, ITEM_GLOW );
		/*I usually make a call to change the materials
		to glass for lanterns for obvious reasons*/
		remove_material ( obj, obj->material_flags );
		set_material ( obj, MAT_GLASS );
	}
	/*shop keepers need to have items in the inventory to sell them,
	i made a call later to item_inventory the items so that they
	dont sell out after a couple purchases. This way every reboot
	your shoppy has new and different stuff to sell all the time*/
	if ( !mob->pIndexData->pShop )
	{ wear_obj ( mob, obj, FALSE ); }
}

void make_weapon ( Item* obj, Creature* mob, int random_vnum, const char* verb )
{
	char buf[MSL];
	char weapon[MSL];

	obj = create_object ( get_obj_index ( random_vnum ), 0 );
	obj_to_char ( obj, mob );

	obj->item_type = ITEM_WEAPON;
	obj->wear_flags = ITEM_TAKE | ITEM_WIELD;

	obj_level ( obj, mob );

	switch ( number_range ( 0, 8 ) ) {
		case ( 0 ) :
			obj->value[0] = WEAPON_EXOTIC;
			obj->value[3] = 19;
			if ( number_percent ( ) <= 15 )
			{ obj->value[4] = WEAPON_TWO_HANDS; }

			sprintf ( weapon, "blade" );
			break;

		case ( 1 ) :
			obj->value[0] = WEAPON_SWORD;
			obj->value[3] = 3;

			switch ( number_range ( 0, 7 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "shortsword" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "rapier" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "scimitar" );
					break;
				case ( 3 ) :
					sprintf ( weapon, "sword" );
					break;
				case ( 4 ) :
					sprintf ( weapon, "longsword" );
					break;
				case ( 5 ) :
					sprintf ( weapon, "falchion" );
					break;
				case ( 6 ) :
					sprintf ( weapon, "bastard sword" );
					break;
				case ( 7 ) :
					sprintf ( weapon, "greatsword" );
					obj->value[4] = WEAPON_TWO_HANDS;
					break;
			}
			break;
		case ( 2 ) :
			obj->value[0] = WEAPON_DAGGER;
			obj->value[3] = 11;
			sprintf ( weapon, "dagger" );
			break;
		case ( 3 ) :
			obj->value[0] = WEAPON_SPEAR;
			obj->value[3] = 34;

			switch ( number_range ( 0, 2 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "spear" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "lance" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "javelin" );
					break;
			}
			break;
		case ( 4 ) :
			obj->value[0] = WEAPON_POLEARM;
			obj->value[3] = 21;
			obj->value[4] = WEAPON_TWO_HANDS;

			switch ( number_range ( 0, 4 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "guisarme" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "polearm" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "ranseur" );
					break;
				case ( 3 ) :
					sprintf ( weapon, "glaive" );
					break;
				case ( 4 ) :
					sprintf ( weapon, "trident" );
					break;
			}

			break;
		case ( 5 ) :
			obj->value[0] = WEAPON_MACE;
			obj->value[3] = 8;

			switch ( number_range ( 0, 5 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "mace" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "hammer" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "bludgeon" );
					break;
				case ( 3 ) :
					sprintf ( weapon, "morningstar" );
					break;
				case ( 4 ) :
					sprintf ( weapon, "warhammer" );
					break;
				case ( 5 ) :
					sprintf ( weapon, "club" );
					break;
			}

			break;
		case ( 6 ) :
			obj->value[0] = WEAPON_AXE;
			obj->value[3] = 25;

			switch ( number_range ( 0, 5 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "throwing axe" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "battleaxe" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "greataxe" );
					obj->value[4] = WEAPON_TWO_HANDS;
					break;
				case ( 3 ) :
					sprintf ( weapon, "cleaver" );
					break;
				case ( 4 ) :
					sprintf ( weapon, "pick" );
					break;
				case ( 5 ) :
					sprintf ( weapon, "pickaxe" );
					break;
			}

			break;
		case ( 7 ) :
			obj->value[0] = WEAPON_FLAIL;
			obj->value[3] = 37;

			switch ( number_range ( 0, 2 ) ) {
				case ( 0 ) :
					sprintf ( weapon, "flail" );
					break;
				case ( 1 ) :
					sprintf ( weapon, "dire flail" );
					break;
				case ( 2 ) :
					sprintf ( weapon, "chain" );
					break;
			}

			break;
		case ( 8 ) :
			obj->value[0] = WEAPON_WHIP;
			obj->value[3] = 4;

			sprintf ( weapon, "whip" );
			break;
	}

	set_material ( obj, 0 );

	/*just another leather weapon check*/
	if ( IS_SET ( obj->material_flags, MAT_LEATHER ) ) {
		remove_material ( obj, MAT_LEATHER );
		set_material ( obj, MAT_PRACTICE );
	}

	/*lets let ppl know its a practice weapon*/
	if ( IS_SET ( obj->material_flags, MAT_PRACTICE ) ) {
		if ( IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
		{ sprintf ( buf, "two handed practice %s", weapon ); }
		else
		{ sprintf ( buf, "practice %s", weapon ); }
	} else {
		if ( IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
		{ sprintf ( buf, "%s two handed %s", verb, weapon ); }
		else
		{ sprintf ( buf, "%s %s", verb, weapon ); }
	}

	obj_weight ( obj );

	/*practice weapons get nothing, nothing i say!*/
	if ( !IS_SET ( obj->material_flags, MAT_PRACTICE ) )
	{ paf_apply ( obj ); }

	string_object ( obj, buf );

	set_obj_stats ( obj, mob, FALSE );

	obj_condition ( obj );

	obj_cost ( obj, mob );

	set_material_based_flags ( obj, mob );

	if ( !mob->pIndexData->pShop )
	{ wear_obj ( mob, obj, FALSE ); }
}

extern const char *target_name;
/*This is the generator.*/
void create_random ( Creature * mob, const char *argument )
{
	Item *obj = NULL;
	char arg[MIL];
	/*These arent needed IF you want to have all the code set
	in the same spots. Im only using different vnums in order to
	track item types and vnums in game. I also have additional code
	for wands, rings, trinkets, more eq slots but I basic'ed this down
	and you can expand yourself by following suit.*/
	int armor_vnum = OBJ_VNUM_RANDOM_ARMOR;
	int light_vnum = OBJ_VNUM_RANDOM_LIGHT;
	int weapon_vnum = OBJ_VNUM_RANDOM_WEAPON;

	target_name = one_argument ( argument, arg );

	if ( !str_cmp ( arg, "light" ) ) {
		make_armor ( obj, mob, light_vnum, ITEM_LIGHT, ITEM_TAKE, light_types[number_range ( 0, MAX_LIGHT )] );
	} else if ( !str_cmp ( arg, "neck" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_NECK | ITEM_TAKE, neck_types[number_range ( 0, MAX_NECK )] );
	} else if ( !str_cmp ( arg, "about" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_ABOUT | ITEM_TAKE, about_types[number_range ( 0, MAX_ABOUT )] );
	} else if ( !str_cmp ( arg, "helm" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_HEAD | ITEM_TAKE, head_types[number_range ( 0, MAX_HEAD )] );
	} else if ( !str_cmp ( arg, "arms" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_ARMS | ITEM_TAKE, arms_types[number_range ( 0, MAX_ARMS )] );
	} else if ( !str_cmp ( arg, "legs" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_LEGS | ITEM_TAKE, legs_types[number_range ( 0, MAX_LEGS )] );
	} else if ( !str_cmp ( arg, "hands" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_HANDS | ITEM_TAKE, hand_types[number_range ( 0, MAX_HAND )] );
	} else if ( !str_cmp ( arg, "feet" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_FEET | ITEM_TAKE, feet_types[number_range ( 0, MAX_FEET )] );
	} else if ( !str_cmp ( arg, "wrist" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_WRIST | ITEM_TAKE, wrist_types[number_range ( 0, MAX_WRIST )] );
	} else if ( !str_cmp ( arg, "shield" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_SHIELD | ITEM_TAKE, shield_types[number_range ( 0, MAX_SHIELD )] );
	} else if ( !str_cmp ( arg, "waist" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_WAIST | ITEM_TAKE, waist_types[number_range ( 0, MAX_WAIST )] );
	} else if ( !str_cmp ( arg, "torso" ) ) {
		make_armor ( obj, mob, armor_vnum, ITEM_ARMOR, ITEM_WEAR_BODY | ITEM_TAKE, body_types[number_range ( 0, MAX_BODY )] );
	} else if ( !str_cmp ( arg, "weapon" ) ) {
		make_weapon ( obj, mob, weapon_vnum, verb_types[number_range ( 0, MAX_VERBS )] );
	}
	tail_chain( );
}

void create_random_equipment ( Creature * mob )
{
	static const char *type[] = {
		"helm", "face", "body",
		"arms", "hands", "legs",
		"feet", "waist", "about",
		"wrist", "neck"
	};

	int n_type = number_range ( 0, nelems ( type ) - 1 );

	if ( !IS_NPC ( mob ) )
	{ return; }

	if ( !IS_SET ( mob->act, ACT_RANDOM_EQ ) )
	{ return; }

	if ( mob->pIndexData->pShop ) {
		/*so if your shopkeeper is set with the
		act random flag AND has a weapon chosen as his
		random eq, he will load 5 of them. If he has
		light chosen as his random eq he will load 5
		of them, OR he will just mix his stuff up from
		the list above. This is basic, ive extended my
		code here quite a bit:)*/
		if ( IS_SET ( mob->random, RANDOM_WEAPON ) ) {
			create_random ( mob, "weapon" );
			create_random ( mob, "weapon" );
			create_random ( mob, "weapon" );
			create_random ( mob, "weapon" );
			create_random ( mob, "weapon" );
			return;
		}

		else if ( IS_SET ( mob->random, RANDOM_LIGHT ) ) {
			create_random ( mob, "light" );
			create_random ( mob, "light" );
			create_random ( mob, "light" );
			create_random ( mob, "light" );
			create_random ( mob, "light" );
			return;
		} else {
			create_random ( mob, type[n_type] );
			create_random ( mob, type[n_type] );
			create_random ( mob, type[n_type] );
			create_random ( mob, type[n_type] );
			create_random ( mob, type[n_type] );
		}
	}

	/*If your mobs NOT a shop keeper, he will load his eq
	for each slot he is assigned. You can also add percentage
	checks to see if the item will load or not, I thought about
	it but didnt feel it nessssesssssary unless I wanted to double
	up on neck and wrist or rings like shown below.*/
	if ( IS_SET ( mob->random, RANDOM_HELM ) )
	{ create_random ( mob, "helm" ); }
	if ( IS_SET ( mob->random, RANDOM_TORSO ) )
	{ create_random ( mob, "torso" ); }
	if ( IS_SET ( mob->random, RANDOM_ARMS ) )
	{ create_random ( mob, "arms" ); }
	if ( IS_SET ( mob->random, RANDOM_HANDS ) )
	{ create_random ( mob, "hands" ); }
	if ( IS_SET ( mob->random, RANDOM_LEGS ) )
	{ create_random ( mob, "legs" ); }
	if ( IS_SET ( mob->random, RANDOM_FEET ) )
	{ create_random ( mob, "feet" ); }
	if ( IS_SET ( mob->random, RANDOM_WAIST ) )
	{ create_random ( mob, "waist" ); }
	if ( IS_SET ( mob->random, RANDOM_ABOUT ) )
	{ create_random ( mob, "about" ); }
	if ( IS_SET ( mob->random, RANDOM_WRIST ) ) {
		create_random ( mob, "wrist" );
		if ( number_percent ( ) <= 25 )
		{ create_random ( mob, "wrist" ); }
	}

	if ( IS_SET ( mob->random, RANDOM_NECK ) ) {
		create_random ( mob, "neck" );
		if ( number_percent ( ) <= 25 )
		{ create_random ( mob, "neck" ); }
	}

	if ( IS_SET ( mob->random, RANDOM_LIGHT ) )
	{ create_random ( mob, "light" ); }

	if ( IS_SET ( mob->random, RANDOM_WEAPON ) )
	{ create_random ( mob, "weapon" ); }
	if ( IS_SET ( mob->random, RANDOM_SHIELD ) )
	{ create_random ( mob, "shield" ); }

	tail_chain( );
}

// -- EOF

