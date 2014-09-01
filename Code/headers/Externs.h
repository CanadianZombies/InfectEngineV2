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

#ifndef _Externs_H
#define _Externs_H

/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];

extern	const	struct	archetype_type	archetype_table	[MAX_CLASS];
extern	const	struct	weapon_type	weapon_table	[];
extern  const   struct  item_type	item_table	[];
extern	const	struct	wiznet_type	wiznet_table	[];
extern	const	struct	attack_type	attack_table	[];
extern  const	struct  race_type	race_table	[];
extern	const	struct	pc_race_type	pc_race_table	[];
extern  const	struct	spec_type	spec_table	[];
extern	const	struct	liq_type	liq_table	[];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern  const   struct  group_type      group_table	[MAX_GROUP];
extern          struct social_type      social_table	[MAX_SOCIALS];
extern	const   char *				title_table	[MAX_CLASS]
[MAX_LEVEL + 1]
[2];



/*
 * Global variables.
 */
extern const int EV_SECOND;
extern const int EV_HALF;
extern const int EV_MINUTE;
extern const int EV_FIVE;
extern const int EV_TEN;
extern const int EV_FIFTEEN;
extern const int EV_HOUR;
extern const int EV_DAY;
extern const int EV_WEEK;

extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;
extern		const char 	  *	afk_flag;
extern		Creature	  *	char_list;
extern		Socket   *	socket_list;
extern		Item	  *	object_list;

extern		MPROG_CODE	  *	mprog_list;

extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		TIME_INFO_DATA		time_info;
extern		EnvironmentDataData		weather_info;
extern		bool			MOBtrigger;
extern		unsigned long 		connect_count;


/*
 * Global Constants
 */
extern	const char *	dir_name        [];
extern	const	sh_int	rev_dir         [];          /* sh_int - ROM OLC */
extern	const	struct	spec_type	spec_table	[];

/*
 * Global variables
 */
extern		Zone *		area_first;
extern		Zone *		area_last;
extern		SHOP_DATA *		shop_last;

extern		int			top_affect;
extern		int			top_area;
extern		int			top_ed;
extern		int			top_exit;
extern		int			top_help;
extern		int			top_mob_index;
extern		int			top_obj_index;
extern		int			top_reset;
extern		int			top_room;
extern		int			top_shop;

extern		int			top_vnum_mob;
extern		int			top_vnum_obj;
extern		int			top_vnum_room;

extern		NPCData *	mob_index_hash  [MAX_KEY_HASH];
extern		ItemData *	obj_index_hash  [MAX_KEY_HASH];
extern		RoomData *	room_index_hash [MAX_KEY_HASH];

/* game tables */
extern	const	struct	clan_type	clan_table[MAX_CLAN];
extern	const	struct	position_type	position_table[];
extern	const	struct	sex_type	sex_table[];
extern	const	struct	size_type	size_table[];

/* flag tables */
extern	const	struct	flag_type	act_flags[];
extern	const	struct	flag_type	plr_flags[];
extern	const	struct	flag_type	affect_flags[];
extern	const	struct	flag_type	off_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	exit_flags[];
extern 	const	struct  flag_type	mprog_flags[];
extern	const	struct	flag_type	area_flags[];
extern	const	struct	flag_type	sector_flags[];
extern	const	struct	flag_type	door_resets[];
extern	const	struct	flag_type	wear_loc_strings[];
extern	const	struct	flag_type	wear_loc_flags[];
extern	const	struct	flag_type	res_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	vuln_flags[];
extern	const	struct	flag_type	type_flags[];
extern	const	struct	flag_type	apply_flags[];
extern	const	struct	flag_type	sex_flags[];
extern	const	struct	flag_type	furniture_flags[];
extern	const	struct	flag_type	weapon_archetype[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	weapon_type2[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	size_flags[];
extern	const	struct	flag_type	position_flags[];
extern	const	struct	flag_type	ac_type[];
extern	const	struct	bit_type	bitvector_type[];
extern	const	struct 	flag_type material_flags[];
extern  const struct  flag_type random_eq_flags[];
/*
 * OLC Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	hedit_table[];


extern struct song_data song_table[MAX_SONGS];
/* the command table itself */
extern	const	struct	mob_cmd_type	mob_cmd_table	[];


extern bool fBootDb;
extern int		newmobs;
extern int		newobjs;
extern NPCData 	* mob_index_hash          [MAX_KEY_HASH];
extern ItemData 	* obj_index_hash          [MAX_KEY_HASH];
extern int		top_mob_index;
extern int		top_obj_index;
extern int  		top_affect;
extern int		top_ed;
extern Zone 	* area_first;

extern int mobile_count;


/* from db2.c */
extern int	social_count;
extern	const	struct	cmd_type	cmd_table	[];

DECLARE_OLC_FUN ( oedit_req0 );
DECLARE_OLC_FUN ( oedit_req1 );
DECLARE_OLC_FUN ( oedit_req2 );
DECLARE_OLC_FUN ( oedit_req3 );
DECLARE_OLC_FUN ( oedit_req4 );
DECLARE_OLC_FUN ( oedit_req5 );
DECLARE_OLC_FUN ( oedit_repop );
DECLARE_OLC_FUN ( medit_repop );
Command ( cmd_use );
#endif
