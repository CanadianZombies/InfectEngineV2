#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_bamfin )
{
	if ( !IS_NPC ( ch ) ) {
		smash_tilde ( argument );

		if ( argument[0] == '\0' ) {
			writeBuffer ( Format( "Your poofin is %s\n\r", ch->pcdata->bamfin ), ch );
			return;
		}

		if ( str_str ( argument, ch->name ) == NULL ) {
			writeBuffer ( "You must include your name.\n\r", ch );
			return;
		}

		delete [] ch->pcdata->bamfin;
		ch->pcdata->bamfin = assign_string ( argument );

		writeBuffer(Format("Your poofin is now %s\n\r", ch->pcdata->bamfin), ch );
	}
	return;
}

