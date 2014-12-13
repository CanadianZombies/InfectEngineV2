#include "Copyright.h"
#include "Engine.h"

/* ofind and mfind replaced with vnum, vnum skill also added */
DefineCommand ( cmd_vnum )
{
	char arg[MAX_INPUT_LENGTH];
	char *string;

	string = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  vnum obj <name>\n\r", ch );
		writeBuffer ( "  vnum mob <name>\n\r", ch );
		writeBuffer ( "  vnum skill <skill or spell>\n\r", ch );
		return;
	}

	if ( SameString ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_ofind, string );
		return;
	}

	if ( SameString ( arg, "mob" ) || SameString ( arg, "char" ) ) {
		cmd_function ( ch, &cmd_mfind, string );
		return;
	}

	if ( SameString ( arg, "skill" ) || SameString ( arg, "spell" ) ) {
		cmd_function ( ch, &cmd_slookup, string );
		return;
	}
	/* do both */
	cmd_function ( ch, &cmd_mfind, argument );
	cmd_function ( ch, &cmd_ofind, argument );
}

