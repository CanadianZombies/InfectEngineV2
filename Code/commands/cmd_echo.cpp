#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_echo )
{
	Socket *d;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Global echo what?\n\r", ch );
		return;
	}

	for ( d = socket_list; d; d = d->next ) {
		if ( d->connected == STATE_PLAYING ) {
			if ( get_trust ( d->character ) >= get_trust ( ch ) )
			{ writeBuffer ( "global> ", d->character ); }
			writeBuffer ( argument, d->character );
			writeBuffer ( "\n\r",   d->character );
		}
	}

	return;
}
