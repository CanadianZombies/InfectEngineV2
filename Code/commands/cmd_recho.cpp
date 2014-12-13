#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_recho )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Local echo what?\n\r", ch );

		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == STATE_PLAYING
				&&   d->character->in_room == IN_ROOM ( ch ) ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "local> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r",   d->character );
		}
	}

	return;
}
