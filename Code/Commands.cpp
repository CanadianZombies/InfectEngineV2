/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"


bool	check_social	args ( ( Creature *ch, const char *command,
								 const char *argument ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] = {
	/*
	 * Common movement commands.
	 */
	{ "north",		cmd_north,	POS_STANDING,    0,  LOG_NEVER, 0 },
	{ "east",		cmd_east,	POS_STANDING,	 0,  LOG_NEVER, 0 },
	{ "south",		cmd_south,	POS_STANDING,	 0,  LOG_NEVER, 0 },
	{ "west",		cmd_west,	POS_STANDING,	 0,  LOG_NEVER, 0 },
	{ "up",		cmd_up,		POS_STANDING,	 0,  LOG_NEVER, 0 },
	{ "down",		cmd_down,	POS_STANDING,	 0,  LOG_NEVER, 0 },

	/*
	 * Common other commands.
	 * Placed here so one and two letter abbreviations work.
	 */
	{ "at",             cmd_at,          POS_DEAD,       L6,  LOG_NORMAL, 1 },
	{ "cast",		cmd_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "auction",        cmd_auction,     POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "buy",		cmd_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "channels",       cmd_channels,    POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "exits",		cmd_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "get",		cmd_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "goto",           cmd_goto,        POS_DEAD,       L8,  LOG_NORMAL, 1 },
	{ "group",          cmd_group,       POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "guild",		cmd_guild,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "hit",		cmd_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
	{ "backpack",	cmd_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "kill",		cmd_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "look",		cmd_look,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "clan",		cmd_clantalk,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "music",          cmd_music,   	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "order",		cmd_order,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "practice",       cmd_practice,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "rest",		cmd_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "sit",		cmd_sit,		POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "sockets",        cmd_sockets,	POS_DEAD,       L4,  LOG_NORMAL, 1 },
	{ "stand",		cmd_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "tell",		cmd_tell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "unlock",         cmd_unlock,      POS_RESTING,     0,  LOG_NORMAL, 1 },
	{ "wield",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "wizhelp",	cmd_wizhelp,	POS_DEAD,	IM,  LOG_NORMAL, 1 },

	/*
	 * Informational commands.
	 */
	{ "affects",	cmd_affects,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "zones",		cmd_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "changes",	cmd_changes,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "commands",	cmd_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "compare",	cmd_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "consider",	cmd_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "count",		cmd_count,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "credits",	cmd_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "gear",		cmd_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "examine",	cmd_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	/*  { "groups",		cmd_groups,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 }, */
	{ "help",		cmd_help,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "idea",		cmd_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "info",           cmd_groups,      POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "motd",		cmd_motd,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "news",		cmd_news,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "read",		cmd_read,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "report",		cmd_report,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "rules",		cmd_rules,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "sheet",		cmd_score,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "skills",		cmd_skills,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "socials",	cmd_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "show",		cmd_show,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "spells",		cmd_spells,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "story",		cmd_story,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "time",		cmd_time,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "weather",	cmd_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "users",		cmd_who,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "worth",		cmd_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },

	/*
	 * Configuration commands.
	 */
	{ "alia",		cmd_alia,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
	{ "alias",		cmd_alias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "autolist",	cmd_autolist,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "autoassist",	cmd_autoassist,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "autoexit",	cmd_autoexit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "autogold",	cmd_autogold,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "autoloot",	cmd_autoloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "autosac",	cmd_autosac,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "autosplit",	cmd_autosplit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "brief",		cmd_brief,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "combine",	cmd_combine,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "compact",	cmd_compact,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "description",	cmd_description,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "delet",		cmd_delet,	POS_DEAD,	 0,  LOG_ALWAYS, 0 },
	{ "delete",		cmd_delete,	POS_STANDING,	 0,  LOG_ALWAYS, 1 },
	{ "nofollow",	cmd_nofollow,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "noloot",		cmd_noloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "nosummon",	cmd_nosummon,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "outfit",		cmd_outfit,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "password",	cmd_password,	POS_DEAD,	 0,  LOG_NEVER,  1 },
	{ "prompt",		cmd_prompt,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "scroll",		cmd_scroll,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "title",		cmd_title,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "unalias",	cmd_unalias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "wimpy",		cmd_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1 },

	/*
	 * Communication commands.
	 */
	{ "afk",		cmd_afk,		POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "answer",		cmd_answer,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "deaf",		cmd_deaf,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "emote",		cmd_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "pmote",		cmd_pmote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ ".",		cmd_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
	{ "gossip",		cmd_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ ",",		cmd_emote,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
	{ "grats",		cmd_grats,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "gtell",		cmd_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ ";",		cmd_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
	{ "note",		cmd_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "pose",		cmd_pose,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "question",	cmd_question,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "quote",		cmd_quote,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "quiet",		cmd_quiet,	POS_SLEEPING, 	 0,  LOG_NORMAL, 1 },
	{ "reply",		cmd_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "replay",		cmd_replay,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "say",		cmd_say,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "'",		cmd_say,		POS_RESTING,	 0,  LOG_NORMAL, 0 },
	{ "shout",		cmd_shout,	POS_RESTING,	 3,  LOG_NORMAL, 1 },
	{ "unread",		cmd_unread,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "yell",		cmd_yell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },

	/*
	 * Object manipulation commands.
	 */
	{ "brandish",	cmd_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "close",		cmd_close,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "drink",		cmd_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "drop",		cmd_drop,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "eat",		cmd_eat,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "envenom",	cmd_envenom,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "fill",		cmd_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "give",		cmd_give,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "heal",		cmd_heal,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "hold",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "list",		cmd_list,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "lock",		cmd_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "open",		cmd_open,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "pick",		cmd_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "pour",		cmd_pour,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "put",		cmd_put,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "quaff",		cmd_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "recite",		cmd_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "remove",		cmd_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "sell",		cmd_sell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "take",		cmd_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "sacrifice",	cmd_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "junk",           cmd_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, 0 },
	{ "tap",      	cmd_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, 0 },
	/*  { "unlock",		cmd_unlock,	POS_RESTING,	 0,  LOG_NORMAL, 1 }, */
	{ "value",		cmd_value,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "wear",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "zap",		cmd_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1 },

	/*
	 * Combat commands.
	 */
	{ "backstab",	cmd_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "bash",		cmd_bash,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },
	{ "bs",		cmd_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
	{ "berserk",	cmd_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "dirt",		cmd_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "disarm",		cmd_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "flee",		cmd_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "kick",		cmd_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "murde",		cmd_murde,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
	{ "murder",		cmd_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS, 1 },
	{ "rescue",		cmd_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
	{ "surrender",	cmd_surrender,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },
	{ "trip",		cmd_trip,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },

	/*
	 * Mob command interpreter (placed here for faster scan...)
	 */
	{ "mob",		cmd_mob,		POS_DEAD,	 0,  LOG_NEVER,  0 },

	/*
	 * Miscellaneous commands.
	 */
	{ "enter", 		cmd_enter, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "follow",		cmd_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "gain",		cmd_gain,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "go",		cmd_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
	/*  { "group",		cmd_group,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 }, */
	{ "groups",		cmd_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "hide",		cmd_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "play",		cmd_play,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	/*  { "practice",	cmd_practice,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 }, */
	{ "qui",		cmd_qui,		POS_DEAD,	 0,  LOG_NORMAL, 0 },
	{ "quit",		cmd_quit,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "recall",		cmd_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "/",		cmd_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
	{ "rent",		cmd_rent,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
	{ "save",		cmd_save,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "sleep",		cmd_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "sneak",		cmd_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "split",		cmd_split,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "steal",		cmd_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "train",		cmd_train,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "visible",	cmd_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "wake",		cmd_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "where",		cmd_where,	POS_RESTING,	 0,  LOG_NORMAL, 1 },



	/*
	 * Immortal commands.
	 */
	{ "advance",	cmd_advance,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },
	{ "dump",		cmd_dump,	POS_DEAD,	ML,  LOG_ALWAYS, 0 },
	{ "trust",		cmd_trust,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },
	{ "violate",	cmd_violate,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },

	{ "allow",		cmd_allow,	POS_DEAD,	L2,  LOG_ALWAYS, 1 },
	{ "ban",		cmd_ban,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },
	{ "deny",		cmd_deny,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	{ "disconnect",	cmd_disconnect,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },
	{ "flag",		cmd_flag,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "freeze",		cmd_freeze,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "permban",	cmd_permban,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	{ "protect",	cmd_protect,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	{ "reboo",		cmd_reboo,	POS_DEAD,	L1,  LOG_NORMAL, 0 },
	{ "reboot",		cmd_reboot,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	{ "set",		cmd_set,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },
	{ "shutdow",	cmd_shutdow,	POS_DEAD,	L1,  LOG_NORMAL, 0 },
	{ "shutdown",	cmd_shutdown,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	/*  { "sockets",	cmd_sockets,	POS_DEAD,	L4,  LOG_NORMAL, 1 }, */
	{ "lockdown",	cmd_lockdown,	POS_DEAD,	L2,  LOG_ALWAYS, 1 },

	{ "force",		cmd_force,	POS_DEAD,	L7,  LOG_ALWAYS, 1 },
	{ "load",		cmd_load,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "newbielockdown",	cmd_newbielockdown,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "nochannels",	cmd_nochannels,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "noemote",	cmd_noemote,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "noshout",	cmd_noshout,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "notell",		cmd_notell,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "pecho",		cmd_pecho,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "pardon",		cmd_pardon,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },
	{ "purge",		cmd_purge,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "restore",	cmd_restore,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	{ "sla",		cmd_sla,		POS_DEAD,	L3,  LOG_NORMAL, 0 },
	{ "slay",		cmd_slay,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },
	{ "teleport",	cmd_transfer,    POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "transfer",	cmd_transfer,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },

	/*  { "at",		cmd_at,		POS_DEAD,	L6,  LOG_NORMAL, 1 }, */
	{ "poofin",		cmd_bamfin,	POS_DEAD,	L8,  LOG_NORMAL, 1 },
	{ "poofout",	cmd_bamfout,	POS_DEAD,	L8,  LOG_NORMAL, 1 },
	{ "gecho",		cmd_echo,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
	/*  { "goto",		cmd_goto,	POS_DEAD,	L8,  LOG_NORMAL, 1 }, */
	{ "holylight",	cmd_holylight,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "incognito",	cmd_incognito,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "invis",		cmd_invis,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
	{ "log",		cmd_log,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },
	{ "memory",		cmd_memory,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "mwhere",		cmd_mwhere,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "owhere",		cmd_owhere,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "peace",		cmd_peace,	POS_DEAD,	L5,  LOG_NORMAL, 1 },
	{ "penalty",	cmd_penalty,	POS_DEAD,	L7,  LOG_NORMAL, 1 },
	{ "echo",		cmd_recho,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
	{ "return",         cmd_return,      POS_DEAD,       L6,  LOG_NORMAL, 1 },
	{ "snoop",		cmd_snoop,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "stat",		cmd_stat,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "string",		cmd_string,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
	{ "switch",		cmd_switch,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
	{ "wizinvis",	cmd_invis,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "vnum",		cmd_vnum,	POS_DEAD,	L4,  LOG_NORMAL, 1 },
	{ "zecho",		cmd_zecho,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },

	{ "clone",		cmd_clone,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },

	{ "staffaid",	cmd_wiznet,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "immtalk",	cmd_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "imotd",          cmd_imotd,       POS_DEAD,       IM,  LOG_NORMAL, 1 },
	{ ":",		cmd_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
	{ "smote",		cmd_smote,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "prefi",		cmd_prefi,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
	{ "prefix",		cmd_prefix,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
	{ "mpdump",		cmd_mpdump,	POS_DEAD,	IM,  LOG_NEVER,  1 },
	{ "mpstat",		cmd_mpstat,	POS_DEAD,	IM,  LOG_NEVER,  1 },

	/*
	 * OLC
	 */
	{ "edit",		cmd_olc,		POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "asave",          cmd_asave,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "alist",		cmd_alist,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "resets",		cmd_resets,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "redit",		cmd_redit,	POS_DEAD,    0,	 LOG_NORMAL, 1 },
	{ "medit",		cmd_medit,	POS_DEAD,    0,	 LOG_NORMAL, 1 },
	{ "aedit",		cmd_aedit,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "oedit",		cmd_oedit,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "mpedit",		cmd_mpedit,	POS_DEAD,    0,  LOG_NORMAL, 1 },
	{ "hedit",		cmd_hedit,	POS_DEAD,    0,  LOG_NORMAL, 1 },

	{ "sitrep",		cmd_sitrep,	POS_DEAD,   MAX_LEVEL, LOG_NORMAL, 1 },

	/*
	 * End of list.
	 */
	{ "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0 }
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret ( Creature *ch, const char *argument )
{
	char command[MAX_INPUT_LENGTH];
	char logline[MAX_INPUT_LENGTH];
	int cmd;
	int trust;
	bool found;

	/*
	 * Strip leading spaces.
	 */
	while ( isspace ( *argument ) )
	{ argument++; }
	if ( argument[0] == '\0' )
	{ return; }

	/*
	 * No hiding.
	 */
	REMOVE_BIT ( ch->affected_by, AFF_HIDE );

	/*
	 * Implement freeze command.
	 */
	if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_FREEZE ) ) {
		writeBuffer ( "You have been disabled, nothing you do will help!\n\r", ch );
		return;
	}

	/*
	 * Grab the command word.
	 * Special parsing so ' can be a command,
	 *   also no spaces needed after punctuation.
	 */
	strcpy ( logline, argument );
	if ( !isalpha ( argument[0] ) && !isdigit ( argument[0] ) ) {
		command[0] = argument[0];
		command[1] = '\0';
		argument++;
		while ( isspace ( *argument ) )
		{ argument++; }
	} else {
		argument = one_argument ( argument, command );
	}

	/*
	 * Look for command in command table.
	 */
	found = FALSE;
	trust = get_trust ( ch );
	for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ ) {
		if ( command[0] == cmd_table[cmd].name[0]
				&&   !str_prefix ( command, cmd_table[cmd].name )
				&&   cmd_table[cmd].level <= trust ) {
			found = TRUE;
			break;
		}
	}

	/*
	 * Log and snoop.
	 */
	if ( cmd_table[cmd].log == LOG_NEVER )
	{ strcpy ( logline, "" ); }

	// -- log the command data
	if ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_LOG ) )
			||   fLogAll
			||   cmd_table[cmd].log == LOG_ALWAYS ) {
		wiznet ( Format ( "Log: %s %s", ch->name, logline ), ch, NULL, WIZ_SECURE, 0, get_trust ( ch ) );
		log_hd ( LOG_SECURITY | LOG_COMMAND, Format ( "%s has called executed: %s", ch->name, !IS_NULLSTR ( logline ) ? logline : "{SUSPECT LOG_NEVER, NO LOGLINE DATA RECEIVED}" ) );
	}

	if ( ch->desc != NULL && ch->desc->snoop_by != NULL ) {
		write_to_buffer ( ch->desc->snoop_by, "% ",    2 );
		write_to_buffer ( ch->desc->snoop_by, logline, 0 );
		write_to_buffer ( ch->desc->snoop_by, "\n\r",  2 );
	}

	if ( !found ) {
		/*
		 * Look for command in socials table.
		 */
		if ( !check_social ( ch, command, argument ) )
		{ writeBuffer ( "Unknown Option.\n\r", ch ); }
		return;
	}

	/*
	 * Character not in position for command?
	 */
	if ( ch->position < cmd_table[cmd].position ) {
		switch ( ch->position ) {
			case POS_DEAD:
				writeBuffer ( "Lie still; you are DEAD.\n\r", ch );
				break;

			case POS_MORTAL:
			case POS_INCAP:
				writeBuffer ( "You are hurt far too bad for that.\n\r", ch );
				break;

			case POS_STUNNED:
				writeBuffer ( "You are too stunned to do that.\n\r", ch );
				break;

			case POS_SLEEPING:
				writeBuffer ( "In your dreams, or what?\n\r", ch );
				break;

			case POS_RESTING:
				writeBuffer ( "Nah... You feel too relaxed...\n\r", ch );
				break;

			case POS_SITTING:
				writeBuffer ( "Better stand up first.\n\r", ch );
				break;

			case POS_FIGHTING:
				writeBuffer ( "No way!  You are still fighting!\n\r", ch );
				break;

		}
		return;
	}

	// -- execute the command and log the details.
	log_hd ( LOG_DEBUG, Format ( "Pre-Cmd:  %s(%p) executed by %s, args: %s", command, cmd_table[cmd].cmd_fun, ch->name, !IS_NULLSTR ( argument ) ? argument : "{No Args}" ) );

	// -- in case one of our commands throws an exception, we want to be careful and log what we can!
	try {
		( *cmd_table[cmd].cmd_fun ) ( ch, command, argument );
	} catch ( ... ) {
		CATCH ( false );
	}
	log_hd ( LOG_DEBUG, Format ( "Post-Cmd: %s(%p) executed by %s successfully!", command, cmd_table[cmd].cmd_fun, ch ? ch->name : "{Creature non-existant post command}" ) );

	tail_chain( );
	return;
}

/* function to keep argument safe in all commands -- no static strings */
void _cmd_function ( Creature *ch, CmdData *cmd_fun, const char *L_command, const char *argument, const char *file, const char *function, int line )
{
	char *command_string;

	/* copy the string */
	command_string = assign_string ( argument );

	// -- cmd_function now puts this data in the debug log for safe keeping, incase the dispatch causes a crash.
	log_hd ( LOG_DEBUG, Format ( "%s->%s->%d->cmd_function->Cmd:%s(%p) Cr:%s(%p) Args:(%s)", file, function, line, L_command, cmd_fun, ch ? ch->name : "", ch, argument ? argument : "{NULL DATA}" ) );

	/* dispatch the command */
	( *cmd_fun ) ( ch, L_command, command_string );

	/* free the string */
	PURGE_DATA ( command_string );
}

bool check_social ( Creature *ch, const char *command, const char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	int cmd;
	bool found;

	found  = FALSE;
	for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ ) {
		if ( command[0] == social_table[cmd].name[0]
				&&   !str_prefix ( command, social_table[cmd].name ) ) {
			found = TRUE;
			break;
		}
	}

	if ( !found )
	{ return FALSE; }

	if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) ) {
		writeBuffer ( "You are anti-social!\n\r", ch );
		return TRUE;
	}

	switch ( ch->position ) {
		case POS_DEAD:
			writeBuffer ( "Lie still; you are DEAD.\n\r", ch );
			return TRUE;

		case POS_INCAP:
		case POS_MORTAL:
			writeBuffer ( "You are hurt far too bad for that.\n\r", ch );
			return TRUE;

		case POS_STUNNED:
			writeBuffer ( "You are too stunned to do that.\n\r", ch );
			return TRUE;

		case POS_SLEEPING:
			/*
			 * I just know this is the path to a 12" 'if' statement.  :(
			 * But two players asked for it already!  -- Furey
			 */
			if ( !str_cmp ( social_table[cmd].name, "snore" ) )
			{ break; }
			writeBuffer ( "In your dreams, or what?\n\r", ch );
			return TRUE;

	}

	one_argument ( argument, arg );
	victim = NULL;
	if ( arg[0] == '\0' ) {
		act ( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
		act ( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
	} else if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
	} else if ( victim == ch ) {
		act ( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
		act ( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
	} else {
		act ( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
		act ( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
		act ( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

		if ( !IS_NPC ( ch ) && IS_NPC ( victim )
				&&   !IS_AFFECTED ( victim, AFF_CHARM )
				&&   IS_AWAKE ( victim )
				&&   victim->desc == NULL ) {
			switch ( number_bits ( 4 ) ) {
				case 0:

				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					act ( social_table[cmd].others_found,
						  victim, NULL, ch, TO_NOTVICT );
					act ( social_table[cmd].char_found,
						  victim, NULL, ch, TO_CHAR    );
					act ( social_table[cmd].vict_found,
						  victim, NULL, ch, TO_VICT    );
					break;

				case 9:
				case 10:
				case 11:
				case 12:
					act ( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
					act ( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
					act ( "$n slaps you.", victim, NULL, ch, TO_VICT    );
					break;
			}
		}
	}

	return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( const char *arg )
{

	if ( *arg == '\0' )
	{ return FALSE; }

	if ( *arg == '+' || *arg == '-' )
	{ arg++; }

	for ( ; *arg != '\0'; arg++ ) {
		if ( !isdigit ( *arg ) )
		{ return FALSE; }
	}

	return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument ( const char *argument, char *arg )
{
	char *pdot;
	int number;

	for ( pdot = ( char * ) argument; *pdot != '\0'; pdot++ ) {
		if ( *pdot == '.' ) {
			*pdot = '\0';
			number = atoi ( argument );
			*pdot = '.';
			strcpy ( arg, pdot + 1 );
			return number;
		}
	}

	strcpy ( arg, argument );
	return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument ( const char *argument, char *arg )
{
	char *pdot;
	int number;

	for ( pdot = ( char * ) argument; *pdot != '\0'; pdot++ ) {
		if ( *pdot == '*' ) {
			*pdot = '\0';
			number = atoi ( argument );
			*pdot = '*';
			strcpy ( arg, pdot + 1 );
			return number;
		}
	}

	strcpy ( arg, argument );
	return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument ( const char *argument, char *arg_first )
{
	char cEnd;

	char *rargument = ( char * ) argument;

	while ( isspace ( *rargument ) )
	{ rargument++; }

	cEnd = ' ';
	if ( *rargument == '\'' || *rargument == '"' )
	{ cEnd = *rargument++; }

	while ( *rargument != '\0' ) {
		if ( *rargument == cEnd ) {
			rargument++;
			break;
		}
		*arg_first = LOWER ( *rargument );
		arg_first++;
		rargument++;
	}
	*arg_first = '\0';

	while ( isspace ( *rargument ) )
	{ rargument++; }

	return rargument;
}

/*
 * Contributed by Alander.
 */
DefineCommand ( cmd_commands )
{
	char buf[MAX_STRING_LENGTH];
	int cmd;
	int col;

	col = 0;
	for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ ) {
		if ( cmd_table[cmd].level <  LEVEL_HERO
				&&   cmd_table[cmd].level <= get_trust ( ch )
				&&   cmd_table[cmd].show ) {
			sprintf ( buf, "%-12s", cmd_table[cmd].name );
			writeBuffer ( buf, ch );
			if ( ++col % 6 == 0 )
			{ writeBuffer ( "\n\r", ch ); }
		}
	}

	if ( col % 6 != 0 )
	{ writeBuffer ( "\n\r", ch ); }
	return;
}

DefineCommand ( cmd_wizhelp )
{
	char buf[MAX_STRING_LENGTH];
	int cmd;
	int col;

	col = 0;
	for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ ) {
		if ( cmd_table[cmd].level >= LEVEL_HERO
				&&   cmd_table[cmd].level <= get_trust ( ch )
				&&   cmd_table[cmd].show ) {
			sprintf ( buf, "%-12s", cmd_table[cmd].name );
			writeBuffer ( buf, ch );
			if ( ++col % 6 == 0 )
			{ writeBuffer ( "\n\r", ch ); }
		}
	}

	if ( col % 6 != 0 )
	{ writeBuffer ( "\n\r", ch ); }
	return;
}

