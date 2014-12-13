#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_smote )
{
	Creature *vch;
	char *letter, *name;
	char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
	int matches = 0;

	if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) ) {
		writeBuffer ( "You can't show your emotions.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Emote what?\n\r", ch );
		return;
	}

	if ( str_str ( argument, ch->name ) == NULL ) {
		writeBuffer ( "You must include your name in an smote.\n\r", ch );
		return;
	}

	writeBuffer ( argument, ch );
	writeBuffer ( "\n\r", ch );

	for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch->next_in_room ) {
		if ( vch->desc == NULL || vch == ch )
		{ continue; }

		if ( ( letter = str_str ( argument, vch->name ) ) == NULL ) {
			writeBuffer ( argument, vch );
			writeBuffer ( "\n\r", vch );
			continue;
		}

		strcpy ( temp, argument );
		temp[strlen ( argument ) - strlen ( letter )] = '\0';
		last[0] = '\0';
		name = vch->name;

		for ( ; *letter != '\0'; letter++ ) {
			if ( *letter == '\'' && matches == ( signed int ) strlen ( vch->name ) ) {
				strcat ( temp, "r" );
				continue;
			}

			if ( *letter == 's' && matches == ( signed int ) strlen ( vch->name ) ) {
				matches = 0;
				continue;
			}

			if ( matches == ( signed int ) strlen ( vch->name ) ) {
				matches = 0;
			}

			if ( *letter == *name ) {
				matches++;
				name++;
				if ( matches == ( signed int ) strlen ( vch->name ) ) {
					strcat ( temp, "you" );
					last[0] = '\0';
					name = vch->name;
					continue;
				}
				strncat ( last, letter, 1 );
				continue;
			}

			matches = 0;
			strcat ( temp, last );
			strncat ( temp, letter, 1 );
			last[0] = '\0';
			name = vch->name;
		}

		writeBuffer ( temp, vch );
		writeBuffer ( "\n\r", vch );
	}

	return;
}

