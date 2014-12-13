#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_makestaff )
{
	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: /makestaff <username> <flag> <on|off>\n\r", ch );
		writeBuffer ( "Flags: staff, builder, relations, security, coder\n\r", ch );
		return;
	}

	char first[1000];
	char second[1000];

	argument = ChopC ( argument, first );
	argument = ChopC ( argument, second );


	if ( IS_NULLSTR ( first ) || IS_NULLSTR ( second ) || IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: /makestaff <username> <flag> <on|off>\n\r", ch );
		return;
	}

	if ( !SameString ( argument, "on" ) && !SameString ( argument, "off" ) ) {
		writeBuffer ( "Toggle requires on or off.\n\r", ch );
		return;
	}

	bool onOff = false;
	if ( SameString ( argument, "on" ) )
	{ onOff = true; }

	Creature *c, *cn;

	for ( c = char_list; c; c = cn ) {
		cn = c->next;

		// -- skip NPC's
		if ( IS_NPC ( c ) ) { continue; }

		if ( SameString ( c->name, first ) ) {
			if ( c == ch && SameString ( c->name, "Omega" ) ) {
				writeBuffer ( "You cannot flag yourself with staff flags!\n\r", ch );
				return;
			}

			if ( SameString ( second, "staff" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_STAFF ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_STAFF ); }
				writeBuffer ( "Staff flag toggled.\n\r", ch );
				return;
			}

			if ( SameString ( second, "security" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_SECURITY ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_SECURITY ); }
				writeBuffer ( "Vanguard flag toggled.\n\r", ch );
				return;
			}

			if ( SameString ( second, "relations" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_RELATIONS ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_RELATIONS ); }
				writeBuffer ( "Player Relations flag toggled.\n\r", ch );
				return;
			}

			if ( SameString ( second, "builder" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_BUILDER ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_BUILDER ); }
				writeBuffer ( "Builder flag toggled.\n\r", ch );
				return;
			}

			if ( SameString ( second, "coder" ) ) {
				if ( onOff == true )
				{ SET_BIT ( c->sflag, CR_CODER ); }
				else
				{ REMOVE_BIT ( c->sflag, CR_CODER ); }
				writeBuffer ( "Coder flag toggled.\n\r", ch );
				return;
			}

			writeBuffer ( "Unknown flag option!\n\r", ch );
			return;
		}
	}
	writeBuffer ( "We couldn't find the player you wanted to adjust\n\r", ch );
	return;
}
