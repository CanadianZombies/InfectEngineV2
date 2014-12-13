#include "Copyright.h"
#include "Engine.h"

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
DefineCommand ( cmd_force )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' ) {
		writeBuffer ( "Force whom to do what?\n\r", ch );
		return;
	}

	ChopC ( argument, arg2 );

	if ( SameString ( arg2, "delete" ) || !str_prefix ( arg2, "mob" ) ) {
		writeBuffer ( "That will NOT be done.\n\r", ch );
		return;
	}

	sprintf ( buf, "$n forces you to '%s'.", argument );

	if ( SameString ( arg, "all" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 3 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch ) ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else if ( SameString ( arg, "players" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 2 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch )
					&&	 vch->level < LEVEL_HERO ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else if ( SameString ( arg, "gods" ) ) {
		Creature *vch;
		Creature *vch_next;

		if ( get_trust ( ch ) < MAX_LEVEL - 2 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next ) {
			vch_next = vch->next;

			if ( !IS_NPC ( vch ) && get_trust ( vch ) < get_trust ( ch )
					&&   vch->level >= LEVEL_HERO ) {
				act ( buf, ch, NULL, vch, TO_VICT );
				interpret ( vch, argument );
			}
		}
	} else {
		Creature *victim;

		if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		if ( victim == ch ) {
			writeBuffer ( "Aye aye, right away!\n\r", ch );
			return;
		}

		if ( !is_room_owner ( ch, IN_ROOM ( victim ) )
				&&  IN_ROOM ( ch ) != IN_ROOM ( victim )
				&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
			writeBuffer ( "That character is in a private room.\n\r", ch );
			return;
		}

		if ( get_trust ( victim ) >= get_trust ( ch ) ) {
			writeBuffer ( "Do it yourself!\n\r", ch );
			return;
		}

		if ( !IS_NPC ( victim ) && get_trust ( ch ) < MAX_LEVEL - 3 ) {
			writeBuffer ( "Not at your level!\n\r", ch );
			return;
		}

		act ( buf, ch, NULL, victim, TO_VICT );
		interpret ( victim, argument );
	}

	writeBuffer ( "Ok.\n\r", ch );
	return;
}
