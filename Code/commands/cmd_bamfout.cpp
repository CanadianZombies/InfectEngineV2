#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_bamfout )
{
	char buf[MAX_STRING_LENGTH];

	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			snprintf ( buf, sizeof ( buf ), "Your poofout is %s\n\r", ch->pcdata->bamfout );
			writeBuffer ( buf, ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		PURGE_DATA ( ch->pcdata->bamfout );
		ch->pcdata->bamfout = assign_string ( argument );

		snprintf ( buf, sizeof ( buf ), "Your poofout is now %s\n\r", ch->pcdata->bamfout );
		writeBuffer ( buf, ch );
	}
	return;
}

