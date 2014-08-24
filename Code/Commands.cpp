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
#include "recycle.h"

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

const struct staff_cmd_type staff_cmd_table[] = {
	{ "makestaff",	cmd_makestaff,	CR_CODER, LOG_ALWAYS, 1, "Toggle staff flags on/off for selected users" },
	{ "sitrep",	cmd_sitrep,	CR_CODER, LOG_NORMAL, 1, "Sitrep displays system logs to the coder" },
	{ "advance",	cmd_advance,	CR_HEAD,  LOG_ALWAYS, 1, "Advance allows raising/lowering of levels" },
	{ "dump",	cmd_dump,	CR_CODER, LOG_ALWAYS, 1, "Dump drops current memory buckets to flatfile for debugging" },
	{ "trust",	cmd_trust,	CR_HEAD,  LOG_ALWAYS, 1, "Trust allows you to raise/lower the trust value" },
	{ "violate",	cmd_violate,	CR_SECURITY,  LOG_ALWAYS, 1, "Violate allows you to enter rooms you couldn't normally enter" },

	{ "shutdow",	cmd_shutdow,	CR_CODER, LOG_NORMAL, 0, "" },
	{ "shutdown",	cmd_shutdown,	CR_CODER, LOG_ALWAYS, 1, "Shutdown will powerdown the mud" },
	{ "reboo",	cmd_reboo,	CR_CODER, LOG_NORMAL, 0, "" },
	{ "reboot",	cmd_reboot,	CR_CODER, LOG_ALWAYS, 1, "Reboot will cause the mud to shutdown and restart" },
	{ "log",	cmd_log,	CR_SECURITY, LOG_ALWAYS, 1, "Log allows you to log user input" },
	{ "permban",	cmd_permban,	CR_SECURITY, LOG_ALWAYS, 1, "Permban will block a site forever" },
	{ "protect",	cmd_protect,	CR_SECURITY, LOG_ALWAYS, 1, "Protect prevents selected user from being snooped" },
	{ "deny",	cmd_deny,	CR_SECURITY, LOG_ALWAYS, 1, "Deny will block a players access to the mud" },

	{ "allow",	cmd_allow,	CR_SECURITY, LOG_ALWAYS, 1, "Allow will revoke a siteban" },
	{ "ban",	cmd_ban,	CR_SECURITY, LOG_ALWAYS, 1, "Ban will block a site from connecting" },
	{ "lockdown",	cmd_lockdown,	CR_SECURITY, LOG_ALWAYS, 1, "Lockdown will prevent all non-staff from entering the MUD" },

	{ "set",	cmd_set,	CR_RELATIONS, LOG_ALWAYS, 1, "Set will allow you to set the stats of items/players/npcs" },

	{ "disconnect",	cmd_disconnect,	CR_SECURITY, LOG_ALWAYS, 1, "Disconnect will cause a connected player to disconnect" },
	{ "pardon",	cmd_pardon,	CR_SECURITY, LOG_ALWAYS, 1, "Revoke a previous punishment" },
	
	{ "sla",	cmd_sla,	CR_BUILDER,  LOG_NORMAL, 0, "" },
	{ "slay",	cmd_slay,	CR_BUILDER,  LOG_ALWAYS, 1, "Slay will terminate the life an NPC or Player (to be used in testing only)" },

	{ "gecho",	cmd_echo,	CR_RELATIONS, LOG_ALWAYS, 1, "Globally echo a selected string" },
	{ "load",	cmd_load, CR_RELATIONS|CR_BUILDER, LOG_ALWAYS, 1, "Load a NPC/Item into the game" },
	{ "newbielockdown",	cmd_newbielockdown,	CR_SECURITY,  LOG_ALWAYS, 1, "Block newbies from entering the MUD" },

	{ "flag",	cmd_flag,	CR_RELATIONS,	LOG_ALWAYS, 1, "Change assigned flags to a loaded Item or NPC/Player" },
	{ "freeze",	cmd_freeze,	CR_SECURITY,	LOG_ALWAYS, 1, "Completely freeze a player in his/her place"},
	{ "pecho",	cmd_pecho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message without giving a name" },
	{ "purge",	cmd_purge,	CR_BUILDER,     LOG_ALWAYS, 1, "Purge a loaded Item/NPC from the MUD" },
	{ "restore",	cmd_restore,	CR_RELATIONS,	LOG_ALWAYS, 1, "Restore a player to full health" },
	{ "vnum",	cmd_vnum,	CR_RELATIONS|CR_BUILDER, LOG_NORMAL, 1, "Find the vnum of a Item/NPC" },
	{ "zecho",	cmd_zecho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message to a zone" },
	{ "guild",	cmd_guild,	CR_RELATIONS,   LOG_ALWAYS, 1, "Move a player into a Guild" },
	{ "sockets",    cmd_sockets,	CR_SECURITY|CR_HEAD, LOG_NORMAL, 1, "See the currently connected sockets" },

	{ "peace",	cmd_peace,	CR_RELATIONS|CR_SECURITY,  LOG_NORMAL, 1, "Stop all fighting in a room" },
	{ "snoop",	cmd_snoop,	CR_SECURITY,	LOG_ALWAYS, 1, "Personally review a players actions as if through their eyes" },
	{ "string",	cmd_string,	CR_RELATIONS,	LOG_ALWAYS, 1, "Change the string values of Items/NPC's for quests" },
	{ "clone",	cmd_clone,	CR_RELATIONS,	LOG_ALWAYS, 1, "Create a carbon copy of an Item" },
	{ "nochannels",	cmd_nochannels,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore channel Privilages" },
	{ "noemote",	cmd_noemote,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore emote Privilages" },
	{ "noshout",	cmd_noshout,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore shout Privilages" },
	{ "notell",	cmd_notell,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore tell Privilages" },
	{ "transfer",	cmd_transfer,	CR_SECURITY|CR_RELATIONS, LOG_ALWAYS, 1, "Transfer a selected NPC/Player to your location" },

	{ "at",         cmd_at,         CR_RELATIONS,   LOG_NORMAL, 1, "Perform a command as if you were at the selected NPC/Player" },
	{ "echo",	cmd_recho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message" },
	{ "return",     cmd_return,     CR_RELATIONS,   LOG_NORMAL, 1, "Return from the switched state" },
	{ "switch",	cmd_switch,	CR_RELATIONS,	LOG_ALWAYS, 1, "Switch into the body of a NPC" },

	{ "penalty",	cmd_penalty,	CR_SECURITY,	LOG_NORMAL, 1, "Review security message board" },
	{ "force",	cmd_force,	CR_STAFF,	LOG_ALWAYS, 1, "Force a player to perform an action" },
	{ "goto",       cmd_goto,       CR_STAFF,       LOG_NORMAL, 1, "Goto a selected location" },

	{ "poofin",	cmd_bamfin,	CR_STAFF,       LOG_NORMAL, 1, "Change the message for when you goto a room" },
	{ "poofout",	cmd_bamfout,	CR_STAFF,       LOG_NORMAL, 1, "Change the message for when you use goto to leave a room" },

	{ "godsight",	cmd_holylight,	CR_STAFF,	LOG_NORMAL, 1, "See special data about Items/Rooms/NPC's" },
	{ "incognito",	cmd_incognito,	CR_STAFF,	LOG_NORMAL, 1, "Become invisible within the room only" },
	{ "invis",	cmd_invis,	CR_STAFF,	LOG_NORMAL, 0, "Become invisible" },
	{ "memory",	cmd_memory,	CR_CODER,	LOG_NORMAL, 1, "Review current memory usage" },
	{ "mwhere",	cmd_mwhere,	CR_STAFF,	LOG_NORMAL, 1, "Find the location of a NPC" },
	{ "owhere",	cmd_owhere,	CR_STAFF,	LOG_NORMAL, 1, "Find the location of a Item" },
	{ "stat",	cmd_stat,	CR_STAFF,	LOG_NORMAL, 1, "Find the stats of an Item/NPC/Player" },
	{ "wizinvis",	cmd_invis,	CR_STAFF,	LOG_NORMAL, 1, "Become invisible" },
	{ "staffaid",	cmd_wiznet,	CR_STAFF,	LOG_NORMAL, 1, "Display special staff flagged messages" },
	{ "stalk",	cmd_immtalk,	CR_STAFF,	LOG_NORMAL, 1, "Staff communication channel" },
	{ "imotd",      cmd_imotd,      CR_STAFF,       LOG_NORMAL, 1, "Review the Staff Message of the Day" },
	{ ":",		cmd_immtalk,	CR_STAFF,	LOG_NORMAL, 0, "" },
	{ "smote",	cmd_smote,	CR_RELATIONS,	LOG_NORMAL, 1, "Exactly like pmote just requires you to enter your name" },
	{ "prefi",	cmd_prefi,	CR_STAFF,	LOG_NORMAL, 0, "" },
	{ "prefix",	cmd_prefix,	CR_STAFF,	LOG_NORMAL, 1, "Assign a prefix to staffaid" },
	{ "mpdump",	cmd_mpdump,	CR_CODER,	LOG_NEVER,  1, "Dump mudprog data to flatfile" },
	{ "mpstat",	cmd_mpstat,	CR_CODER,	LOG_NEVER,  1, "Get the status of current mudprogs" },
	{ "wizhelp",	cmd_wizhelp,	CR_STAFF,	LOG_NORMAL, 1, "See a list of staff commands" },

	{ "edit",	cmd_olc,	CR_BUILDER,    LOG_NORMAL, 1, "Online Creation Suite control command" },
	{ "asave",      cmd_asave,	CR_BUILDER,    LOG_NORMAL, 1, "Save work completed within the Online Creation Suite" },
	{ "zlist",	cmd_alist,	CR_BUILDER,    LOG_NORMAL, 1, "List all zones within the MUD"},
	{ "resets",	cmd_resets,	CR_BUILDER,    LOG_NORMAL, 1, "Review/Edit the resets of a given room" },
	{ "hedit",	cmd_hedit,	CR_BUILDER,    LOG_NORMAL, 1, "Online Creation: Help Editor (Beta Build)" },
	{ NULL, NULL, 0, 0, 0, NULL },
};

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

	{ "cast",		cmd_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
	{ "auction",        cmd_auction,     POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "buy",		cmd_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "channels",       cmd_channels,    POS_DEAD,        0,  LOG_NORMAL, 1 },
	{ "exits",		cmd_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "get",		cmd_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "group",          cmd_group,       POS_SLEEPING,    0,  LOG_NORMAL, 1 },
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
	{ "stand",		cmd_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
	{ "tell",		cmd_tell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "unlock",         cmd_unlock,      POS_RESTING,     0,  LOG_NORMAL, 1 },
	{ "wield",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },

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
	{ "users",		cmd_users,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
	{ "worth",		cmd_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },

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
	{ "colours",	cmd_colours,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
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
	{ "value",		cmd_value,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "wear",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "zap",		cmd_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1 },

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

	{ "mob",		cmd_mob,		POS_DEAD,	 0,  LOG_NEVER,  0 },

	{ "enter", 		cmd_enter, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "follow",		cmd_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "gain",		cmd_gain,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
	{ "go",		cmd_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
	{ "groups",		cmd_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
	{ "hide",		cmd_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
	{ "play",		cmd_play,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
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

	{ "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0 }
};


void attempt_staff_command(Creature *ch, const std::string &pcomm, const std::string &argument ) {

	char logline[MAX_INPUT_LENGTH];		// -- for the relic spy code from ROM.
	std::string staff_command = pcomm;
        staff_command.erase ( 0, pcomm.find_first_not_of ( '/' ) );
        int cmd = 0;
	bool found = false;

	// -- log all of our staff commands executed under debug due to the volatile nature.
	log_hd(LOG_DEBUG, Format(" --- %s --- Staff Command: %s     Args: %s", ch->name, !pcomm.empty() ? pcomm.c_str() : "", !argument.empty() ? argument.c_str() : ""));

	// -- NPC's shouldn't EVER execute staff commands, thats why they have their mudprog commands.
	if(IS_NPC(ch)) {
		log_hd(LOG_DEBUG|LOG_SECURITY, Format("\t--- NPC executing staff command!  Security issue!!!!! (%s)", ch->name));
	}

	// -- create our logline (gotta support spys everywhere!)
	strcpy ( logline, Format("%s %s", pcomm.c_str(), argument.empty() ? "" : argument.c_str()) );

        for ( cmd = 0; !IS_NULLSTR(staff_cmd_table[cmd].name); cmd++ ) {
                if ( staff_command[0] == staff_cmd_table[cmd].name[0]
                &&  !str_cmp ( staff_command.c_str(), staff_cmd_table[cmd].name )) {
                	found = true;
                        break;
                }
        }

	// -- either it isn't found or we are not flagged for that command
	if(!found || !IS_SET(ch->sflag, staff_cmd_table[cmd].flag)) {
		writeBuffer("Unknown Option.\r\n",ch);
		return;
	}

	// -- LOG_NEVER means we stop logging it! (Atleast to the wiznet/staffaid channel)
	if ( cmd_table[cmd].log == LOG_NEVER )
	{ strcpy ( logline, "" ); }

	// -- log the command data
	if ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_LOG ) )
	||   fLogAll
	||   staff_cmd_table[cmd].log == LOG_ALWAYS ) {
		wiznet ( Format ( "Log: %s %s", ch->name, logline ), ch, NULL, WIZ_SECURE, 0, get_trust ( ch ) );
		log_hd ( LOG_SECURITY | LOG_COMMAND, Format ( "%s has executed: %s", ch->name, !IS_NULLSTR ( logline ) ? logline : "{SUSPECT LOG_NEVER, NO LOGLINE DATA RECEIVED}" ) );
	}

	// -- getting our snoop-on!
	if ( ch->desc != NULL && ch->desc->snoop_by != NULL ) {
		write_to_buffer ( ch->desc->snoop_by, "% ",    2 );
		write_to_buffer ( ch->desc->snoop_by, logline, 0 );
		write_to_buffer ( ch->desc->snoop_by, "\n\r",  2 );
	}


	// -- execute the command and log the details.
	log_hd ( LOG_DEBUG, Format ( "Pre-Staff-Cmd:  %s(%p) executed by %s, args: %s", staff_command.c_str(), staff_cmd_table[cmd].cmd_fun, ch->name, !argument.empty() ? argument.c_str() : "{No Args}" ) );

	// -- in case one of our commands throws an exception, we want to be careful and log what we can!
	try {
		( *staff_cmd_table[cmd].cmd_fun ) ( ch, staff_command.c_str(), argument.c_str() );
	} catch ( ... ) {
		CATCH ( false );
	}
	log_hd ( LOG_DEBUG, Format ( "Post-Staff-Cmd: %s(%p) executed by %s successfully!", staff_command.c_str(), staff_cmd_table[cmd].cmd_fun, ch ? ch->name : "{Creature non-existant post command}" ) );

	tail_chain( );
	return;
}

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

	// -- are we utilizing a staff command? Lets find out today!?
	if(argument[0] == '/') {
		char argbuf[100000];
		char *comm1;

		comm1 = one_argument(argument, argbuf);
		attempt_staff_command(ch, argbuf, comm1);
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
		log_hd ( LOG_SECURITY | LOG_COMMAND, Format ( "%s has executed: %s", ch->name, !IS_NULLSTR ( logline ) ? logline : "{SUSPECT LOG_NEVER, NO LOGLINE DATA RECEIVED}" ) );
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
	BUFFER *output = new_buf();
	int cmd;
	int col = 0;
	for ( cmd = 0; !IS_NULLSTR(staff_cmd_table[cmd].name); cmd++) {
		if(IS_SET(ch->sflag, staff_cmd_table[cmd].flag) ) {
			if(staff_cmd_table[cmd].show) {
				add_buf(output, Format("\ay%13s \ac- \aw%-.30s.\an\t", staff_cmd_table[cmd].name, staff_cmd_table[cmd].helpmsg));
				if(++col == 2) { add_buf(output, "\r\n"); col = 0; }
			}
		}
	}

	if(col != 0) { add_buf(output, "\r\n" ); }
	writePage(buf_string(output), ch);
	recycle_buf(output);
	return;
}

