#include "Copyright.h"
#include "Engine.h"

/* equips a character */
DefineCommand ( cmd_outfit )
{
	Item *obj;
	int i, sn, vnum;

	if ( !IS_SET ( ch->sflag, CR_STAFF ) ) {
		if ( ch->level > 5 || IS_NPC ( ch ) ) {
			writeBuffer ( "Find it yourself!\n\r", ch );
			return;
		}
	}

	if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_BANNER ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_LIGHT );
	}

	if ( ( obj = get_eq_char ( ch, WEAR_BODY ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_VEST ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_BODY );
	}

	/* do the weapon thing */
	if ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL ) {
		sn = 0;
		vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

		for ( i = 0; weapon_table[i].name != NULL; i++ ) {
			if ( ch->pcdata->learned[sn] <
					ch->pcdata->learned[*weapon_table[i].gsn] ) {
				sn = *weapon_table[i].gsn;
				vnum = weapon_table[i].vnum;
			}
		}

		obj = create_object ( get_obj_index ( vnum ), 0 );
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_WIELD );
	}

	if ( ( ( obj = get_eq_char ( ch, WEAR_WIELD ) ) == NULL
			||   !IS_WEAPON_STAT ( obj, WEAPON_TWO_HANDS ) )
			&&  ( obj = get_eq_char ( ch, WEAR_SHIELD ) ) == NULL ) {
		obj = create_object ( get_obj_index ( OBJ_VNUM_SCHOOL_SHIELD ), 0 );
		obj->cost = 0;
		obj_to_char ( obj, ch );
		equip_char ( ch, obj, WEAR_SHIELD );
	}

	writeBuffer ( "You have been equipped by Mota.\n\r", ch );
}
