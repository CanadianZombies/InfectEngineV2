#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_mfind )
{
	extern int top_mob_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	NPCData *pMobIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Find whom?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* SameString( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_mob_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for ( vnum = 0; nMatch < top_mob_index; vnum++ ) {
		if ( ( pMobIndex = get_mob_index ( vnum ) ) != NULL ) {
			nMatch++;
			if ( fAll || is_name ( argument, pMobIndex->player_name ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %s\n\r",
						  pMobIndex->vnum, pMobIndex->short_descr );
				writeBuffer ( buf, ch );
			}
		}
	}

	if ( !found )
	{ writeBuffer ( "No mobiles by that name.\n\r", ch ); }

	return;
}
