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
	{ "events",	cmd_events,	CR_CODER, LOG_NORMAL, 1, "Review currently active Events" },
	{ "os",		cmd_os,		CR_CODER, LOG_NORMAL, 1, "View operating system related data" },
	{ "makestaff",	cmd_makestaff,	CR_CODER, LOG_ALWAYS, 1, "Toggle staff flags on/off for selected users" },
	{ "sitrep",	cmd_sitrep,	CR_CODER, LOG_NORMAL, 1, "Sitrep displays system logs to the coder" },
	{ "levelset",	cmd_advance,	CR_HEAD,  LOG_ALWAYS, 1, "Change the level of a selected PC" },
	{ "randomitem", cmd_randomitem, CR_HEAD,  LOG_ALWAYS, 1, "Generates a random item on the player" },

	{ "dump",	cmd_dump,	CR_CODER, LOG_ALWAYS, 1, "Dump drops current memory buckets to flatfile for debugging" },
	{ "trust",	cmd_trust,	CR_HEAD,  LOG_ALWAYS, 1, "Trust allows you to raise/lower the trust value" },
	{ "violate",	cmd_violate,	CR_SECURITY,  LOG_ALWAYS, 1, "Violate allows you to enter rooms you couldn't normally enter" },

	{ "shutdown",	cmd_shutdown,	CR_CODER, LOG_ALWAYS, 1, "Shutdown will powerdown the mud" },
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
	{ "load",	cmd_load, CR_RELATIONS | CR_BUILDER, LOG_ALWAYS, 1, "Load a NPC/Item into the game" },
	{ "newblockdown", cmd_newbielockdown,	CR_SECURITY,  LOG_ALWAYS, 1, "Block newbies from entering the MUD" },

	{ "flag",	cmd_flag,	CR_RELATIONS,	LOG_ALWAYS, 1, "Change assigned flags to a loaded Item or NPC/Player" },
	{ "freeze",	cmd_freeze,	CR_SECURITY,	LOG_ALWAYS, 1, "Completely freeze a player in his/her place"},
	{ "pecho",	cmd_pecho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message without giving a name" },
	{ "purge",	cmd_purge,	CR_BUILDER,     LOG_ALWAYS, 1, "Purge a loaded Item/NPC from the MUD" },
	{ "restore",	cmd_restore,	CR_RELATIONS,	LOG_ALWAYS, 1, "Restore a player to full health" },
	{ "vnum",	cmd_vnum,	CR_RELATIONS | CR_BUILDER, LOG_NORMAL, 1, "Find the vnum of a Item/NPC" },
	{ "zecho",	cmd_zecho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message to a zone" },
	{ "guild",	cmd_guild,	CR_RELATIONS,   LOG_ALWAYS, 1, "Move a player into a Guild" },
	{ "sockets",    cmd_sockets,	CR_SECURITY | CR_HEAD, LOG_NORMAL, 1, "See the currently connected sockets" },

	{ "peace",	cmd_peace,	CR_RELATIONS | CR_SECURITY,  LOG_NORMAL, 1, "Stop all fighting in a room" },
	{ "snoop",	cmd_snoop,	CR_SECURITY,	LOG_ALWAYS, 1, "Personally review a players actions as if through their eyes" },
	{ "string",	cmd_string,	CR_RELATIONS,	LOG_ALWAYS, 1, "Change the string values of Items/NPC's for quests" },
	{ "clone",	cmd_clone,	CR_RELATIONS,	LOG_ALWAYS, 1, "Create a carbon copy of an Item" },
	{ "nochannels",	cmd_nochannels,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore channel Privilages" },
	{ "noemote",	cmd_noemote,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore emote Privilages" },
	{ "noshout",	cmd_noshout,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore shout Privilages" },
	{ "notell",	cmd_notell,	CR_SECURITY,	LOG_ALWAYS, 1, "Strip away or Restore tell Privilages" },
	{ "transfer",	cmd_transfer,	CR_SECURITY | CR_RELATIONS, LOG_ALWAYS, 1, "Transfer a selected NPC/Player to your location" },

	{ "at",         cmd_at,         CR_RELATIONS,   LOG_NORMAL, 1, "Perform a command as if you were at the selected NPC/Player" },
	{ "echo",	cmd_recho,	CR_RELATIONS,	LOG_ALWAYS, 1, "Echo a message to a room" },
	{ "return",     cmd_return,     CR_RELATIONS,   LOG_NORMAL, 1, "Return from the switched state" },
	{ "switch",	cmd_switch,	CR_RELATIONS,	LOG_ALWAYS, 1, "Switch into the body of a NPC" },
	{ "tweet",	cmd_tweet,	CR_RELATIONS,	LOG_ALWAYS, 1, "Post to our twitter account" },
	{ "twitlist",	cmd_twitlist,	CR_RELATIONS,   LOG_NORMAL, 1, "Review and remove tweets posted to our system" },

	{ "penalty",	cmd_penalty,	CR_SECURITY,	LOG_NORMAL, 1, "Review security message board" },
	{ "force",	cmd_force,	CR_STAFF,	LOG_ALWAYS, 1, "Force a player to perform an action" },
	{ "goto",       cmd_goto,       CR_STAFF,       LOG_NORMAL, 1, "Goto a selected location" },

	{ "poofin",	cmd_bamfin,	CR_STAFF,       LOG_NORMAL, 1, "Change the message for when you goto a room" },
	{ "poofout",	cmd_bamfout,	CR_STAFF,       LOG_NORMAL, 1, "Change the message for when you use goto to leave a room" },

	{ "godsight",	cmd_holylight,	CR_STAFF,	LOG_NORMAL, 1, "See special data about Items/Rooms/NPC's" },
	{ "incognito",	cmd_incognito,	CR_STAFF,	LOG_NORMAL, 1, "Become invisible within the room only" },
	{ "invis",	cmd_invis,	CR_STAFF,	LOG_NORMAL, 0, "Become invisible to the whole mud" },
	{ "memory",	cmd_memory,	CR_CODER,	LOG_NORMAL, 1, "Review current memory usage" },
	{ "mwhere",	cmd_mwhere,	CR_STAFF,	LOG_NORMAL, 1, "Find the location of a NPC" },
	{ "owhere",	cmd_owhere,	CR_STAFF,	LOG_NORMAL, 1, "Find the location of a Item" },
	{ "stat",	cmd_stat,	CR_STAFF,	LOG_NORMAL, 1, "Find the stats of an Item/NPC/Player" },
	{ "wizinvis",	cmd_invis,	CR_STAFF,	LOG_NORMAL, 1, "Become invisible" },
	{ "staffaid",	cmd_wiznet,	CR_STAFF,	LOG_NORMAL, 1, "Display special staff flagged messages" },
	{ "stalk",	cmd_immtalk,	CR_STAFF,	LOG_NORMAL, 1, "Staff communication channel" },
	{ "imotd",      cmd_imotd,      CR_STAFF,       LOG_NORMAL, 1, "Review the Staff Message of the Day" },
	{ "smote",	cmd_smote,	CR_RELATIONS,	LOG_NORMAL, 1, "Exactly like pmote just requires you to enter your name" },
	{ "prefi",	cmd_prefi,	CR_STAFF,	LOG_NORMAL, 0, "" },
	{ "prefix",	cmd_prefix,	CR_STAFF,	LOG_NORMAL, 1, "Assign a prefix to staffaid" },
	{ "noloot",		cmd_noloot,	CR_STAFF,  LOG_NORMAL, 1, "Prevent your corpse from being looted" },
	{ "nosummon",	cmd_nosummon,	CR_STAFF, LOG_NORMAL, 1, "Prevent players from summoning you to them" },

	{ "mpdump",	cmd_mpdump,	CR_CODER,	LOG_NEVER,  1, "Dump mudprog data to flatfile" },
	{ "mpstat",	cmd_mpstat,	CR_CODER,	LOG_NEVER,  1, "Get the status of current mudprogs" },
	{ "commands",	cmd_scommands,	CR_STAFF,	LOG_NORMAL, 1, "See a list of staff commands" },

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

	{ "north",		cmd_north,	POS_STANDING,    0,  LOG_NEVER, 1, CAT_MOVE },
	{ "east",		cmd_east,	POS_STANDING,	 0,  LOG_NEVER, 1, CAT_MOVE },
	{ "south",		cmd_south,	POS_STANDING,	 0,  LOG_NEVER, 1, CAT_MOVE },
	{ "west",		cmd_west,	POS_STANDING,	 0,  LOG_NEVER, 1, CAT_MOVE },
	{ "up",			cmd_up,		POS_STANDING,	 0,  LOG_NEVER, 1, CAT_MOVE },
	{ "down",		cmd_down,	POS_STANDING,	 0,  LOG_NEVER, 1, CAT_MOVE },

	{ "cast",		cmd_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "auction",        	cmd_auction,     POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "buy",		cmd_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "channels",       	cmd_channels,    POS_DEAD,        0,  LOG_NORMAL, 1, CAT_COMM },
	{ "exits",		cmd_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "get",		cmd_get,		POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "group",          	cmd_group,       POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_INFO },
	{ "hit",		cmd_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, CAT_COMBAT },
	{ "backpack",		cmd_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "kill",		cmd_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "look",		cmd_look,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "clan",		cmd_clantalk,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "music",          cmd_music,   	POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_COMM },
	{ "order",		cmd_order,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "practice",   cmd_practice,	POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_INFO },
	{ "rest",		cmd_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "sit",		cmd_sit,	POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "stand",		cmd_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "tell",		cmd_tell,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "unlock",     cmd_unlock,      POS_RESTING,    0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "wield",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },

	{ "affects",		cmd_affects,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "zones",		cmd_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "changes",		cmd_changes,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "commands",		cmd_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "compare",		cmd_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "consider",		cmd_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "count",		cmd_count,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "credits",		cmd_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "gear",		cmd_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "examine",		cmd_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "help",		cmd_help,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "idea",		cmd_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "info",           	cmd_groups,     POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_INFO },
	{ "motd",		cmd_motd,	POS_DEAD,        0,  LOG_NORMAL, 1, CAT_INFO },
	{ "news",		cmd_news,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "read",		cmd_read,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "report",		cmd_report,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "rules",		cmd_rules,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "sheet",		cmd_score,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "skills",		cmd_skills,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "socials",		cmd_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "show",		cmd_show,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "spells",		cmd_spells,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "story",		cmd_story,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "time",		cmd_time,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "weather",		cmd_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "users",		cmd_users,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "worth",		cmd_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_INFO },

	{ "levelup",		cmd_levelup,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_CONFIG },

	{ "alia",		cmd_alia,	POS_DEAD,	 0,  LOG_NORMAL, 0, CAT_CONFIG },
	{ "alias",		cmd_alias,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "brief",		cmd_brief,	POS_DEAD,        0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "config",   cmd_config,   POS_DEAD,  0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "configstatus",	cmd_configstatus,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "colours",	cmd_colours,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "description",	cmd_description,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "delet",		cmd_delet,	POS_DEAD,	 0,  LOG_ALWAYS, 0, CAT_CONFIG },
	{ "delete",		cmd_delete,	POS_STANDING,	 0,  LOG_ALWAYS, 1, CAT_CONFIG },
	{ "nofollow",	cmd_nofollow,	POS_DEAD,        0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "outfit",		cmd_outfit,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "password",	cmd_password,	POS_DEAD,	 0,  LOG_NEVER,  1, CAT_CONFIG },
	{ "prompt",		cmd_prompt,	POS_DEAD,        0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "scroll",		cmd_scroll,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "title",		cmd_title,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "unalias",	cmd_unalias,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "wimpy",		cmd_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },

	{ "afk",		cmd_afk,		POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "answer",		cmd_answer,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "deaf",		cmd_deaf,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "emote",		cmd_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "pmote",		cmd_pmote,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "gossip",		cmd_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "grats",		cmd_grats,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "gtell",		cmd_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "note",		cmd_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "question",		cmd_question,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "quote",		cmd_quote,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "quiet",		cmd_quiet,	POS_SLEEPING, 	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "reply",		cmd_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "replay",		cmd_replay,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "say",		cmd_say,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },
	{ "shout",		cmd_shout,	POS_RESTING,	 3,  LOG_NORMAL, 1, CAT_COMM },
	{ "unread",		cmd_unread,	POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_COMM },
	{ "yell",		cmd_yell,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMM },

	{ "about",      cmd_about,  POS_RESTING,     0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "brandish",	cmd_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "close",		cmd_close,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "drink",		cmd_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "drop",		cmd_drop,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "eat",		cmd_eat,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "envenom",		cmd_envenom,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "fill",		cmd_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "give",		cmd_give,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "heal",		cmd_heal,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "hold",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "list",		cmd_list,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "lock",		cmd_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "open",		cmd_open,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "pick",		cmd_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "pour",		cmd_pour,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "put",		cmd_put,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "quaff",		cmd_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "recite",		cmd_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "remove",		cmd_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "sell",		cmd_sell,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "take",		cmd_get,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "sacrifice",		cmd_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "use",		cmd_use,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "value",		cmd_value,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "wear",		cmd_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "zap",		cmd_zap,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "search", cmd_search, POS_RESTING, 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "unarm",  cmd_unarm, POS_RESTING,  0,  LOG_NORMAL, 1, CAT_ITEM },

	{ "backstab",		cmd_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "bash",		cmd_bash,	POS_FIGHTING,    0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "bs",			cmd_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, CAT_COMBAT },
	{ "berserk",		cmd_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "dirt",		cmd_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "disarm",		cmd_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "flee",		cmd_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "kick",		cmd_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "murde",		cmd_murde,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, CAT_COMBAT },
	{ "murder",		cmd_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS, 1, CAT_COMBAT },
	{ "rescue",		cmd_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 0, CAT_COMBAT },
	{ "surrender",		cmd_surrender,	POS_FIGHTING,    0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "trip",		cmd_trip,	POS_FIGHTING,    0,  LOG_NORMAL, 1, CAT_COMBAT },

	{ "mob",		cmd_mob,	POS_DEAD,	 0,  LOG_NEVER,  0, CAT_INFO },

	{ "enter", 		cmd_enter, 	POS_STANDING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "follow",		cmd_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "go",			cmd_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0, CAT_MOVE },
	{ "groups",		cmd_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 1, CAT_INFO },
	{ "hide",		cmd_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "play",		cmd_play,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "qui",		cmd_qui,	POS_DEAD,	 0,  LOG_NORMAL, 0, CAT_CONFIG },
	{ "quit",		cmd_quit,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "recall",		cmd_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "rent",		cmd_rent,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_CONFIG },
	{ "save",		cmd_save,	POS_DEAD,	 0,  LOG_NORMAL, 0, CAT_CONFIG },
	{ "sleep",		cmd_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "sneak",		cmd_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "split",		cmd_split,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "steal",		cmd_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1, CAT_ITEM },
	{ "train",		cmd_train,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "visible",		cmd_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_COMBAT },
	{ "version",		cmd_version,	POS_DEAD,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ "wake",		cmd_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, CAT_MOVE },
	{ "where",		cmd_where,	POS_RESTING,	 0,  LOG_NORMAL, 1, CAT_INFO },
	{ NULL,		0,		POS_DEAD,	 0,  LOG_NORMAL, 0, CAT_INFO }
};


void attempt_staff_command ( Creature *ch, const std::string &pcomm, const std::string &argument )
{
	char logline[MAX_INPUT_LENGTH];		// -- for the relic spy code from ROM.
	std::string staff_command = pcomm;
	staff_command.erase ( 0, pcomm.find_first_not_of ( '/' ) );
	int cmd = 0;
	bool found = false;

	// -- log all of our staff commands executed under debug due to the volatile nature.
	log_hd ( LOG_DEBUG, Format ( " --- %s --- Staff Command: %s     Args: %s", ch->name, !pcomm.empty() ? pcomm.c_str() : "", !argument.empty() ? argument.c_str() : "" ) );

	// -- NPC's shouldn't EVER execute staff commands, thats why they have their mudprog commands.
	if ( IS_NPC ( ch ) ) {
		log_hd ( LOG_DEBUG | LOG_SECURITY, Format ( "\t--- NPC executing staff command!  Security issue!!!!! (%s)", ch->name ) );
	}

	// -- create our logline (gotta support spys everywhere!)
	strcpy ( logline, Format ( "%s %s", pcomm.c_str(), argument.empty() ? "" : argument.c_str() ) );

	for ( cmd = 0; !IS_NULLSTR ( staff_cmd_table[cmd].name ); cmd++ ) {
		if ( staff_command[0] == staff_cmd_table[cmd].name[0]
				&&  SameString ( staff_command.c_str(), staff_cmd_table[cmd].name ) ) {
			found = true;
			break;
		}
	}

	// -- either it isn't found or we are not flagged for that command
	if ( !found ) {
		writeBuffer ( "Unknown Option.\r\n", ch );
		return;
	}

	// -- god mode? Oh Boooii!
	if ( !mGodMode ) {
		if ( !IS_SET ( ch->sflag, staff_cmd_table[cmd].flag ) ) {
			writeBuffer ( "Unknown Option.\r\n", ch );
			return;
		}
	}
	// -- LOG_NEVER means we stop logging it! (Atleast to the wiznet/staffaid channel)
	if ( staff_cmd_table[cmd].log == LOG_NEVER )
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
		( *staff_cmd_table[cmd].cmd_fun ) ( ch, staff_command.c_str(), argument.c_str(), cmd );
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

	// -- handle the query system!
	if ( !IS_NPC ( ch ) ) {
		if ( ch->queries.queryintcommand ) {
			if ( ( *ch->queries.queryintfunc ) ( ch, ch->queries.queryintcommand, argument ) )
			{ return; }
		}
	}

	ch->queries.querycommand = 0;
	ch->queries.querydata = NULL;
	ch->queries.queryfunc = NULL;

	// -- are we utilizing a staff command? Lets find out today!?
	if ( argument[0] == '/' ) {
		char argbuf[100000];
		char *comm1;

		comm1 = ChopC ( argument, argbuf );
		attempt_staff_command ( ch, argbuf, comm1 );
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
		argument = ChopC ( argument, command );
	}

	/*
	 * Look for command in command table.
	 */
	found = FALSE;
	trust = get_trust ( ch );
	for ( cmd = 0; !IS_NULLSTR ( cmd_table[cmd].name ); cmd++ ) {
		if ( command[0] == cmd_table[cmd].name[0]
				&&   !str_prefix ( command, cmd_table[cmd].name )
				&&   cmd_table[cmd].level <= trust ) {
			found = TRUE;
			break;
		}
	}

	// -- Not a real thing ?
	if ( cmd_table[cmd].log == LOG_NEVER )
	{ strcpy ( logline, "" ); }

	// -- log the command data
	if ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_LOG ) )
			||   fLogAll
			||   cmd_table[cmd].log == LOG_ALWAYS ) {
		wiznet ( Format ( "Log: %s %s", ch->name, logline ), ch, NULL, WIZ_SECURE, 0, get_trust ( ch ) );
		log_hd ( LOG_COMMAND, Format ( "%s has executed: %s", ch->name, !IS_NULLSTR ( logline ) ? logline : "{SUSPECT LOG_NEVER, NO LOGLINE DATA RECEIVED}" ) );
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
		( *cmd_table[cmd].cmd_fun ) ( ch, command, argument, 0 ); // -- we use 0 instead of cmd as cmd is the actual command number.
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
	//	ASSERT_THROW ( !cmd_fun, "_cmd_function called with a null CmdData (CmdData *cmd_fun is NULL)." );

	try {
		char command_string[MAX_OUTPUT_BUFFER] = {'\0'};

		if ( !cmd_fun ) {	THROW_ERROR ( "cmd_fun is NULL" );	}

		// -- typically this should be an impossibility.  But just in-case
		if ( strlen ( argument ) > MAX_OUTPUT_BUFFER ) {
			THROW_ERROR ( "_cmd_function: Argument supplied exceeds MAX_OUTPUT_BUFFER length" );
		}

		strncpy ( command_string, argument, MAX_OUTPUT_BUFFER );

		// -- cmd_function now puts this data in the debug log for safe keeping, incase the dispatch causes a crash.
		log_hd ( LOG_DEBUG, Format ( "%s->%s->%d->cmd_function->Cmd:%s(%p) Cr:%s(%p) Args:(%s)", file, function, line, L_command, cmd_fun, ch ? ch->name : "", ch, argument ? argument : "{NULL DATA}" ) );

		/* dispatch the command */
		( *cmd_fun ) ( ch, L_command, command_string, -1 );

		tail_chain();
	} catch ( ... ) {
		CATCH ( false );
	}
	return;
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
			if ( SameString ( social_table[cmd].name, "snore" ) )
			{ break; }
			writeBuffer ( "In your dreams, or what?\n\r", ch );
			return TRUE;
	}

	ChopC ( argument, arg );
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
			switch ( Math::instance().bits ( 4 ) ) {
				case 0:

				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					act ( social_table[cmd].others_found, victim, NULL, ch, TO_NOTVICT );
					act ( social_table[cmd].char_found, victim, NULL, ch, TO_CHAR    );
					act ( social_table[cmd].vict_found, victim, NULL, ch, TO_VICT    );
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

DefineCommand ( cmd_commands )
{
	BUFFER *bf = new_buf();
	int lcmd;
	int col;
	char char_level;

	col = 0;
	for ( int x = 0; x != MAX_CATEGORY; x++ ) {
		if ( col != 0 ) {
			col = 0;
			add_buf ( bf, "\r\n" );
		}

		switch ( x ) {
			default:
				break;
			case CAT_MOVE:
				add_buf ( bf, 	"\ay*** \aRMovement \ay***\an\r\n" );
				break;
			case CAT_COMM:
				add_buf ( bf, 	"\ay*** \aRCommunication \ay***\an\r\n" );
				break;
			case CAT_COMBAT:
				add_buf ( bf, 	"\ay*** \aRCombat / Skills \ay***\an\r\n" );
				break;
			case CAT_ITEM:
				add_buf ( bf, 	"\ay*** \aRItem \ay***\an\r\n" );
				break;
			case CAT_INFO:
				add_buf ( bf, 	"\ay*** \aRInformation \ay***\an\r\n" );
				break;
			case CAT_CONFIG:
				add_buf ( bf, 	"\ay*** \aRConfigurational \ay***\an\r\n" );
				break;
		};
		for ( char_level = 'a'; char_level <= 'z'; char_level++ ) { 		// -- alphabetical sort (kind of)
			for ( lcmd = 0; !IS_NULLSTR ( cmd_table[lcmd].name ); lcmd++ ) {	// -- loop through the actual commands
				if ( cmd_table[lcmd].level <= get_trust ( ch )
						&&   cmd_table[lcmd].show
						&&   cmd_table[lcmd].category == x
						&&   cmd_table[lcmd].name[0] == char_level ) {		// -- same initial character
					add_buf ( bf, Format ( "\ac%-12s", cmd_table[lcmd].name ) );
					if ( ++col == 5 )
					{ add_buf ( bf, "\r\n" ); col = 0; }
				}
			}
		}
	}
	if ( col != 0 )
	{ add_buf ( bf, "\r\n" ); }

	add_buf ( bf, "\an" );		// -- stop colour bleeding
	writePage ( buf_string ( bf ), ch );
	recycle_buf ( bf );
	return;
}

DefineCommand ( cmd_scommands )
{
	BUFFER *output = new_buf();
	int lcmd;
	int col = 0;
	char char_level;

	for ( char_level = 'a'; char_level <= 'z'; char_level++ ) {
		for ( lcmd = 0; !IS_NULLSTR ( staff_cmd_table[lcmd].name ); lcmd++ ) {
			if ( staff_cmd_table[lcmd].name[0] != char_level ) { continue; }
			if ( IS_SET ( ch->sflag, staff_cmd_table[lcmd].flag ) ) {
				if ( staff_cmd_table[lcmd].show ) {
					char nbuf[100] = {'\0'};
					snprintf ( nbuf, 100, "/%s", staff_cmd_table[lcmd].name );
					add_buf ( output, Format ( "\ay%13s \ac- \aw%.30s.\t", nbuf, staff_cmd_table[lcmd].helpmsg ) );
					if ( ++col == 2 ) { add_buf ( output, "\r\n" ); col = 0; }
				}
			}
		}
	}
	if ( col != 0 ) { add_buf ( output, "\r\n" ); }

	add_buf ( output, "\an" );		// -- stop colour-bleed
	writePage ( buf_string ( output ), ch );
	recycle_buf ( output );
	return;
}


// -- EOF

