#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_disconnect )
{
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	Creature *victim;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		if ( ch ) {
			writeBuffer ( "Disconnect whom?\n\r", ch );
		}
		return;
	}

	if ( is_number ( arg ) ) {
		int desc;

		desc = atoi ( arg );
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->descriptor == desc ) {
				close_socket ( d );
				if ( ch ) {
					writeBuffer ( Format ( "Ok, %d has been disconnected.\n\r", desc ), ch );
				}
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
	if ( ch ) {
		writeBuffer ( "Descriptor not found!\n\r", ch );
	}
	return;
}
