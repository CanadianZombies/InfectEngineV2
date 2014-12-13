#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_pardon )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Creature *victim;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( SameString ( arg2, "killer" ) ) {
		if ( IS_SET ( victim->act, PLR_KILLER ) ) {
			REMOVE_BIT ( victim->act, PLR_KILLER );
			writeBuffer ( "Killer flag removed.\n\r", ch );
			writeBuffer ( "You are no longer a KILLER.\n\r", victim );
		}
		return;
	}

	if ( SameString ( arg2, "thief" ) ) {
		if ( IS_SET ( victim->act, PLR_THIEF ) ) {
			REMOVE_BIT ( victim->act, PLR_THIEF );
			writeBuffer ( "Thief flag removed.\n\r", ch );
			writeBuffer ( "You are no longer a THIEF.\n\r", victim );
		}
		return;
	}

	writeBuffer ( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	return;
}

