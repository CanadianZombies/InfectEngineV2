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
 **************************************************************************/

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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

/* RT code to delete yourself */

DefineCommand ( cmd_delet )
{
	writeBuffer ( "You must type the full command to delete yourself.\n\r", ch );
}

DefineCommand ( cmd_delete )
{
	char strsave[MAX_INPUT_LENGTH];

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ch->pcdata->confirm_delete ) {
		if ( argument[0] != '\0' ) {
			writeBuffer ( "Delete status removed.\n\r", ch );
			ch->pcdata->confirm_delete = FALSE;
			return;
		} else {
			sprintf ( strsave, "%s%s", PLAYER_DIR, capitalize ( ch->name ) );
			wiznet ( "$N turns $Mself into line noise.", ch, NULL, 0, 0, 0 );
			stop_fighting ( ch, TRUE );
			cmd_function ( ch, &cmd_quit, "delete" );
			unlink ( strsave );
			return;
		}
	}

	if ( argument[0] != '\0' ) {
		writeBuffer ( "Just type delete. No argument.\n\r", ch );
		return;
	}

	writeBuffer ( "Type delete again to confirm this command.\n\r", ch );
	writeBuffer ( "WARNING: this command is irreversible.\n\r", ch );
	writeBuffer ( "Typing delete with an argument will undo delete status.\n\r",
				  ch );
	ch->pcdata->confirm_delete = TRUE;
	wiznet ( "$N is contemplating deletion.", ch, NULL, 0, 0, get_trust ( ch ) );
}


/* RT code to display channel status */
DefineCommand ( cmd_channels )
{
	char buf[MAX_STRING_LENGTH];

	/* lists all channels and their status */
	writeBuffer ( "   channel     status\n\r", ch );
	writeBuffer ( "---------------------\n\r", ch );

	writeBuffer ( "gossip         ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOGOSSIP ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "auction        ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOAUCTION ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "music          ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOMUSIC ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "Q/A            ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOQUESTION ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "Quote          ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOQUOTE ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "grats          ", ch );
	if ( !IS_SET ( ch->comm, COMM_NOGRATS ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	if ( IsStaff ( ch ) ) {
		writeBuffer ( "god channel    ", ch );
		if ( !IS_SET ( ch->comm, COMM_NOWIZ ) )
		{ writeBuffer ( "ON\n\r", ch ); }
		else
		{ writeBuffer ( "OFF\n\r", ch ); }
	}

	writeBuffer ( "shouts         ", ch );
	if ( !IS_SET ( ch->comm, COMM_SHOUTSOFF ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "tells          ", ch );
	if ( !IS_SET ( ch->comm, COMM_DEAF ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	writeBuffer ( "quiet mode     ", ch );
	if ( IS_SET ( ch->comm, COMM_QUIET ) )
	{ writeBuffer ( "ON\n\r", ch ); }
	else
	{ writeBuffer ( "OFF\n\r", ch ); }

	if ( IS_SET ( ch->comm, COMM_AFK ) )
	{ writeBuffer ( "You are AFK.\n\r", ch ); }

	if ( IS_SET ( ch->comm, COMM_SNOOP_PROOF ) )
	{ writeBuffer ( "You are immune to snooping.\n\r", ch ); }

	if ( ch->lines != PAGELEN ) {
		if ( ch->lines ) {
			snprintf ( buf, sizeof ( buf ), "You display %d lines of scroll.\n\r", ch->lines + 2 );
			writeBuffer ( buf, ch );
		} else
		{ writeBuffer ( "Scroll buffering is off.\n\r", ch ); }
	}

	if ( ch->prompt != NULL ) {
		snprintf ( buf, sizeof ( buf ), "Your current prompt is: %s\n\r", ch->prompt );
		writeBuffer ( buf, ch );
	}

	if ( IS_SET ( ch->comm, COMM_NOSHOUT ) )
	{ writeBuffer ( "You cannot shout.\n\r", ch ); }

	if ( IS_SET ( ch->comm, COMM_NOTELL ) )
	{ writeBuffer ( "You cannot use tell.\n\r", ch ); }

	if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
	{ writeBuffer ( "You cannot use channels.\n\r", ch ); }

	if ( IS_SET ( ch->comm, COMM_NOEMOTE ) )
	{ writeBuffer ( "You cannot show emotions.\n\r", ch ); }

}

/* RT deaf blocks out all shouts */

DefineCommand ( cmd_deaf )
{

	if ( IS_SET ( ch->comm, COMM_DEAF ) ) {
		writeBuffer ( "You can now hear tells again.\n\r", ch );
		REMOVE_BIT ( ch->comm, COMM_DEAF );
	} else {
		writeBuffer ( "From now on, you won't hear tells.\n\r", ch );
		SET_BIT ( ch->comm, COMM_DEAF );
	}
}

/* RT quiet blocks out all communication */

DefineCommand ( cmd_quiet )
{
	if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
		writeBuffer ( "Quiet mode removed.\n\r", ch );
		REMOVE_BIT ( ch->comm, COMM_QUIET );
	} else {
		writeBuffer ( "From now on, you will only hear says and emotes.\n\r", ch );
		SET_BIT ( ch->comm, COMM_QUIET );
	}
}

/* afk command */

DefineCommand ( cmd_afk )
{
	if ( IS_SET ( ch->comm, COMM_AFK ) ) {
		writeBuffer ( "AFK mode removed. Type 'replay' to see tells.\n\r", ch );
		REMOVE_BIT ( ch->comm, COMM_AFK );
	} else {
		writeBuffer ( "You are now in AFK mode.\n\r", ch );
		SET_BIT ( ch->comm, COMM_AFK );
	}
}

DefineCommand ( cmd_replay )
{
	if ( IS_NPC ( ch ) ) {
		writeBuffer ( "You can't replay.\n\r", ch );
		return;
	}

	if ( buf_string ( ch->pcdata->buffer ) [0] == '\0' ) {
		writeBuffer ( "You have no tells to replay.\n\r", ch );
		return;
	}

	writePage ( buf_string ( ch->pcdata->buffer ), ch );
	clear_buf ( ch->pcdata->buffer );
}

/* RT auction rewritten in ROM style */
DefineCommand ( cmd_auction )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOAUCTION ) ) {
			writeBuffer ( "Auction channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOAUCTION );
		} else {
			writeBuffer ( "Auction channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOAUCTION );
		}
	} else { /* auction message sent, turn auction on if it is off */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;
		}

		REMOVE_BIT ( ch->comm, COMM_NOAUCTION );
	}

	sprintf ( buf, "You auction '%s'\n\r", argument );
	writeBuffer ( buf, ch );
	for ( d = socket_list; d != NULL; d = d->next ) {
		Creature *victim;

		victim = d->original ? d->original : d->character;

		if ( d->connected == CON_PLAYING &&
				d->character != ch &&
				!IS_SET ( victim->comm, COMM_NOAUCTION ) &&
				!IS_SET ( victim->comm, COMM_QUIET ) ) {
			act_new ( "$n auctions '$t'",
					  ch, argument, d->character, TO_VICT, POS_DEAD );
		}
	}
}

/* RT chat replaced with ROM gossip */
DefineCommand ( cmd_gossip )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOGOSSIP ) ) {
			writeBuffer ( "Gossip channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOGOSSIP );
		} else {
			writeBuffer ( "Gossip channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOGOSSIP );
		}
	} else { /* gossip message sent, turn gossip on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;

		}

		REMOVE_BIT ( ch->comm, COMM_NOGOSSIP );

		sprintf ( buf, "You gossip '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOGOSSIP ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n gossips '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

DefineCommand ( cmd_grats )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOGRATS ) ) {
			writeBuffer ( "Grats channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOGRATS );
		} else {
			writeBuffer ( "Grats channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOGRATS );
		}
	} else { /* grats message sent, turn grats on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;

		}

		REMOVE_BIT ( ch->comm, COMM_NOGRATS );

		sprintf ( buf, "You grats '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOGRATS ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n grats '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

DefineCommand ( cmd_quote )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOQUOTE ) ) {
			writeBuffer ( "Quote channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOQUOTE );
		} else {
			writeBuffer ( "Quote channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOQUOTE );
		}
	} else { /* quote message sent, turn quote on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;

		}

		REMOVE_BIT ( ch->comm, COMM_NOQUOTE );

		sprintf ( buf, "You quote '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOQUOTE ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n quotes '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

/* RT question channel */
DefineCommand ( cmd_question )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOQUESTION ) ) {
			writeBuffer ( "Q/A channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOQUESTION );
		} else {
			writeBuffer ( "Q/A channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOQUESTION );
		}
	} else { /* question sent, turn Q/A on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;
		}

		REMOVE_BIT ( ch->comm, COMM_NOQUESTION );

		sprintf ( buf, "You question '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOQUESTION ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n questions '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

/* RT answer channel - uses same line as questions */
DefineCommand ( cmd_answer )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOQUESTION ) ) {
			writeBuffer ( "Q/A channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOQUESTION );
		} else {
			writeBuffer ( "Q/A channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOQUESTION );
		}
	} else { /* answer sent, turn Q/A on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;
		}

		REMOVE_BIT ( ch->comm, COMM_NOQUESTION );

		sprintf ( buf, "You answer '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOQUESTION ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n answers '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

/* RT music channel */
DefineCommand ( cmd_music )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOMUSIC ) ) {
			writeBuffer ( "Music channel is now ON.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOMUSIC );
		} else {
			writeBuffer ( "Music channel is now OFF.\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOMUSIC );
		}
	} else { /* music sent, turn music on if it isn't already */
		if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
			writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
			return;
		}

		if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
			writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
			return;
		}

		REMOVE_BIT ( ch->comm, COMM_NOMUSIC );

		sprintf ( buf, "You MUSIC: '%s'\n\r", argument );
		writeBuffer ( buf, ch );
		sprintf ( buf, "$n MUSIC: '%s'", argument );
		for ( d = socket_list; d != NULL; d = d->next ) {
			Creature *victim;

			victim = d->original ? d->original : d->character;

			if ( d->connected == CON_PLAYING &&
					d->character != ch &&
					!IS_SET ( victim->comm, COMM_NOMUSIC ) &&
					!IS_SET ( victim->comm, COMM_QUIET ) ) {
				act_new ( "$n MUSIC: '$t'",
						  ch, argument, d->character, TO_VICT, POS_SLEEPING );
			}
		}
	}
}

/* clan channels */
DefineCommand ( cmd_clantalk )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( !is_clan ( ch ) || clan_table[ch->clan].independent ) {
		writeBuffer ( "You aren't in a clan.\n\r", ch );
		return;
	}
	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOCLAN ) ) {
			writeBuffer ( "Clan channel is now ON\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOCLAN );
		} else {
			writeBuffer ( "Clan channel is now OFF\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOCLAN );
		}
		return;
	}

	if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) ) {
		writeBuffer ( "The gods have revoked your channel priviliges.\n\r", ch );
		return;
	}

	REMOVE_BIT ( ch->comm, COMM_NOCLAN );

	sprintf ( buf, "You clan '%s'\n\r", argument );
	writeBuffer ( buf, ch );
	sprintf ( buf, "$n clans '%s'", argument );
	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->connected == CON_PLAYING &&
				d->character != ch &&
				is_same_clan ( ch, d->character ) &&
				!IS_SET ( d->character->comm, COMM_NOCLAN ) &&
				!IS_SET ( d->character->comm, COMM_QUIET ) ) {
			act_new ( "$n clans '$t'", ch, argument, d->character, TO_VICT, POS_DEAD );
		}
	}

	return;
}

DefineCommand ( cmd_immtalk )
{
	char buf[MAX_STRING_LENGTH];
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_NOWIZ ) ) {
			writeBuffer ( "Immortal channel is now ON\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_NOWIZ );
		} else {
			writeBuffer ( "Immortal channel is now OFF\n\r", ch );
			SET_BIT ( ch->comm, COMM_NOWIZ );
		}
		return;
	}

	REMOVE_BIT ( ch->comm, COMM_NOWIZ );

	sprintf ( buf, "$n: %s", argument );
	act_new ( "$n: $t", ch, argument, NULL, TO_CHAR, POS_DEAD );
	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->connected == CON_PLAYING &&
				IsStaff ( d->character ) &&
				!IS_SET ( d->character->comm, COMM_NOWIZ ) ) {
			act_new ( "$n: $t", ch, argument, d->character, TO_VICT, POS_DEAD );
		}
	}

	return;
}

DefineCommand ( cmd_say )
{
	if ( argument[0] == '\0' ) {
		writeBuffer ( "Say what?\n\r", ch );
		return;
	}

	act ( "$n says '$T'", ch, NULL, argument, TO_ROOM );
	act ( "You say '$T'", ch, NULL, argument, TO_CHAR );

	if ( !IS_NPC ( ch ) ) {
		Creature *mob, *mob_next;
		for ( mob = ch->in_room->people; mob != NULL; mob = mob_next ) {
			mob_next = mob->next_in_room;
			if ( IS_NPC ( mob ) && HAS_TRIGGER ( mob, TRIG_SPEECH )
					&&   mob->position == mob->pIndexData->default_pos )
			{ mp_act_trigger ( argument, mob, ch, NULL, NULL, TRIG_SPEECH ); }
		}
	}
	return;
}

DefineCommand ( cmd_shout )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		if ( IS_SET ( ch->comm, COMM_SHOUTSOFF ) ) {
			writeBuffer ( "You can hear shouts again.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_SHOUTSOFF );
		} else {
			writeBuffer ( "You will no longer hear shouts.\n\r", ch );
			SET_BIT ( ch->comm, COMM_SHOUTSOFF );
		}
		return;
	}

	if ( IS_SET ( ch->comm, COMM_NOSHOUT ) ) {
		writeBuffer ( "You can't shout.\n\r", ch );
		return;
	}

	REMOVE_BIT ( ch->comm, COMM_SHOUTSOFF );

	WAIT_STATE ( ch, 12 );

	act ( "You shout '$T'", ch, NULL, argument, TO_CHAR );
	for ( d = socket_list; d != NULL; d = d->next ) {
		Creature *victim;

		victim = d->original ? d->original : d->character;

		if ( d->connected == CON_PLAYING &&
				d->character != ch &&
				!IS_SET ( victim->comm, COMM_SHOUTSOFF ) &&
				!IS_SET ( victim->comm, COMM_QUIET ) ) {
			act ( "$n shouts '$t'", ch, argument, d->character, TO_VICT );
		}
	}

	return;
}

DefineCommand ( cmd_tell )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	if ( IS_SET ( ch->comm, COMM_NOTELL ) || IS_SET ( ch->comm, COMM_DEAF ) ) {
		writeBuffer ( "Your message didn't get through.\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->comm, COMM_QUIET ) ) {
		writeBuffer ( "You must turn off quiet mode first.\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->comm, COMM_DEAF ) ) {
		writeBuffer ( "You must turn off deaf mode first.\n\r", ch );
		return;
	}

	argument = one_argument ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "Tell whom what?\n\r", ch );
		return;
	}

	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
	 */
	if ( ( victim = get_char_world ( ch, arg ) ) == NULL
			|| ( IS_NPC ( victim ) && victim->in_room != ch->in_room ) ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL && !IS_NPC ( victim ) ) {
		act ( "$N seems to have misplaced $S link...try again later.",
			  ch, NULL, victim, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "%s tells you '%s'\n\r", PERS ( ch, victim ), argument );
		buf[0] = UPPER ( buf[0] );
		add_buf ( victim->pcdata->buffer, buf );
		return;
	}

	if ( ! ( IsStaff ( ch ) && ch->level > LEVEL_IMMORTAL ) && !IS_AWAKE ( victim ) ) {
		act ( "$E can't hear you.", ch, 0, victim, TO_CHAR );
		return;
	}

	if ( ( IS_SET ( victim->comm, COMM_QUIET ) || IS_SET ( victim->comm, COMM_DEAF ) )
			&& !IsStaff ( ch ) ) {
		act ( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_AFK ) ) {
		if ( IS_NPC ( victim ) ) {
			act ( "$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR );
			return;
		}

		act ( "$E is AFK, but your tell will go through when $E returns.",
			  ch, NULL, victim, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "%s tells you '%s'\n\r", PERS ( ch, victim ), argument );
		buf[0] = UPPER ( buf[0] );
		add_buf ( victim->pcdata->buffer, buf );
		return;
	}

	act ( "You tell $N '$t'", ch, argument, victim, TO_CHAR );
	act_new ( "$n tells you '$t'", ch, argument, victim, TO_VICT, POS_DEAD );
	victim->reply	= ch;

	if ( !IS_NPC ( ch ) && IS_NPC ( victim ) && HAS_TRIGGER ( victim, TRIG_SPEECH ) )
	{ mp_act_trigger ( argument, victim, ch, NULL, NULL, TRIG_SPEECH ); }

	return;
}

DefineCommand ( cmd_reply )
{
	Creature *victim;
	char buf[MAX_STRING_LENGTH];

	if ( IS_SET ( ch->comm, COMM_NOTELL ) ) {
		writeBuffer ( "Your message didn't get through.\n\r", ch );
		return;
	}

	if ( ( victim = ch->reply ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL && !IS_NPC ( victim ) ) {
		act ( "$N seems to have misplaced $S link...try again later.",
			  ch, NULL, victim, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "%s tells you '%s'\n\r", PERS ( ch, victim ), argument );
		buf[0] = UPPER ( buf[0] );
		add_buf ( victim->pcdata->buffer, buf );
		return;
	}

	if ( !IsStaff ( ch ) && !IS_AWAKE ( victim ) ) {
		act ( "$E can't hear you.", ch, 0, victim, TO_CHAR );
		return;
	}

	if ( ( IS_SET ( victim->comm, COMM_QUIET ) || IS_SET ( victim->comm, COMM_DEAF ) )
			&&  !IsStaff ( ch ) && !IsStaff ( victim ) ) {
		act_new ( "$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD );
		return;
	}

	if ( !IsStaff ( victim ) && !IS_AWAKE ( ch ) ) {
		writeBuffer ( "In your dreams, or what?\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_AFK ) ) {
		if ( IS_NPC ( victim ) ) {
			act_new ( "$E is AFK, and not receiving tells.",
					  ch, NULL, victim, TO_CHAR, POS_DEAD );
			return;
		}

		act_new ( "$E is AFK, but your tell will go through when $E returns.",
				  ch, NULL, victim, TO_CHAR, POS_DEAD );
		snprintf ( buf, sizeof ( buf ), "%s tells you '%s'\n\r", PERS ( ch, victim ), argument );
		buf[0] = UPPER ( buf[0] );
		add_buf ( victim->pcdata->buffer, buf );
		return;
	}

	act_new ( "You tell $N '$t'", ch, argument, victim, TO_CHAR, POS_DEAD );
	act_new ( "$n tells you '$t'", ch, argument, victim, TO_VICT, POS_DEAD );
	victim->reply	= ch;

	return;
}

DefineCommand ( cmd_yell )
{
	Socket *d;

	if ( IS_SET ( ch->comm, COMM_NOSHOUT ) ) {
		writeBuffer ( "You can't yell.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Yell what?\n\r", ch );
		return;
	}


	act ( "You yell '$t'", ch, argument, NULL, TO_CHAR );
	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->connected == CON_PLAYING
				&&   d->character != ch
				&&   d->character->in_room != NULL
				&&   d->character->in_room->area == ch->in_room->area
				&&   !IS_SET ( d->character->comm, COMM_QUIET ) ) {
			act ( "$n yells '$t'", ch, argument, d->character, TO_VICT );
		}
	}

	return;
}


DefineCommand ( cmd_emote )
{
	if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) ) {
		writeBuffer ( "You can't show your emotions.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Emote what?\n\r", ch );
		return;
	}

	MOBtrigger = FALSE;
	act ( "$n $T", ch, NULL, argument, TO_ROOM );
	act ( "$n $T", ch, NULL, argument, TO_CHAR );
	MOBtrigger = TRUE;
	return;
}

DefineCommand ( cmd_pmote )
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

	act ( "$n $t", ch, argument, NULL, TO_CHAR );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room ) {
		if ( vch->desc == NULL || vch == ch )
		{ continue; }

		if ( ( letter = str_str ( argument, vch->name ) ) == NULL ) {
			MOBtrigger = FALSE;
			act ( "$N $t", vch, argument, ch, TO_CHAR );
			MOBtrigger = TRUE;
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

		MOBtrigger = FALSE;
		act ( "$N $t", vch, temp, ch, TO_CHAR );
		MOBtrigger = TRUE;
	}

	return;
}


/*
 * All the posing stuff.
 */
struct	pose_table_type {
	const char *	message[2 * MAX_CLASS];
};

const	struct	pose_table_type	pose_table	[]	= {
	{
		{
			"You sizzle with energy.",
			"$n sizzles with energy.",
			"You feel very holy.",
			"$n looks very holy.",
			"You perform a small card trick.",
			"$n performs a small card trick.",
			"You show your bulging muscles.",
			"$n shows $s bulging muscles."
		}
	},

	{
		{
			"You turn into a butterfly, then return to your normal shape.",
			"$n turns into a butterfly, then returns to $s normal shape.",
			"You nonchalantly turn wine into water.",
			"$n nonchalantly turns wine into water.",
			"You wiggle your ears alternately.",
			"$n wiggles $s ears alternately.",
			"You crack nuts between your fingers.",
			"$n cracks nuts between $s fingers."
		}
	},

	{
		{
			"Blue sparks fly from your fingers.",
			"Blue sparks fly from $n's fingers.",
			"A halo appears over your head.",
			"A halo appears over $n's head.",
			"You nimbly tie yourself into a knot.",
			"$n nimbly ties $mself into a knot.",
			"You grizzle your teeth and look mean.",
			"$n grizzles $s teeth and looks mean."
		}
	},

	{
		{
			"Little red lights dance in your eyes.",
			"Little red lights dance in $n's eyes.",
			"You recite words of wisdom.",
			"$n recites words of wisdom.",
			"You juggle with daggers, apples, and eyeballs.",
			"$n juggles with daggers, apples, and eyeballs.",
			"You hit your head, and your eyes roll.",
			"$n hits $s head, and $s eyes roll."
		}
	},

	{
		{
			"A slimy green monster appears before you and bows.",
			"A slimy green monster appears before $n and bows.",
			"Deep in prayer, you levitate.",
			"Deep in prayer, $n levitates.",
			"You steal the underwear off every person in the room.",
			"Your underwear is gone!  $n stole it!",
			"Crunch, crunch -- you munch a bottle.",
			"Crunch, crunch -- $n munches a bottle."
		}
	},

	{
		{
			"You turn everybody into a little pink elephant.",
			"You are turned into a little pink elephant by $n.",
			"An angel consults you.",
			"An angel consults $n.",
			"The dice roll ... and you win again.",
			"The dice roll ... and $n wins again.",
			"... 98, 99, 100 ... you do pushups.",
			"... 98, 99, 100 ... $n does pushups."
		}
	},

	{
		{
			"A small ball of light dances on your fingertips.",
			"A small ball of light dances on $n's fingertips.",
			"Your body glows with an unearthly light.",
			"$n's body glows with an unearthly light.",
			"You count the money in everyone's pockets.",
			"Check your money, $n is counting it.",
			"Arnold Schwarzenegger admires your physique.",
			"Arnold Schwarzenegger admires $n's physique."
		}
	},

	{
		{
			"Smoke and fumes leak from your nostrils.",
			"Smoke and fumes leak from $n's nostrils.",
			"A spot light hits you.",
			"A spot light hits $n.",
			"You balance a pocket knife on your tongue.",
			"$n balances a pocket knife on your tongue.",
			"Watch your feet, you are juggling granite boulders.",
			"Watch your feet, $n is juggling granite boulders."
		}
	},

	{
		{
			"The light flickers as you rap in magical languages.",
			"The light flickers as $n raps in magical languages.",
			"Everyone levitates as you pray.",
			"You levitate as $n prays.",
			"You produce a coin from everyone's ear.",
			"$n produces a coin from your ear.",
			"Oomph!  You squeeze water out of a granite boulder.",
			"Oomph!  $n squeezes water out of a granite boulder."
		}
	},

	{
		{
			"Your head disappears.",
			"$n's head disappears.",
			"A cool breeze refreshes you.",
			"A cool breeze refreshes $n.",
			"You step behind your shadow.",
			"$n steps behind $s shadow.",
			"You pick your teeth with a spear.",
			"$n picks $s teeth with a spear."
		}
	},

	{
		{
			"A fire elemental singes your hair.",
			"A fire elemental singes $n's hair.",
			"The sun pierces through the clouds to illuminate you.",
			"The sun pierces through the clouds to illuminate $n.",
			"Your eyes dance with greed.",
			"$n's eyes dance with greed.",
			"Everyone is swept off their foot by your hug.",
			"You are swept off your feet by $n's hug."
		}
	},

	{
		{
			"The sky changes color to match your eyes.",
			"The sky changes color to match $n's eyes.",
			"The ocean parts before you.",
			"The ocean parts before $n.",
			"You deftly steal everyone's weapon.",
			"$n deftly steals your weapon.",
			"Your karate chop splits a tree.",
			"$n's karate chop splits a tree."
		}
	},

	{
		{
			"The stones dance to your command.",
			"The stones dance to $n's command.",
			"A thunder cloud kneels to you.",
			"A thunder cloud kneels to $n.",
			"The Grey Mouser buys you a beer.",
			"The Grey Mouser buys $n a beer.",
			"A strap of your armor breaks over your mighty thews.",
			"A strap of $n's armor breaks over $s mighty thews."
		}
	},

	{
		{
			"The heavens and grass change colour as you smile.",
			"The heavens and grass change colour as $n smiles.",
			"The Burning Man speaks to you.",
			"The Burning Man speaks to $n.",
			"Everyone's pocket explodes with your fireworks.",
			"Your pocket explodes with $n's fireworks.",
			"A boulder cracks at your frown.",
			"A boulder cracks at $n's frown."
		}
	},

	{
		{
			"Everyone's clothes are transparent, and you are laughing.",
			"Your clothes are transparent, and $n is laughing.",
			"An eye in a pyramid winks at you.",
			"An eye in a pyramid winks at $n.",
			"Everyone discovers your dagger a centimeter from their eye.",
			"You discover $n's dagger a centimeter from your eye.",
			"Mercenaries arrive to do your bidding.",
			"Mercenaries arrive to do $n's bidding."
		}
	},

	{
		{
			"A black hole swallows you.",
			"A black hole swallows $n.",
			"Valentine Michael Smith offers you a glass of water.",
			"Valentine Michael Smith offers $n a glass of water.",
			"Where did you go?",
			"Where did $n go?",
			"Four matched Percherons bring in your chariot.",
			"Four matched Percherons bring in $n's chariot."
		}
	},

	{
		{
			"The world shimmers in time with your whistling.",
			"The world shimmers in time with $n's whistling.",
			"The great god Mota gives you a staff.",
			"The great god Mota gives $n a staff.",
			"Click.",
			"Click.",
			"Atlas asks you to relieve him.",
			"Atlas asks $n to relieve him."
		}
	}
};



DefineCommand ( cmd_pose )
{
	int level;
	int pose;

	if ( IS_NPC ( ch ) )
	{ return; }

	level = UMIN ( ch->level, ( signed int ) sizeof ( pose_table ) / ( signed int ) sizeof ( pose_table[0] ) - 1 );
	pose  = number_range ( 0, level );

	act ( pose_table[pose].message[2 * ch->archetype + 0], ch, NULL, NULL, TO_CHAR );
	act ( pose_table[pose].message[2 * ch->archetype + 1], ch, NULL, NULL, TO_ROOM );

	return;
}

DefineCommand ( cmd_rent )
{
	writeBuffer ( "There is currently being rewritten, use 'save' instead.\n\r", ch );
	return;
}


DefineCommand ( cmd_qui )
{
	writeBuffer ( "If you want to QUIT, you have to spell it out.\n\r", ch );
	return;
}

DefineCommand ( cmd_quit )
{
	Socket *d, *d_next;
	int id;

	if(!str_cmp(argument, "delete")) {
		writeBuffer ("Alas, all good things must come to an end.\n\r", ch );
		act ( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
		log_hd ( LOG_SECURITY,  Format ( "%s has quit.", ch->name ) );

		wiznet ( "$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0, get_trust ( ch ) );

		save_char_obj ( ch );
		id = ch->id;
		d = ch->desc;
		extract_char ( ch, TRUE );
		if ( d != NULL )
		{ close_socket ( d ); }

		/* toast evil cheating bastards */
		for ( d = socket_list; d != NULL; d = d_next ) {
			Creature *tch;

			d_next = d->next;
			tch = d->original ? d->original : d->character;
			if ( tch && tch->id == id ) {
				extract_char ( tch, TRUE );
				close_socket ( d );
			}
		}
		return;
	}

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( ch->position == POS_FIGHTING ) {
		writeBuffer ( "No way! You are fighting.\n\r", ch );
		return;
	}

	if ( ch->position  < POS_STUNNED  ) {
		writeBuffer ( "You're not DEAD yet.\n\r", ch );
		return;
	}

	if(cmd == 1001) {
		switch(argument[0]) {
		default:
			ch->queries.querycommand = 0;
			return;
		case 'y':
		case 'Y':
			writeBuffer ("Alas, all good things must come to an end.\n\r", ch );
			act ( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
			log_hd ( LOG_SECURITY,  Format ( "%s has quit.", ch->name ) );

			wiznet ( "$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0, get_trust ( ch ) );

			save_char_obj ( ch );
			id = ch->id;
			d = ch->desc;
			extract_char ( ch, TRUE );
			if ( d != NULL )
			{ close_socket ( d ); }

			/* toast evil cheating bastards */
			for ( d = socket_list; d != NULL; d = d_next ) {
				Creature *tch;

				d_next = d->next;
				tch = d->original ? d->original : d->character;
				if ( tch && tch->id == id ) {
					extract_char ( tch, TRUE );
					close_socket ( d );
				}
			}
			return; // -- end case y
		}
	}
	// -- set up the query prompt
	ch->queries.queryfunc = cmd_quit;
	strcpy(ch->queries.queryprompt, "Are you sure you want to quit? (Y/n)");
	ch->queries.querycommand = 1001;
	return;
}

DefineCommand ( cmd_save )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	save_char_obj ( ch );
	writeBuffer ( "Saved to disk.\n\r", ch );

	WAIT_STATE ( ch, 2 * PULSE_VIOLENCE );
	return;
}

DefineCommand ( cmd_follow )
{
	/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Follow whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL ) {
		act ( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
		return;
	}

	if ( victim == ch ) {
		if ( ch->master == NULL ) {
			writeBuffer ( "You already follow yourself.\n\r", ch );
			return;
		}
		stop_follower ( ch );
		return;
	}

	if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_NOFOLLOW ) && !IsStaff ( ch ) ) {
		act ( "$N doesn't seem to want any followers.\n\r",
			  ch, NULL, victim, TO_CHAR );
		return;
	}

	REMOVE_BIT ( ch->act, PLR_NOFOLLOW );

	if ( ch->master != NULL )
	{ stop_follower ( ch ); }

	add_follower ( ch, victim );
	return;
}


void add_follower ( Creature *ch, Creature *master )
{
	if ( ch->master != NULL ) {
		log_hd ( LOG_ERROR, "Add_follower: non-null master." );
		return;
	}

	ch->master        = master;
	ch->leader        = NULL;

	if ( can_see ( master, ch ) )
	{ act ( "$n now follows you.", ch, NULL, master, TO_VICT ); }

	act ( "You now follow $N.",  ch, NULL, master, TO_CHAR );

	return;
}



void stop_follower ( Creature *ch )
{
	if ( ch->master == NULL ) {
		log_hd ( LOG_ERROR, "Stop_follower: null master." );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) ) {
		REMOVE_BIT ( ch->affected_by, AFF_CHARM );
		affect_strip ( ch, gsn_charm_person );
	}

	if ( can_see ( ch->master, ch ) && ch->in_room != NULL ) {
		act ( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
		act ( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
	}
	if ( ch->master->pet == ch )
	{ ch->master->pet = NULL; }

	ch->master = NULL;
	ch->leader = NULL;
	return;
}

/* nukes charmed monsters and pets */
void nuke_pets ( Creature *ch )
{
	Creature *pet;

	if ( ( pet = ch->pet ) != NULL ) {
		stop_follower ( pet );
		if ( pet->in_room != NULL )
		{ act ( "$N slowly fades away.", ch, NULL, pet, TO_NOTVICT ); }
		extract_char ( pet, TRUE );
	}
	ch->pet = NULL;

	return;
}



void die_follower ( Creature *ch )
{
	Creature *fch;

	if ( ch->master != NULL ) {
		if ( ch->master->pet == ch )
		{ ch->master->pet = NULL; }
		stop_follower ( ch );
	}

	ch->leader = NULL;

	for ( fch = char_list; fch != NULL; fch = fch->next ) {
		if ( fch->master == ch )
		{ stop_follower ( fch ); }
		if ( fch->leader == ch )
		{ fch->leader = fch; }
	}

	return;
}

DefineCommand ( cmd_order )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	Creature *victim;
	Creature *och;
	Creature *och_next;
	bool found;
	bool fAll;

	argument = one_argument ( argument, arg );
	one_argument ( argument, arg2 );

	if ( !str_cmp ( arg2, "delete" ) || !str_cmp ( arg2, "mob" ) ) {
		writeBuffer ( "That will NOT be done.\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "Order whom to do what?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) ) {
		writeBuffer ( "You feel like taking, not giving, orders.\n\r", ch );
		return;
	}

	if ( !str_cmp ( arg, "all" ) ) {
		fAll   = TRUE;
		victim = NULL;
	} else {
		fAll   = FALSE;
		if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		if ( victim == ch ) {
			writeBuffer ( "Aye aye, right away!\n\r", ch );
			return;
		}

		if ( !IS_AFFECTED ( victim, AFF_CHARM ) || victim->master != ch
				||  ( IsStaff ( victim ) && victim->trust >= ch->trust ) ) {
			writeBuffer ( "Do it yourself!\n\r", ch );
			return;
		}
	}

	found = FALSE;
	for ( och = ch->in_room->people; och != NULL; och = och_next ) {
		och_next = och->next_in_room;

		if ( IS_AFFECTED ( och, AFF_CHARM )
				&&   och->master == ch
				&& ( fAll || och == victim ) ) {
			found = TRUE;
			sprintf ( buf, "$n orders you to '%s'.", argument );
			act ( buf, ch, NULL, och, TO_VICT );
			interpret ( och, argument );
		}
	}

	if ( found ) {
		WAIT_STATE ( ch, PULSE_VIOLENCE );
		writeBuffer ( "Ok.\n\r", ch );
	} else
	{ writeBuffer ( "You have no followers here.\n\r", ch ); }
	return;
}

DefineCommand ( cmd_group )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	one_argument ( argument, arg );

	if ( arg[0] == '\0' ) {
		Creature *gch;
		Creature *leader;

		leader = ( ch->leader != NULL ) ? ch->leader : ch;
		sprintf ( buf, "%s's group:\n\r", PERS ( leader, ch ) );
		writeBuffer ( buf, ch );

		for ( gch = char_list; gch != NULL; gch = gch->next ) {
			if ( is_same_group ( gch, ch ) ) {
				sprintf ( buf,
						  "[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
						  gch->level,
						  IS_NPC ( gch ) ? "Mob" : archetype_table[gch->archetype].who_name,
						  capitalize ( PERS ( gch, ch ) ),
						  gch->hit,   gch->max_hit,
						  gch->mana,  gch->max_mana,
						  gch->move,  gch->max_move,
						  gch->exp    );
				writeBuffer ( buf, ch );
			}
		}
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) ) {
		writeBuffer ( "But you are following someone else!\n\r", ch );
		return;
	}

	if ( victim->master != ch && ch != victim ) {
		act_new ( "$N isn't following you.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
		return;
	}

	if ( IS_AFFECTED ( victim, AFF_CHARM ) ) {
		writeBuffer ( "You can't remove charmed mobs from your group.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED ( ch, AFF_CHARM ) ) {
		act_new ( "You like your master too much to leave $m!",
				  ch, NULL, victim, TO_VICT, POS_SLEEPING );
		return;
	}

	if ( is_same_group ( victim, ch ) && ch != victim ) {
		victim->leader = NULL;
		act_new ( "$n removes $N from $s group.",
				  ch, NULL, victim, TO_NOTVICT, POS_RESTING );
		act_new ( "$n removes you from $s group.",
				  ch, NULL, victim, TO_VICT, POS_SLEEPING );
		act_new ( "You remove $N from your group.",
				  ch, NULL, victim, TO_CHAR, POS_SLEEPING );
		return;
	}

	victim->leader = ch;
	act_new ( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
	act_new ( "You join $n's group.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
	act_new ( "$N joins your group.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
	return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
DefineCommand ( cmd_split )
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	Creature *gch;
	int members;
	int amount_gold = 0, amount_silver = 0;
	int share_gold, share_silver;
	int extra_gold, extra_silver;

	argument = one_argument ( argument, arg1 );
	one_argument ( argument, arg2 );

	if ( arg1[0] == '\0' ) {
		writeBuffer ( "Split how much?\n\r", ch );
		return;
	}

	amount_silver = atoi ( arg1 );

	if ( arg2[0] != '\0' )
	{ amount_gold = atoi ( arg2 ); }

	if ( amount_gold < 0 || amount_silver < 0 ) {
		writeBuffer ( "Your group wouldn't like that.\n\r", ch );
		return;
	}

	if ( amount_gold == 0 && amount_silver == 0 ) {
		writeBuffer ( "You hand out zero coins, but no one notices.\n\r", ch );
		return;
	}

	if ( ch->gold <  amount_gold || ch->silver < amount_silver ) {
		writeBuffer ( "You don't have that much to split.\n\r", ch );
		return;
	}

	members = 0;
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) {
		if ( is_same_group ( gch, ch ) && !IS_AFFECTED ( gch, AFF_CHARM ) )
		{ members++; }
	}

	if ( members < 2 ) {
		writeBuffer ( "Just keep it all.\n\r", ch );
		return;
	}

	share_silver = amount_silver / members;
	extra_silver = amount_silver % members;

	share_gold   = amount_gold / members;
	extra_gold   = amount_gold % members;

	if ( share_gold == 0 && share_silver == 0 ) {
		writeBuffer ( "Don't even bother, cheapskate.\n\r", ch );
		return;
	}

	ch->silver	-= amount_silver;
	ch->silver	+= share_silver + extra_silver;
	ch->gold 	-= amount_gold;
	ch->gold 	+= share_gold + extra_gold;

	if ( share_silver > 0 ) {
		snprintf ( buf, sizeof ( buf ),
				   "You split %d silver coins. Your share is %d silver.\n\r",
				   amount_silver, share_silver + extra_silver );
		writeBuffer ( buf, ch );
	}

	if ( share_gold > 0 ) {
		snprintf ( buf, sizeof ( buf ),
				   "You split %d gold coins. Your share is %d gold.\n\r",
				   amount_gold, share_gold + extra_gold );
		writeBuffer ( buf, ch );
	}

	if ( share_gold == 0 ) {
		snprintf ( buf, sizeof ( buf ), "$n splits %d silver coins. Your share is %d silver.",
				   amount_silver, share_silver );
	} else if ( share_silver == 0 ) {
		snprintf ( buf, sizeof ( buf ), "$n splits %d gold coins. Your share is %d gold.",
				   amount_gold, share_gold );
	} else {
		snprintf ( buf, sizeof ( buf ),
				   "$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
				   amount_silver, amount_gold, share_silver, share_gold );
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room ) {
		if ( gch != ch && is_same_group ( gch, ch ) && !IS_AFFECTED ( gch, AFF_CHARM ) ) {
			act ( buf, ch, NULL, gch, TO_VICT );
			gch->gold += share_gold;
			gch->silver += share_silver;
		}
	}

	return;
}

DefineCommand ( cmd_gtell )
{
	Creature *gch;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Tell your group what?\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->comm, COMM_NOTELL ) ) {
		writeBuffer ( "Your message didn't get through!\n\r", ch );
		return;
	}

	for ( gch = char_list; gch != NULL; gch = gch->next ) {
		if ( is_same_group ( gch, ch ) )
			act_new ( "$n tells the group '$t'",
					  ch, argument, gch, TO_VICT, POS_SLEEPING );
	}

	return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group ( Creature *ach, Creature *bch )
{
	if ( ach == NULL || bch == NULL )
	{ return FALSE; }

	if ( ach->leader != NULL ) { ach = ach->leader; }
	if ( bch->leader != NULL ) { bch = bch->leader; }
	return ach == bch;
}

// -- EOF

