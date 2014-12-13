

#include "Engine.h"

/* does aliasing and other fun stuff */
void substitute_alias ( Socket *d, char *argument )
{
	Creature *ch;
	char buf[MAX_STRING_LENGTH], prefix[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH];
	char *point;
	int alias;

	ch = d->original ? d->original : d->character;

	/* check for prefix */
	if ( !IS_NULLSTR ( ch->prefix ) && str_prefix ( "prefix", argument ) ) {
		if ( strlen ( ch->prefix ) + strlen ( argument ) > MAX_INPUT_LENGTH )
		{ writeBuffer ( "Line to long, prefix not processed.\r\n", ch ); }
		else {
			sprintf ( prefix, "%s %s", ch->prefix, argument );
			argument = prefix;
		}
	}

	if ( IS_NPC ( ch ) || ch->pcdata->alias[0] == NULL
			||	!str_prefix ( "alias", argument ) || !str_prefix ( "una", argument )
			||  !str_prefix ( "prefix", argument ) ) {
		interpret ( d->character, argument );
		return;
	}

	strcpy ( buf, argument );

	for ( alias = 0; alias < MAX_ALIAS; alias++ ) { /* go through the aliases */
		if ( ch->pcdata->alias[alias] == NULL )
		{ break; }

		if ( !str_prefix ( ch->pcdata->alias[alias], argument ) ) {
			point = ChopC ( argument, name );
			if ( !strcmp ( ch->pcdata->alias[alias], name ) ) {
				buf[0] = '\0';
				strcat ( buf, ch->pcdata->alias_sub[alias] );
				strcat ( buf, " " );
				strcat ( buf, point );

				if ( strlen ( buf ) > MAX_INPUT_LENGTH - 1 ) {
					writeBuffer (
						"Alias substitution too long. Truncated.\r\n", ch );
					buf[MAX_INPUT_LENGTH - 1] = '\0';
				}
				break;
			}
		}
	}
	interpret ( d->character, buf );
}

// -- EOF

