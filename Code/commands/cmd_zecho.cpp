#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_zecho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Zone echo what?\n\r", ch );
		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == STATE_PLAYING
				&&  d->character->in_room != NULL && IN_ROOM ( ch ) != NULL
				&&  d->character->in_room->area == IN_ROOM ( ch )->area ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "zone> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r", d->character );
		}
	}
}

