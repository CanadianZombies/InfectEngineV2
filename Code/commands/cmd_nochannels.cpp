#include "Copyright.h"
#include "Engine.h"

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
