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

#ifndef _Prototypes_H
#define _Prototypes_H
char *	crypt		args ( ( const char *key, const char *salt ) );

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	Creature
#define MID	NPCData
#define OD	Item
#define OID	ItemData
#define RID	RoomData
#define SF	SPEC_FUN
#define AD	Affect
#define MPC	MPROG_CODE

/* act_comm.c */
void  	check_sex	args ( ( Creature *ch ) );
void	add_follower	args ( ( Creature *ch, Creature *master ) );
void	stop_follower	args ( ( Creature *ch ) );
void 	nuke_pets	args ( ( Creature *ch ) );
void	die_follower	args ( ( Creature *ch ) );
bool	is_same_group	args ( ( Creature *ach, Creature *bch ) );

/* act_enter.c */
RID  *get_random_room   args ( ( Creature *ch ) );

/* act_info.c */
void	set_title	args ( ( Creature *ch, char *title ) );

/* act_move.c */
void	move_char	args ( ( Creature *ch, int door, bool follow ) );

/* act_obj.c */
bool can_loot		args ( ( Creature *ch, Item *obj ) );
void	wear_obj	args ( ( Creature *ch, Item *obj, bool fReplace ) );
void    get_obj         args ( ( Creature *ch, Item *obj,
								 Item *container ) );

/* act_wiz.c */
void wiznet		args ( ( const char *str, Creature *ch, Item *obj,
						 long flag, long flag_skip, int min_level ) );
/* alias.c */
void 	substitute_alias args ( ( Socket *d, char *input ) );

/* ban.c */
bool	check_ban	args ( ( char *site, int type ) );


/* comm.c */
void	show_string	args ( ( struct descriptor_data *d, const char *input ) );
void	close_socket	args ( ( Socket *dclose ) );
void	write_to_buffer	args ( ( Socket *d, const char *txt,
								 int length ) );
void	writeBuffer	args ( ( const char *txt, Creature *ch ) );
void	writePage	args ( ( const char *txt, Creature *ch ) );
void	act		args ( ( const char *format, Creature *ch,
						 const void *arg1, const void *arg2, int type ) );
void	act_new		args ( ( const char *format, Creature *ch,
							 const void *arg1, const void *arg2, int type,
							 int min_pos ) );
void	printf_to_char	args ( ( Creature *, const char *, ... ) );

/* db.c */
void	reset_area      args ( ( Zone * pArea ) );		/* OLC */
void	reset_room	args ( ( RoomData *pRoom ) );	/* OLC */
char *	print_flags	args ( ( int flag ) );
void	boot_db		args ( ( void ) );
void	area_update	args ( ( void ) );
CD *	create_mobile	args ( ( NPCData *pMobIndex ) );
void	clone_mobile	args ( ( Creature *parent, Creature *clone ) );
OD *	create_object	args ( ( ItemData *pObjIndex, int level ) );
void	clone_object	args ( ( Item *parent, Item *clone ) );
void	clear_char	args ( ( Creature *ch ) );
char *	get_extra_descr	args ( ( const char *name, DescriptionData *ed ) );
MID *	get_mob_index	args ( ( int vnum ) );
OID *	get_obj_index	args ( ( int vnum ) );
RID *	get_room_index	args ( ( int vnum ) );
MPC *	get_mprog_index args ( ( int vnum ) );
char	fread_letter	args ( ( FILE *fp ) );
int	fread_number	args ( ( FILE *fp ) );
long 	fread_flag	args ( ( FILE *fp ) );
char *	fread_string	args ( ( FILE *fp ) );
char *  fread_string_eol args ( ( FILE *fp ) );
void	fread_to_eol	args ( ( FILE *fp ) );
char *	fread_word	args ( ( FILE *fp ) );
long	flag_convert	args ( ( char letter ) );
void *	alloc_mem	args ( ( int sMem ) );
void *	alloc_perm	args ( ( int sMem ) );
void	recycle_mem	args ( ( void *pMem, int sMem ) );
char *	assign_string_old		args ( ( const char *str ) );
char *assign_string ( const std::string &str );
void	smash_tilde	args ( ( const char *str ) );
bool	str_cmp		args ( ( const char *astr, const char *bstr ) );
bool	str_prefix	args ( ( const char *astr, const char *bstr ) );
bool	str_infix	args ( ( const char *astr, const char *bstr ) );
bool	str_suffix	args ( ( const char *astr, const char *bstr ) );
char *	capitalize	args ( ( const char *str ) );
void	append_file	args ( ( Creature *ch, const char *file, const char *str ) );
void	tail_chain	args ( ( void ) );
void	convert_mob ( NPCData *mob );
void	convert_obj ( ItemData *obj );
void convert_mobile ( NPCData *pMobIndex );           /* OLC ROM */
void convert_objects ( void );                               /* OLC ROM */
void convert_object ( ItemData *pObjIndex );           /* OLC ROM */
void assign_area_vnum ( int vnum );

/* wrapper function for safe command execution */
#define cmd_function(ch, cmd_fun, argument) _cmd_function((ch), (cmd_fun), STR(cmd_fun), (argument), __FILE__, __PRETTY_FUNCTION__, __LINE__)
void _cmd_function args ( ( Creature *ch, CmdData *cmd_fun, const char *L_command, const char *argument, const char *file, const char *function, int line ) );

/*
 * OLC Interpreter Prototypes
 */
void    aedit           args ( ( Creature *ch, const char *argument ) );
void    redit           args ( ( Creature *ch, const char *argument ) );
void    medit           args ( ( Creature *ch, const char *argument ) );
void    oedit           args ( ( Creature *ch, const char *argument ) );
void	mpedit		args ( ( Creature *ch, const char *argument ) );
void	hedit		args ( ( Creature *, const char * ) );
Zone *get_vnum_area	args ( ( int vnum ) );
Zone *get_area_data	args ( ( int vnum ) );
int flag_value			args ( ( const struct flag_type *flag_table,
								 const char *argument ) );
const char *flag_string		args ( ( const struct flag_type *flag_table,
									 int bits ) );
void add_reset			args ( ( RoomData *room,
								 Reset *pReset, int index ) );


/* effect.c */
void	acid_effect	args ( ( void *vo, int level, int dam, int target ) );
void	cold_effect	args ( ( void *vo, int level, int dam, int target ) );
void	fire_effect	args ( ( void *vo, int level, int dam, int target ) );
void	poison_effect	args ( ( void *vo, int level, int dam, int target ) );
void	shock_effect	args ( ( void *vo, int level, int dam, int target ) );


/* fight.c */
bool 	is_safe		args ( ( Creature *ch, Creature *victim ) );
bool 	is_safe_spell	args ( ( Creature *ch, Creature *victim, bool area ) );
void	violence_update	args ( ( void ) );
void	multi_hit	args ( ( Creature *ch, Creature *victim, int dt ) );
bool	damage		args ( ( Creature *ch, Creature *victim, int dam,
							 int dt, int archetype, bool show ) );
bool    damage_old      args ( ( Creature *ch, Creature *victim, int dam,
								 int dt, int archetype, bool show ) );
void	update_pos	args ( ( Creature *victim ) );
void	stop_fighting	args ( ( Creature *ch, bool fBoth ) );
void	check_killer	args ( ( Creature *ch, Creature *victim ) );

/* handler.c */
AD  	*affect_find args ( ( Affect *paf, int sn ) );
void	affect_check	args ( ( Creature *ch, int where, int vector ) );
int	count_users	args ( ( Item *obj ) );
void 	deduct_cost	args ( ( Creature *ch, int cost ) );
void	affect_enchant	args ( ( Item *obj ) );
int 	check_immune	args ( ( Creature *ch, int dam_type ) );
int 	material_lookup args ( ( const char *name ) );
int	weapon_lookup	args ( ( const char *name ) );
int	weapon_type	args ( ( const char *name ) );
const char 	*weapon_name	args ( ( int weapon_Type ) );
const char	*item_name	args ( ( int item_type ) );
int	attack_lookup	args ( ( const char *name ) );
long	wiznet_lookup	args ( ( const char *name ) );
int	archetype_lookup	args ( ( const char *name ) );
bool	is_clan		args ( ( Creature *ch ) );
bool	is_same_clan	args ( ( Creature *ch, Creature *victim ) );
bool	is_old_mob	args ( ( Creature *ch ) );
int	get_skill	args ( ( Creature *ch, int sn ) );
int	get_weapon_sn	args ( ( Creature *ch ) );
int	get_weapon_skill args ( ( Creature *ch, int sn ) );
int     get_age         args ( ( Creature *ch ) );
void	reset_char	args ( ( Creature *ch )  );
int	get_trust	args ( ( Creature *ch ) );
int	get_curr_stat	args ( ( Creature *ch, int stat ) );
int 	get_max_train	args ( ( Creature *ch, int stat ) );
int	can_carry_n	args ( ( Creature *ch ) );
int	can_carry_w	args ( ( Creature *ch ) );
bool	is_name		args ( ( const char *str, char *namelist ) );
bool	is_exact_name	args ( ( const char *str, const char *namelist ) );
void	affect_to_char	args ( ( Creature *ch, Affect *paf ) );
void	affect_to_obj	args ( ( Item *obj, Affect *paf ) );
void	affect_remove	args ( ( Creature *ch, Affect *paf ) );
void	affect_remove_obj args ( ( Item *obj, Affect *paf ) );
void	affect_strip	args ( ( Creature *ch, int sn ) );
bool	is_affected	args ( ( Creature *ch, int sn ) );
void	affect_join	args ( ( Creature *ch, Affect *paf ) );
void	char_from_room	args ( ( Creature *ch ) );
void	char_to_room	args ( ( Creature *ch, RoomData *pRoomIndex ) );
void	obj_to_char	args ( ( Item *obj, Creature *ch ) );
void	obj_from_char	args ( ( Item *obj ) );
int	apply_ac	args ( ( Item *obj, int iWear, int type ) );
OD *	get_eq_char	args ( ( Creature *ch, int iWear ) );
void	equip_char	args ( ( Creature *ch, Item *obj, int iWear ) );
void	unequip_char	args ( ( Creature *ch, Item *obj ) );
int	count_obj_list	args ( ( ItemData *obj, Item *list ) );
void	obj_from_room	args ( ( Item *obj ) );
void	obj_to_room	args ( ( Item *obj, RoomData *pRoomIndex ) );
void	obj_to_obj	args ( ( Item *obj, Item *obj_to ) );
void	obj_from_obj	args ( ( Item *obj ) );
void	extract_obj	args ( ( Item *obj ) );
void	extract_char	args ( ( Creature *ch, bool fPull ) );
CD *	get_char_room	args ( ( Creature *ch, const char *argument ) );
CD *	get_char_world	args ( ( Creature *ch, const char *argument ) );
OD *	get_obj_type	args ( ( ItemData *pObjIndexData ) );
OD *	get_obj_list	args ( ( Creature *ch, const char *argument,
								 Item *list ) );
OD *	get_obj_carry	args ( ( Creature *ch, const char *argument,
								 Creature *viewer ) );
OD *	get_obj_wear	args ( ( Creature *ch, const char *argument ) );
OD *	get_obj_here	args ( ( Creature *ch, const char *argument ) );
OD *	get_obj_world	args ( ( Creature *ch, const char *argument ) );
OD *	create_money	args ( ( int gold, int silver ) );
int	get_obj_number	args ( ( Item *obj ) );
int	get_obj_weight	args ( ( Item *obj ) );
int	get_true_weight	args ( ( Item *obj ) );
bool	room_is_dark	args ( ( RoomData *pRoomIndex ) );
bool	is_room_owner	args ( ( Creature *ch, RoomData *room ) );
bool	room_is_private	args ( ( RoomData *pRoomIndex ) );
bool	can_see		args ( ( Creature *ch, Creature *victim ) );
bool	can_see_obj	args ( ( Creature *ch, Item *obj ) );
bool	can_see_room	args ( ( Creature *ch, RoomData *pRoomIndex ) );
bool	can_drop_obj	args ( ( Creature *ch, Item *obj ) );
const char *	affect_loc_name	args ( ( int location ) );
const char *	affect_bit_name	args ( ( int vector ) );
const char *	extra_bit_name	args ( ( int extra_flags ) );
const char * 	wear_bit_name	args ( ( int wear_flags ) );
const char *	act_bit_name	args ( ( int act_flags ) );
const char *	off_bit_name	args ( ( int off_flags ) );
const char *  imm_bit_name	args ( ( int imm_flags ) );
const char * 	form_bit_name	args ( ( int form_flags ) );
const char *	part_bit_name	args ( ( int part_flags ) );
const char *	weapon_bit_name	args ( ( int weapon_flags ) );
const char *  comm_bit_name	args ( ( int comm_flags ) );
const char *	cont_bit_name	args ( ( int cont_flags ) );
const char *	material_bit_name args ( ( int bit ) );
const char *random_eq_bit_name ( int random_eq_flags );

/* interp.c */
#define DefineCommand(name)   void name (Creature *ch, const char *L_command, const char *argument, int cmd)
void	interpret	args ( ( Creature *ch, const char *argument ) );
bool	is_number	args ( ( const char *arg ) );
int	number_argument	args ( ( const char *argument, char *arg ) );
int	mult_argument	args ( ( const char *argument, char *arg ) );
char *	ChopC	args ( ( const char *argument, char *arg_first ) );

/* magic.c */
int	find_spell	args ( ( Creature *ch, const char *name ) );
int 	mana_cost 	( Creature *ch, int min_mana, int level );
int	skill_lookup	args ( ( const char *name ) );
int	slot_lookup	args ( ( int slot ) );
bool	saves_spell	args ( ( int level, Creature *victim, int dam_type ) );
void	obj_cast_spell	args ( ( int sn, int level, Creature *ch,
								 Creature *victim, Item *obj ) );

/* mob_prog.c */
void	program_flow	args ( ( int vnum, char *source, Creature *mob, Creature *ch,
								 const void *arg1, const void *arg2 ) );
void	mp_act_trigger	args ( ( const char *argument, Creature *mob, Creature *ch,
								 const void *arg1, const void *arg2, int type ) );
bool	mp_percent_trigger args ( ( Creature *mob, Creature *ch,
									const void *arg1, const void *arg2, int type ) );
void	mp_bribe_trigger  args ( ( Creature *mob, Creature *ch, int amount ) );
bool	mp_exit_trigger   args ( ( Creature *ch, int dir ) );
void	mp_give_trigger   args ( ( Creature *mob, Creature *ch, Item *obj ) );
void 	mp_greet_trigger  args ( ( Creature *ch ) );
void	mp_hprct_trigger  args ( ( Creature *mob, Creature *ch ) );

/* mob_cmds.c */
void	mob_interpret	args ( ( Creature *ch, const char *argument ) );

/* utilities.cpp */
Prototype ( void _Error, ( const std::string &errorStr, const std::string &fileStr, const std::string &funcStr, int lineOfFile ) );
void catchException ( bool willAbort, const std::string &file, const std::string &function, int lineATcatch );
const char *Format ( const char *fmt, ... );
void build_directories ( void );
#define log_hd(logFlag, logStr) _log_hd(logFlag, __FILE__, __FUNCTION__, __LINE__, logStr)
void _log_hd ( long logFlag, const char *mFile, const char *mFunction, int mLine, const std::string &logStr );
const char *getVersion ( void );
char *str_str ( const char *astr, const char *bstr );
void announce ( const std::string &outStr );
const char *grab_time_log ( time_t the_ttime );
const char *wrapstr ( const char *str );
const char *whoami();

// Utilities.Twitter.cpp
const char *getDateTime ( time_t timeVal );
char* all_capitalize ( const char *str );
void issueSystemCommandNoReturn ( const std::string &argument );
std::string ChopString ( const std::string &argument, std::string &first );
bool SameString ( const std::string &a, const std::string &b );
bool IsSameList ( const std::string &nameToFind, const std::string &namelist );
std::string addTweetHashtags ( const std::string &tweetStr );
void tweetStatement ( const std::string &tweet );



/* save.c */
void	save_char_obj	args ( ( Creature *ch ) );
bool	load_char_obj	args ( ( Socket *d, char *name ) );

/* skills.c */
bool 	parse_gen_groups args ( ( Creature *ch, char *argument ) );
void 	list_group_costs args ( ( Creature *ch ) );
void    list_group_known args ( ( Creature *ch ) );
int 	exp_per_level	args ( ( Creature *ch, int points ) );
void 	check_improve	args ( ( Creature *ch, int sn, bool success,
								 int multiplier ) );
int 	group_lookup	args ( ( const char *name ) );
void	gn_add		args ( ( Creature *ch, int gn ) );
void 	gn_remove	args ( ( Creature *ch, int gn ) );
void 	group_add	args ( ( Creature *ch, const char *name, bool deduct ) );
void	group_remove	args ( ( Creature *ch, const char *name ) );

/* special.c */
SF *	spec_lookup	args ( ( const char *name ) );
char *	spec_name	args ( ( SPEC_FUN *function ) );

/* teleport.c */
RID *	room_by_name	args ( ( char *target, int level, bool error ) );

/* update.c */
void	advance_level	args ( ( Creature *ch, bool hide ) );
void	gain_exp	args ( ( Creature *ch, int gain ) );
void	gain_condition	args ( ( Creature *ch, int iCond, int value ) );
void	update_handler	args ( ( void ) );
void	msdp_update	args ( ( void ) );

/* string.c */
void EnterStringEditor ( Creature *cr, char **pEditString );
char *ChopC ( const char *argument, char *first_arg );
std::string ChopString ( const std::string &argument, std::string &first );
char * string_replace ( const char * orig, const char * old, const char * inew );
//char *	string_replace	args ( ( const char * orig, const char * old, const char * inew ) );
void    StringEditorOptions      args ( ( Creature *ch, char *argument ) );
char *  StringEditor_FormatString   args ( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args ( ( char *argument, char *arg_first, bool fCase ) );
char *	string_unpad	args ( ( char * argument ) );
char *	string_proper	args ( ( char * argument ) );

/* olc.c */
bool	run_olc_editor	args ( ( Socket *d ) );
char	*olc_ed_name	args ( ( Creature *ch ) );
char	*olc_ed_vnum	args ( ( Creature *ch ) );
bool show_commands		args ( ( Creature *ch, const char *argument ) );
bool show_help			args ( ( Creature *ch, const char *argument ) );
bool edit_done			args ( ( Creature *ch ) );
bool show_version		args ( ( Creature *ch, const char *argument ) );



/* lookup.c */
int	race_lookup	args ( ( const char *name ) );
int	item_lookup	args ( ( const char *name ) );
int	liq_lookup	args ( ( const char *name ) );

/* note recycling */
#define ND NOTE_DATA
ND	*new_note args ( ( void ) );
void	recycle_note args ( ( NOTE_DATA *note ) );
#undef ND

/* ban data recycling */
#define BD Ban
BD	*new_ban args ( ( void ) );
void	recycle_ban args ( ( Ban *ban ) );
#undef BD

/* descriptor recycling */
#define DD Socket
DD	*new_descriptor args ( ( void ) );
void	recycle_descriptor args ( ( Socket *d ) );
#undef DD

/* char gen data recycling */
#define GD GEN_DATA
GD 	*new_gen_data args ( ( void ) );
void	recycle_gen_data args ( ( GEN_DATA * gen ) );
#undef GD

/* extra descr recycling */
#define ED DescriptionData
ED	*new_extra_descr args ( ( void ) );
void	recycle_extra_descr args ( ( DescriptionData *ed ) );
#undef ED

/* affect recycling */
#define AD Affect
AD	*new_affect args ( ( void ) );
void	recycle_affect args ( ( Affect *af ) );
#undef AD

/* object recycling */
#define OD Item
OD	*new_obj args ( ( void ) );
void	recycle_obj args ( ( Item *obj ) );
#undef OD

/* character recyling */
#define CD Creature
#define PD PlayerData
CD	*new_char args ( ( void ) );
void	recycle_char args ( ( Creature *ch ) );
PD	*new_pcdata args ( ( void ) );
void	recycle_pcdata args ( ( PlayerData *pcdata ) );
#undef PD
#undef CD


/* mob id and memory procedures */
#define MD MobMemory
long 	get_pc_id args ( ( void ) );
long	get_mob_id args ( ( void ) );
MD	*new_mem_data args ( ( void ) );
void	recycle_mem_data args ( ( MobMemory *memory ) );
MD	*find_memory args ( ( MobMemory *memory, long id ) );
#undef MD

/* buffer procedures */
BUFFER	*new_buf args ( ( void ) );
BUFFER  *new_buf_size args ( ( int size ) );
void	recycle_buf args ( ( BUFFER *buffer ) );
bool	add_buf args ( ( BUFFER *buffer, const char *string ) );
void	clear_buf args ( ( BUFFER *buffer ) );
char	*buf_string args ( ( BUFFER *buffer ) );

HELP_AREA *	new_had		args ( ( void ) );
HELP_DATA *	new_help	args ( ( void ) );
void		recycle_help	args ( ( HELP_DATA * ) );

/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	DescriptionData
Reset	*new_reset_data		args ( ( void ) );
void		recycle_reset_data		args ( ( Reset *pReset ) );
Zone	*new_area		args ( ( void ) );
void		recycle_area		args ( ( Zone *pArea ) );
Exit	*new_exit		args ( ( void ) );
void		recycle_exit		args ( ( Exit *pExit ) );
ED 		*new_extra_descr	args ( ( void ) );
void		recycle_extra_descr	args ( ( ED *pExtra ) );
RoomData *new_room_index		args ( ( void ) );
void		recycle_room_index		args ( ( RoomData *pRoom ) );
Affect	*new_affect		args ( ( void ) );
void		recycle_affect		args ( ( Affect* pAf ) );
SHOP_DATA	*new_shop		args ( ( void ) );
void		recycle_shop		args ( ( SHOP_DATA *pShop ) );
ItemData	*new_obj_index		args ( ( void ) );
void		recycle_obj_index		args ( ( ItemData *pObj ) );
NPCData	*new_mob_index		args ( ( void ) );
void		recycle_mob_index		args ( ( NPCData *pMob ) );
#undef	ED

void		show_liqlist		args ( ( Creature *ch ) );
void		show_damlist		args ( ( Creature *ch ) );

const char *		mprog_type_to_name	args ( ( int type ) );
MPROG_LIST      *new_mprog              args ( ( void ) );
void            recycle_mprog              args ( ( MPROG_LIST *mp ) );
MPROG_CODE	*new_mpcode		args ( ( void ) );
void		recycle_mpcode		args ( ( MPROG_CODE *pMcode ) );

void song_update args ( ( void ) );
void load_songs	args ( ( void ) );

void random_shop ( Creature *mob );

int	clan_lookup	args ( ( const char *name ) );
int	position_lookup	args ( ( const char *name ) );
int 	sex_lookup	args ( ( const char *name ) );
int 	size_lookup	args ( ( const char *name ) );
int	flag_lookup	args ( ( const char *, const struct flag_type * ) );
HELP_DATA * help_lookup	args ( ( const char * ) );
HELP_AREA * had_lookup	args ( ( const char * ) );

void    make_treasure_chest args ( ( Creature *ch, Creature *victim, int level ) );


// -- Commands
Command (	cmd_advance	);
Command ( cmd_affects	);
Command ( cmd_afk		);
Command ( cmd_alia		);
Command ( cmd_alias	);
Command (	cmd_allow	);
Command ( cmd_answer	);
Command (	cmd_areas	);
Command (	cmd_at		);
Command (	cmd_auction	);
Command ( cmd_configstatus	);
Command (	cmd_backstab	);
Command (	cmd_bamfin	);
Command (	cmd_bamfout	);
Command (	cmd_ban		);
Command ( cmd_bash		);
Command ( cmd_berserk	);
Command (	cmd_brandish	);
Command ( cmd_brief	);
Command (	cmd_buy		);
Command (	cmd_cast		);
Command ( cmd_changes	);
Command ( cmd_channels	);
Command ( cmd_clone	);
Command (	cmd_close	);
Command (	cmd_commands	);
Command (	cmd_compare	);
Command ( cmd_config );
Command (	cmd_consider	);
Command ( cmd_count	);
Command (	cmd_credits	);
Command ( cmd_deaf		);
Command ( cmd_delet	);
Command ( cmd_delete	);
Command (	cmd_deny		);
Command (	cmd_description	);
Command ( cmd_dirt		);
Command (	cmd_disarm	);
Command (	cmd_disconnect	);
Command (	cmd_down		);
Command (	cmd_drink	);
Command (	cmd_drop		);
Command ( cmd_dump		);
Command (	cmd_east		);
Command (	cmd_eat		);
Command (	cmd_echo		);
Command (	cmd_emote	);
Command ( cmd_enter	);
Command ( cmd_envenom	);
Command (	cmd_equipment	);
Command (	cmd_examine	);
Command (	cmd_exits	);
Command (	cmd_fill		);
Command ( cmd_flag		);
Command (	cmd_flee		);
Command (	cmd_follow	);
Command (	cmd_force	);
Command (	cmd_freeze	);
Command (	cmd_get		);
Command (	cmd_give		);
Command ( cmd_gossip	);
Command (	cmd_goto		);
Command ( cmd_grats	);
Command (	cmd_group	);
Command ( cmd_groups	);
Command (	cmd_gtell	);
Command ( cmd_guild    	);
Command ( cmd_heal		);
Command (	cmd_help		);
Command (	cmd_hide		);
Command (	cmd_holylight	);
Command (	cmd_idea		);
Command (	cmd_immtalk	);
Command ( cmd_incognito	);
Command ( cmd_clantalk	);
Command ( cmd_imotd	);
Command (	cmd_inventory	);
Command (	cmd_invis	);
Command (	cmd_kick		);
Command (	cmd_kill		);
Command (	cmd_list		);
Command ( cmd_load		);
Command (	cmd_lock		);
Command (	cmd_log		);
Command (	cmd_look		);
Command (	cmd_memory	);
Command (	cmd_mfind	);
Command (	cmd_mload	);
Command (	cmd_mset		);
Command (	cmd_mstat	);
Command (	cmd_mwhere	);
Command ( cmd_mob		);
Command ( cmd_motd		);
Command ( cmd_mpstat	);
Command ( cmd_mpdump	);
Command (	cmd_murde	);
Command (	cmd_murder	);
Command ( cmd_music	);
Command ( cmd_newbielockdown	);
Command ( cmd_news		);
Command ( cmd_nochannels	);
Command (	cmd_noemote	);
Command ( cmd_nofollow	);
Command ( cmd_noloot	);
Command (	cmd_north	);
Command (	cmd_noshout	);
Command ( cmd_nosummon	);
Command (	cmd_note		);
Command (	cmd_notell	);
Command (	cmd_ofind	);
Command (	cmd_oload	);
Command (	cmd_open		);
Command (	cmd_order	);
Command (	cmd_oset		);
Command (	cmd_ostat	);
Command ( cmd_outfit	);
Command ( cmd_owhere	);
Command (	cmd_pardon	);
Command (	cmd_password	);
Command (	cmd_peace	);
Command ( cmd_pecho	);
Command ( cmd_penalty	);
Command ( cmd_permban	);
Command (	cmd_pick		);
Command ( cmd_play		);
Command ( cmd_pmote	);
Command (	cmd_pose		);
Command ( cmd_pour		);
Command (	cmd_practice	);
Command ( cmd_prefi	);
Command ( cmd_prefix	);
Command ( cmd_prompt	);
Command ( cmd_protect	);
Command (	cmd_purge	);
Command (	cmd_put		);
Command (	cmd_quaff	);
Command ( cmd_question	);
Command (	cmd_qui		);
Command ( cmd_quiet	);
Command (	cmd_quit		);
Command ( cmd_quote	);
Command ( cmd_read		);
Command (	cmd_reboot	);
Command (	cmd_recall	);
Command (	cmd_recho	);
Command (	cmd_recite	);
Command (	cmd_remove	);
Command (	cmd_rent		);
Command ( cmd_replay	);
Command (	cmd_reply	);
Command (	cmd_report	);
Command (	cmd_rescue	);
Command (	cmd_rest		);
Command (	cmd_restore	);
Command (	cmd_return	);
Command (	cmd_rset		);
Command (	cmd_rstat	);
Command ( cmd_rules	);
Command (	cmd_sacrifice	);
Command (	cmd_save		);
Command (	cmd_say		);
Command (	cmd_scan		);
Command (	cmd_score	);
Command ( cmd_scroll	);
Command (	cmd_sell		);
Command ( cmd_set		);
Command (	cmd_shout	);
Command ( cmd_show		);
Command (	cmd_shutdown	);
Command ( cmd_sit		);
Command ( cmd_skills	);
Command (	cmd_sla		);
Command (	cmd_slay		);
Command (	cmd_sleep	);
Command (	cmd_slookup	);
Command ( cmd_smote	);
Command (	cmd_sneak	);
Command (	cmd_snoop	);
Command ( cmd_socials	);
Command (	cmd_south	);
Command ( cmd_sockets	);
Command ( cmd_spells	);
Command (	cmd_split	);
Command (	cmd_sset		);
Command (	cmd_stand	);
Command ( cmd_stat		);
Command (	cmd_steal	);
Command ( cmd_story	);
Command ( cmd_string	);
Command (	cmd_surrender	);
Command (	cmd_switch	);
Command (	cmd_tell		);
Command (	cmd_time		);
Command (	cmd_title	);
Command (	cmd_train	);
Command (	cmd_transfer	);
Command ( cmd_trip		);
Command (	cmd_trust	);
Command ( cmd_unalias	);
Command (	cmd_unlock	);
Command ( cmd_unread	);
Command (	cmd_up		);
Command (	cmd_value	);
Command (	cmd_visible	);
Command ( cmd_violate	);
Command ( cmd_vnum		);
Command (	cmd_wake		);
Command (	cmd_wear		);
Command (	cmd_weather	);
Command (	cmd_west		);
Command (	cmd_where	);
Command (	cmd_users		);
Command (	cmd_wimpy	);
Command (	cmd_wizhelp	);
Command (	cmd_lockdown	);
Command ( cmd_wiznet	);
Command ( cmd_worth	);
Command (	cmd_yell		);
Command (	cmd_zap		);
Command ( cmd_zecho	);
Command ( cmd_olc		);
Command ( cmd_asave	);
Command ( cmd_alist	);
Command ( cmd_resets	);
Command ( cmd_redit	);
Command ( cmd_aedit	);
Command ( cmd_medit	);
Command ( cmd_oedit	);
Command ( cmd_mpedit	);
Command ( cmd_hedit	);
Command ( cmd_sitrep	);
Command ( cmd_makestaff );
Command ( cmd_colours );
Command ( cmd_version );
Command ( cmd_os );
Command ( cmd_events );
Command ( cmd_tweet );
Command ( cmd_twitlist );
Command ( cmd_levelup );
Command ( cmd_aedit        );
Command ( cmd_redit        );
Command ( cmd_oedit        );
Command ( cmd_medit        );
Command ( cmd_mpedit	);
Command ( cmd_hedit	);
Command ( cmd_randomitem );
Command ( cmd_search );
Command ( cmd_unarm );
// -- mob_cmds
/*
 * Command functions.
 * Defined in mob_cmds.c
 */
Command (	cmd_mpasound	);
Command (	cmd_mpgecho	);
Command (	cmd_mpzecho	);
Command (	cmd_mpkill	);
Command (	cmd_mpassist	);
Command (	cmd_mpjunk	);
Command (	cmd_mpechoaround	);
Command (	cmd_mpecho	);
Command (	cmd_mpechoat	);
Command (	cmd_mpmload	);
Command (	cmd_mpoload	);
Command (	cmd_mppurge	);
Command (	cmd_mpgoto	);
Command (	cmd_mpat		);
Command (	cmd_mptransfer	);
Command (	cmd_mpgtransfer	);
Command (	cmd_mpforce	);
Command (	cmd_mpgforce	);
Command (	cmd_mpvforce	);
Command (	cmd_mpcast	);
Command (	cmd_mpdamage	);
Command (	cmd_mpremember	);
Command (	cmd_mpforget	);
Command (	cmd_mpdelay	);
Command (	cmd_mpcancel	);
Command (	cmd_mpcall	);
Command (	cmd_mpflee	);
Command (	cmd_mpotransfer	);
Command (	cmd_mpremove	);


// -- Spells
DECLARE_SPELL_FUN ( spell_null );

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

// -- OLC Functions
/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN ( aedit_show		);
DECLARE_OLC_FUN ( aedit_create		);
DECLARE_OLC_FUN ( aedit_name		);
DECLARE_OLC_FUN ( aedit_file		);
DECLARE_OLC_FUN ( aedit_age		);
/* DECLARE_OLC_FUN( aedit_recall	);       ROM OLC */
DECLARE_OLC_FUN ( aedit_reset		);
DECLARE_OLC_FUN ( aedit_security		);
DECLARE_OLC_FUN ( aedit_builder		);
DECLARE_OLC_FUN ( aedit_vnum		);
DECLARE_OLC_FUN ( aedit_lvnum		);
DECLARE_OLC_FUN ( aedit_uvnum		);
DECLARE_OLC_FUN ( aedit_credits		);


/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN ( redit_show		);
DECLARE_OLC_FUN ( redit_create		);
DECLARE_OLC_FUN ( redit_name		);
DECLARE_OLC_FUN ( redit_desc		);
DECLARE_OLC_FUN ( redit_ed		);
DECLARE_OLC_FUN ( redit_format		);
DECLARE_OLC_FUN ( redit_north		);
DECLARE_OLC_FUN ( redit_south		);
DECLARE_OLC_FUN ( redit_east		);
DECLARE_OLC_FUN ( redit_west		);
DECLARE_OLC_FUN ( redit_up		);
DECLARE_OLC_FUN ( redit_down		);
DECLARE_OLC_FUN ( redit_mreset		);
DECLARE_OLC_FUN ( redit_oreset		);
DECLARE_OLC_FUN ( redit_mlist		);
DECLARE_OLC_FUN ( redit_rlist		);
DECLARE_OLC_FUN ( redit_olist		);
DECLARE_OLC_FUN ( redit_mshow		);
DECLARE_OLC_FUN ( redit_oshow		);
DECLARE_OLC_FUN ( redit_heal		);
DECLARE_OLC_FUN ( redit_mana		);
DECLARE_OLC_FUN ( redit_clan		);
DECLARE_OLC_FUN ( redit_owner		);
DECLARE_OLC_FUN ( redit_room		);
DECLARE_OLC_FUN ( redit_sector		);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN ( oedit_show		);
DECLARE_OLC_FUN ( oedit_create		);
DECLARE_OLC_FUN ( oedit_name		);
DECLARE_OLC_FUN ( oedit_short		);
DECLARE_OLC_FUN ( oedit_long		);
DECLARE_OLC_FUN ( oedit_addaffect	);
DECLARE_OLC_FUN ( oedit_addapply		);
DECLARE_OLC_FUN ( oedit_delaffect	);
DECLARE_OLC_FUN ( oedit_value0		);
DECLARE_OLC_FUN ( oedit_value1		);
DECLARE_OLC_FUN ( oedit_value2		);
DECLARE_OLC_FUN ( oedit_value3		);
DECLARE_OLC_FUN ( oedit_value4		); /* ROM */
DECLARE_OLC_FUN ( oedit_weight		);
DECLARE_OLC_FUN ( oedit_cost		);
DECLARE_OLC_FUN ( oedit_ed		);

DECLARE_OLC_FUN ( oedit_extra            ); /* ROM */
DECLARE_OLC_FUN ( oedit_wear             ); /* ROM */
DECLARE_OLC_FUN ( oedit_type             ); /* ROM */
DECLARE_OLC_FUN ( oedit_affect           ); /* ROM */
DECLARE_OLC_FUN ( oedit_material		); /* ROM */
DECLARE_OLC_FUN ( oedit_level            ); /* ROM */
DECLARE_OLC_FUN ( oedit_condition        ); /* ROM */

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN ( medit_show		);
DECLARE_OLC_FUN ( medit_create		);
DECLARE_OLC_FUN ( medit_name		);
DECLARE_OLC_FUN ( medit_short		);
DECLARE_OLC_FUN ( medit_long		);
DECLARE_OLC_FUN ( medit_shop		);
DECLARE_OLC_FUN ( medit_desc		);
DECLARE_OLC_FUN ( medit_level		);
DECLARE_OLC_FUN ( medit_align		);
DECLARE_OLC_FUN ( medit_spec		);
DECLARE_OLC_FUN ( medit_random );
DECLARE_OLC_FUN ( medit_sex		); /* ROM */
DECLARE_OLC_FUN ( medit_act		); /* ROM */
DECLARE_OLC_FUN ( medit_affect		); /* ROM */
DECLARE_OLC_FUN ( medit_ac		); /* ROM */
DECLARE_OLC_FUN ( medit_form		); /* ROM */
DECLARE_OLC_FUN ( medit_part		); /* ROM */
DECLARE_OLC_FUN ( medit_imm		); /* ROM */
DECLARE_OLC_FUN ( medit_res		); /* ROM */
DECLARE_OLC_FUN ( medit_vuln		); /* ROM */
DECLARE_OLC_FUN ( medit_material		); /* ROM */
DECLARE_OLC_FUN ( medit_off		); /* ROM */
DECLARE_OLC_FUN ( medit_size		); /* ROM */
DECLARE_OLC_FUN ( medit_hitdice		); /* ROM */
DECLARE_OLC_FUN ( medit_manadice		); /* ROM */
DECLARE_OLC_FUN ( medit_damdice		); /* ROM */
DECLARE_OLC_FUN ( medit_race		); /* ROM */
DECLARE_OLC_FUN ( medit_position		); /* ROM */
DECLARE_OLC_FUN ( medit_gold		); /* ROM */
DECLARE_OLC_FUN ( medit_hitroll		); /* ROM */
DECLARE_OLC_FUN ( medit_damtype		); /* ROM */
DECLARE_OLC_FUN ( medit_group		); /* ROM */
DECLARE_OLC_FUN ( medit_addmprog		); /* ROM */
DECLARE_OLC_FUN ( medit_delmprog		); /* ROM */

/* Mobprog editor */
DECLARE_OLC_FUN ( mpedit_create		);
DECLARE_OLC_FUN ( mpedit_code		);
DECLARE_OLC_FUN ( mpedit_show		);
DECLARE_OLC_FUN ( mpedit_list		);

/* Editor de helps */
DECLARE_OLC_FUN ( hedit_keyword		);
DECLARE_OLC_FUN ( hedit_text		);
DECLARE_OLC_FUN ( hedit_new		);
DECLARE_OLC_FUN ( hedit_level		);
DECLARE_OLC_FUN ( hedit_delete		);
DECLARE_OLC_FUN ( hedit_show		);
DECLARE_OLC_FUN ( hedit_list		);



#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef AD

#endif
