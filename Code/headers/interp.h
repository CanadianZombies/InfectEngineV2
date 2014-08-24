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

/* this is a listing of all the commands and command related data */

/* wrapper function for safe command execution */
#define cmd_function(ch, cmd_fun, argument) _cmd_function((ch), (cmd_fun), STR(cmd_fun), (argument), __FILE__, __PRETTY_FUNCTION__, __LINE__)
void _cmd_function args ( ( Creature *ch, CmdData *cmd_fun, const char *L_command, const char *argument, const char *file, const char *function, int line ) );

/* for command types */
#define ML 	MAX_LEVEL	/* implementor */
#define L1	MAX_LEVEL - 1  	/* creator */
#define L2	MAX_LEVEL - 2	/* supreme being */
#define L3	MAX_LEVEL - 3	/* deity */
#define L4 	MAX_LEVEL - 4	/* god */
#define L5	MAX_LEVEL - 5	/* immortal */
#define L6	MAX_LEVEL - 6	/* demigod */
#define L7	MAX_LEVEL - 7	/* angel */
#define L8	MAX_LEVEL - 8	/* avatar */
#define IM	LEVEL_IMMORTAL 	/* avatar */
#define HE	LEVEL_HERO	/* hero */

#define COM_INGORE	1

#define CAT_MOVE 	1
#define CAT_COMM 	2
#define CAT_COMBAT 	3
#define CAT_ITEM 	4
#define CAT_INFO	5
#define CAT_CONFIG      6
#define MAX_CATEGORY    7
/*
 * Structure for a command in the command lookup table.
 */
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

/* the command table itself */
extern	const	struct	cmd_type	cmd_table	[];

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
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
Command ( cmd_autoassist	);
Command ( cmd_autoexit	);
Command ( cmd_autogold	);
Command ( cmd_autolist	);
Command ( cmd_autoloot	);
Command ( cmd_autosac	);
Command ( cmd_autosplit	);
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
Command ( cmd_combine	);
Command ( cmd_compact	);
Command (	cmd_compare	);
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
// -- EOF


