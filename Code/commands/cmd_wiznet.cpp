#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_wiznet )
{
	int flag;
	char buf[MAX_STRING_LENGTH];

	if ( IS_NULLSTR ( argument ) ) {
		if ( IS_SET ( ch->wiznet, WIZ_ON ) ) {
			writeBuffer ( "Staff Aid turned on.\n\r", ch );
			REMOVE_BIT ( ch->wiznet, WIZ_ON );
		} else {
			writeBuffer ( "Staff Aid turned off!\n\r", ch );
			SET_BIT ( ch->wiznet, WIZ_ON );
		}
		return;
	}

	if ( !str_prefix ( argument, "on" ) ) {
		writeBuffer ( "Staff Aid turned on!\n\r", ch );
		SET_BIT ( ch->wiznet, WIZ_ON );
		return;
	}

	if ( !str_prefix ( argument, "off" ) ) {
		writeBuffer ( "Staff Aid turned off!\n\r", ch );
		REMOVE_BIT ( ch->wiznet, WIZ_ON );
		return;
	}

	/* show wiznet status */
	if ( !str_prefix ( argument, "status" ) ) {
		buf[0] = '\0';
		int col = 0;
		if ( !IS_SET ( ch->wiznet, WIZ_ON ) ) {
			strcat ( buf, "off " );
			col++;
		}
		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ ) {
			if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) ) {
				strcat ( buf, Format ( "%15s\t", wiznet_table[flag].name ) );
				if ( ++col == 5 ) {
					strcat ( buf, "\r\n" );
				}
			}
		}
		if ( col != 0 );
		strcat ( buf, "\n\r" );

		writeBuffer ( Format ( "Staff Aid:\n\r%s\r\n", buf ), ch );
		return;
	}

	if ( !str_prefix ( argument, "show" ) )
		/* list of all wiznet options */
	{
		buf[0] = '\0';
		int col = 0;
		for ( flag = 0; wiznet_table[flag].name != NULL; flag++ ) {
			if ( wiznet_table[flag].level <= get_trust ( ch ) ) {
				strcat ( buf, Format ( "%15s\t", wiznet_table[flag].name ) );
				if ( ++col == 5 ) {
					col = 0;
					strcat ( buf, "\r\n" );
				}
			}
		}

		if ( col != 0 )
		{ strcat ( buf, "\n\r" ); }

		writeBuffer ( Format ( "Staff Aid options Available:\n\r%s", buf ), ch );
		return;
	}

	flag = wiznet_lookup ( argument );

	if ( flag == -1 || get_trust ( ch ) < wiznet_table[flag].level ) {
		writeBuffer ( "No such option.\n\r", ch );
		return;
	}

	if ( IS_SET ( ch->wiznet, wiznet_table[flag].flag ) ) {
		snprintf ( buf, sizeof ( buf ), "You will no longer see %s on staff aid.\n\r",
				   wiznet_table[flag].name );
		writeBuffer ( buf, ch );
		REMOVE_BIT ( ch->wiznet, wiznet_table[flag].flag );
		return;
	} else {
		snprintf ( buf, sizeof ( buf ), "You will now see %s on staff aid.\n\r",
				   wiznet_table[flag].name );
		writeBuffer ( buf, ch );
		SET_BIT ( ch->wiznet, wiznet_table[flag].flag );
		return;
	}

}
