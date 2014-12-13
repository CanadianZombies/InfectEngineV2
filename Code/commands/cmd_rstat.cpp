#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_rstat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	RoomData *location;
	Item *obj;
	Creature *rch;
	int door;

	ChopC ( argument, arg );
	location = ( arg[0] == '\0' ) ? IN_ROOM ( ch ) : find_location ( ch, arg );
	if ( location == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, location ) && IN_ROOM ( ch ) != location
			&&  room_is_private ( location ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name: '%s'\n\rArea: '%s'\n\r",
			  location->name,
			  location->area->name );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
			  location->vnum,
			  location->sector_type,
			  location->light,
			  location->heal_rate,
			  location->mana_rate );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Room flags: %d.\n\rDescription:\n\r%s",
			  location->room_flags,
			  location->description );
	writeBuffer ( buf, ch );

	if ( location->extra_descr != NULL ) {
		DescriptionData *ed;

		writeBuffer ( "Extra description keywords: '", ch );
		for ( ed = location->extra_descr; ed; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}
		writeBuffer ( "'.\n\r", ch );
	}

	writeBuffer ( "Characters:", ch );
	for ( rch = location->people; rch; rch = rch->next_in_room ) {
		if ( can_see ( ch, rch ) ) {
			writeBuffer ( " ", ch );
			ChopC ( rch->name, buf );
			writeBuffer ( buf, ch );
		}
	}

	writeBuffer ( ".\n\rObjects:   ", ch );
	for ( obj = location->contents; obj; obj = obj->next_content ) {
		writeBuffer ( " ", ch );
		ChopC ( obj->name, buf );
		writeBuffer ( buf, ch );
	}
	writeBuffer ( ".\n\r", ch );

	for ( door = 0; door <= 5; door++ ) {
		Exit *pexit;

		if ( ( pexit = location->exit[door] ) != NULL ) {
			sprintf ( buf,
					  "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

					  door,
					  ( pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum ),
					  pexit->key,
					  pexit->exit_info,
					  pexit->keyword,
					  pexit->description[0] != '\0'
					  ? pexit->description : "(none).\n\r" );
			writeBuffer ( buf, ch );
		}
	}

	return;
}
