#include "Copyright.h"
#include "Engine.h"

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

