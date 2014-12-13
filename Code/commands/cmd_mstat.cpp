#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_mstat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Affect *paf;
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Stat whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	sprintf ( buf, "Name: %s\n\r",
			  victim->name );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
			  IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
			  IS_NPC ( victim ) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
			  race_table[victim->race].name,
			  IS_NPC ( victim ) ? victim->group : 0, sex_table[victim->sex].name,
			  IN_ROOM ( victim ) == NULL    ?        0 : IN_ROOM ( victim )->vnum
			);
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) ) {
		snprintf ( buf, sizeof ( buf ), "Count: %d  Killed: %d\n\r",
				   victim->pIndexData->count, victim->pIndexData->killed );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf,
			  "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
			  victim->perm_stat[STAT_STR],
			  get_curr_stat ( victim, STAT_STR ),
			  victim->perm_stat[STAT_INT],
			  get_curr_stat ( victim, STAT_INT ),
			  victim->perm_stat[STAT_WIS],
			  get_curr_stat ( victim, STAT_WIS ),
			  victim->perm_stat[STAT_DEX],
			  get_curr_stat ( victim, STAT_DEX ),
			  victim->perm_stat[STAT_CON],
			  get_curr_stat ( victim, STAT_CON ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",
			  victim->hit,         victim->max_hit,
			  victim->mana,        victim->max_mana,
			  victim->move,        victim->max_move,
			  IS_NPC ( ch ) ? 0 : victim->practice );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Lv: %d  Class: %s  Align: %d  Gold: %ld  Silver: %ld  Exp: %d\n\r",
			  victim->level,
			  IS_NPC ( victim ) ? "mobile" : archetype_table[victim->archetype].name,
			  victim->alignment,
			  victim->gold, victim->silver, victim->exp );
	writeBuffer ( buf, ch );

	snprintf ( buf, sizeof ( buf ), "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
			   GET_AC ( victim, AC_PIERCE ), GET_AC ( victim, AC_BASH ),
			   GET_AC ( victim, AC_SLASH ),  GET_AC ( victim, AC_EXOTIC ) );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
			  GET_HITROLL ( victim ), GET_DAMROLL ( victim ), victim->saving_throw,
			  size_table[victim->size].name, position_table[victim->position].name,
			  victim->wimpy );
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) && victim->pIndexData->new_format ) {
		snprintf ( buf, sizeof ( buf ), "Damage: %dd%d  Message:  %s\n\r",
				   victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE],
				   attack_table[victim->dam_type].noun );
		writeBuffer ( buf, ch );
	}
	sprintf ( buf, "Fighting: %s\n\r",
			  FIGHTING ( victim ) ? FIGHTING ( victim )->name : "(none)" );
	writeBuffer ( buf, ch );

	writeBuffer ( Format ( "Material bits: %s\n\r", material_bit_name ( victim->material_flags ) ), ch );

	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf,
				  "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d\n\r",
				  victim->pcdata->condition[COND_THIRST],
				  victim->pcdata->condition[COND_HUNGER],
				  victim->pcdata->condition[COND_FULL],
				  victim->pcdata->condition[COND_DRUNK] );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf, "Carry number: %d  Carry weight: %ld\n\r",
			  victim->carry_number, get_carry_weight ( victim ) / 10 );
	writeBuffer ( buf, ch );


	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf,
				  "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
				  get_age ( victim ),
				  ( int ) ( victim->played + current_time - victim->logon ) / 3600,
				  victim->pcdata->last_level,
				  victim->timer );
		writeBuffer ( buf, ch );
	}

	snprintf ( buf, sizeof ( buf ), "Act: %s\n\r", act_bit_name ( victim->act ) );
	writeBuffer ( buf, ch );

	if ( victim->comm ) {
		snprintf ( buf, sizeof ( buf ), "Comm: %s\n\r", comm_bit_name ( victim->comm ) );
		writeBuffer ( buf, ch );
	}

	if ( IS_NPC ( victim ) && victim->off_flags ) {
		snprintf ( buf, sizeof ( buf ), "Offense: %s\n\r", off_bit_name ( victim->off_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->imm_flags ) {
		snprintf ( buf, sizeof ( buf ), "Immune: %s\n\r", imm_bit_name ( victim->imm_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->res_flags ) {
		snprintf ( buf, sizeof ( buf ), "Resist: %s\n\r", imm_bit_name ( victim->res_flags ) );
		writeBuffer ( buf, ch );
	}

	if ( victim->vuln_flags ) {
		snprintf ( buf, sizeof ( buf ), "Vulnerable: %s\n\r", imm_bit_name ( victim->vuln_flags ) );
		writeBuffer ( buf, ch );
	}

	snprintf ( buf, sizeof ( buf ), "Form: %s\n\rParts: %s\n\r",
			   form_bit_name ( victim->form ), part_bit_name ( victim->parts ) );
	writeBuffer ( buf, ch );

	if ( victim->affected_by ) {
		snprintf ( buf, sizeof ( buf ), "Affected by %s\n\r",
				   affect_bit_name ( victim->affected_by ) );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf, "Master: %s  Leader: %s  Pet: %s\n\r",
			  victim->master      ? victim->master->name   : "(none)",
			  victim->leader      ? victim->leader->name   : "(none)",
			  victim->pet 	    ? victim->pet->name	     : "(none)" );
	writeBuffer ( buf, ch );

	if ( !IS_NPC ( victim ) ) {
		sprintf ( buf, "Security: %d.\n\r", victim->pcdata->security );	/* OLC */
		writeBuffer ( buf, ch );					/* OLC */
	}

	sprintf ( buf, "Short description: %s\n\rLong  description: %s",
			  victim->short_descr,
			  !IS_NULLSTR ( victim->long_descr ) ? victim->long_descr : "(none)\n\r" );
	writeBuffer ( buf, ch );

	if ( IS_NPC ( victim ) && victim->spec_fun != 0 ) {
		snprintf ( buf, sizeof ( buf ), "Mobile has special procedure %s.\n\r",
				   spec_name ( victim->spec_fun ) );
		writeBuffer ( buf, ch );
	}

	for ( paf = victim->affected; paf != NULL; paf = paf->next ) {
		sprintf ( buf,
				  "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
				  skill_table[ ( int ) paf->type].name,
				  affect_loc_name ( paf->location ),
				  paf->modifier,
				  paf->duration,
				  affect_bit_name ( paf->bitvector ),
				  paf->level
				);
		writeBuffer ( buf, ch );
	}

	return;
}

