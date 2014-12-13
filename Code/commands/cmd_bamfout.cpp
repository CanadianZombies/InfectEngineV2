#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_bamfout )
{
	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			writeBuffer(Format("Your poofout is %s\n\r", ch->pcdata->bamfout), ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		delete [] ch->pcdata->bamfout;
		ch->pcdata->bamfout = assign_string ( argument );

		writeBuffer(Format("Your poofout is now %s\n\r", ch->pcdata->bamfout), ch );
	}
	return;
}

