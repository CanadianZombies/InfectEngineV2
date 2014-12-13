#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_ostat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Affect *paf;
	Item *obj;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Stat what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "Nothing like that in hell, earth, or heaven.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name(s): %s\n\r",
			  obj->name );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",
			  obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",
			  item_name ( obj->item_type ), obj->pIndexData->reset_num );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Short description: %s\n\rLong description: %s\n\r",
			  obj->short_descr, obj->description );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
			  wear_bit_name ( obj->wear_flags ), extra_bit_name ( obj->extra_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
			  1,           get_obj_number ( obj ),
			  obj->weight, get_obj_weight ( obj ), get_true_weight ( obj ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
			  obj->level, obj->cost, obj->condition, obj->timer );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
			  obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
			  IN_OBJ ( obj )     == NULL    ? "(none)" : IN_OBJ ( obj )->short_descr,
			  CARRIED_BY ( obj ) == NULL    ? "(none)" :
			  can_see ( ch, CARRIED_BY ( obj ) ) ? CARRIED_BY ( obj )->name
			  : "someone",
			  obj->wear_loc );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Values: %d %d %d %d %d\n\r",
			  obj->value[0], obj->value[1], obj->value[2], obj->value[3],
			  obj->value[4] );
	writeBuffer ( buf, ch );

	writeBuffer ( Format ( "Material bits: %s\n\r", material_bit_name ( obj->material_flags ) ), ch );

	/* now give out vital statistics as per identify */

	switch ( obj->item_type ) {
		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			sprintf ( buf, "Level %d spells of:", obj->value[0] );
			writeBuffer ( buf, ch );

			if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[1]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[2]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[3]].name, ch );
				writeBuffer ( "'", ch );
			}

			if ( obj->value[4] >= 0 && obj->value[4] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[4]].name, ch );
				writeBuffer ( "'", ch );
			}

			writeBuffer ( ".\n\r", ch );
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			sprintf ( buf, "Has %d(%d) charges of level %d",
					  obj->value[1], obj->value[2], obj->value[0] );
			writeBuffer ( buf, ch );

			if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ) {
				writeBuffer ( " '", ch );
				writeBuffer ( skill_table[obj->value[3]].name, ch );
				writeBuffer ( "'", ch );
			}

			writeBuffer ( ".\n\r", ch );
			break;

		case ITEM_DRINK_CON:
			snprintf ( buf, sizeof ( buf ), "It holds %s-colored %s.\n\r",
					   liq_table[obj->value[2]].liq_color,
					   liq_table[obj->value[2]].liq_name );
			writeBuffer ( buf, ch );
			break;


		case ITEM_WEAPON:
			writeBuffer ( "Weapon type is ", ch );
			switch ( obj->value[0] ) {
				case ( WEAPON_EXOTIC ) :
					writeBuffer ( "exotic\n\r", ch );
					break;
				case ( WEAPON_SWORD ) :
					writeBuffer ( "sword\n\r", ch );
					break;
				case ( WEAPON_DAGGER ) :
					writeBuffer ( "dagger\n\r", ch );
					break;
				case ( WEAPON_SPEAR ) :
					writeBuffer ( "spear/staff\n\r", ch );
					break;
				case ( WEAPON_MACE ) :
					writeBuffer ( "mace/club\n\r", ch );
					break;
				case ( WEAPON_AXE ) :
					writeBuffer ( "axe\n\r", ch );
					break;
				case ( WEAPON_FLAIL ) :
					writeBuffer ( "flail\n\r", ch );
					break;
				case ( WEAPON_WHIP ) :
					writeBuffer ( "whip\n\r", ch );
					break;
				case ( WEAPON_POLEARM ) :
					writeBuffer ( "polearm\n\r", ch );
					break;
				default:
					writeBuffer ( "unknown\n\r", ch );
					break;
			}
			if ( obj->pIndexData->new_format )
				snprintf ( buf, sizeof ( buf ), "Damage is %dd%d (average %d)\n\r",
						   obj->value[1], obj->value[2],
						   ( 1 + obj->value[2] ) * obj->value[1] / 2 );
			else
				sprintf ( buf, "Damage is %d to %d (average %d)\n\r",
						  obj->value[1], obj->value[2],
						  ( obj->value[1] + obj->value[2] ) / 2 );
			writeBuffer ( buf, ch );

			snprintf ( buf, sizeof ( buf ), "Damage noun is %s.\n\r",
					   ( obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE ) ?
					   attack_table[obj->value[3]].noun : "undefined" );
			writeBuffer ( buf, ch );

			if ( obj->value[4] ) { /* weapon flags */
				snprintf ( buf, sizeof ( buf ), "Weapons flags: %s\n\r",
						   weapon_bit_name ( obj->value[4] ) );
				writeBuffer ( buf, ch );
			}
			break;

		case ITEM_ARMOR:
			sprintf ( buf,
					  "Armor archetype is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
					  obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_CONTAINER:
			snprintf ( buf, sizeof ( buf ), "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
					   obj->value[0], obj->value[3], cont_bit_name ( obj->value[1] ) );
			writeBuffer ( buf, ch );
			if ( obj->value[4] != 100 ) {
				snprintf ( buf, sizeof ( buf ), "Weight multiplier: %d%%\n\r",
						   obj->value[4] );
				writeBuffer ( buf, ch );
			}
			break;
	}


	if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL ) {
		DescriptionData *ed;

		writeBuffer ( "Extra description keywords: '", ch );

		for ( ed = obj->extra_descr; ed != NULL; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}

		for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next ) {
			writeBuffer ( ed->keyword, ch );
			if ( ed->next != NULL )
			{ writeBuffer ( " ", ch ); }
		}

		writeBuffer ( "'\n\r", ch );
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next ) {
		sprintf ( buf, "Affects %s by %d, level %d",
				  affect_loc_name ( paf->location ), paf->modifier, paf->level );
		writeBuffer ( buf, ch );
		if ( paf->duration > -1 )
		{ snprintf ( buf, sizeof ( buf ), ", %d hours.\n\r", paf->duration ); }
		else
		{ snprintf ( buf, sizeof ( buf ), ".\n\r" ); }
		writeBuffer ( buf, ch );
		if ( paf->bitvector ) {
			switch ( paf->where ) {
				case TO_AFFECTS:
					snprintf ( buf, sizeof ( buf ), "Adds %s affect.\n",
							   affect_bit_name ( paf->bitvector ) );
					break;
				case TO_WEAPON:
					snprintf ( buf, sizeof ( buf ), "Adds %s weapon flags.\n",
							   weapon_bit_name ( paf->bitvector ) );
					break;
				case TO_OBJECT:
					snprintf ( buf, sizeof ( buf ), "Adds %s object flag.\n",
							   extra_bit_name ( paf->bitvector ) );
					break;
				case TO_IMMUNE:
					snprintf ( buf, sizeof ( buf ), "Adds immunity to %s.\n",
							   imm_bit_name ( paf->bitvector ) );
					break;
				case TO_RESIST:
					snprintf ( buf, sizeof ( buf ), "Adds resistance to %s.\n\r",
							   imm_bit_name ( paf->bitvector ) );
					break;
				case TO_VULN:
					snprintf ( buf, sizeof ( buf ), "Adds vulnerability to %s.\n\r",
							   imm_bit_name ( paf->bitvector ) );
					break;
				default:
					snprintf ( buf, sizeof ( buf ), "Unknown bit %d: %d\n\r",
							   paf->where, paf->bitvector );
					break;
			}
			writeBuffer ( buf, ch );
		}
	}

	if ( !obj->enchanted )
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next ) {
			sprintf ( buf, "Affects %s by %d, level %d.\n\r",
					  affect_loc_name ( paf->location ), paf->modifier, paf->level );
			writeBuffer ( buf, ch );
			if ( paf->bitvector ) {
				switch ( paf->where ) {
					case TO_AFFECTS:
						snprintf ( buf, sizeof ( buf ), "Adds %s affect.\n",
								   affect_bit_name ( paf->bitvector ) );
						break;
					case TO_OBJECT:
						snprintf ( buf, sizeof ( buf ), "Adds %s object flag.\n",
								   extra_bit_name ( paf->bitvector ) );
						break;
					case TO_IMMUNE:
						snprintf ( buf, sizeof ( buf ), "Adds immunity to %s.\n",
								   imm_bit_name ( paf->bitvector ) );
						break;
					case TO_RESIST:
						snprintf ( buf, sizeof ( buf ), "Adds resistance to %s.\n\r",
								   imm_bit_name ( paf->bitvector ) );
						break;
					case TO_VULN:
						snprintf ( buf, sizeof ( buf ), "Adds vulnerability to %s.\n\r",
								   imm_bit_name ( paf->bitvector ) );
						break;
					default:
						snprintf ( buf, sizeof ( buf ), "Unknown bit %d: %d\n\r",
								   paf->where, paf->bitvector );
						break;
				}
				writeBuffer ( buf, ch );
			}
		}

	return;
}
