#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_ofind )
{
	extern int top_obj_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	ItemData *pObjIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Find what?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* SameString( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_obj_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for ( vnum = 0; nMatch < top_obj_index; vnum++ ) {
		if ( ( pObjIndex = get_obj_index ( vnum ) ) != NULL ) {
			nMatch++;
			if ( fAll || is_name ( argument, pObjIndex->name ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %s\n\r",
						  pObjIndex->vnum, pObjIndex->short_descr );
				writeBuffer ( buf, ch );
			}
		}
	}

	if ( !found )
	{ writeBuffer ( "No objects by that name.\n\r", ch ); }

	return;
}

