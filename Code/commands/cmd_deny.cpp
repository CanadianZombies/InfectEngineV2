#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_deny )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Deny whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	SET_BIT ( victim->act, PLR_DENY );
	writeBuffer ( "You are denied access!\n\r", victim );
	snprintf ( buf, sizeof ( buf ), "$N denies access to %s", victim->name );
	wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	writeBuffer ( "OK.\n\r", ch );
	save_char_obj ( victim );
	stop_fighting ( victim, TRUE );
	cmd_function ( victim, &cmd_quit, "" );

	return;
}

