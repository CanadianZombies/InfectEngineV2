#include "Copyright.h"
#include "Engine.h"

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

	if ( SameString ( arg1, "all" ) ) {
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->connected == STATE_PLAYING
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
