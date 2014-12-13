#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_pecho )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	argument = ChopC ( argument, arg );

	if ( argument[0] == '\0' || arg[0] == '\0' ) {
		writeBuffer ( "Personal echo what?\n\r", ch );
		return;
	}

	if  ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "Target not found.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) && get_trust ( ch ) != MAX_LEVEL )
	{ writeBuffer ( "personal> ", victim ); }

	writeBuffer ( argument, victim );
	writeBuffer ( "\n\r", victim );
	writeBuffer ( "personal> ", ch );
	writeBuffer ( argument, ch );
	writeBuffer ( "\n\r", ch );
}

