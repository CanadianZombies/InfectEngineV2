#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_guild )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	int clan;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Syntax: guild <char> <cln name>\n\r", ch );
		return;
	}
	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't playing.\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg2, "none" ) ) {
		writeBuffer ( "They are now clanless.\n\r", ch );
		writeBuffer ( "You are now a member of no clan!\n\r", victim );
		victim->clan = 0;
		return;
	}

	if ( ( clan = clan_lookup ( arg2 ) ) == 0 ) {
		writeBuffer ( "No such clan exists.\n\r", ch );
		return;
	}

	if ( clan_table[clan].independent ) {
		snprintf ( buf, sizeof ( buf ), "They are now a %s.\n\r", clan_table[clan].name );
		writeBuffer ( buf, ch );
		snprintf ( buf, sizeof ( buf ), "You are now a %s.\n\r", clan_table[clan].name );
		writeBuffer ( buf, victim );
	} else {
		snprintf ( buf, sizeof ( buf ), "They are now a member of clan %s.\n\r",
				   capitalize ( clan_table[clan].name ) );
		writeBuffer ( buf, ch );
		snprintf ( buf, sizeof ( buf ), "You are now a member of clan %s.\n\r",
				   capitalize ( clan_table[clan].name ) );
	}

	victim->clan = clan;
}

