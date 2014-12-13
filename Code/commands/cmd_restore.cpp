#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_restore )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;
	Creature *vch;
	Socket *d;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' || SameString ( arg, "room" ) ) {
		/* cure room */

		for ( vch = IN_ROOM ( ch )->people; vch != NULL; vch = vch->next_in_room ) {
			affect_strip ( vch, gsn_plague );
			affect_strip ( vch, gsn_poison );
			affect_strip ( vch, gsn_blindness );
			affect_strip ( vch, gsn_sleep );
			affect_strip ( vch, gsn_curse );

			vch->hit 	= vch->max_hit;
			vch->mana	= vch->max_mana;
			vch->move	= vch->max_move;
			update_pos ( vch );
			act ( "$n has restored you.", ch, NULL, vch, TO_VICT );
		}

		snprintf ( buf, sizeof ( buf ), "$N restored room %d.", IN_ROOM ( ch )->vnum );
		wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );

		writeBuffer ( "Room restored.\n\r", ch );
		return;

	}

	if ( get_trust ( ch ) >=  MAX_LEVEL - 1 && SameString ( arg, "all" ) ) {
		/* cure all */

		for ( d = socket_list; d != NULL; d = d->next ) {
			victim = d->character;

			if ( victim == NULL || IS_NPC ( victim ) )
			{ continue; }

			affect_strip ( victim, gsn_plague );
			affect_strip ( victim, gsn_poison );
			affect_strip ( victim, gsn_blindness );
			affect_strip ( victim, gsn_sleep );
			affect_strip ( victim, gsn_curse );

			victim->hit 	= victim->max_hit;
			victim->mana	= victim->max_mana;
			victim->move	= victim->max_move;
			update_pos ( victim );
			if ( IN_ROOM ( victim ) != NULL )
			{ act ( "$n has restored you.", ch, NULL, victim, TO_VICT ); }
		}
		writeBuffer ( "All active players restored.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	affect_strip ( victim, gsn_plague );
	affect_strip ( victim, gsn_poison );
	affect_strip ( victim, gsn_blindness );
	affect_strip ( victim, gsn_sleep );
	affect_strip ( victim, gsn_curse );
	victim->hit  = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	update_pos ( victim );
	act ( "$n has restored you.", ch, NULL, victim, TO_VICT );
	snprintf ( buf, sizeof ( buf ), "$N restored %s",
			   IS_NPC ( victim ) ? victim->short_descr : victim->name );
	wiznet ( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}

