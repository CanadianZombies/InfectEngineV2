#include "Copyright.h"
#include "Engine.h"

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

