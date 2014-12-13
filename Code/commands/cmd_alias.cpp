#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_alias )
{
	Creature *rch;
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	int pos;

	smash_tilde ( argument );

	if ( ch->desc == NULL )
	{ rch = ch; }
	else
	{ rch = ch->desc->original ? ch->desc->original : ch; }

	if ( IS_NPC ( rch ) )
	{ return; }

	argument = ChopC ( argument, arg );


	if ( arg[0] == '\0' ) {

		if ( rch->pcdata->alias[0] == NULL ) {
			writeBuffer ( "You have no aliases defined.\n\r", ch );
			return;
		}
		writeBuffer ( "Your current aliases are:\n\r", ch );

		for ( pos = 0; pos < MAX_ALIAS; pos++ ) {
			if ( rch->pcdata->alias[pos] == NULL
					||	rch->pcdata->alias_sub[pos] == NULL )
			{ break; }

			snprintf ( buf, sizeof ( buf ), "    %s:  %s\n\r", rch->pcdata->alias[pos],
					   rch->pcdata->alias_sub[pos] );
			writeBuffer ( buf, ch );
		}
		return;
	}

	if ( !str_prefix ( "una", arg ) || SameString ( "alias", arg ) ) {
		writeBuffer ( "Sorry, that word is reserved.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		for ( pos = 0; pos < MAX_ALIAS; pos++ ) {
			if ( rch->pcdata->alias[pos] == NULL
					||	rch->pcdata->alias_sub[pos] == NULL )
			{ break; }

			if ( SameString ( arg, rch->pcdata->alias[pos] ) ) {
				snprintf ( buf, sizeof ( buf ), "%s aliases to '%s'.\n\r", rch->pcdata->alias[pos],
						   rch->pcdata->alias_sub[pos] );
				writeBuffer ( buf, ch );
				return;
			}
		}

		writeBuffer ( "That alias is not defined.\n\r", ch );
		return;
	}

	if ( !str_prefix ( argument, "delete" ) || !str_prefix ( argument, "prefix" ) ) {
		writeBuffer ( "That shall not be done!\n\r", ch );
		return;
	}

	for ( pos = 0; pos < MAX_ALIAS; pos++ ) {
		if ( rch->pcdata->alias[pos] == NULL )
		{ break; }

		if ( SameString ( arg, rch->pcdata->alias[pos] ) ) { /* redefine an alias */
			PURGE_DATA ( rch->pcdata->alias_sub[pos] );
			rch->pcdata->alias_sub[pos] = assign_string ( argument );
			snprintf ( buf, sizeof ( buf ), "%s is now realiased to '%s'.\n\r", arg, argument );
			writeBuffer ( buf, ch );
			return;
		}
	}

	if ( pos >= MAX_ALIAS ) {
		writeBuffer ( "Sorry, you have reached the alias limit.\n\r", ch );
		return;
	}

	/* make a new alias */
	rch->pcdata->alias[pos]		= assign_string ( arg );
	rch->pcdata->alias_sub[pos]	= assign_string ( argument );
	snprintf ( buf, sizeof ( buf ), "%s is now aliased to '%s'.\n\r", arg, argument );
	writeBuffer ( buf, ch );
}
