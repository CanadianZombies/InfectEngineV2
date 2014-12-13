#include "Copyright.h"
#include "Engine.h"


/* RT to replace the 3 stat commands */
DefineCommand ( cmd_stat )
{
	char arg[MAX_INPUT_LENGTH];
	char *string;
	Item *obj;
	RoomData *location;
	Creature *victim;

	string = ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  stat <name>\n\r", ch );
		writeBuffer ( "  stat obj <name>\n\r", ch );
		writeBuffer ( "  stat mob <name>\n\r", ch );
		writeBuffer ( "  stat room <number>\n\r", ch );
		return;
	}

	if ( SameString ( arg, "room" ) ) {
		cmd_function ( ch, &cmd_rstat, string );
		return;
	}

	if ( SameString ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_ostat, string );
		return;
	}

	if ( SameString ( arg, "char" )  || SameString ( arg, "mob" ) ) {
		cmd_function ( ch, &cmd_mstat, string );
		return;
	}

	/* do it the old way */

	obj = get_obj_world ( ch, argument );
	if ( obj != NULL ) {
		cmd_function ( ch, &cmd_ostat, argument );
		return;
	}

	victim = get_char_world ( ch, argument );
	if ( victim != NULL ) {
		cmd_function ( ch, &cmd_mstat, argument );
		return;
	}

	location = find_location ( ch, argument );
	if ( location != NULL ) {
		cmd_function ( ch, &cmd_rstat, argument );
		return;
	}

	writeBuffer ( "Nothing by that name found anywhere.\n\r", ch );
}

