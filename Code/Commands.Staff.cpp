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

/*
 * Local functions.
 */
RoomData *	find_location	args ( ( Creature *ch, const char *arg ) );

DefineCommand ( cmd_wiznet )
{
	int flag;
	char buf[MAX_STRING_LENGTH];

	if ( IS_NULLSTR ( argument ) ) {
		if ( IS_SET ( ch->wiznet, WIZ_ON ) ) {
			writeBuffer ( "Staff Aid turned on.\n\r", ch );
			REMOVE_BIT ( ch->wiznet, WIZ_ON );
		} else {
			writeBuffer ( "Staff Aid turned off!\n\r", ch );
			SET_BIT ( ch->wiznet, WIZ_ON );
		}
		return;
	}

	if ( !str_prefix ( argument, "on" ) ) {
		writeBuffer ( "Staff Aid turned on!\n\r", ch );
		SET_BIT ( ch->wiznet, WIZ_ON );
		return;
	}

	if ( !str_prefix ( argument, "off" ) ) {
		writeBuffer ( "Staff Aid turned off!\n\r", ch );
		REMOVE_BIT ( ch->wiznet, WIZ_ON );
		return;
	}

	/* show wiznet status */
	if ( !str_prefix ( argument, "status" ) ) {
		buf[0] = '\0';
		int col = 0;
		if ( !IS_SET ( ch->wiznet, WIZ_ON ) ) {
			strcat ( buf, "off " );
			col++;
		}
		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ ) {
			if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) ) {
				strcat ( buf, Format ( "%15s\t", wiznet_table[flag].name ) );
				if ( ++col == 5 ) {
					strcat ( buf, "\r\n" );
				}
			}
		}
		if ( col != 0 );
		strcat ( buf, "\n\r" );

		writeBuffer ( Format ( "Staff Aid:\n\r%s\r\n", buf ), ch );
		return;
	}

	if ( !str_prefix ( argument, "show" ) )
		/* list of all wiznet options */
	{
		buf[0] = '\0';
		int col = 0;
		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ ) {
			if ( wiznet_table[flag].level <= get_trust ( ch ) ) {
				strcat ( buf, Format ( "%15s\t", wiznet_table[flag].name ) );
				if ( ++col == 5 ) {
					col = 0;
					strcat ( buf, "\r\n" );
				}
			}
		}

		if ( col != 0 )
		{ strcat ( buf, "\n\r" ); }

		writeBuffer ( Format ( "Staff Aid options Available:\n\r%s", buf ), ch );
		return;
	}

	flag = wiznet_lookup ( argument );

	if ( flag == -1 || get_trust ( ch ) < wiznet_table[flag].level ) {
		writeBuffer ( "No such option.\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) ) {
		snprintf ( buf, sizeof ( buf ), "You will no longer see %s on staff aid.\n\r",
				   wiznet_table[flag].name );
		writeBuffer ( buf, ch );
		REMOVE_BIT ( ch->wiznet, wiznet_table[flag].flag );
		return;
	} else {
		snprintf ( buf, sizeof ( buf ), "You will now see %s on staff aid.\n\r",
				   wiznet_table[flag].name );
		writeBuffer ( buf, ch );
		SET_BIT ( ch->wiznet, wiznet_table[flag].flag );
		return;
	}

}

void wiznet ( const char *str, Creature *ch, Item *obj,
			  long flag, long flag_skip, int min_level )
{
	Socket *d;

	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->connected == CON_PLAYING
				&&  IsStaff ( d->character )
				&&  IS_SET ( d->character->wiznet, WIZ_ON )
				&&  ( !flag || IS_SET ( d->character->wiznet, flag ) )
				&&  ( !flag_skip || !IS_SET ( d->character->wiznet, flag_skip ) )
				&&  get_trust ( d->character ) >= min_level
				&&  d->character != ch ) {
			if ( IS_SET ( d->character->wiznet, WIZ_PREFIX ) )
			{ writeBuffer ( "\t", d->character ); }
			act_new ( Format ( "\ac[\aYSTAFF\ac] \aW%s\an", str ), d->character, obj, ch, TO_CHAR, POS_DEAD );
		}
	}

	return;
}

DefineCommand ( cmd_guild )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	int clan;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Syntax: guild <char> <cln name>\n\r", ch );
		return;
	}
	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't playing.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg2, "none" ) ) {
		writeBuffer ( "They are now clanless.\n\r", ch );
		writeBuffer ( "You are now a member of no clan!\n\r", victim );
		victim->clan = 0;
		return;
	}

	if ( ( clan = clan_lookup ( arg2 ) ) == 0 ) {
		writeBuffer ( "No such clan exists.\n\r", ch );
		return;
	}

	if ( clan_table[clan].independent ) {
		snprintf ( buf, sizeof ( buf ), "They are now a %s.\n\r", clan_table[clan].name );
		writeBuffer ( buf, ch );
		snprintf ( buf, sizeof ( buf ), "You are now a %s.\n\r", clan_table[clan].name );
		writeBuffer ( buf, victim );
	} else {
		snprintf ( buf, sizeof ( buf ), "They are now a member of clan %s.\n\r",
				   capitalize ( clan_table[clan].name ) );
		writeBuffer ( buf, ch );
		snprintf ( buf, sizeof ( buf ), "You are now a member of clan %s.\n\r",
				   capitalize ( clan_table[clan].name ) );
	}

	victim->clan = clan;
}

/* equips a character */
DefineCommand ( cmd_outfit )
{
	Item *obj;
	int i, sn, vnum;

	if ( !IS_SET ( ch->sflag, CR_STAFF ) ) {
		if ( ch->level > 5 || IS_NPC ( ch ) ) {
			writeBuffer ( "Find it yourself!\n\r", ch );
			return;
		}
	}

	if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_BANNER ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_LIGHT );
	}

	if ( ( obj = get_eq_char ( ch, WEAR_BODY ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_VEST ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_BODY );
	}

	/* do the weapon thing */
	if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL ) {
		sn = 0;
		vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

		for ( i = 0; weapon_table[i].name != NULL; i++ ) {
			if ( ch->pcdata->learned[sn] <
					ch->pcdata->learned[*weapon_table[i].gsn] ) {
				sn = *weapon_table[i].gsn;
				vnum = weapon_table[i].vnum;
			}
		}

		obj = create_object ( get_obj_index ( vnum ), 0 );
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_WIELD );
	}

	if ( ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL
			||   !IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
			&&  ( obj = get_eq_char ( ch, WEAR_SHIELD ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_SHIELD ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_SHIELD );
	}

	writeBuffer ( "You have been equipped by Mota.\n\r", ch );
}


/* RT nochannels command, for those spammers */
DefineCommand ( cmd_nochannels )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Nochannel whom?", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOCHANNELS ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOCHANNELS );
		writeBuffer ( "The gods have restored your channel priviliges.\n\r",
					  victim );
		writeBuffer ( "NOCHANNELS removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores channels to %s", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOCHANNELS );
		writeBuffer ( "The gods have revoked your channel priviliges.\n\r",
					  victim );
		writeBuffer ( "NOCHANNELS set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's channels.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}


DefineCommand ( cmd_smote )
{
	Creature *vch;
	char *letter, *name;
	char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
	int matches = 0;

	if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) ) {
		writeBuffer ( "You can't show your emotions.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Emote what?\n\r", ch );
		return;
	}

	if ( str_str ( argument, ch->name ) == NULL ) {
		writeBuffer ( "You must include your name in an smote.\n\r", ch );
		return;
	}

	writeBuffer ( argument, ch );
	writeBuffer ( "\n\r", ch );

	for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch->next_in_room ) {
		if ( vch->desc == NULL || vch == ch )
		{ continue; }

		if ( ( letter = str_str ( argument, vch->name ) ) == NULL ) {
			writeBuffer ( argument, vch );
			writeBuffer ( "\n\r", vch );
			continue;
		}

		strcpy ( temp, argument );
		temp[strlen ( argument ) - strlen ( letter )] = '\0';
		last[0] = '\0';
		name = vch->name;

		for ( ; *letter != '\0'; letter++ ) {
			if ( *letter == '\'' && matches == ( signed int ) strlen ( vch->name ) ) {
				strcat ( temp, "r" );
				continue;
			}

			if ( *letter == 's' && matches == ( signed int ) strlen ( vch->name ) ) {
				matches = 0;
				continue;
			}

			if ( matches == ( signed int ) strlen ( vch->name ) ) {
				matches = 0;
			}

			if ( *letter == *name ) {
				matches++;
				name++;
				if ( matches == ( signed int ) strlen ( vch->name ) ) {
					strcat ( temp, "you" );
					last[0] = '\0';
					name = vch->name;
					continue;
				}
				strncat ( last, letter, 1 );
				continue;
			}

			matches = 0;
			strcat ( temp, last );
			strncat ( temp, letter, 1 );
			last[0] = '\0';
			name = vch->name;
		}

		writeBuffer ( temp, vch );
		writeBuffer ( "\n\r", vch );
	}

	return;
}

DefineCommand ( cmd_bamfin )
{
	char buf[MAX_STRING_LENGTH];

	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			snprintf ( buf, sizeof ( buf ), "Your poofin is %s\n\r", ch->pcdata->bamfin );
			writeBuffer ( buf, ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		PURGE_DATA ( ch->pcdata->bamfin );
		ch->pcdata->bamfin = assign_string ( argument );

		snprintf ( buf, sizeof ( buf ), "Your poofin is now %s\n\r", ch->pcdata->bamfin );
		writeBuffer ( buf, ch );
	}
	return;
}

DefineCommand ( cmd_bamfout )
{
	char buf[MAX_STRING_LENGTH];

	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			snprintf ( buf, sizeof ( buf ), "Your poofout is %s\n\r", ch->pcdata->bamfout );
			writeBuffer ( buf, ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		PURGE_DATA ( ch->pcdata->bamfout );
		ch->pcdata->bamfout = assign_string ( argument );

		snprintf ( buf, sizeof ( buf ), "Your poofout is now %s\n\r", ch->pcdata->bamfout );
		writeBuffer ( buf, ch );
	}
	return;
}



DefineCommand ( cmd_deny )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Deny whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	SET_BIT ( victim->act, PLR_DENY );
	writeBuffer ( "You are denied access!\n\r", victim );
	snprintf ( buf, sizeof ( buf ), "$N denies access to %s", victim->name );
	wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	writeBuffer ( "OK.\n\r", ch );
	save_char_obj ( victim );
	stop_fighting ( victim, TRUE );
	cmd_function ( victim, &cmd_quit, "" );

	return;
}



DefineCommand ( cmd_disconnect )
{
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	Creature *victim;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Disconnect whom?\n\r", ch );
		return;
	}

	if ( is_number ( arg ) ) {
		int desc;

		desc = atoi ( arg );
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->descriptor == desc ) {
				close_socket ( d );
				writeBuffer ( "Ok.\n\r", ch );
				return;
			}
		}
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL ) {
		act ( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
		return;
	}

	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d == victim->desc ) {
			close_socket ( d );
			writeBuffer ( "Ok.\n\r", ch );
			return;
		}
	}

	log_hd ( LOG_ERROR, "cmd_disconnect: descriptor not found, cannot perform disconnection." );
	writeBuffer ( "Descriptor not found!\n\r", ch );
	return;
}



DefineCommand ( cmd_pardon )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Creature *victim;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg2, "killer" ) ) {
		if ( IS_SET ( victim->act, PLR_KILLER ) ) {
			REMOVE_BIT ( victim->act, PLR_KILLER );
			writeBuffer ( "Killer flag removed.\n\r", ch );
			writeBuffer ( "You are no longer a KILLER.\n\r", victim );
		}
		return;
	}

	if ( !str_cmp ( arg2, "thief" ) ) {
		if ( IS_SET ( victim->act, PLR_THIEF ) ) {
			REMOVE_BIT ( victim->act, PLR_THIEF );
			writeBuffer ( "Thief flag removed.\n\r", ch );
			writeBuffer ( "You are no longer a THIEF.\n\r", victim );
		}
		return;
	}

	writeBuffer ( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	return;
}



DefineCommand ( cmd_echo )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Global echo what?\n\r", ch );
		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == CON_PLAYING ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "global> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r",   d->character );
		}
	}

	return;
}



DefineCommand ( cmd_recho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Local echo what?\n\r", ch );

		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == CON_PLAYING
				&&   d->character->in_room == IN_ROOM ( ch ) ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "local> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r",   d->character );
		}
	}

	return;
}

DefineCommand ( cmd_zecho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Zone echo what?\n\r", ch );
		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == CON_PLAYING
				&&  d->character->in_room != NULL && IN_ROOM ( ch ) != NULL
				&&  d->character->in_room->area == IN_ROOM ( ch )->area ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "zone> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r", d->character );
		}
	}
}

DefineCommand ( cmd_pecho )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	argument = ChopC ( argument, arg );

	if ( argument[0] == '\0' || arg[0] == '\0' ) {
		writeBuffer ( "Personal echo what?\n\r", ch );
		return;
	}

	if  ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "Target not found.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) && get_trust ( ch ) != MAX_LEVEL )
	{ writeBuffer ( "personal> ", victim ); }

	writeBuffer ( argument, victim );
	writeBuffer ( "\n\r", victim );
	writeBuffer ( "personal> ", ch );
	writeBuffer ( argument, ch );
	writeBuffer ( "\n\r", ch );
}


RoomData *find_location ( Creature *ch, const char *arg )
{
	Creature *victim;
	Item *obj;

	if ( is_number ( arg ) )
	{ return get_room_index ( atoi ( arg ) ); }

	if ( ( victim = get_char_world ( ch, arg ) ) != NULL )
	{ return IN_ROOM ( victim ); }

	if ( ( obj = get_obj_world ( ch, arg ) ) != NULL )
	{ return obj->in_room; }

	return NULL;
}



DefineCommand ( cmd_transfer )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	RoomData *location;
	Socket *d;
	Creature *victim;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' ) {
		writeBuffer ( "Transfer whom (and where)?\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg1, "all" ) ) {
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->connected == CON_PLAYING
					&&   d->character != ch
					&&   d->character->in_room != NULL
					&&   can_see ( ch, d->character ) ) {
				char buf[MAX_STRING_LENGTH];
				sprintf ( buf, "%s %s", d->character->name, arg2 );
				cmd_function ( ch, &cmd_transfer, buf );
			}
		}
		return;
	}

	/*
	 * Thanks to Grodyn for the optional location parameter.
	 */
	if ( arg2[0] == '\0' ) {
		location = IN_ROOM ( ch );
	} else {
		if ( ( location = find_location ( ch, arg2 ) ) == NULL ) {
			writeBuffer ( "No such location.\n\r", ch );
			return;
		}

		if ( !is_room_owner ( ch, location ) && room_is_private ( location )
				&&  get_trust ( ch ) < MAX_LEVEL ) {
			writeBuffer ( "That room is private right now.\n\r", ch );
			return;
		}
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IN_ROOM ( victim ) == NULL ) {
		writeBuffer ( "They are in limbo.\n\r", ch );
		return;
	}

	if ( FIGHTING ( victim ) != NULL )
	{ stop_fighting ( victim, TRUE ); }
	act ( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
	char_from_room ( victim );
	char_to_room ( victim, location );
	act ( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
	if ( ch != victim )
	{ act ( "$n has transferred you.", ch, NULL, victim, TO_VICT ); }
	cmd_function ( victim, &cmd_look, "auto" );
	writeBuffer ( "Ok.\n\r", ch );
}



DefineCommand ( cmd_at )
{
	char arg[MAX_INPUT_LENGTH];
	RoomData *location;
	RoomData *original;
	Item *on;
	Creature *wch;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "At where what?\n\r", ch );
		return;
	}

	if ( ( location = find_location ( ch, arg ) ) == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, location ) && room_is_private ( location )
			&&  get_trust ( ch ) < MAX_LEVEL ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	original = IN_ROOM ( ch );
	on = ch->on;
	char_from_room ( ch );
	char_to_room ( ch, location );
	interpret ( ch, argument );

	/*
	 * See if 'ch' still exists before continuing!
	 * Handles 'at XXXX quit' case.
	 */
	for ( wch = char_list; wch != NULL; wch = wch->next ) {
		if ( wch == ch ) {
			char_from_room ( ch );
			char_to_room ( ch, original );
			ch->on = on;
			break;
		}
	}

	return;
}



DefineCommand ( cmd_goto )
{
	RoomData *location;
	Creature *rch;
	int count = 0;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Goto where?\n\r", ch );
		return;
	}

	if ( ( location = find_location ( ch, argument ) ) == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	count = 0;
	for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
	{ count++; }

	if ( !is_room_owner ( ch, location ) && room_is_private ( location )
			&&  ( count > 1 || get_trust ( ch ) < MAX_LEVEL ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	if ( FIGHTING ( ch ) != NULL )
	{ stop_fighting ( ch, TRUE ); }

	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( get_trust ( rch ) >= ch->invis_level ) {
			if ( ch->pcdata != NULL && !IS_NULLSTR ( ch->pcdata->bamfout ) )
			{ act ( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT ); }
			else
			{ act ( "$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT ); }
		}
	}

	char_from_room ( ch );
	char_to_room ( ch, location );


	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( get_trust ( rch ) >= ch->invis_level ) {
			if ( ch->pcdata != NULL && !IS_NULLSTR ( ch->pcdata->bamfin ) )
			{ act ( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT ); }
			else
			{ act ( "$n appears in a swirling mist.", ch, NULL, rch, TO_VICT ); }
		}
	}

	cmd_function ( ch, &cmd_look, "auto" );
	return;
}

DefineCommand ( cmd_violate )
{
	RoomData *location;
	Creature *rch;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Goto where?\n\r", ch );
		return;
	}

	if ( ( location = find_location ( ch, argument ) ) == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !room_is_private ( location ) ) {
		writeBuffer ( "That room isn't private, use goto.\n\r", ch );
		return;
	}

	if ( FIGHTING ( ch ) != NULL )
	{ stop_fighting ( ch, TRUE ); }

	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( get_trust ( rch ) >= ch->invis_level ) {
			if ( ch->pcdata != NULL && !IS_NULLSTR ( ch->pcdata->bamfout ) )
			{ act ( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT ); }
			else
			{ act ( "$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT ); }
		}
	}

	char_from_room ( ch );
	char_to_room ( ch, location );


	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( get_trust ( rch ) >= ch->invis_level ) {
			if ( ch->pcdata != NULL && !IS_NULLSTR ( ch->pcdata->bamfin ) )
			{ act ( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT ); }
			else
			{ act ( "$n appears in a swirling mist.", ch, NULL, rch, TO_VICT ); }
		}
	}

	cmd_function ( ch, &cmd_look, "auto" );
	return;
}

/* RT to replace the 3 stat commands */
DefineCommand ( cmd_stat )
{
	char arg[MAX_INPUT_LENGTH];
	char *string;
	Item *obj;
	RoomData *location;
	Creature *victim;

	string = ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  stat <name>\n\r", ch );
		writeBuffer ( "  stat obj <name>\n\r", ch );
		writeBuffer ( "  stat mob <name>\n\r", ch );
		writeBuffer ( "  stat room <number>\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "room" ) ) {
		cmd_function ( ch, &cmd_rstat, string );
		return;
	}

	if ( !str_cmp ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_ostat, string );
		return;
	}

	if ( !str_cmp ( arg, "char" )  || !str_cmp ( arg, "mob" ) ) {
		cmd_function ( ch, &cmd_mstat, string );
		return;
	}

	/* do it the old way */

	obj = get_obj_world ( ch, argument );
	if ( obj != NULL ) {
		cmd_function ( ch, &cmd_ostat, argument );
		return;
	}

	victim = get_char_world ( ch, argument );
	if ( victim != NULL ) {
		cmd_function ( ch, &cmd_mstat, argument );
		return;
	}

	location = find_location ( ch, argument );
	if ( location != NULL ) {
		cmd_function ( ch, &cmd_rstat, argument );
		return;
	}

	writeBuffer ( "Nothing by that name found anywhere.\n\r", ch );
}

DefineCommand ( cmd_rstat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	RoomData *location;
	Item *obj;
	Creature *rch;
	int door;

	ChopC ( argument, arg );
	location = ( arg[0] == '\0' ) ? IN_ROOM ( ch ) : find_location ( ch, arg );
	if ( location == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, location ) && IN_ROOM ( ch ) != location
			&&  room_is_private ( location ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name: '%s'\n\rArea: '%s'\n\r",
			  location->name,
			  location->area->name );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
			  location->vnum,
			  location->sector_type,
			  location->light,
			  location->heal_rate,
			  location->mana_rate );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Room flags: %d.\n\rDescription:\n\r%s",
			  location->room_flags,
			  location->description );
	writeBuffer ( buf, ch );

	if ( location->extra_descr != NULL ) {
		DescriptionData *ed;

		writeBuffer ( "Extra description keywords: '", ch );
		for ( ed = location->extra_descr; ed; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}
		writeBuffer ( "'.\n\r", ch );
	}

	writeBuffer ( "Characters:", ch );
	for ( rch = location->people; rch; rch = rch->next_in_room ) {
		if ( can_see ( ch, rch ) ) {
			writeBuffer ( " ", ch );
			ChopC ( rch->name, buf );
			writeBuffer ( buf, ch );
		}
	}

	writeBuffer ( ".\n\rObjects:   ", ch );
	for ( obj = location->contents; obj; obj = obj->next_content ) {
		writeBuffer ( " ", ch );
		ChopC ( obj->name, buf );
		writeBuffer ( buf, ch );
	}
	writeBuffer ( ".\n\r", ch );

	for ( door = 0; door <= 5; door++ ) {
		Exit *pexit;

		if ( ( pexit = location->exit[door] ) != NULL ) {
			sprintf ( buf,
					  "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

					  door,
					  ( pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum ),
					  pexit->key,
					  pexit->exit_info,
					  pexit->keyword,
					  pexit->description[0] != '\0'
					  ? pexit->description : "(none).\n\r" );
			writeBuffer ( buf, ch );
		}
	}

	return;
}



DefineCommand ( cmd_ostat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Affect *paf;
	Item *obj;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Stat what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "Nothing like that in hell, earth, or heaven.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name(s): %s\n\r",
			  obj->name );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",
			  obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",
			  item_name ( obj->item_type ), obj->pIndexData->reset_num );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Short description: %s\n\rLong description: %s\n\r",
			  obj->short_descr, obj->description );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
			  wear_bit_name ( obj->wear_flags ), extra_bit_name ( obj->extra_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
			  1,           get_obj_number ( obj ),
			  obj->weight, get_obj_weight ( obj ), get_true_weight ( obj ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
			  obj->level, obj->cost, obj->condition, obj->timer );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
			  obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
			  IN_OBJ ( obj )     == NULL    ? "(none)" : IN_OBJ ( obj )->short_descr,
			  CARRIED_BY ( obj ) == NULL    ? "(none)" :
			  can_see ( ch, CARRIED_BY ( obj ) ) ? CARRIED_BY ( obj )->name
			  : "someone",
			  obj->wear_loc );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Values: %d %d %d %d %d\n\r",
			  obj->value[0], obj->value[1], obj->value[2], obj->value[3],
			  obj->value[4] );
	writeBuffer ( buf, ch );

	writeBuffer ( Format ( "Material bits: %s\n\r", material_bit_name ( obj->material_flags ) ), ch );

	/* now give out vital statistics as per identify */

	switch ( obj->item_type ) {
		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			sprintf ( buf, "Level %d spells of:", obj->value[0] );
			writeBuffer ( buf, ch );

			if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[1]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[2]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[3]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[4]].name, ch );
				writeBuffer ( "'", ch );
			}

			writeBuffer ( ".\n\r", ch );
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			sprintf ( buf, "Has %d(%d) charges of level %d",
					  obj->value[1], obj->value[2], obj->value[0] );
			writeBuffer ( buf, ch );

			if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[3]].name, ch );
				writeBuffer ( "'", ch );
			}

			writeBuffer ( ".\n\r", ch );
			break;

		case ITEM_DRINK_CON:
			snprintf ( buf, sizeof ( buf ), "It holds %s-colored %s.\n\r",
					   liq_table[obj->value[2]].liq_color,
					   liq_table[obj->value[2]].liq_name );
			writeBuffer ( buf, ch );
			break;


		case ITEM_WEAPON:
			writeBuffer ( "Weapon type is ", ch );
			switch ( obj->value[0] ) {
				case ( WEAPON_EXOTIC ) :
					writeBuffer ( "exotic\n\r", ch );
					break;
				case ( WEAPON_SWORD ) :
					writeBuffer ( "sword\n\r", ch );
					break;
				case ( WEAPON_DAGGER ) :
					writeBuffer ( "dagger\n\r", ch );
					break;
				case ( WEAPON_SPEAR ) :
					writeBuffer ( "spear/staff\n\r", ch );
					break;
				case ( WEAPON_MACE ) :
					writeBuffer ( "mace/club\n\r", ch );
					break;
				case ( WEAPON_AXE ) :
					writeBuffer ( "axe\n\r", ch );
					break;
				case ( WEAPON_FLAIL ) :
					writeBuffer ( "flail\n\r", ch );
					break;
				case ( WEAPON_WHIP ) :
					writeBuffer ( "whip\n\r", ch );
					break;
				case ( WEAPON_POLEARM ) :
					writeBuffer ( "polearm\n\r", ch );
					break;
				default:
					writeBuffer ( "unknown\n\r", ch );
					break;
			}
			if ( obj->pIndexData->new_format )
				snprintf ( buf, sizeof ( buf ), "Damage is %dd%d (average %d)\n\r",
						   obj->value[1], obj->value[2],
						   ( 1 + obj->value[2] ) * obj->value[1] / 2 );
			else
				sprintf ( buf, "Damage is %d to %d (average %d)\n\r",
						  obj->value[1], obj->value[2],
						  ( obj->value[1] + obj->value[2] ) / 2 );
			writeBuffer ( buf, ch );

			snprintf ( buf, sizeof ( buf ), "Damage noun is %s.\n\r",
					   ( obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE ) ?
					   attack_table[obj->value[3]].noun : "undefined" );
			writeBuffer ( buf, ch );

			if ( obj->value[4] ) { /* weapon flags */
				snprintf ( buf, sizeof ( buf ), "Weapons flags: %s\n\r",
						   weapon_bit_name ( obj->value[4] ) );
				writeBuffer ( buf, ch );
			}
			break;

		case ITEM_ARMOR:
			sprintf ( buf,
					  "Armor archetype is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
					  obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_CONTAINER:
			snprintf ( buf, sizeof ( buf ), "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
					   obj->value[0], obj->value[3], cont_bit_name ( obj->value[1] ) );
			writeBuffer ( buf, ch );
			if ( obj->value[4] != 100 ) {
				snprintf ( buf, sizeof ( buf ), "Weight multiplier: %d%%\n\r",
						   obj->value[4] );
				writeBuffer ( buf, ch );
			}
			break;
	}


	if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL ) {
		DescriptionData *ed;

		writeBuffer ( "Extra description keywords: '", ch );

		for ( ed = obj->extra_descr; ed != NULL; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}

		for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}

		writeBuffer ( "'\n\r", ch );
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next ) {
		sprintf ( buf, "Affects %s by %d, level %d",
				  affect_loc_name ( paf->location ), paf->modifier, paf->level );
		writeBuffer ( buf, ch );
		if ( paf->duration > -1 )
		{ snprintf ( buf, sizeof ( buf ), ", %d hours.\n\r", paf->duration ); }
		else
		{ snprintf ( buf, sizeof ( buf ), ".\n\r" ); }
		writeBuffer ( buf, ch );
		if ( paf->bitvector ) {
			switch ( paf->where ) {
				case TO_AFFECTS:
					snprintf ( buf, sizeof ( buf ), "Adds %s affect.\n",
							   affect_bit_name ( paf->bitvector ) );
					break;
				case TO_WEAPON:
					snprintf ( buf, sizeof ( buf ), "Adds %s weapon flags.\n",
							   weapon_bit_name ( paf->bitvector ) );
					break;
				case TO_OBJECT:
					snprintf ( buf, sizeof ( buf ), "Adds %s object flag.\n",
							   extra_bit_name ( paf->bitvector ) );
					break;
				case TO_IMMUNE:
					snprintf ( buf, sizeof ( buf ), "Adds immunity to %s.\n",
							   imm_bit_name ( paf->bitvector ) );
					break;
				case TO_RESIST:
					snprintf ( buf, sizeof ( buf ), "Adds resistance to %s.\n\r",
							   imm_bit_name ( paf->bitvector ) );
					break;
				case TO_VULN:
					snprintf ( buf, sizeof ( buf ), "Adds vulnerability to %s.\n\r",
							   imm_bit_name ( paf->bitvector ) );
					break;
				default:
					snprintf ( buf, sizeof ( buf ), "Unknown bit %d: %d\n\r",
							   paf->where, paf->bitvector );
					break;
			}
			writeBuffer ( buf, ch );
		}
	}

	if ( !obj->enchanted )
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next ) {
			sprintf ( buf, "Affects %s by %d, level %d.\n\r",
					  affect_loc_name ( paf->location ), paf->modifier, paf->level );
			writeBuffer ( buf, ch );
			if ( paf->bitvector ) {
				switch ( paf->where ) {
					case TO_AFFECTS:
						snprintf ( buf, sizeof ( buf ), "Adds %s affect.\n",
								   affect_bit_name ( paf->bitvector ) );
						break;
					case TO_OBJECT:
						snprintf ( buf, sizeof ( buf ), "Adds %s object flag.\n",
								   extra_bit_name ( paf->bitvector ) );
						break;
					case TO_IMMUNE:
						snprintf ( buf, sizeof ( buf ), "Adds immunity to %s.\n",
								   imm_bit_name ( paf->bitvector ) );
						break;
					case TO_RESIST:
						snprintf ( buf, sizeof ( buf ), "Adds resistance to %s.\n\r",
								   imm_bit_name ( paf->bitvector ) );
						break;
					case TO_VULN:
						snprintf ( buf, sizeof ( buf ), "Adds vulnerability to %s.\n\r",
								   imm_bit_name ( paf->bitvector ) );
						break;
					default:
						snprintf ( buf, sizeof ( buf ), "Unknown bit %d: %d\n\r",
								   paf->where, paf->bitvector );
						break;
				}
				writeBuffer ( buf, ch );
			}
		}

	return;
}



DefineCommand ( cmd_mstat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Affect *paf;
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Stat whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name: %s\n\r",
			  victim->name );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
			  IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
			  IS_NPC ( victim ) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
			  race_table[victim->race].name,
			  IS_NPC ( victim ) ? victim->group : 0, sex_table[victim->sex].name,
			  IN_ROOM ( victim ) == NULL    ?        0 : IN_ROOM ( victim )->vnum
			);
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) ) {
		snprintf ( buf, sizeof ( buf ), "Count: %d  Killed: %d\n\r",
				   victim->pIndexData->count, victim->pIndexData->killed );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf,
			  "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
			  victim->perm_stat[STAT_STR],
			  get_curr_stat ( victim, STAT_STR ),
			  victim->perm_stat[STAT_INT],
			  get_curr_stat ( victim, STAT_INT ),
			  victim->perm_stat[STAT_WIS],
			  get_curr_stat ( victim, STAT_WIS ),
			  victim->perm_stat[STAT_DEX],
			  get_curr_stat ( victim, STAT_DEX ),
			  victim->perm_stat[STAT_CON],
			  get_curr_stat ( victim, STAT_CON ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",
			  victim->hit,         victim->max_hit,
			  victim->mana,        victim->max_mana,
			  victim->move,        victim->max_move,
			  IS_NPC ( ch ) ? 0 : victim->practice );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Lv: %d  Class: %s  Align: %d  Gold: %ld  Silver: %ld  Exp: %d\n\r",
			  victim->level,
			  IS_NPC ( victim ) ? "mobile" : archetype_table[victim->archetype].name,
			  victim->alignment,
			  victim->gold, victim->silver, victim->exp );
	writeBuffer ( buf, ch );

	snprintf ( buf, sizeof ( buf ), "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
			   GET_AC ( victim, AC_PIERCE ), GET_AC ( victim, AC_BASH ),
			   GET_AC ( victim, AC_SLASH ),  GET_AC ( victim, AC_EXOTIC ) );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
			  GET_HITROLL ( victim ), GET_DAMROLL ( victim ), victim->saving_throw,
			  size_table[victim->size].name, position_table[victim->position].name,
			  victim->wimpy );
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) && victim->pIndexData->new_format ) {
		snprintf ( buf, sizeof ( buf ), "Damage: %dd%d  Message:  %s\n\r",
				   victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE],
				   attack_table[victim->dam_type].noun );
		writeBuffer ( buf, ch );
	}
	sprintf ( buf, "Fighting: %s\n\r",
			  FIGHTING ( victim ) ? FIGHTING ( victim )->name : "(none)" );
	writeBuffer ( buf, ch );

	writeBuffer ( Format ( "Material bits: %s\n\r", material_bit_name ( victim->material_flags ) ), ch );

	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf,
				  "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d\n\r",
				  victim->pcdata->condition[COND_THIRST],
				  victim->pcdata->condition[COND_HUNGER],
				  victim->pcdata->condition[COND_FULL],
				  victim->pcdata->condition[COND_DRUNK] );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf, "Carry number: %d  Carry weight: %ld\n\r",
			  victim->carry_number, get_carry_weight ( victim ) / 10 );
	writeBuffer ( buf, ch );


	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf,
				  "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
				  get_age ( victim ),
				  ( int ) ( victim->played + current_time - victim->logon ) / 3600,
				  victim->pcdata->last_level,
				  victim->timer );
		writeBuffer ( buf, ch );
	}

	snprintf ( buf, sizeof ( buf ), "Act: %s\n\r", act_bit_name ( victim->act ) );
	writeBuffer ( buf, ch );

	if ( victim->comm ) {
		snprintf ( buf, sizeof ( buf ), "Comm: %s\n\r", comm_bit_name ( victim->comm ) );
		writeBuffer ( buf, ch );
	}

	if ( IS_NPC ( victim ) && victim->off_flags ) {
		snprintf ( buf, sizeof ( buf ), "Offense: %s\n\r", off_bit_name ( victim->off_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->imm_flags ) {
		snprintf ( buf, sizeof ( buf ), "Immune: %s\n\r", imm_bit_name ( victim->imm_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->res_flags ) {
		snprintf ( buf, sizeof ( buf ), "Resist: %s\n\r", imm_bit_name ( victim->res_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->vuln_flags ) {
		snprintf ( buf, sizeof ( buf ), "Vulnerable: %s\n\r", imm_bit_name ( victim->vuln_flags ) );
		writeBuffer ( buf, ch );
	}

	snprintf ( buf, sizeof ( buf ), "Form: %s\n\rParts: %s\n\r",
			   form_bit_name ( victim->form ), part_bit_name ( victim->parts ) );
	writeBuffer ( buf, ch );

	if ( victim->affected_by ) {
		snprintf ( buf, sizeof ( buf ), "Affected by %s\n\r",
				   affect_bit_name ( victim->affected_by ) );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf, "Master: %s  Leader: %s  Pet: %s\n\r",
			  victim->master      ? victim->master->name   : "(none)",
			  victim->leader      ? victim->leader->name   : "(none)",
			  victim->pet 	    ? victim->pet->name	     : "(none)" );
	writeBuffer ( buf, ch );

	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf, "Security: %d.\n\r", victim->pcdata->security );	/* OLC */
		writeBuffer ( buf, ch );					/* OLC */
	}

	sprintf ( buf, "Short description: %s\n\rLong  description: %s",
			  victim->short_descr,
			  !IS_NULLSTR ( victim->long_descr ) ? victim->long_descr : "(none)\n\r" );
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) && victim->spec_fun != 0 ) {
		snprintf ( buf, sizeof ( buf ), "Mobile has special procedure %s.\n\r",
				   spec_name ( victim->spec_fun ) );
		writeBuffer ( buf, ch );
	}

	for ( paf = victim->affected; paf != NULL; paf = paf->next ) {
		sprintf ( buf,
				  "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
				  skill_table[ ( int ) paf->type].name,
				  affect_loc_name ( paf->location ),
				  paf->modifier,
				  paf->duration,
				  affect_bit_name ( paf->bitvector ),
				  paf->level
				);
		writeBuffer ( buf, ch );
	}

	return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */
DefineCommand ( cmd_vnum )
{
	char arg[MAX_INPUT_LENGTH];
	char *string;

	string = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  vnum obj <name>\n\r", ch );
		writeBuffer ( "  vnum mob <name>\n\r", ch );
		writeBuffer ( "  vnum skill <skill or spell>\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_ofind, string );
		return;
	}

	if ( !str_cmp ( arg, "mob" ) || !str_cmp ( arg, "char" ) ) {
		cmd_function ( ch, &cmd_mfind, string );
		return;
	}

	if ( !str_cmp ( arg, "skill" ) || !str_cmp ( arg, "spell" ) ) {
		cmd_function ( ch, &cmd_slookup, string );
		return;
	}
	/* do both */
	cmd_function ( ch, &cmd_mfind, argument );
	cmd_function ( ch, &cmd_ofind, argument );
}


DefineCommand ( cmd_mfind )
{
	extern int top_mob_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	NPCData *pMobIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Find whom?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* !str_cmp( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_mob_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for ( vnum = 0; nMatch < top_mob_index; vnum++ ) {
		if ( ( pMobIndex = get_mob_index ( vnum ) ) != NULL ) {
			nMatch++;
			if ( fAll || is_name ( argument, pMobIndex->player_name ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %s\n\r",
						  pMobIndex->vnum, pMobIndex->short_descr );
				writeBuffer ( buf, ch );
			}
		}
	}

	if ( !found )
	{ writeBuffer ( "No mobiles by that name.\n\r", ch ); }

	return;
}



DefineCommand ( cmd_ofind )
{
	extern int top_obj_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	ItemData *pObjIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Find what?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* !str_cmp( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_obj_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for ( vnum = 0; nMatch < top_obj_index; vnum++ ) {
		if ( ( pObjIndex = get_obj_index ( vnum ) ) != NULL ) {
			nMatch++;
			if ( fAll || is_name ( argument, pObjIndex->name ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %s\n\r",
						  pObjIndex->vnum, pObjIndex->short_descr );
				writeBuffer ( buf, ch );
			}
		}
	}

	if ( !found )
	{ writeBuffer ( "No objects by that name.\n\r", ch ); }

	return;
}


DefineCommand ( cmd_owhere )
{
	char buf[MAX_INPUT_LENGTH];
	BUFFER *buffer;
	Item *obj;
	Item *in_obj;
	bool found;
	int number = 0, max_found;

	found = FALSE;
	number = 0;
	max_found = 200;

	buffer = new_buf();

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Find what?\n\r", ch );
		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		if ( !can_see_obj ( ch, obj ) || !is_name ( argument, obj->name )
				||   ch->level < obj->level )
		{ continue; }

		found = TRUE;
		number++;

		for ( in_obj = obj; IN_OBJ ( in_obj ) != NULL; in_obj = IN_OBJ ( in_obj ) )
			;

		if ( CARRIED_BY ( in_obj ) != NULL && can_see ( ch, CARRIED_BY ( in_obj ) )
				&&   CARRIED_BY ( in_obj )->in_room != NULL )
			sprintf ( buf, "%3d) %s is carried by %s [Room %d]\n\r",
					  number, obj->short_descr, PERS ( CARRIED_BY ( in_obj ), ch ),
					  CARRIED_BY ( in_obj )->in_room->vnum );
		else if ( in_obj->in_room != NULL && can_see_room ( ch, in_obj->in_room ) )
			sprintf ( buf, "%3d) %s is in %s [Room %d]\n\r",
					  number, obj->short_descr, in_obj->in_room->name,
					  in_obj->in_room->vnum );
		else
		{ sprintf ( buf, "%3d) %s is somewhere\n\r", number, obj->short_descr ); }

		buf[0] = UPPER ( buf[0] );
		add_buf ( buffer, buf );

		if ( number >= max_found )
		{ break; }
	}

	if ( !found )
	{ writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch ); }
	else
	{ writePage ( buf_string ( buffer ), ch ); }

	recycle_buf ( buffer );
}


DefineCommand ( cmd_mwhere )
{
	char buf[MAX_STRING_LENGTH];
	BUFFER *buffer;
	Creature *victim;
	bool found;
	int count = 0;

	if ( argument[0] == '\0' ) {
		Socket *d;

		/* show characters logged */

		buffer = new_buf();
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->character != NULL && d->connected == CON_PLAYING
					&&  d->character->in_room != NULL && can_see ( ch, d->character )
					&&  can_see_room ( ch, d->character->in_room ) ) {
				victim = d->character;
				count++;
				if ( d->original != NULL )
					snprintf ( buf, sizeof ( buf ), "%3d) %s (in the body of %s) is in %s [%d]\n\r",
							   count, d->original->name, victim->short_descr,
							   IN_ROOM ( victim )->name, IN_ROOM ( victim )->vnum );
				else
					snprintf ( buf, sizeof ( buf ), "%3d) %s is in %s [%d]\n\r",
							   count, victim->name, IN_ROOM ( victim )->name,
							   IN_ROOM ( victim )->vnum );
				add_buf ( buffer, buf );
			}
		}

		writePage ( buf_string ( buffer ), ch );
		recycle_buf ( buffer );
		return;
	}

	found = FALSE;
	buffer = new_buf();
	for ( victim = char_list; victim != NULL; victim = victim->next ) {
		if ( IN_ROOM ( victim ) != NULL
				&&   is_name ( argument, victim->name ) ) {
			found = TRUE;
			count++;
			sprintf ( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
					  IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
					  IS_NPC ( victim ) ? victim->short_descr : victim->name,
					  IN_ROOM ( victim )->vnum,
					  IN_ROOM ( victim )->name );
			add_buf ( buffer, buf );
		}
	}

	if ( !found )
	{ act ( "You didn't find any $T.", ch, NULL, argument, TO_CHAR ); }
	else
	{ writePage ( buf_string ( buffer ), ch ); }

	recycle_buf ( buffer );

	return;
}

DefineCommand ( cmd_reboot )
{
	extern bool is_shutdown;
	Socket *d, *d_next;
	Creature *vch;

	if ( cmd == 307 ) {
		switch ( argument[0] ) {
			default:
				ch->queries.querycommand = 0;
				break;
			case 'y':
			case 'Y':
				if ( ch->invis_level < LEVEL_HERO ) {
					cmd_function ( ch, &cmd_echo, Format ( "Reboot by %s.", ch->name ) );
				}

				{
					const char *twit_table[] = {
						"<< Infect Engine is undergoing a #reboot for #maintenance!",
						"<< Infect Engine will now #reboot, please check back shortly!!",
						"<< Infect Engine Core is rebooting for #maintenance",
						"<< The mud is rebooting for server #maintenance!",
						Format ( "<< %s is undergoing a #temporary #reboot!", "The Infected City" ),
						Format ( "<< %s(%s) is undergoing a #reboot!", "The Infected City", getVersion() ),
						Format ( "<< %s will reboot!", "The Infected City" ),
						Format ( "<< %s: %s -- #reboot!", "The Infected City", getVersion() ),
						NULL,
					};

					int cnt = 0;
					for ( int y = 0; twit_table[y] != NULL; y++ )
					{ cnt++; }

					std::string tweetStr = twit_table[Math::instance().range ( 0, ( cnt - 1 ) )];
					tweetStr = addTweetHashtags ( tweetStr );
					issueSystemCommandNoReturn ( Format ( "curl -u %s:%s -d \"status=%s\" http://localhost:8080/1.1/statuses/update.json",
														  "CombatMUD", "temppassword", C_STR ( tweetStr ) ) );

				}

				is_shutdown = TRUE;
				for ( d = socket_list; d != NULL; d = d_next ) {
					d_next = d->next;
					vch = d->original ? d->original : d->character;
					if ( vch != NULL )
					{ save_char_obj ( vch ); }
					close_socket ( d );
				}
		}
	} else {
		// -- set up the query prompt
		ch->queries.queryfunc = cmd_reboot;
		strcpy ( ch->queries.queryprompt, "Are you sure you want to reboot the MUD? (Y/n)" );
		ch->queries.querycommand = 307;
	}
	return;
}


DefineCommand ( cmd_shutdown )
{
	extern bool is_shutdown;
	Socket *d, *d_next;
	Creature *vch;

	if ( cmd == 307 ) {
		switch ( argument[0] ) {
			default:
				ch->queries.querycommand = 0;
				break;
			case 'y':
			case 'Y':
				// -- just make sure it is in the log properly.
				log_hd ( LOG_SECURITY, Format ( "%s has agree'd to shutdown InfectEngine", ch->name ) );

				append_file ( ch, SHUTDOWN_FILE, Format ( "Shutdown by %s.", ch->name ) );
				if ( ch->invis_level < LEVEL_HERO ) {
					cmd_function ( ch, &cmd_echo, Format ( "Shutdown by %s.", ch->name ) );
				}

				{
					const char *twit_table[] = {
						"<< Infect Engine is undergoing a #shutdown for #maintenance!",
						"<< Infect Engine will now #hotreboot, please check back shortly!!",
						"<< Infect Engine Core is shutting down for #maintenance",
						"<< The mud is shutting down for server #maintenance!",
						Format ( "<< %s is undergoing a #temporary #shutdown!", "The Infected City" ),
						Format ( "<< %s(%s) is undergoing a #shutdown!", "The Infected City", getVersion() ),
						Format ( "<< %s will shutdown!", "The Infected City" ),
						Format ( "<< %s: %s -- #shutdown!", "The Infected City", getVersion() ),
						NULL,
					};

					int cnt = 0;
					for ( int y = 0; twit_table[y] != NULL; y++ )
					{ cnt++; }

					std::string tweetStr = twit_table[Math::instance().range ( 0, ( cnt - 1 ) )];
					tweetStr = addTweetHashtags ( tweetStr );
					issueSystemCommandNoReturn ( Format ( "curl -u %s:%s -d \"status=%s\" http://localhost:8080/1.1/statuses/update.json",
														  "CombatMUD", "temppassword", C_STR ( tweetStr ) ) );

				}

				is_shutdown = TRUE;
				for ( d = socket_list; d != NULL; d = d_next ) {
					d_next = d->next;
					vch = d->original ? d->original : d->character;
					if ( vch != NULL )
					{ save_char_obj ( vch ); }
					close_socket ( d );
				}
				break;
		}
	} else {
		// -- set up the query prompt
		ch->queries.queryfunc = cmd_shutdown;
		strcpy ( ch->queries.queryprompt, "Are you sure you want to shutdown the MUD? (Y/n)" );
		ch->queries.querycommand = 307;
	}
	return;
}

DefineCommand ( cmd_protect )
{
	Creature *victim;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Protect whom from snooping?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "You can't find them.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_SNOOP_PROOF ) ) {
		act_new ( "$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		writeBuffer ( "Your snoop-proofing was just removed.\n\r", victim );
		REMOVE_BIT ( victim->comm, COMM_SNOOP_PROOF );
	} else {
		act_new ( "$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		writeBuffer ( "You are now immune to snooping.\n\r", victim );
		SET_BIT ( victim->comm, COMM_SNOOP_PROOF );
	}
}



DefineCommand ( cmd_snoop )
{
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	Creature *victim;
	char buf[MAX_STRING_LENGTH];

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Snoop whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL ) {
		writeBuffer ( "No descriptor to snoop.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Cancelling all snoops.\n\r", ch );
		wiznet ( "$N stops being such a snoop.",
				 ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust ( ch ) );
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->snoop_by == ch->desc )
			{ d->snoop_by = NULL; }
		}
		return;
	}

	if ( victim->desc->snoop_by != NULL ) {
		writeBuffer ( "Busy already.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, IN_ROOM ( victim ) ) && IN_ROOM ( ch ) != IN_ROOM ( victim )
			&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch )
			||   IS_SET ( victim->comm, COMM_SNOOP_PROOF ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( ch->desc != NULL ) {
		for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by ) {
			if ( d->character == victim || d->original == victim ) {
				writeBuffer ( "No snoop loops.\n\r", ch );
				return;
			}
		}
	}

	victim->desc->snoop_by = ch->desc;
	snprintf ( buf, sizeof ( buf ), "$N starts snooping on %s",
			   ( IS_NPC ( ch ) ? victim->short_descr : victim->name ) );
	wiznet ( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}



DefineCommand ( cmd_switch )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Switch into whom?\n\r", ch );
		return;
	}

	if ( ch->desc == NULL )
	{ return; }

	if ( ch->desc->original != NULL ) {
		writeBuffer ( "You are already switched.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) ) {
		writeBuffer ( "You can only switch into mobiles.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, IN_ROOM ( victim ) ) && IN_ROOM ( ch ) != IN_ROOM ( victim )
			&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( victim->desc != NULL ) {
		writeBuffer ( "Character in use.\n\r", ch );
		return;
	}

	snprintf ( buf, sizeof ( buf ), "$N switches into %s", victim->short_descr );
	wiznet ( buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust ( ch ) );

	ch->desc->character = victim;
	ch->desc->original  = ch;
	victim->desc        = ch->desc;
	ch->desc            = NULL;
	/* change communications to match */
	if ( ch->prompt != NULL )
	{ victim->prompt = assign_string ( ch->prompt ); }
	victim->comm = ch->comm;
	victim->lines = ch->lines;
	writeBuffer ( "Ok.\n\r", victim );
	return;
}



DefineCommand ( cmd_return )
{
	char buf[MAX_STRING_LENGTH];

	if ( ch->desc == NULL )
	{ return; }

	if ( ch->desc->original == NULL ) {
		writeBuffer ( "You aren't switched.\n\r", ch );
		return;
	}

	writeBuffer (
		"You return to your original body. Type replay to see any missed tells.\n\r",
		ch );
	if ( ch->prompt != NULL ) {
		PURGE_DATA ( ch->prompt );
		ch->prompt = NULL;
	}

	snprintf ( buf, sizeof ( buf ), "$N returns from %s.", ch->short_descr );
	wiznet ( buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust ( ch ) );
	ch->desc->character       = ch->desc->original;
	ch->desc->original        = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc                  = NULL;
	return;
}

/* trust levels for load and clone */
bool obj_check ( Creature *ch, Item *obj )
{
	if ( IS_TRUSTED ( ch, GOD )
			|| ( IS_TRUSTED ( ch, IMMORTAL ) && obj->level <= 20 && obj->cost <= 1000 )
			|| ( IS_TRUSTED ( ch, DEMI )	    && obj->level <= 10 && obj->cost <= 500 )
			|| ( IS_TRUSTED ( ch, ANGEL )    && obj->level <=  5 && obj->cost <= 250 )
			|| ( IS_TRUSTED ( ch, AVATAR )   && obj->level ==  0 && obj->cost <= 100 ) )
	{ return TRUE; }
	else
	{ return FALSE; }
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone ( Creature *ch, Item *obj, Item *clone )
{
	Item *c_obj, *t_obj;


	for ( c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content ) {
		if ( obj_check ( ch, c_obj ) ) {
			t_obj = create_object ( c_obj->pIndexData, 0 );
			clone_object ( c_obj, t_obj );
			obj_to_obj ( t_obj, clone );
			recursive_clone ( ch, c_obj, t_obj );
		}
	}
}

/* command that is similar to load */
DefineCommand ( cmd_clone )
{
	char arg[MAX_INPUT_LENGTH];
	char *rest;
	Creature *mob;
	Item  *obj;

	rest = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Clone what?\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "object" ) ) {
		mob = NULL;
		obj = get_obj_here ( ch, rest );
		if ( obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else if ( !str_prefix ( arg, "mobile" ) || !str_prefix ( arg, "character" ) ) {
		obj = NULL;
		mob = get_char_room ( ch, rest );
		if ( mob == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else { /* find both */
		mob = get_char_room ( ch, argument );
		obj = get_obj_here ( ch, argument );
		if ( mob == NULL && obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	}

	/* clone an object */
	if ( obj != NULL ) {
		Item *clone;

		if ( !obj_check ( ch, obj ) ) {
			writeBuffer (
				"Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_object ( obj->pIndexData, 0 );
		clone_object ( obj, clone );
		if ( CARRIED_BY ( obj ) != NULL )
		{ obj_to_char ( clone, ch ); }
		else
		{ obj_to_room ( clone, IN_ROOM ( ch ) ); }
		recursive_clone ( ch, obj, clone );

		act ( "$n has created $p.", ch, clone, NULL, TO_ROOM );
		act ( "You clone $p.", ch, clone, NULL, TO_CHAR );
		wiznet ( "$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
		return;
	} else if ( mob != NULL ) {
		Creature *clone;
		Item *new_obj;
		char buf[MAX_STRING_LENGTH];

		if ( !IS_NPC ( mob ) ) {
			writeBuffer ( "You can only clone mobiles.\n\r", ch );
			return;
		}

		if ( ( mob->level > 20 && !IS_TRUSTED ( ch, GOD ) )
				||  ( mob->level > 10 && !IS_TRUSTED ( ch, IMMORTAL ) )
				||  ( mob->level >  5 && !IS_TRUSTED ( ch, DEMI ) )
				||  ( mob->level >  0 && !IS_TRUSTED ( ch, ANGEL ) )
				||  !IS_TRUSTED ( ch, AVATAR ) ) {
			writeBuffer (
				"Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_mobile ( mob->pIndexData );
		clone_mobile ( mob, clone );

		for ( obj = mob->carrying; obj != NULL; obj = obj->next_content ) {
			if ( obj_check ( ch, obj ) ) {
				new_obj = create_object ( obj->pIndexData, 0 );
				clone_object ( obj, new_obj );
				recursive_clone ( ch, obj, new_obj );
				obj_to_char ( new_obj, clone );
				new_obj->wear_loc = obj->wear_loc;
			}
		}
		char_to_room ( clone, IN_ROOM ( ch ) );
		act ( "$n has created $N.", ch, NULL, clone, TO_ROOM );
		act ( "You clone $N.", ch, NULL, clone, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "$N clones %s.", clone->short_descr );
		wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
		return;
	}
}

/* RT to replace the two load commands */
DefineCommand ( cmd_load )
{
	char arg[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  load mob <vnum>\n\r", ch );
		writeBuffer ( "  load obj <vnum> <level>\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "mob" ) || !str_cmp ( arg, "char" ) ) {
		cmd_function ( ch, &cmd_mload, argument );
		return;
	}

	if ( !str_cmp ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_oload, argument );
		return;
	}
	/* echo syntax */
	cmd_function ( ch, &cmd_load, "" );
}


DefineCommand ( cmd_mload )
{
	char arg[MAX_INPUT_LENGTH];
	NPCData *pMobIndex;
	Creature *victim;
	char buf[MAX_STRING_LENGTH];

	ChopC ( argument, arg );

	if ( arg[0] == '\0' || !is_number ( arg ) ) {
		writeBuffer ( "Syntax: load mob <vnum>.\n\r", ch );
		return;
	}

	if ( ( pMobIndex = get_mob_index ( atoi ( arg ) ) ) == NULL ) {
		writeBuffer ( "No mob has that vnum.\n\r", ch );
		return;
	}

	victim = create_mobile ( pMobIndex );
	char_to_room ( victim, IN_ROOM ( ch ) );
	act ( "$n has created $N!", ch, NULL, victim, TO_ROOM );
	snprintf ( buf, sizeof ( buf ), "$N loads %s.", victim->short_descr );
	wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}



DefineCommand ( cmd_oload )
{
	char arg1[MAX_INPUT_LENGTH] , arg2[MAX_INPUT_LENGTH];
	ItemData *pObjIndex;
	Item *obj;
	int level;

	argument = ChopC ( argument, arg1 );
	ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number ( arg1 ) ) {
		writeBuffer ( "Syntax: load obj <vnum> <level>.\n\r", ch );
		return;
	}

	level = get_trust ( ch ); /* default */

	if ( arg2[0] != '\0' ) { /* load with a level */
		if ( !is_number ( arg2 ) ) {
			writeBuffer ( "Syntax: oload <vnum> <level>.\n\r", ch );
			return;
		}
		level = atoi ( arg2 );
		if ( level < 0 || level > get_trust ( ch ) ) {
			writeBuffer ( "Level must be be between 0 and your level.\n\r", ch );
			return;
		}
	}

	if ( ( pObjIndex = get_obj_index ( atoi ( arg1 ) ) ) == NULL ) {
		writeBuffer ( "No object has that vnum.\n\r", ch );
		return;
	}

	obj = create_object ( pObjIndex, level );
	if ( CAN_WEAR ( obj, ITEM_TAKE ) )
	{ obj_to_char ( obj, ch ); }
	else
	{ obj_to_room ( obj, IN_ROOM ( ch ) ); }
	act ( "$n has created $p!", ch, obj, NULL, TO_ROOM );
	wiznet ( "$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_purge )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[100];
	Creature *victim;
	Item *obj;
	Socket *d;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		/* 'purge' */
		Creature *vnext;
		Item  *obj_next;

		for ( victim = IN_ROOM ( ch )->people; victim != NULL; victim = vnext ) {
			vnext = victim->next_in_room;
			if ( IS_NPC ( victim ) && !IS_SET ( victim->act, ACT_NOPURGE )
					&&   victim != ch /* safety precaution */ )
			{ extract_char ( victim, TRUE ); }
		}

		for ( obj = IN_ROOM ( ch )->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( !IS_OBJ_STAT ( obj, ITEM_NOPURGE ) )
			{ extract_obj ( obj ); }
		}

		act ( "$n purges the room!", ch, NULL, NULL, TO_ROOM );
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) ) {

		if ( ch == victim ) {
			writeBuffer ( "Ho ho ho.\n\r", ch );
			return;
		}

		if ( get_trust ( ch ) <= get_trust ( victim ) ) {
			writeBuffer ( "Maybe that wasn't a good idea...\n\r", ch );
			snprintf ( buf, sizeof ( buf ), "%s tried to purge you!\n\r", ch->name );
			writeBuffer ( buf, victim );
			return;
		}

		act ( "$n disintegrates $N.", ch, 0, victim, TO_NOTVICT );

		if ( victim->level > 1 )
		{ save_char_obj ( victim ); }
		d = victim->desc;
		extract_char ( victim, TRUE );
		if ( d != NULL )
		{ close_socket ( d ); }

		return;
	}

	act ( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
	extract_char ( victim, TRUE );
	return;
}



DefineCommand ( cmd_advance )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Creature *victim;
	int level;
	int iLevel;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) ) {
		writeBuffer ( "Syntax: levelset <player> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "That player is not here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( ( level = atoi ( arg2 ) ) < 1 || level > MAX_LEVEL ) {
		writeBuffer ( Format ( "Level must be 1 to %d.\n\r", MAX_LEVEL ), ch );
		return;
	}

	if ( level > get_trust ( ch ) ) {
		writeBuffer ( "Limited to your trust level.\n\r", ch );
		return;
	}

	/*
	 * Lower level:
	 *   Reset to level 1.
	 *   Then raise again.
	 *   Currently, an imp can lower another imp.
	 *   -- Swiftest
	 */
	if ( level <= victim->level ) {
		int temp_prac;

		writeBuffer ( "Lowering a player's level!\n\r", ch );
		writeBuffer ( "Your level has been changed.\n\r", victim );
		temp_prac = victim->practice;
		victim->level    = 1;
		victim->exp      = exp_per_level ( victim, victim->pcdata->points );
		victim->max_hit  = 10;
		victim->max_mana = 100;
		victim->max_move = 100;
		victim->practice = 0;
		victim->hit      = victim->max_hit;
		victim->mana     = victim->max_mana;
		victim->move     = victim->max_move;
		advance_level ( victim, TRUE );
		victim->practice = temp_prac;
	} else {
		writeBuffer ( "Raising a player's level!\n\r", ch );
		writeBuffer ( "Your level has been changed\n\r", victim );
	}

	for ( iLevel = victim->level ; iLevel < level; iLevel++ ) {
		victim->level += 1;
		advance_level ( victim, TRUE );
	}
	writeBuffer ( Format ( "You are now level %d.\n\r", victim->level ), victim );
	victim->exp   = exp_per_level ( victim, victim->pcdata->points )
					* UMAX ( 1, victim->level );
	victim->trust = 0;
	save_char_obj ( victim );
	return;
}

DefineCommand ( cmd_trust )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	int level;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) ) {
		writeBuffer ( "Syntax: trust <char> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "That player is not here.\n\r", ch );
		return;
	}

	if ( ( level = atoi ( arg2 ) ) < 0 || level > MAX_LEVEL ) {
		snprintf ( buf, sizeof ( buf ), "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL );
		writeBuffer ( buf, ch );
		return;
	}

	if ( level > get_trust ( ch ) ) {
		writeBuffer ( "Limited to your trust.\n\r", ch );
		return;
	}

	victim->trust = level;
	return;
}



DefineCommand ( cmd_restore )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;
	Creature *vch;
	Socket *d;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' || !str_cmp ( arg, "room" ) ) {
		/* cure room */

		for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch->next_in_room ) {
			affect_strip ( vch, gsn_plague );
			affect_strip ( vch, gsn_poison );
			affect_strip ( vch, gsn_blindness );
			affect_strip ( vch, gsn_sleep );
			affect_strip ( vch, gsn_curse );

			vch->hit 	= vch->max_hit;
			vch->mana	= vch->max_mana;
			vch->move	= vch->max_move;
			update_pos ( vch );
			act ( "$n has restored you.", ch, NULL, vch, TO_VICT );
		}

		snprintf ( buf, sizeof ( buf ), "$N restored room %d.", IN_ROOM ( ch )->vnum );
		wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );

		writeBuffer ( "Room restored.\n\r", ch );
		return;

	}

	if ( get_trust ( ch ) >=  MAX_LEVEL - 1 && !str_cmp ( arg, "all" ) ) {
		/* cure all */

		for ( d = socket_list; d != NULL; d = d->next ) {
			victim = d->character;

			if ( victim == NULL || IS_NPC ( victim ) )
			{ continue; }

			affect_strip ( victim, gsn_plague );
			affect_strip ( victim, gsn_poison );
			affect_strip ( victim, gsn_blindness );
			affect_strip ( victim, gsn_sleep );
			affect_strip ( victim, gsn_curse );

			victim->hit 	= victim->max_hit;
			victim->mana	= victim->max_mana;
			victim->move	= victim->max_move;
			update_pos ( victim );
			if ( IN_ROOM ( victim ) != NULL )
			{ act ( "$n has restored you.", ch, NULL, victim, TO_VICT ); }
		}
		writeBuffer ( "All active players restored.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	affect_strip ( victim, gsn_plague );
	affect_strip ( victim, gsn_poison );
	affect_strip ( victim, gsn_blindness );
	affect_strip ( victim, gsn_sleep );
	affect_strip ( victim, gsn_curse );
	victim->hit  = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	update_pos ( victim );
	act ( "$n has restored you.", ch, NULL, victim, TO_VICT );
	snprintf ( buf, sizeof ( buf ), "$N restored %s",
			   IS_NPC ( victim ) ? victim->short_descr : victim->name );
	wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}


DefineCommand ( cmd_freeze )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Freeze whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->act, PLR_FREEZE ) ) {
		REMOVE_BIT ( victim->act, PLR_FREEZE );
		writeBuffer ( "You can play again.\n\r", victim );
		writeBuffer ( "FREEZE removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N thaws %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->act, PLR_FREEZE );
		writeBuffer ( "You can't do ANYthing!\n\r", victim );
		writeBuffer ( "FREEZE set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N puts %s in the deep freeze.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	save_char_obj ( victim );

	return;
}



DefineCommand ( cmd_log )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Log whom?\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "all" ) ) {
		if ( fLogAll ) {
			fLogAll = FALSE;
			writeBuffer ( "Log ALL off.\n\r", ch );
		} else {
			fLogAll = TRUE;
			writeBuffer ( "Log ALL on.\n\r", ch );
		}
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	/*
	 * No level check, gods can log anyone.
	 */
	if ( IS_SET ( victim->act, PLR_LOG ) ) {
		REMOVE_BIT ( victim->act, PLR_LOG );
		writeBuffer ( "LOG removed.\n\r", ch );
	} else {
		SET_BIT ( victim->act, PLR_LOG );
		writeBuffer ( "LOG set.\n\r", ch );
	}

	return;
}



DefineCommand ( cmd_noemote )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Noemote whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}


	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOEMOTE ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOEMOTE );
		writeBuffer ( "You can emote again.\n\r", victim );
		writeBuffer ( "NOEMOTE removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores emotes to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOEMOTE );
		writeBuffer ( "You can't emote!\n\r", victim );
		writeBuffer ( "NOEMOTE set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's emotes.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_noshout )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Noshout whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOSHOUT ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOSHOUT );
		writeBuffer ( "You can shout again.\n\r", victim );
		writeBuffer ( "NOSHOUT removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores shouts to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOSHOUT );
		writeBuffer ( "You can't shout!\n\r", victim );
		writeBuffer ( "NOSHOUT set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's shouts.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_notell )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Notell whom?", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOTELL ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOTELL );
		writeBuffer ( "You can tell again.\n\r", victim );
		writeBuffer ( "NOTELL removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores tells to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOTELL );
		writeBuffer ( "You can't tell!\n\r", victim );
		writeBuffer ( "NOTELL set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's tells.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_peace )
{
	Creature *rch;

	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( FIGHTING ( rch ) != NULL )
		{ stop_fighting ( rch, TRUE ); }
		if ( IS_NPC ( rch ) && IS_SET ( rch->act, ACT_AGGRESSIVE ) )
		{ REMOVE_BIT ( rch->act, ACT_AGGRESSIVE ); }
	}

	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_lockdown )
{
	extern bool lockdown;
	lockdown = !lockdown;

	if ( lockdown ) {
		wiznet ( "$N has lockdowned the game.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Game lockdowned.\n\r", ch );
	} else {
		wiznet ( "$N removes lockdown.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Game un-lockdowned.\n\r", ch );
	}

	return;
}

/* RT anti-newbie code */

DefineCommand ( cmd_newbielockdown )
{
	extern bool newbielockdown;
	newbielockdown = !newbielockdown;

	if ( newbielockdown ) {
		wiznet ( "$N locks out new characters.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "New characters have been locked out.\n\r", ch );
	} else {
		wiznet ( "$N allows new characters back in.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Newlock removed.\n\r", ch );
	}

	return;
}


DefineCommand ( cmd_slookup )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int sn;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Lookup which skill or spell?\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "all" ) ) {
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name == NULL )
			{ break; }
			sprintf ( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
					  sn, skill_table[sn].slot, skill_table[sn].name );
			writeBuffer ( buf, ch );
		}
	} else {
		if ( ( sn = skill_lookup ( arg ) ) < 0 ) {
			writeBuffer ( "No such skill or spell.\n\r", ch );
			return;
		}

		sprintf ( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
				  sn, skill_table[sn].slot, skill_table[sn].name );
		writeBuffer ( buf, ch );
	}

	return;
}

/* RT set replaces sset, mset, oset, and rset */
DefineCommand ( cmd_set )
{
	char arg[MAX_INPUT_LENGTH];

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set mob   <name> <field> <value>\n\r", ch );
		writeBuffer ( "  set obj   <name> <field> <value>\n\r", ch );
		writeBuffer ( "  set room  <room> <field> <value>\n\r", ch );
		writeBuffer ( "  set skill <name> <spell or skill> <value>\n\r", ch );
		return;
	}

	argument = ChopC ( argument, arg );

	if ( !str_prefix ( arg, "mobile" ) || !str_prefix ( arg, "character" ) ) {
		cmd_function ( ch, &cmd_mset, argument );
		return;
	}

	if ( !str_prefix ( arg, "skill" ) || !str_prefix ( arg, "spell" ) ) {
		cmd_function ( ch, &cmd_sset, argument );
		return;
	}

	if ( !str_prefix ( arg, "object" ) ) {
		cmd_function ( ch, &cmd_oset, argument );
		return;
	}

	if ( !str_prefix ( arg, "room" ) ) {
		cmd_function ( ch, &cmd_rset, argument );
		return;
	}
	/* echo syntax */
	cmd_function ( ch, &cmd_set, "" );
}


DefineCommand ( cmd_sset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Creature *victim;
	int value;
	int sn;
	bool fAll;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	argument = ChopC ( argument, arg3 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set skill <name> <spell or skill> <value>\n\r", ch );
		writeBuffer ( "  set skill <name> all <value>\n\r", ch );
		writeBuffer ( "   (use the name of the skill, not the number)\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	fAll = !str_cmp ( arg2, "all" );
	sn   = 0;
	if ( !fAll && ( sn = skill_lookup ( arg2 ) ) < 0 ) {
		writeBuffer ( "No such skill or spell.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value.
	 */
	if ( !is_number ( arg3 ) ) {
		writeBuffer ( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi ( arg3 );
	if ( value < 0 || value > 100 ) {
		writeBuffer ( "Value range is 0 to 100.\n\r", ch );
		return;
	}

	if ( fAll ) {
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name != NULL )
			{ victim->pcdata->learned[sn]	= value; }
		}
	} else {
		victim->pcdata->learned[sn] = value;
	}

	return;
}


DefineCommand ( cmd_mset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	char buf[100];
	Creature *victim;
	int value;

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Unknown Syntax!\r\nset char <name> <field> <value>\r\n", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    str int wis dex con sex archetype level\n\r",	ch );
		writeBuffer ( "    race group gold silver hp mana move prac\n\r", ch );
		writeBuffer ( "    align train thirst hunger drunk full\n\r",	ch );
		writeBuffer ( "    security\n\r",				ch );
		return;
	}

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set char <name> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    str int wis dex con sex archetype level\n\r",	ch );
		writeBuffer ( "    race group gold silver hp mana move prac\n\r", ch );
		writeBuffer ( "    align train thirst hunger drunk full\n\r",	ch );
		writeBuffer ( "    security\n\r",				ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	/* clear zones for mobs */
	victim->zone = NULL;

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = is_number ( arg3 ) ? atoi ( arg3 ) : -1;

	/*
	 * Set something.
	 */
	if ( !str_cmp ( arg2, "str" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_STR ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Strength range is 3 to %d\n\r.",
					   get_max_train ( victim, STAT_STR ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_STR] = value;
		return;
	}

	if ( !str_cmp ( arg2, "security" ) ) {	/* OLC */
		if ( IS_NPC ( ch ) ) {
			writeBuffer ( "Si, claro.\n\r", ch );
			return;
		}

		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value > ch->pcdata->security || value < 0 ) {
			if ( ch->pcdata->security != 0 ) {
				sprintf ( buf, "Valid security is 0-%d.\n\r",
						  ch->pcdata->security );
				writeBuffer ( buf, ch );
			} else {
				writeBuffer ( "Valid security is 0 only.\n\r", ch );
			}
			return;
		}
		victim->pcdata->security = value;
		return;
	}

	if ( !str_cmp ( arg2, "int" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_INT ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Intelligence range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_INT ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_INT] = value;
		return;
	}

	if ( !str_cmp ( arg2, "wis" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_WIS ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Wisdom range is 3 to %d.\n\r", get_max_train ( victim, STAT_WIS ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_WIS] = value;
		return;
	}

	if ( !str_cmp ( arg2, "dex" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_DEX ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Dexterity range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_DEX ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_DEX] = value;
		return;
	}

	if ( !str_cmp ( arg2, "con" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_CON ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Constitution range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_CON ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_CON] = value;
		return;
	}

	if ( !str_prefix ( arg2, "sex" ) ) {
		if ( value < 0 || value > 2 ) {
			writeBuffer ( "Sex range is 0 to 2.\n\r", ch );
			return;
		}
		victim->sex = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->true_sex = value; }
		return;
	}

	if ( !str_prefix ( arg2, "archetype" ) ) {
		int archetype;

		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Mobiles have no archetype.\n\r", ch );
			return;
		}

		archetype = archetype_lookup ( arg3 );
		if ( archetype == -1 ) {
			char buf[MAX_STRING_LENGTH];

			strcpy ( buf, "Possible archetypees are: " );
			for ( archetype = 0; archetype < MAX_CLASS; archetype++ ) {
				if ( archetype > 0 )
				{ strcat ( buf, " " ); }
				strcat ( buf, archetype_table[archetype].name );
			}
			strcat ( buf, ".\n\r" );

			writeBuffer ( buf, ch );
			return;
		}

		victim->archetype = archetype;
		return;
	}

	if ( !str_prefix ( arg2, "level" ) ) {
		if ( !IS_NPC ( victim ) ) {
			writeBuffer ( "Not on PC's.\n\r", ch );
			return;
		}

		if ( value < 0 || value > MAX_LEVEL ) {
			snprintf ( buf, sizeof ( buf ), "Level range is 0 to %d.\n\r", MAX_LEVEL );
			writeBuffer ( buf, ch );
			return;
		}
		victim->level = value;
		return;
	}

	if ( !str_prefix ( arg2, "gold" ) ) {
		victim->gold = value;
		return;
	}

	if ( !str_prefix ( arg2, "silver" ) ) {
		victim->silver = value;
		return;
	}

	if ( !str_prefix ( arg2, "hp" ) ) {
		if ( value < -10 || value > 30000 ) {
			writeBuffer ( "Hp range is -10 to 30,000 hit points.\n\r", ch );
			return;
		}
		victim->max_hit = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_hit = value; }
		return;
	}

	if ( !str_prefix ( arg2, "mana" ) ) {
		if ( value < 0 || value > 30000 ) {
			writeBuffer ( "Mana range is 0 to 30,000 mana points.\n\r", ch );
			return;
		}
		victim->max_mana = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_mana = value; }
		return;
	}

	if ( !str_prefix ( arg2, "move" ) ) {
		if ( value < 0 || value > 30000 ) {
			writeBuffer ( "Move range is 0 to 30,000 move points.\n\r", ch );
			return;
		}
		victim->max_move = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_move = value; }
		return;
	}

	if ( !str_prefix ( arg2, "practice" ) ) {
		if ( value < 0 || value > 250 ) {
			writeBuffer ( "Practice range is 0 to 250 sessions.\n\r", ch );
			return;
		}
		victim->practice = value;
		return;
	}

	if ( !str_prefix ( arg2, "train" ) ) {
		if ( value < 0 || value > 50 ) {
			writeBuffer ( "Training session range is 0 to 50 sessions.\n\r", ch );
			return;
		}
		victim->train = value;
		return;
	}

	if ( !str_prefix ( arg2, "align" ) ) {
		if ( value < -1000 || value > 1000 ) {
			writeBuffer ( "Alignment range is -1000 to 1000.\n\r", ch );
			return;
		}
		victim->alignment = value;
		return;
	}

	if ( !str_prefix ( arg2, "thirst" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Thirst range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_THIRST] = value;
		return;
	}

	if ( !str_prefix ( arg2, "drunk" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Drunk range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_DRUNK] = value;
		return;
	}

	if ( !str_prefix ( arg2, "full" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_FULL] = value;
		return;
	}

	if ( !str_prefix ( arg2, "hunger" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_HUNGER] = value;
		return;
	}

	if ( !str_prefix ( arg2, "race" ) ) {
		int race;

		race = race_lookup ( arg3 );

		if ( race == 0 ) {
			writeBuffer ( "That is not a valid race.\n\r", ch );
			return;
		}

		if ( !IS_NPC ( victim ) && !race_table[race].pc_race ) {
			writeBuffer ( "That is not a valid player race.\n\r", ch );
			return;
		}

		victim->race = race;
		return;
	}

	if ( !str_prefix ( arg2, "group" ) ) {
		if ( !IS_NPC ( victim ) ) {
			writeBuffer ( "Only on NPCs.\n\r", ch );
			return;
		}
		victim->group = value;
		return;
	}


	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_mset, "" );
	return;
}

DefineCommand ( cmd_string )
{
	char type [MAX_INPUT_LENGTH];
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Creature *victim;
	Item *obj;

	smash_tilde ( argument );
	argument = ChopC ( argument, type );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  string char <name> <field> <string>\n\r", ch );
		writeBuffer ( "    fields: name short long desc title spec\n\r", ch );
		writeBuffer ( "  string obj  <name> <field> <string>\n\r", ch );
		writeBuffer ( "    fields: name short long extended\n\r", ch );
		return;
	}

	if ( !str_prefix ( type, "character" ) || !str_prefix ( type, "mobile" ) ) {
		if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		/* clear zone for mobs */
		victim->zone = NULL;

		/* string something */

		if ( !str_prefix ( arg2, "name" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Not on PC's.\n\r", ch );
				return;
			}
			PURGE_DATA ( victim->name );
			victim->name = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "description" ) ) {
			PURGE_DATA ( victim->description );
			victim->description = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "short" ) ) {
			PURGE_DATA ( victim->short_descr );
			victim->short_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "long" ) ) {
			PURGE_DATA ( victim->long_descr );
			strcat ( arg3, "\n\r" );
			victim->long_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "title" ) ) {
			if ( IS_NPC ( victim ) ) {
				writeBuffer ( "Not on NPC's.\n\r", ch );
				return;
			}

			set_title ( victim, arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "spec" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Not on PC's.\n\r", ch );
				return;
			}

			if ( ( victim->spec_fun = spec_lookup ( arg3 ) ) == 0 ) {
				writeBuffer ( "No such spec fun.\n\r", ch );
				return;
			}

			return;
		}
	}

	if ( !str_prefix ( type, "object" ) ) {
		/* string an obj */

		if ( ( obj = get_obj_world ( ch, arg1 ) ) == NULL ) {
			writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch );
			return;
		}

		if ( !str_prefix ( arg2, "name" ) ) {
			PURGE_DATA ( obj->name );
			obj->name = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "short" ) ) {
			PURGE_DATA ( obj->short_descr );
			obj->short_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "long" ) ) {
			PURGE_DATA ( obj->description );
			obj->description = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "ed" ) || !str_prefix ( arg2, "extended" ) ) {
			DescriptionData *ed;

			argument = ChopC ( argument, arg3 );
			if ( argument == NULL ) {
				writeBuffer ( "Syntax: oset <object> ed <keyword> <string>\n\r",
							  ch );
				return;
			}

			char *dstr = ( char * ) argument;

			strcat ( dstr, "\n\r" );

			ed = new_extra_descr();

			ed->keyword		= assign_string ( arg3     );
			ed->description	= assign_string ( dstr );
			ed->next		= obj->extra_descr;
			obj->extra_descr	= ed;
			return;
		}
	}


	/* echo bad use message */
	cmd_function ( ch, &cmd_string, "" );
}



DefineCommand ( cmd_oset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Item *obj;
	int value;

	smash_tilde ( argument );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set obj <object> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",				ch );
		writeBuffer ( "    value0 value1 value2 value3 value4 (v1-v4)\n\r",	ch );
		writeBuffer ( "    extra wear level weight cost timer\n\r",		ch );
		return;
	}

	if ( ( obj = get_obj_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = atoi ( arg3 );

	/*
	 * Set something.
	 */
	if ( !str_cmp ( arg2, "value0" ) || !str_cmp ( arg2, "v0" ) ) {
		obj->value[0] = UMIN ( 50, value );
		return;
	}

	if ( !str_cmp ( arg2, "value1" ) || !str_cmp ( arg2, "v1" ) ) {
		obj->value[1] = value;
		return;
	}

	if ( !str_cmp ( arg2, "value2" ) || !str_cmp ( arg2, "v2" ) ) {
		obj->value[2] = value;
		return;
	}

	if ( !str_cmp ( arg2, "value3" ) || !str_cmp ( arg2, "v3" ) ) {
		obj->value[3] = value;
		return;
	}

	if ( !str_cmp ( arg2, "value4" ) || !str_cmp ( arg2, "v4" ) ) {
		obj->value[4] = value;
		return;
	}

	if ( !str_prefix ( arg2, "extra" ) ) {
		obj->extra_flags = value;
		return;
	}

	if ( !str_prefix ( arg2, "wear" ) ) {
		obj->wear_flags = value;
		return;
	}

	if ( !str_prefix ( arg2, "level" ) ) {
		obj->level = value;
		return;
	}

	if ( !str_prefix ( arg2, "weight" ) ) {
		obj->weight = value;
		return;
	}

	if ( !str_prefix ( arg2, "cost" ) ) {
		obj->cost = value;
		return;
	}

	if ( !str_prefix ( arg2, "timer" ) ) {
		obj->timer = value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_oset, "" );
	return;
}



DefineCommand ( cmd_rset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	RoomData *location;
	int value;

	smash_tilde ( argument );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set room <location> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    flags sector\n\r",				ch );
		return;
	}

	if ( ( location = find_location ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, location ) && IN_ROOM ( ch ) != location
			&&  room_is_private ( location ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value.
	 */
	if ( !is_number ( arg3 ) ) {
		writeBuffer ( "Value must be numeric.\n\r", ch );
		return;
	}
	value = atoi ( arg3 );

	/*
	 * Set something.
	 */
	if ( !str_prefix ( arg2, "flags" ) ) {
		location->room_flags	= value;
		return;
	}

	if ( !str_prefix ( arg2, "sector" ) ) {
		location->sector_type	= value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_rset, "" );
	return;
}



DefineCommand ( cmd_sockets )
{
	char buf[2 * MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	int count;

	count	= 0;
	buf[0]	= '\0';

	ChopC ( argument, arg );
	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->character != NULL && can_see ( ch, d->character )
				&& ( arg[0] == '\0' || is_name ( arg, d->character->name )
					 || ( d->original && is_name ( arg, d->original->name ) ) ) ) {
			count++;
			sprintf ( buf + strlen ( buf ), "[%3d %2d] %s@%s\n\r",
					  d->descriptor,
					  d->connected,
					  d->original  ? d->original->name  :
					  d->character ? d->character->name : "(none)",
					  d->host
					);
		}
	}
	if ( count == 0 ) {
		writeBuffer ( "No one by that name is connected.\n\r", ch );
		return;
	}

	sprintf ( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
	strcat ( buf, buf2 );
	writePage ( buf, ch );
	return;
}



/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
DefineCommand ( cmd_force )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "Force whom to do what?\n\r", ch );
		return;
	}

	ChopC ( argument, arg2 );

	if ( !str_cmp ( arg2, "delete" ) || !str_prefix ( arg2, "mob" ) ) {
		writeBuffer ( "That will NOT be done.\n\r", ch );
		return;
	}

	sprintf ( buf, "$n forces you to '%s'.", argument );

	if ( !str_cmp ( arg, "all" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 3 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch ) ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else if ( !str_cmp ( arg, "players" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 2 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch )
					&&	 vch->level < LEVEL_HERO ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else if ( !str_cmp ( arg, "gods" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 2 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch )
					&&   vch->level >= LEVEL_HERO ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else {
		Creature *victim;

		if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		if ( victim == ch ) {
			writeBuffer ( "Aye aye, right away!\n\r", ch );
			return;
		}

		if ( !is_room_owner ( ch, IN_ROOM ( victim ) )
				&&  IN_ROOM ( ch ) != IN_ROOM ( victim )
				&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
			writeBuffer ( "That character is in a private room.\n\r", ch );
			return;
		}

		if ( get_trust ( victim ) >= get_trust ( ch ) ) {
			writeBuffer ( "Do it yourself!\n\r", ch );
			return;
		}

		if ( !IS_NPC ( victim ) && get_trust ( ch ) < MAX_LEVEL - 3 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		act ( buf, ch, NULL, victim, TO_VICT );
		interpret ( victim, argument );
	}

	writeBuffer ( "Ok.\n\r", ch );
	return;
}



/*
 * New routines by Dionysos.
 */
DefineCommand ( cmd_invis )
{
	int level;
	char arg[MAX_STRING_LENGTH];

	/* RT code for taking a level argument */
	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
		/* take the default path */

		if ( ch->invis_level ) {
			ch->invis_level = 0;
			act ( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly fade back into existence.\n\r", ch );
		} else {
			ch->invis_level = get_trust ( ch );
			act ( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly vanish into thin air.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi ( arg );
		if ( level < 2 || level > get_trust ( ch ) ) {
			writeBuffer ( "Invis level must be between 2 and your level.\n\r", ch );
			return;
		} else {
			ch->reply = NULL;
			ch->invis_level = level;
			act ( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly vanish into thin air.\n\r", ch );
		}
	}

	return;
}


DefineCommand ( cmd_incognito )
{
	int level;
	char arg[MAX_STRING_LENGTH];

	/* RT code for taking a level argument */
	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
		/* take the default path */

		if ( ch->incog_level ) {
			ch->incog_level = 0;
			act ( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You are no longer cloaked.\n\r", ch );
		} else {
			ch->incog_level = get_trust ( ch );
			act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You cloak your presence.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi ( arg );
		if ( level < 2 || level > get_trust ( ch ) ) {
			writeBuffer ( "Incog level must be between 2 and your level.\n\r", ch );
			return;
		} else {
			ch->reply = NULL;
			ch->incog_level = level;
			act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You cloak your presence.\n\r", ch );
		}
	}

	return;
}



DefineCommand ( cmd_holylight )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( IS_SET ( ch->act, PLR_HOLYLIGHT ) ) {
		REMOVE_BIT ( ch->act, PLR_HOLYLIGHT );
		writeBuffer ( "God Sight has been disabled.\r\n", ch );
	} else {
		SET_BIT ( ch->act, PLR_HOLYLIGHT );
		writeBuffer ( "God Sight has been enabled.\r\n", ch );
	}

	return;
}

/* prefix command: it will put the string typed on each line typed */

DefineCommand ( cmd_prefi )
{
	writeBuffer ( "You cannot abbreviate the prefix command.\r\n", ch );
	return;
}

DefineCommand ( cmd_prefix )
{
	char buf[MAX_INPUT_LENGTH];

	if ( argument[0] == '\0' ) {
		if ( ch->prefix[0] == '\0' ) {
			writeBuffer ( "You have no prefix to clear.\r\n", ch );
			return;
		}

		writeBuffer ( "Prefix removed.\r\n", ch );
		PURGE_DATA ( ch->prefix );
		ch->prefix = assign_string ( "" );
		return;
	}

	if ( ch->prefix[0] != '\0' ) {
		snprintf ( buf, sizeof ( buf ), "Prefix changed to %s.\r\n", argument );
		PURGE_DATA ( ch->prefix );
	} else {
		snprintf ( buf, sizeof ( buf ), "Prefix set to %s.\r\n", argument );
	}
	writeBuffer ( buf, ch );
	ch->prefix = assign_string ( argument );
}

DefineCommand ( cmd_makestaff )
{
	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: /makestaff <username> <flag> <on|off>\n\r", ch );
		writeBuffer ( "Flags: staff, builder, relations, security, coder\n\r", ch );
		return;
	}

	char first[1000];
	char second[1000];

	argument = ChopC ( argument, first );
	argument = ChopC ( argument, second );


	if ( IS_NULLSTR ( first ) || IS_NULLSTR ( second ) || IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: /makestaff <username> <flag> <on|off>\n\r", ch );
		return;
	}

	if ( str_cmp ( argument, "on" ) && str_cmp ( argument, "off" ) ) {
		writeBuffer ( "Toggle requires on or off.\n\r", ch );
		return;
	}

	bool onOff = false;
	if ( !str_cmp ( argument, "on" ) )
	{ onOff = true; }

	Creature *c, *cn;

	for ( c = char_list; c; c = cn ) {
		cn = c->next;

		// -- skip NPC's
		if ( IS_NPC ( c ) ) { continue; }

		if ( !str_cmp ( c->name, first ) ) {
			if ( c == ch && !str_cmp ( c->name, "Omega" ) ) {
				writeBuffer ( "You cannot flag yourself with staff flags!\n\r", ch );
				return;
			}

			if ( !str_cmp ( second, "staff" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_STAFF ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_STAFF ); }
				writeBuffer ( "Staff flag toggled.\n\r", ch );
				return;
			}

			if ( !str_cmp ( second, "security" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_SECURITY ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_SECURITY ); }
				writeBuffer ( "Vanguard flag toggled.\n\r", ch );
				return;
			}

			if ( !str_cmp ( second, "relations" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_RELATIONS ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_RELATIONS ); }
				writeBuffer ( "Player Relations flag toggled.\n\r", ch );
				return;
			}

			if ( !str_cmp ( second, "builder" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_BUILDER ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_BUILDER ); }
				writeBuffer ( "Builder flag toggled.\n\r", ch );
				return;
			}

			if ( !str_cmp ( second, "coder" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_CODER ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_CODER ); }
				writeBuffer ( "Coder flag toggled.\n\r", ch );
				return;
			}

			writeBuffer ( "Unknown flag option!\n\r", ch );
			return;
		}
	}
	writeBuffer ( "We couldn't find the player you wanted to adjust\n\r", ch );
	return;
}


DefineCommand ( cmd_sitrep )
{
	Creature *cr = ch;

	struct staff_rep {
		const char *name;
		int bitvector;
		int level;
		const char *description;
	};

	const struct staff_rep rep_table[] = {
		{"Logs", CF_SEE_LOGS, MAX_LEVEL, "View all logs as they happen" },
		{"Errors", CF_SEE_BUGS, MAX_LEVEL,  "View all bugs as they happen" },
		{"Security", CF_SEE_SECURITY, MAX_LEVEL, "View all security logs as they happen." },
		{"Commands", CF_SEE_COMMANDS, MAX_LEVEL, "See All Issued commands" },
		{"Debug",    CF_SEE_DEBUG,    MAX_LEVEL, "See all DEBUG messages." },
		{"script", CF_SEE_SCRIPT, MAX_LEVEL, "See all Scripting Logs." },
		{"suicide",  CF_SEE_ABORT, MAX_LEVEL, "See logs that cause the mud to commit suicide." },
		{NULL, 0, 0, NULL }
	};

	if ( argument[0] == '\0' || argument == NULL ) {
		for ( int x = 0; rep_table[x].name != NULL; x++ ) {
			if ( cr->level >= rep_table[x].level ) {
				writeBuffer ( Format ( "[%15s][%3s] : %s\n\r", rep_table[x].name, IS_SET ( cr->sitrep, rep_table[x].bitvector ) ? "On" : "Off", rep_table[x].description ), cr );
			}
		}
		writeBuffer ( "Type staffrep <flagname> <on|off>\n\r", cr );
		return;
	}

	char first[MAX_STRING_LENGTH];
	char *second;
	second = ChopC ( argument, first );
	if ( second[0] == '\0' || second == NULL ) {
		writeBuffer ( "Syntax: staffrep <flagname> <on|off>\n\r", cr );
		return;
	}

	for ( int x = 0; rep_table[x].name != NULL; x++ ) {
		if ( cr->level >= rep_table[x].level ) {
			if ( !str_cmp ( rep_table[x].name, first ) ) {
				if ( !str_cmp ( second, "on" ) ) {
					SET_BIT ( cr->sitrep, rep_table[x].bitvector );
					writeBuffer ( Format ( "%s has been enabled.\n\r", rep_table[x].name ), cr );
				} else if ( !str_cmp ( second, "off" ) ) {
					REMOVE_BIT ( cr->sitrep, rep_table[x].bitvector );
					writeBuffer ( Format ( "%s has been disabled.\n\r", rep_table[x].name ), cr );
				} else {
					writeBuffer ( "Syntax: staffrep <flagname> <on|off>\n\r", cr );
				}
				return;
			}
		}
	}
	writeBuffer ( "Unknown option!\n\r", cr );
}

DefineCommand ( cmd_events )
{
	EventManager::instance().reportEvents ( ch );
	return;
}

DefineCommand ( cmd_os )
{
	std::string cmpstring ( "" );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( Format ( "InfectEngine is compiled with:        g++\n\r" ) );
	//      cmpstring.append("Last Compile TimeStamp:            %s\n\r", __TIME_STAMP__ );
	cmpstring.append ( Format ( "Nested Include Level:              %d\n\r", __INCLUDE_LEVEL__ ) );
	cmpstring.append ( Format ( "Compiler Version:                  %s\n\r", __VERSION__ ) ) ;
#ifndef __APPLE__
	cmpstring.append ( Format ( "Compiler Flags: %s\n\r", wrapstr ( COMPILED_WITH ) ) );
#endif
	//      cmpstring.append("Compiler Register Prefix:          %s\n\r", __REGISTER_PREFIX__);
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( "                   Compiler Specifics                    \n\r" );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
	cmpstring.append ( Format ( "CHAR_BIT:   %10d    SCHAR_MIN:   %10d\n\r", CHAR_BIT, SCHAR_MIN ) );
	cmpstring.append ( Format ( "SCHAR_MAX:  %10d    UCHAR_MAX:   %10d\n\r", SCHAR_MAX, UCHAR_MAX ) );
	cmpstring.append ( Format ( "CHAR_MIN:   %10d    CHAR_MAX:    %10d\n\r", CHAR_MIN, CHAR_MAX ) );
	cmpstring.append ( Format ( "MB_LEN_MAX: %10d    SHRT_MIN:    %10d\n\r", MB_LEN_MAX, SHRT_MIN ) );
	cmpstring.append ( Format ( "SHRT_MAX:   %10d    USHRT_MAX:   %10d\n\r", SHRT_MAX, USHRT_MAX ) );
	cmpstring.append ( Format ( "INT_MIN:    %10d    INT_MAX:     %10d\n\r", INT_MIN, INT_MAX ) );
	cmpstring.append ( Format ( "UINT_MAX:   %10d    LONG_MIN:    %10ld\n\r", UINT_MAX, LONG_MIN ) );
	cmpstring.append ( Format ( "LONG_MAX:   %10ld    ULONG_MAX:   %10ld\n\r", LONG_MAX, ULONG_MAX ) );
	cmpstring.append ( Format ( "DBL_MIN:    %10e    DBL_MAX:     %10e\n\r", DBL_MIN, DBL_MAX ) );
	cmpstring.append ( Format ( "FLT_MIN:    %10e    FLT_MAX:     %10e\n\r", FLT_MIN, FLT_MAX ) );
	cmpstring.append ( "---------------------------------------------------------\n\r" );
#if defined(HAVE_SYS_UTSNAME_H)
	{
		struct utsname name;
		if ( uname ( &name ) == -1 ) {
			cmpstring.append ( Format ( "PlatformID: Unknown-uname_error%d", errno ) );
		} else {
			cmpstring.append ( Format ( "PlatformID: sysname='%s' nodename='%s' "
										"release='%s' version='%s' machine='%s'",
										name.sysname,
										name.nodename,
										name.release,
										name.version,
										name.machine ) );
		}
	}
#endif
#if defined HAVE_SYS_SYSCTL_H && defined(CTL_KERN) && defined(KERN_VERSION)
	{
		// tag on the KERN_VERSION info if available
		int mib[2];
		size_t len;
		mib[0] = CTL_KERN;
		mib[1] = KERN_VERSION;
		char kernver[512];
		len = sizeof ( kernver );
		if ( sysctl ( mib, 2, &kernver, &len, NULL, 0 ) == 0 ) {
			kernver[sizeof ( kernver ) - 1] = '\0';
			cmpstring.append ( Format ( "Kernel: %s", kernver ) );
		}
	}
#endif

	uid_t uid;
	gid_t gid;
	struct passwd *pw;
	struct utsname uts;
	char computer[256];

	// *Ya, found is true to start.. Only if failed attempts are false  :P* //
	bool found = true;

	uid = getuid();
	gid = getgid();

	cmpstring.append ( Format ( "The mud was last compiled on " __DATE__ " at " __TIME__ ".\n" ) );

	cmpstring.append ( Format ( "User is %s\n", whoami() ) );
	cmpstring.append ( Format ( "User IDS: uid=%d, gid=%d\n",  uid, gid ) );

	cmpstring.append ( "---------------------------------------------------------\n\r" );

	if ( gethostname ( computer, 255 ) != 0 || uname ( &uts ) < 0 ) {
		found = false;
	}

	// *Am i smart.... Ya, i'd like to think so :P* //
	if ( found ) {
		cmpstring.append ( Format ( "Computer host name is %s\n\r", computer ) );
		cmpstring.append ( Format ( "System is %s on %s hardware\n\r", uts.sysname, uts.machine ) );
		cmpstring.append ( Format ( "Nodename is %s\n\r", uts.nodename ) );
		cmpstring.append ( Format ( "Version is %s, %s\n\r", uts.release, wrapstr ( uts.version ) ) );
		cmpstring.append ( "---------------------------------------------------------\n\r" );
	}

	pw = getpwuid ( uid );
	cmpstring.append ( Format ( "UID passwd entry:\n name=%s, uid=%d, gid=%d, home=%s, shell=%s\n\r", pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir, pw->pw_shell ) );

	writePage ( C_STR ( cmpstring ), ch );
	return;
}

DefineCommand ( cmd_tweet )
{
	if ( cmd == 1000 ) {
		if ( LOWER ( argument[0] ) == 'y' ) {
			log_hd ( LOG_DEBUG, Format ( "%s has added '%s' to the twitlist query!", ch->name, ch->queries.query_string ) );
			// -- tweet on behalf of The Infected City
			tweetStatement ( ch->queries.query_string );
			writeBuffer ( "\r\nTweet has been added to the queue.\r\n", ch );
			// -- remove our old data
			PURGE_DATA ( ch->queries.query_string );
			ch->queries.querycommand = 0;
			return;
		}
		// -- we selected anything BUT the Y option, that means
		// -- we are NOT doing it, so we clear out the memory applicable
		// -- and attempt to avoid memory issues later on.
		ch->queries.querycommand = 0;
		PURGE_DATA ( ch->queries.query_string );
		return;
	}

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: tweet <message under 140 characters>\n\r", ch );
		return;
	}

	if ( strlen ( argument ) > 140 ) {
		writeBuffer ( "Tweets must be under 140 characters in length.\n\r", ch );
		return;
	}


	ch->queries.queryfunc = cmd_tweet;
	strcpy ( ch->queries.queryprompt, Format ( "Are you sure you want to tweet '%s' behalf of %s? (y/n)>", argument, "The Infected City" ) );

	// -- Assign the new new string to push
	PURGE_DATA ( ch->queries.query_string );
	ch->queries.query_string = assign_string ( argument );
	ch->queries.querycommand = 1000;
	return;
}

DefineCommand ( cmd_twitlist )
{
	extern std::list<std::string>tweetList;
	if ( IS_NULLSTR ( argument ) ) {
		std::string output ( "" );
		std::list<std::string>::iterator iter, itern;
		int cnt = 0;
		for ( iter = tweetList.begin(); iter != tweetList.end(); iter = itern ) {
			std::string s = ( *iter ).c_str();
			itern = ++iter;

			output.append ( Format ( "[%2d] - %s\n\r", cnt, C_STR ( s ) ) );
			cnt++;
		}

		output.append ( "Type /tweetlist [number] to remove a selected tweet.\n\r" );
		writePage ( C_STR ( output ), ch );
		return;
	}

	if ( !is_number ( argument ) ) {
		writeBuffer ( "Tweets are numbered, please select the tweet you want to remove.\n\r", ch );
		return;
	}

	std::list<std::string>::iterator iter, itern;
	int cnt = 0;
	for ( iter = tweetList.begin(); iter != tweetList.end(); iter = itern ) {
		std::string s = ( *iter ).c_str();
		itern = ++iter;

		if ( cnt == atoi ( argument ) ) {
			tweetList.remove ( s );
			writeBuffer ( "Tweet removed from list.\n\r", ch );
			return;
		}
		cnt++;
	}
	writeBuffer ( "Tweet not found.\n\r", ch );
	return;
}

DefineCommand ( cmd_noloot )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( IS_SET ( ch->act, PLR_CANLOOT ) ) {
		writeBuffer ( "Your corpse is now safe from thieves.\n\r", ch );
		REMOVE_BIT ( ch->act, PLR_CANLOOT );
	} else {
		writeBuffer ( "Your corpse may now be looted.\n\r", ch );
		SET_BIT ( ch->act, PLR_CANLOOT );
	}
}

DefineCommand ( cmd_nosummon )
{
	if ( IS_NPC ( ch ) ) {
		if ( IS_SET ( ch->imm_flags, IMM_SUMMON ) ) {
			writeBuffer ( "You are no longer immune to summon.\n\r", ch );
			REMOVE_BIT ( ch->imm_flags, IMM_SUMMON );
		} else {
			writeBuffer ( "You are now immune to summoning.\n\r", ch );
			SET_BIT ( ch->imm_flags, IMM_SUMMON );
		}
	} else {
		if ( IS_SET ( ch->act, PLR_NOSUMMON ) ) {
			writeBuffer ( "You are no longer immune to summon.\n\r", ch );
			REMOVE_BIT ( ch->act, PLR_NOSUMMON );
		} else {
			writeBuffer ( "You are now immune to summoning.\n\r", ch );
			SET_BIT ( ch->act, PLR_NOSUMMON );
		}
	}
}


