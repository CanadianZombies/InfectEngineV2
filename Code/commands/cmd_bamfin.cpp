#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_bamfin )
{
	char buf[MAX_STRING_LENGTH];

	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			snprintf ( buf, sizeof ( buf ), "Your poofin is %s\n\r", ch->pcdata->bamfin );
			writeBuffer ( buf, ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		PURGE_DATA ( ch->pcdata->bamfin );
		ch->pcdata->bamfin = assign_string ( argument );

		snprintf ( buf, sizeof ( buf ), "Your poofin is now %s\n\r", ch->pcdata->bamfin );
		writeBuffer ( buf, ch );
	}
	return;
}

