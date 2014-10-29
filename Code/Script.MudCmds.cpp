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


/***************************************************************************
 *                                                                         *
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *                                                                         *
 ***************************************************************************/

#include "Engine.h"

Command ( cmd_look 	);
extern RoomData *find_location ( Creature *, const char * );

/*
 * Command table.
 */
const	struct	mob_cmd_type	mob_cmd_table	[] = {
	{	"asound", 	cmd_mpasound	},
	{	"gecho",	cmd_mpgecho	},
	{	"zecho",	cmd_mpzecho	},
	{	"kill",		cmd_mpkill	},
	{	"assist",	cmd_mpassist	},
	{	"junk",		cmd_mpjunk	},
	{	"echo",		cmd_mpecho	},
	{	"echoaround",	cmd_mpechoaround	},
	{	"echoat",	cmd_mpechoat	},
	{	"mload",	cmd_mpmload	},
	{	"oload",	cmd_mpoload	},
	{	"purge",	cmd_mppurge	},
	{	"goto",		cmd_mpgoto	},
	{	"at",		cmd_mpat		},
	{	"transfer",	cmd_mptransfer	},
	{	"gtransfer",	cmd_mpgtransfer	},
	{	"otransfer",	cmd_mpotransfer	},
	{	"force",	cmd_mpforce	},
	{	"gforce",	cmd_mpgforce	},
	{	"vforce",	cmd_mpvforce	},
	{	"cast",		cmd_mpcast	},
	{	"damage",	cmd_mpdamage	},
	{	"remember",	cmd_mpremember	},
	{	"forget",	cmd_mpforget	},
	{	"delay",	cmd_mpdelay	},
	{	"cancel",	cmd_mpcancel	},
	{	"call",		cmd_mpcall	},
	{	"flee",		cmd_mpflee	},
	{	"remove",	cmd_mpremove	},
	{	"",		0		}
};

DefineCommand ( cmd_mob )
{
	/*
	 * Security check!
	 */
	if ( ch->desc != NULL && get_trust ( ch ) < MAX_LEVEL )
	{ return; }
	mob_interpret ( ch, argument );
}

/*
 * Mob command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void mob_interpret ( Creature *ch, const char *argument )
{
	char command[MAX_INPUT_LENGTH];
	int cmd;

	argument = ChopC ( argument, command );

	/*
	 * Look for command in command table.
	 */
	for ( cmd = 0; mob_cmd_table[cmd].name[0] != '\0'; cmd++ ) {
		if ( command[0] == mob_cmd_table[cmd].name[0]
				&&   !str_prefix ( command, mob_cmd_table[cmd].name ) ) {
			( *mob_cmd_table[cmd].cmd_fun ) ( ch, command, argument, cmd );
			tail_chain( );
			return;
		}
	}
	log_hd ( LOG_ERROR, Format ( "Mob_interpret: invalid cmd from mob %d: '%s'",
								 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0, command ) );
}

const char *mprog_type_to_name ( int type )
{
	switch ( type ) {
		case TRIG_ACT:
			return "ACT";
		case TRIG_SPEECH:
			return "SPEECH";
		case TRIG_RANDOM:
			return "RANDOM";
		case TRIG_FIGHT:
			return "FIGHT";
		case TRIG_HPCNT:
			return "HPCNT";
		case TRIG_DEATH:
			return "DEATH";
		case TRIG_ENTRY:
			return "ENTRY";
		case TRIG_GREET:
			return "GREET";
		case TRIG_GRALL:
			return "GRALL";
		case TRIG_GIVE:
			return "GIVE";
		case TRIG_BRIBE:
			return "BRIBE";
		case TRIG_KILL:
			return "KILL";
		case TRIG_DELAY:
			return "DELAY";
		case TRIG_SURR:
			return "SURRENDER";
		case TRIG_EXIT:
			return "EXIT";
		case TRIG_EXALL:
			return "EXALL";
		default:
			return "ERROR";
	}
}

/*
 * Displays MOBprogram triggers of a mobile
 *
 * Syntax: mpstat [name]
 */
DefineCommand ( cmd_mpstat )
{
	char        arg[ MAX_STRING_LENGTH  ];
	MPROG_LIST  *mprg;
	Creature   *victim;
	int i;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Mpstat whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "No such creature.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) ) {
		writeBuffer ( "That is not a mobile.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "No such creature visible.\n\r", ch );
		return;
	}

	sprintf ( arg, "Mobile #%-6d [%s]\n\r",
			  victim->pIndexData->vnum, victim->short_descr );
	writeBuffer ( arg, ch );

	sprintf ( arg, "Delay   %-6d [%s]\n\r",
			  victim->mprog_delay,
			  victim->mprog_target == NULL
			  ? "No target" : victim->mprog_target->name );
	writeBuffer ( arg, ch );

	if ( !victim->pIndexData->mprog_flags ) {
		writeBuffer ( "[No programs set]\n\r", ch );
		return;
	}

	for ( i = 0, mprg = victim->pIndexData->mprogs; mprg != NULL;
			mprg = mprg->next )

	{
		sprintf ( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
				  ++i,
				  mprog_type_to_name ( mprg->trig_type ),
				  mprg->vnum,
				  mprg->trig_phrase );
		writeBuffer ( arg, ch );
	}

	return;

}

/*
 * Displays the source code of a given MOBprogram
 *
 * Syntax: mpdump [vnum]
 */
DefineCommand ( cmd_mpdump )
{
	char buf[ MAX_INPUT_LENGTH ];
	MPROG_CODE *mprg;

	ChopC ( argument, buf );
	if ( ( mprg = get_mprog_index ( atoi ( buf ) ) ) == NULL ) {
		writeBuffer ( "No such MOBprogram.\n\r", ch );
		return;
	}
	writePage ( mprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: mob gecho [string]
 */
DefineCommand ( cmd_mpgecho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpGEcho: missing argument from vnum %d",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == STATE_PLAYING ) {
			if ( IsStaff ( d->character ) )
			{ writeBuffer ( "Mob echo> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r", d->character );
		}
	}
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: mob zecho [string]
 */
DefineCommand ( cmd_mpzecho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpZEcho: missing argument from vnum %d",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( IN_ROOM ( ch ) == NULL )
	{ return; }

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == STATE_PLAYING
				&&   d->character->in_room != NULL
				&&   d->character->in_room->area == IN_ROOM ( ch )->area ) {
			if ( IsStaff ( d->character ) )
			{ writeBuffer ( "Mob echo> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r", d->character );
		}
	}
}

/*
 * Prints the argument to all the rooms aroud the mobile
 *
 * Syntax: mob asound [string]
 */
DefineCommand ( cmd_mpasound )
{

	RoomData *was_in_room;
	int              door;

	if ( argument[0] == '\0' )
	{ return; }

	was_in_room = IN_ROOM ( ch );
	for ( door = 0; door < 6; door++ ) {
		Exit       *pexit;

		if ( ( pexit = was_in_room->exit[door] ) != NULL
				&&   pexit->u1.to_room != NULL
				&&   pexit->u1.to_room != was_in_room ) {
			IN_ROOM ( ch ) = pexit->u1.to_room;
			MOBtrigger  = FALSE;
			act ( argument, ch, NULL, NULL, TO_ROOM );
			MOBtrigger  = TRUE;
		}
	}
	IN_ROOM ( ch ) = was_in_room;
	return;

}

/*
 * Lets the mobile kill any player or mobile without murder
 *
 * Syntax: mob kill [victim]
 */
DefineCommand ( cmd_mpkill )
{
	char      arg[ MAX_INPUT_LENGTH ];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
	{ return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	if ( victim == ch || IS_NPC ( victim ) || ch->position == POS_FIGHTING )
	{ return; }

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master == victim ) {
		log_hd ( LOG_ERROR, Format ( "MpKill - Charmed mob attacking master from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	multi_hit ( ch, victim, TYPE_UNDEFINED );
	return;
}

/*
 * Lets the mobile assist another mob or player
 *
 * Syntax: mob assist [character]
 */
DefineCommand ( cmd_mpassist )
{
	char      arg[ MAX_INPUT_LENGTH ];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
	{ return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	if ( victim == ch || FIGHTING ( ch ) != NULL || FIGHTING ( victim ) == NULL )
	{ return; }

	multi_hit ( ch, FIGHTING ( victim ), TYPE_UNDEFINED );
	return;
}


/*
 * Lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy
 * items using all.xxxxx or just plain all of them
 *
 * Syntax: mob junk [item]
 */

DefineCommand ( cmd_mpjunk )
{
	char      arg[ MAX_INPUT_LENGTH ];
	Item *obj;
	Item *obj_next;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
	{ return; }

	if ( !SameString ( arg, "all" ) && str_prefix ( "all.", arg ) ) {
		if ( ( obj = get_obj_wear ( ch, arg ) ) != NULL ) {
			unequip_char ( ch, obj );
			extract_obj ( obj );
			return;
		}
		if ( ( obj = get_obj_carry ( ch, arg, ch ) ) == NULL )
		{ return; }
		extract_obj ( obj );
	} else
		for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( arg[3] == '\0' || is_name ( &arg[4], obj->name ) ) {
				if ( obj->wear_loc != WEAR_NONE )
				{ unequip_char ( ch, obj ); }
				extract_obj ( obj );
			}
		}

	return;

}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: mob echoaround [victim] [string]
 */

DefineCommand ( cmd_mpechoaround )
{
	char       arg[ MAX_INPUT_LENGTH ];
	Creature *victim;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' )
	{ return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	act ( argument, ch, NULL, victim, TO_NOTVICT );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: mob echoat [victim] [string]
 */
DefineCommand ( cmd_mpechoat )
{
	char       arg[ MAX_INPUT_LENGTH ];
	Creature *victim;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{ return; }

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	act ( argument, ch, NULL, victim, TO_VICT );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: mpecho [string]
 */
DefineCommand ( cmd_mpecho )
{
	if ( argument[0] == '\0' )
	{ return; }
	act ( argument, ch, NULL, NULL, TO_ROOM );
}

/*
 * Lets the mobile load another mobile.
 *
 * Syntax: mob mload [vnum]
 */
DefineCommand ( cmd_mpmload )
{
	char            arg[ MAX_INPUT_LENGTH ];
	NPCData *pMobIndex;
	Creature      *victim;
	int vnum;

	ChopC ( argument, arg );

	if ( IN_ROOM ( ch ) == NULL || arg[0] == '\0' || !is_number ( arg ) )
	{ return; }

	vnum = atoi ( arg );
	if ( ( pMobIndex = get_mob_index ( vnum ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "Mpmload: bad mob index (%d) from mob %d",
									 vnum, IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	victim = create_mobile ( pMobIndex );
	char_to_room ( victim, IN_ROOM ( ch ) );
	return;
}

/*
 * Lets the mobile load an object
 *
 * Syntax: mob oload [vnum] [level] {R}
 */
DefineCommand ( cmd_mpoload )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	ItemData *pObjIndex;
	Item       *obj;
	int             level;
	bool            fToroom = FALSE, fWear = FALSE;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	ChopC ( argument, arg3 );

	if ( arg1[0] == '\0' || !is_number ( arg1 ) ) {
		log_hd ( LOG_ERROR, Format ( "Mpoload - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( arg2[0] == '\0' ) {
		level = get_trust ( ch );
	} else {
		/*
		 * New feature from Alander.
		 */
		if ( !is_number ( arg2 ) ) {
			log_hd ( LOG_ERROR, Format ( "Mpoload - Bad syntax from vnum %d.",
										 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
			return;
		}
		level = atoi ( arg2 );
		if ( level < 0 || level > get_trust ( ch ) ) {
			log_hd ( LOG_ERROR, Format ( "Mpoload - Bad level from vnum %d.",
										 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
			return;
		}
	}

	/*
	 * Added 3rd argument
	 * omitted - load to mobile's inventory
	 * 'R'     - load to room
	 * 'W'     - load to mobile and force wear
	 */
	if ( arg3[0] == 'R' || arg3[0] == 'r' )
	{ fToroom = TRUE; }
	else if ( arg3[0] == 'W' || arg3[0] == 'w' )
	{ fWear = TRUE; }

	if ( ( pObjIndex = get_obj_index ( atoi ( arg1 ) ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "Mpoload - Bad vnum arg from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	obj = create_object ( pObjIndex, level );
	if ( ( fWear || !fToroom ) && CAN_WEAR ( obj, ITEM_TAKE ) ) {
		obj_to_char ( obj, ch );
		if ( fWear )
		{ wear_obj ( ch, obj, TRUE ); }
	} else {
		obj_to_room ( obj, IN_ROOM ( ch ) );
	}

	return;
}

/*
 * Lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room. The mobile cannot
 * purge itself for safety reasons.
 *
 * syntax mob purge {target}
 */
DefineCommand ( cmd_mppurge )
{
	char       arg[ MAX_INPUT_LENGTH ];
	Creature *victim;
	Item  *obj;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		/* 'purge' */
		Creature *vnext;
		Item  *obj_next;

		for ( victim = IN_ROOM ( ch )->people; victim != NULL; victim = vnext ) {
			vnext = victim->next_in_room;
			if ( IS_NPC ( victim ) && victim != ch
					&&   !IS_SET ( victim->act, ACT_NOPURGE ) )
			{ extract_char ( victim, TRUE ); }
		}

		for ( obj = IN_ROOM ( ch )->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( !IS_SET ( obj->extra_flags, ITEM_NOPURGE ) )
			{ extract_obj ( obj ); }
		}

		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		if ( ( obj = get_obj_here ( ch, arg ) ) ) {
			extract_obj ( obj );
		} else {
			log_hd ( LOG_ERROR, Format ( "Mppurge - Bad argument from vnum %d.",
										 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		}
		return;
	}

	if ( !IS_NPC ( victim ) ) {
		log_hd ( LOG_ERROR, Format ( "Mppurge - Purging a PC from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	extract_char ( victim, TRUE );
	return;
}


/*
 * Lets the mobile goto any location it wishes that is not private.
 *
 * Syntax: mob goto [location]
 */
DefineCommand ( cmd_mpgoto )
{
	char             arg[ MAX_INPUT_LENGTH ];
	RoomData *location;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "Mpgoto - No argument from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( ( location = find_location ( ch, arg ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "Mpgoto - No such location from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( FIGHTING ( ch ) != NULL )
	{ stop_fighting ( ch, TRUE ); }

	char_from_room ( ch );
	char_to_room ( ch, location );

	return;
}

/*
 * Lets the mobile do a command at another location.
 *
 * Syntax: mob at [location] [commands]
 */
DefineCommand ( cmd_mpat )
{
	char             arg[ MAX_INPUT_LENGTH ];
	RoomData *location;
	RoomData *original;
	Creature       *wch;
	Item 	    *on;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "Mpat - Bad argument from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( ( location = find_location ( ch, arg ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "Mpat - No such location from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
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

/*
 * Lets the mobile transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: mob transfer [target|'all'] [location]
 */
DefineCommand ( cmd_mptransfer )
{
	char             arg1[ MAX_INPUT_LENGTH ];
	char             arg2[ MAX_INPUT_LENGTH ];
	RoomData *location;
	Creature       *victim;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "Mptransfer - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( SameString ( arg1, "all" ) ) {
		Creature *victim_next;

		for ( victim = IN_ROOM ( ch )->people; victim != NULL; victim = victim_next ) {
			victim_next = victim->next_in_room;
			if ( !IS_NPC ( victim ) ) {
				cmd_function ( ch, &cmd_mptransfer, Format ( "%s %s", victim->name, arg2 ) );
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
			log_hd ( LOG_ERROR, Format ( "Mptransfer - No such location from vnum %d.",
										 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
			return;
		}

		if ( room_is_private ( location ) )
		{ return; }
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
	{ return; }

	if ( IN_ROOM ( victim ) == NULL )
	{ return; }

	if ( FIGHTING ( victim ) != NULL )
	{ stop_fighting ( victim, TRUE ); }
	char_from_room ( victim );
	char_to_room ( victim, location );
	cmd_function ( victim, &cmd_look, "auto" );

	return;
}

/*
 * Lets the mobile transfer all chars in same group as the victim.
 *
 * Syntax: mob gtransfer [victim] [location]
 */
DefineCommand ( cmd_mpgtransfer )
{
	char             arg1[ MAX_INPUT_LENGTH ];
	char             arg2[ MAX_INPUT_LENGTH ];
	char	     buf[MAX_STRING_LENGTH];
	Creature       *who, *victim, *victim_next;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "Mpgtransfer - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( ( who = get_char_room ( ch, arg1 ) ) == NULL )
	{ return; }

	for ( victim = IN_ROOM ( ch )->people; victim; victim = victim_next ) {
		victim_next = victim->next_in_room;
		if ( is_same_group ( who, victim ) ) {
			sprintf ( buf, "%s %s", victim->name, arg2 );
			cmd_function ( ch, &cmd_mptransfer, Format ( "%s %s", victim->name, arg2 ) );
		}
	}
	return;
}

/*
 * Lets the mobile force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the mobile.
 *
 * Syntax: mob force [victim] [commands]
 */
DefineCommand ( cmd_mpforce )
{
	char arg[ MAX_INPUT_LENGTH ];

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "Mpforce - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( SameString ( arg, "all" ) ) {
		Creature *vch;
		Creature *vch_next;

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( IN_ROOM ( vch ) == IN_ROOM ( ch )
					&& get_trust ( vch ) < get_trust ( ch )
					&& can_see ( ch, vch ) ) {
				interpret ( vch, argument );
			}
		}
	} else {
		Creature *victim;

		if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
		{ return; }

		if ( victim == ch )
		{ return; }

		interpret ( victim, argument );
	}

	return;
}

/*
 * Lets the mobile force a group something. Must be mortal level.
 *
 * Syntax: mob gforce [victim] [commands]
 */
DefineCommand ( cmd_mpgforce )
{
	char arg[ MAX_INPUT_LENGTH ];
	Creature *victim, *vch, *vch_next;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpGforce - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	if ( victim == ch )
	{ return; }

	for ( vch = IN_ROOM ( victim )->people; vch != NULL; vch = vch_next ) {
		vch_next = vch->next_in_room;

		if ( is_same_group ( victim, vch ) ) {
			interpret ( vch, argument );
		}
	}
	return;
}

/*
 * Forces all mobiles of certain vnum to do something (except ch)
 *
 * Syntax: mob vforce [vnum] [commands]
 */
DefineCommand ( cmd_mpvforce )
{
	Creature *victim, *victim_next;
	char arg[ MAX_INPUT_LENGTH ];
	int vnum;

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpVforce - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( !is_number ( arg ) ) {
		log_hd ( LOG_ERROR, Format ( "MpVforce - Non-number argument vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	vnum = atoi ( arg );

	for ( victim = char_list; victim; victim = victim_next ) {
		victim_next = victim->next;
		if ( IS_NPC ( victim ) && victim->pIndexData->vnum == vnum
				&&   ch != victim && FIGHTING ( victim ) == NULL )
		{ interpret ( victim, argument ); }
	}
	return;
}


/*
 * Lets the mobile cast spells --
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: mob cast [spell] {target}
 */

DefineCommand ( cmd_mpcast )
{
	Creature *vch;
	Item *obj;
	void *victim = NULL;
	char spell[ MAX_INPUT_LENGTH ],
		 target[ MAX_INPUT_LENGTH ];
	int sn;

	argument = ChopC ( argument, spell );
	ChopC ( argument, target );

	if ( spell[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpCast - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}

	if ( ( sn = skill_lookup ( spell ) ) < 0 ) {
		log_hd ( LOG_ERROR, Format ( "MpCast - No such spell from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	vch = get_char_room ( ch, target );
	obj = get_obj_here ( ch, target );
	switch ( skill_table[sn].target ) {
		default:
			return;
		case TAR_IGNORE:
			break;
		case TAR_CHAR_OFFENSIVE:
			if ( vch == NULL || vch == ch )
			{ return; }
			victim = ( void * ) vch;
			break;
		case TAR_CHAR_DEFENSIVE:
			victim = vch == NULL ? ( void * ) ch : ( void * ) vch;
			break;
		case TAR_CHAR_SELF:
			victim = ( void * ) ch;
			break;
		case TAR_OBJ_CHAR_DEF:
		case TAR_OBJ_CHAR_OFF:
		case TAR_OBJ_INV:
			if ( obj == NULL )
			{ return; }
			victim = ( void * ) obj;
	}
	( *skill_table[sn].spell_fun ) ( sn, ch->level, ch, victim,
									 skill_table[sn].target );
	return;
}

/*
 * Lets mob cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob damage [victim] [min] [max] {kill}
 */
DefineCommand ( cmd_mpdamage )
{
	Creature *victim = NULL, *victim_next;
	char target[ MAX_INPUT_LENGTH ],
		 min[ MAX_INPUT_LENGTH ],
		 max[ MAX_INPUT_LENGTH ];
	int low, high;
	bool fAll = FALSE, fKill = FALSE;

	argument = ChopC ( argument, target );
	argument = ChopC ( argument, min );
	argument = ChopC ( argument, max );

	if ( target[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpDamage - Bad syntax from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	if ( SameString ( target, "all" ) )
	{ fAll = TRUE; }
	else if ( ( victim = get_char_room ( ch, target ) ) == NULL )
	{ return; }

	if ( is_number ( min ) )
	{ low = atoi ( min ); }
	else {
		log_hd ( LOG_ERROR, Format ( "MpDamage - Bad damage min vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0  ) );
		return;
	}
	if ( is_number ( max ) )
	{ high = atoi ( max ); }
	else {
		log_hd ( LOG_ERROR, Format ( "MpDamage - Bad damage max vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	ChopC ( argument, target );

	/*
	 * If kill parameter is omitted, this command is "safe" and will not
	 * kill the victim.
	 */

	if ( target[0] != '\0' )
	{ fKill = TRUE; }
	if ( fAll ) {
		for ( victim = IN_ROOM ( ch )->people; victim; victim = victim_next ) {
			victim_next = victim->next_in_room;
			if ( victim != ch )
				damage ( victim, victim,
						 fKill ?
						 Math::instance().range ( low, high ) : UMIN ( victim->hit, Math::instance().range ( low, high ) ),
						 TYPE_UNDEFINED, DAM_NONE, FALSE );
		}
	} else
		damage ( victim, victim,
				 fKill ?
				 Math::instance().range ( low, high ) : UMIN ( victim->hit, Math::instance().range ( low, high ) ),
				 TYPE_UNDEFINED, DAM_NONE, FALSE );
	return;
}

/*
 * Lets the mobile to remember a target. The target can be referred to
 * with $q and $Q codes in MOBprograms. See also "mob forget".
 *
 * Syntax: mob remember [victim]
 */
DefineCommand ( cmd_mpremember )
{
	char arg[ MAX_INPUT_LENGTH ];
	ChopC ( argument, arg );
	if ( arg[0] != '\0' )
	{ ch->mprog_target = get_char_world ( ch, arg ); }
	else
		log_hd ( LOG_ERROR, Format ( "MpRemember: missing argument from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
}

/*
 * Reverse of "mob remember".
 *
 * Syntax: mob forget
 */
DefineCommand ( cmd_mpforget )
{
	ch->mprog_target = NULL;
}

/*
 * Sets a delay for MOBprogram execution. When the delay time expires,
 * the mobile is checked for a MObprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_MOBILE
 *
 * Syntax: mob delay [pulses]
 */
DefineCommand ( cmd_mpdelay )
{
	char arg[ MAX_INPUT_LENGTH ];

	ChopC ( argument, arg );
	if ( !is_number ( arg ) ) {
		log_hd ( LOG_ERROR, Format ( "MpDelay: invalid arg from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	ch->mprog_delay = atoi ( arg );
}

/*
 * Reverse of "mob delay", deactivates the timer.
 *
 * Syntax: mob cancel
 */
DefineCommand ( cmd_mpcancel )
{
	ch->mprog_delay = -1;
}
/*
 * Lets the mobile to call another MOBprogram withing a MOBprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: mob call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
DefineCommand ( cmd_mpcall )
{
	char arg[ MAX_INPUT_LENGTH ];
	Creature *vch;
	Item *obj1, *obj2;
	MPROG_CODE *prg;
	extern void program_flow ( int, char *, Creature *, Creature *, const void *, const void * );

	argument = ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpCall: missing arguments from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	if ( ( prg = get_mprog_index ( atoi ( arg ) ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "MpCall: invalid prog from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	vch = NULL;
	obj1 = obj2 = NULL;
	argument = ChopC ( argument, arg );
	if ( arg[0] != '\0' )
	{ vch = get_char_room ( ch, arg ); }
	argument = ChopC ( argument, arg );
	if ( arg[0] != '\0' )
	{ obj1 = get_obj_here ( ch, arg ); }
	argument = ChopC ( argument, arg );
	if ( arg[0] != '\0' )
	{ obj2 = get_obj_here ( ch, arg ); }
	program_flow ( prg->vnum, prg->code, ch, vch, ( void * ) obj1, ( void * ) obj2 );
}

/*
 * Forces the mobile to flee.
 *
 * Syntax: mob flee
 *
 */
DefineCommand ( cmd_mpflee )
{
	RoomData *was_in;
	Exit *pexit;
	int door, attempt;

	if ( FIGHTING ( ch ) != NULL )
	{ return; }

	if ( ( was_in = IN_ROOM ( ch ) ) == NULL )
	{ return; }

	for ( attempt = 0; attempt < 6; attempt++ ) {
		door = Math::instance().door( );
		if ( ( pexit = was_in->exit[door] ) == 0
				||   pexit->u1.to_room == NULL
				||   IS_SET ( pexit->exit_info, EX_CLOSED )
				|| ( IS_NPC ( ch )
					 &&   IS_SET ( pexit->u1.to_room->room_flags, ROOM_NO_MOB ) ) )
		{ continue; }

		move_char ( ch, door, FALSE );
		if ( IN_ROOM ( ch ) != was_in )
		{ return; }
	}
}

/*
 * Lets the mobile to transfer an object. The object must be in the same
 * room with the mobile.
 *
 * Syntax: mob otransfer [item name] [location]
 */
DefineCommand ( cmd_mpotransfer )
{
	Item *obj;
	RoomData *location;
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];

	argument = ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		log_hd ( LOG_ERROR, Format ( "MpOTransfer - Missing argument from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	ChopC ( argument, buf );
	if ( ( location = find_location ( ch, buf ) ) == NULL ) {
		log_hd ( LOG_ERROR, Format ( "MpOTransfer - No such location from vnum %d.",
									 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	}
	if ( ( obj = get_obj_here ( ch, arg ) ) == NULL )
	{ return; }
	if ( CARRIED_BY ( obj ) == NULL )
	{ obj_from_room ( obj ); }
	else {
		if ( obj->wear_loc != WEAR_NONE )
		{ unequip_char ( ch, obj ); }
		obj_from_char ( obj );
	}
	obj_to_room ( obj, location );
}

/*
 * Lets the mobile to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: mob remove [victim] [object vnum|'all']
 */
DefineCommand ( cmd_mpremove )
{
	Creature *victim;
	Item *obj, *obj_next;
	int vnum = 0;
	bool fAll = FALSE;
	char arg[ MAX_INPUT_LENGTH ];

	argument = ChopC ( argument, arg );
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
	{ return; }

	ChopC ( argument, arg );
	if ( SameString ( arg, "all" ) )
	{ fAll = TRUE; }
	else if ( !is_number ( arg ) ) {
		log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "MpRemove: Invalid object from vnum %d.",
				 IS_NPC ( ch ) ? ch->pIndexData->vnum : 0 ) );
		return;
	} else
	{ vnum = atoi ( arg ); }

	for ( obj = victim->carrying; obj; obj = obj_next ) {
		obj_next = obj->next_content;
		if ( fAll || obj->pIndexData->vnum == vnum ) {
			unequip_char ( ch, obj );
			obj_from_char ( obj );
			extract_obj ( obj );
		}
	}
}


