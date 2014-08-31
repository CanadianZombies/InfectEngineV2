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

#ifndef _Structures_H
#define _Structures_H
/*
 * Site ban structure.
 */
struct	ban_data {
	Ban *	next;
	bool	valid;
	sh_int	ban_flags;
	sh_int	level;
	char *	name;
};

struct buf_type {
	BUFFER *    next;
	bool        valid;
	sh_int      state;  /* error state of the buffer */
	sh_int      size;   /* size in k */
	char *      string; /* buffer's string */
};

/*
 * Time and weather stuff.
 */
struct	time_info_data {
	int		hour;
	int		day;
	int		month;
	int		year;
};

struct	weather_data {
	int		mmhg;
	int		change;
	int		sky;
	int		sunlight;
};

/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data {
	Socket *	next;
	Socket *	snoop_by;
	Creature *		character;
	Creature *		original;
	bool		valid;
	char *		host;
	sh_int		descriptor;
	sh_int		connected;
	bool		fcommand;
	char		inbuf		[4 * MAX_INPUT_LENGTH];
	char		incomm		[MAX_INPUT_LENGTH];
	char		inlast		[MAX_INPUT_LENGTH];
	int			repeat;
	char *		outbuf;
	int			outsize;
	int			outtop;
	char *		showstr_head;
	char *		showstr_point;
	void *              pEdit;		/* OLC */
	char **             pString;	/* OLC */
	int			editor;		/* OLC */
	protocol_t *        pProtocol;
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type {
	sh_int	tohit;
	sh_int	todam;
	sh_int	carry;
	sh_int	wield;
};

struct	int_app_type {
	sh_int	learn;
};

struct	wis_app_type {
	sh_int	practice;
};

struct	dex_app_type {
	sh_int	defensive;
};

struct	con_app_type {
	sh_int	hitp;
	sh_int	shock;
};


/*
 * Help table types.
 */
struct	help_data {
	HELP_DATA *	next;
	HELP_DATA * next_area;
	sh_int	level;
	char *	keyword;
	char *	text;
};

struct help_area_data {
	HELP_AREA *	next;
	HELP_DATA *	first;
	HELP_DATA *	last;
	Zone *	area;
	char *		filename;
	bool		changed;
};


/*
 * Shop types.
 */
struct	shop_data {
	SHOP_DATA *	next;			/* Next shop in list		*/
	sh_int	keeper;			/* Vnum of shop keeper mob	*/
	sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
	sh_int	profit_buy;		/* Cost multiplier for buying	*/
	sh_int	profit_sell;		/* Cost multiplier for selling	*/
	sh_int	open_hour;		/* First opening hour		*/
	sh_int	close_hour;		/* First closing hour		*/
};

struct	archetype_type {
	const char *	name;			/* the full name of the archetype */
	char 	who_name	[4];	/* Three-letter name for 'who'	*/
	sh_int	attr_prime;		/* Prime attribute		*/
	sh_int	weapon;			/* First weapon			*/
	sh_int	guild[MAX_GUILD];	/* Vnum of guild rooms		*/
	sh_int	skill_adept;		/* Maximum skill level		*/
	sh_int	thac0_00;		/* Thac0 for level  0		*/
	sh_int	thac0_32;		/* Thac0 for level 32		*/
	sh_int	hp_min;			/* Min hp gained on leveling	*/
	sh_int	hp_max;			/* Max hp gained on leveling	*/
	bool	fMana;			/* Class gains mana on level	*/
	const char *	base_group;		/* base skills gained		*/
	const char *	default_group;		/* default skills gained	*/
};

struct item_type {
	int		type;
	const char *	name;
};

struct weapon_type {
	const char *	name;
	sh_int	vnum;
	sh_int	type;
	sh_int	*gsn;
};

struct wiznet_type {
	const char *	name;
	long 	flag;
	int		level;
};

struct attack_type {
	const char *	name;			/* name */
	const char *	noun;			/* message */
	int   	damage;			/* damage archetype */
};

struct race_type {
	const char *	name;			/* call name of the race */
	bool	pc_race;		/* can be chosen by pcs */
	long	act;			/* act bits for the race */
	long	aff;			/* aff bits for the race */
	long	off;			/* off bits for the race */
	long	imm;			/* imm bits for the race */
	long        res;			/* res bits for the race */
	long	vuln;			/* vuln bits for the race */
	long	form;			/* default form flag for the race */
	long	parts;			/* default parts for the race */
};


struct pc_race_type { /* additional data for pc races */
	const char *	name;			/* MUST be in race_type */
	char 	who_name[6];
	sh_int	points;			/* cost in points of the race */
	sh_int	archetype_mult[MAX_CLASS];	/* exp multiplier for archetype, * 100 */
	const char *	skills[5];		/* bonus skills for the race */
	sh_int 	stats[MAX_STATS];	/* starting stats */
	sh_int	max_stats[MAX_STATS];	/* maximum stats */
	sh_int	size;			/* aff bits for the race */
};


struct spec_type {
	const char * 	name;			/* special function name */
	SPEC_FUN *	function;		/* the function */
};



/*
 * Data structure for notes.
 */
struct	note_data {
	NOTE_DATA *	next;
	bool 	valid;
	sh_int	type;
	char *	sender;
	char *	date;
	char *	to_list;
	char *	subject;
	char *	text;
	time_t  	date_stamp;
};



/*
 * An affect.
 */
struct	affect_data {
	Affect *	next;
	bool		valid;
	sh_int		where;
	sh_int		type;
	sh_int		level;
	sh_int		duration;
	sh_int		location;
	sh_int		modifier;
	int			bitvector;
};

/*
 * A kill structure (indexed by level).
 */
struct	kill_data {
	sh_int		number;
	sh_int		killed;
};

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data {
	NPCData *	next;
	SPEC_FUN *		spec_fun;
	SHOP_DATA *		pShop;
	MPROG_LIST *        mprogs;
	Zone *		area;		/* OLC */
	sh_int		vnum;
	sh_int		group;
	bool		new_format;
	sh_int		count;
	sh_int		killed;
	char *		player_name;
	char *		short_descr;
	char *		long_descr;
	char *		description;
	long		act;
	long		affected_by;
	sh_int		alignment;
	sh_int		level;
	sh_int		hitroll;
	sh_int		hit[3];
	sh_int		mana[3];
	sh_int		damage[3];
	sh_int		ac[4];
	sh_int 		dam_type;
	long		off_flags;
	long		imm_flags;
	long		res_flags;
	long		vuln_flags;
	sh_int		start_pos;
	sh_int		default_pos;
	sh_int		sex;
	sh_int		race;
	long		wealth;
	long		form;
	long		parts;
	sh_int		size;
	long		mprog_flags;
	int material_flags;
	int random;
};

/* memory for mobs */
struct mem_data {
	MobMemory 	*next;
	bool	valid;
	int		id;
	int 	reaction;
	time_t 	when;
};

struct critter_query {
	void *querydata;
	void  ( *queryfunc ) ( Creature *c,  const char * L_command, const char *argument, int cmd );
	int ( *queryintfunc ) ( Creature *c, int, const std::string & );

	char queryprompt[1000];  /* prompt for special queries */
	char queryprompt2[1000];  /* prompt for special queries */

	const char *query_string; /* assign_string'd string that we want to carry over */
	int querycommand;        /* command for queries */
	int queryintcommand;     /* command for queries returning integers */
};

/*
 * One character (PC or NPC).
 */
struct	char_data {
	Creature *		next;
	Creature *		next_in_room;
	Creature *		master;
	Creature *		leader;
	Creature *		fighting;
	Creature *		reply;
	Creature *		pet;
	Creature *		mprog_target;

	critter_query		queries;

	MobMemory *		memory;
	SPEC_FUN *		spec_fun;
	NPCData *	pIndexData;
	Socket *	desc;
	Affect *	affected;
	NOTE_DATA *		pnote;
	Item *		carrying;
	Item *		on;
	RoomData *	in_room;
	RoomData *	was_in_room;
	Zone *		zone;
	PlayerData *		pcdata;
	GEN_DATA *		gen_data;
	bool		valid;
	char *		name;
	long		id;
	sh_int		version;
	char *		short_descr;
	char *		long_descr;
	char *		description;
	char *		prompt;
	char *		prefix;
	sh_int		group;
	sh_int		clan;
	sh_int		sex;
	sh_int		archetype;
	sh_int		race;
	sh_int		level;
	sh_int		trust;
	int			played;
	int			lines;  /* for the pager */
	time_t		logon;
	sh_int		timer;
	sh_int		wait;
	sh_int		daze;
	sh_int		hit;
	sh_int		max_hit;
	sh_int		mana;
	sh_int		max_mana;
	sh_int		move;
	sh_int		max_move;
	long		gold;
	long		silver;
	int		exp;
	int		exp_pool;
	long		act;
	long		comm;   /* RT added to pad the vector */
	long		wiznet; /* wiz stuff */
	long                sitrep; // -- Specialty Staff Stuff
	long		imm_flags;
	long		res_flags;
	long		vuln_flags;
	long		sflag;
	sh_int		invis_level;
	sh_int		incog_level;
	long			affected_by;
	sh_int		position;
	sh_int		practice;
	sh_int		train;
	sh_int		carry_weight;
	sh_int		carry_number;
	sh_int		saving_throw;
	sh_int		alignment;
	sh_int		hitroll;
	sh_int		damroll;
	sh_int		armor[4];
	sh_int		wimpy;
	/* stats */
	sh_int		perm_stat[MAX_STATS];
	sh_int		mod_stat[MAX_STATS];
	/* parts stuff */
	long		form;
	long		parts;
	sh_int		size;
	/* mobile stuff */
	long		off_flags;
	sh_int		damage[3];
	sh_int		dam_type;
	sh_int		start_pos;
	sh_int		default_pos;
	sh_int		mprog_delay;
	int material_flags;
	int random;
};



/*
 * Data which only PC's have.
 */
struct	pc_data {
	PlayerData *		next;
	BUFFER * 		buffer;
	bool		valid;
	char *		pwd;
	char *		bamfin;
	char *		bamfout;
	char *		title;
	time_t              last_note;
	time_t              last_idea;
	time_t              last_penalty;
	time_t              last_news;
	time_t              last_changes;
	sh_int		perm_hit;
	sh_int		perm_mana;
	sh_int		perm_move;
	sh_int		true_sex;
	int			last_level;
	sh_int		condition	[4];
	sh_int		learned		[MAX_SKILL];
	bool		group_known	[MAX_GROUP];
	sh_int		points;

	char *		alias[MAX_ALIAS];
	char * 		alias_sub[MAX_ALIAS];
	int 		security;	/* OLC */ /* Builder security */
};

/* Data for generating characters -- only used during generation */
struct gen_data {
	GEN_DATA	*next;
	bool	valid;
	bool	skill_chosen[MAX_SKILL];
	bool	group_chosen[MAX_GROUP];
	int		points_chosen;
};



/*
 * Liquids.
 */
struct	liq_type {
	const char *	liq_name;
	const char *	liq_color;
	sh_int	liq_affect[5];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data {
	DescriptionData *next;	/* Next in list                     */
	bool valid;
	char *keyword;              /* Keyword in look/examine          */
	char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data {
	ItemData *	next;
	DescriptionData *	extra_descr;
	Affect *	affected;
	Zone *		area;		/* OLC */
	bool		new_format;
	char *		name;
	char *		short_descr;
	char *		description;
	sh_int		vnum;
	sh_int		reset_num;
	sh_int		item_type;
	int			extra_flags;
	int			wear_flags;
	sh_int		level;
	sh_int 		condition;
	sh_int		count;
	sh_int		weight;
	int			cost;
	int			value[5];
	int material_flags;
};



/*
 * One object.
 */
struct	obj_data {
	Item *		next;
	Item *		next_content;
	Item *		contains;
	Item *		in_obj;
	Item *		on;
	Creature *		carried_by;
	DescriptionData *	extra_descr;
	Affect *	affected;
	ItemData *	pIndexData;
	RoomData *	in_room;
	bool		valid;
	bool		enchanted;
	char *	        owner;
	char *		name;
	char *		short_descr;
	char *		description;
	sh_int		item_type;
	int			extra_flags;
	int			wear_flags;
	sh_int		wear_loc;
	sh_int		weight;
	int			cost;
	sh_int		level;
	sh_int 		condition;
	sh_int		timer;
	int material_flags;
	int			value	[5];
};



/*
 * Exit data.
 */
struct	exit_data {
	union {
		RoomData *	to_room;
		sh_int			vnum;
	} u1;
	sh_int		exit_info;
	sh_int		key;
	char *		keyword;
	char *		description;
	Exit *		next;		/* OLC */
	int			rs_flags;	/* OLC */
	int			orig_door;	/* OLC */
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data {
	Reset *	next;
	char		command;
	sh_int		arg1;
	sh_int		arg2;
	sh_int		arg3;
	sh_int		arg4;
};



/*
 * Area definition.
 */
struct	area_data {
	Zone *		next;
	HELP_AREA *		helps;
	char *		file_name;
	char *		name;
	char *		credits;
	sh_int		age;
	sh_int		nplayer;
	sh_int		low_range;
	sh_int		high_range;
	sh_int 		min_vnum;
	sh_int		max_vnum;
	bool		empty;
	char *		builders;	/* OLC */ /* Listing of */
	int			vnum;		/* OLC */ /* Area vnum  */
	int			area_flags;	/* OLC */
	int			security;	/* OLC */ /* Value 1-9  */
};



/*
 * Room type.
 */
struct	room_index_data {
	RoomData *	next;
	Creature *		people;
	Item *		contents;
	DescriptionData *	extra_descr;
	Zone *		area;
	Exit *		exit	[6];
	Reset *	reset_first;	/* OLC */
	Reset *	reset_last;	/* OLC */
	char *		name;
	char *		description;
	char *		owner;
	sh_int		vnum;
	int			room_flags;
	sh_int		light;
	sh_int		sector_type;
	sh_int		heal_rate;
	sh_int 		mana_rate;
	sh_int		clan;
};



/*
 * Skills include spells as a particular case.
 */
struct	skill_type {
	const char *	name;			/* Name of skill		*/
	sh_int	skill_level[MAX_CLASS];	/* Level needed by archetype	*/
	sh_int	rating[MAX_CLASS];	/* How hard it is to learn	*/
	SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
	sh_int	target;			/* Legal targets		*/
	sh_int	minimum_position;	/* Position for caster / user	*/
	sh_int *	pgsn;			/* Pointer to associated gsn	*/
	sh_int	slot;			/* Slot for #OBJECT loading	*/
	sh_int	min_mana;		/* Minimum mana used		*/
	sh_int	beats;			/* Waiting time after use	*/
	const char *	noun_damage;		/* Damage message		*/
	const char *	msg_off;		/* Wear off message		*/
	const char *	msg_obj;		/* Wear off message for obects	*/
};

struct  group_type {
	const char *	name;
	sh_int	rating[MAX_CLASS];
	const char *	spells[MAX_IN_GROUP];
};

struct mprog_list {
	int			trig_type;
	char *		trig_phrase;
	sh_int		vnum;
	char *  		code;
	MPROG_LIST * 	next;
	bool		valid;
};

struct mprog_code {
	sh_int		vnum;
	char *		code;
	MPROG_CODE *	next;
};

/*
 * Structure for a social in the socials table.
 */
struct	social_type {
	char      name[20];
	char *    char_no_arg;
	char *    others_no_arg;
	char *    char_found;
	char *    others_found;
	char *    vict_found;
	char *    char_not_found;
	char *      char_auto;
	char *      others_auto;
};

struct flag_type {
	const char *name;
	int bit;
	bool settable;
};

struct clan_type {
	const char 	*name;
	const char 	*who_name;
	sh_int 	hall;
	bool	independent; /* true for loners */
};

struct position_type {
	const char *name;
	const char *short_name;
};

struct sex_type {
	const char *name;
};

struct size_type {
	const char *name;
};

struct	bit_type {
	const	struct	flag_type *	table;
	const char *				help;
};

struct	cmd_type {
	const char * 	name;
	CmdData *	cmd_fun;
	sh_int		position;
	sh_int		level;
	sh_int		log;
	sh_int          show;
	sh_int		category;
};

struct staff_cmd_type {
	const char *name;
	CmdData * cmd_fun;
	long flag;
	sh_int log;
	bool show;
	const char *helpmsg;
};

/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type {
	const char * 	name;
	OLC_FUN *		olc_fun;
};



/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type {
	const char * 	name;
	CmdData *		cmd_fun;
};

struct song_data {
	char *group;
	char *name;
	char *lyrics[MAX_LINES];
	int lines;
};


struct	mob_cmd_type {
	const char * 	name;
	CmdData *		cmd_fun;
};



#endif
