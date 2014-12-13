#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_unalias )
{
	Creature *rch;
	char arg[MAX_INPUT_LENGTH];
	int pos;
	bool found = FALSE;

	if ( ch->desc == NULL )
	{ rch = ch; }
	else
	{ rch = ch->desc->original ? ch->desc->original : ch; }

	if ( IS_NPC ( rch ) )
	{ return; }

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Unalias what?\n\r", ch );
		return;
	}

	for ( pos = 0; pos < MAX_ALIAS; pos++ ) {
		if ( rch->pcdata->alias[pos] == NULL )
		{ break; }

		if ( found ) {
			rch->pcdata->alias[pos - 1]		= rch->pcdata->alias[pos];
			rch->pcdata->alias_sub[pos - 1]	= rch->pcdata->alias_sub[pos];
			rch->pcdata->alias[pos]		= NULL;
			rch->pcdata->alias_sub[pos]		= NULL;
			continue;
		}

		if ( !strcmp ( arg, rch->pcdata->alias[pos] ) ) {
			writeBuffer ( "Alias removed.\n\r", ch );
			PURGE_DATA ( rch->pcdata->alias[pos] );
			PURGE_DATA ( rch->pcdata->alias_sub[pos] );
			rch->pcdata->alias[pos] = NULL;
			rch->pcdata->alias_sub[pos] = NULL;
			found = TRUE;
		}
	}

	if ( !found )
	{ writeBuffer ( "No alias of that name to remove.\n\r", ch ); }
}

