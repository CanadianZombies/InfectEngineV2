/*###################################################################################
#                    Infected City powered by InfectEngine                          #
#            InfectEngine is powered by CombatMUD Core Infrastructure               #
#  Original Diku Mud Copyright (c) 1990, 1991 by Sebastian Hammer, Michael Sifert   #
#               Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe                  #
# Merc Copyright (c) 1992, 1993 by Michael Chastain, Michael Quan, and Mitchell Tse #
#      ROM 2.4 Copyright (c) 1993-1998 Russ Taylor, Brian Moore, Gabrielle Taylor   #
#####################################################################################
# InfectEngine and CombatMUD Engine are both ground up C++ MUDs by David Simmerson  #
# InfectEngine V2 is an attempt to Hybrid ROM24B6 and InfectEngineV1 and CombatMUD  #
# together.  All source falls under the DIKU license, Merc and ROM licences however #
# if individual functions are retrieved from this base that were not originally part#
#   of Diku, Merc or ROM they will fall under the MIT license as per the original   #
#                      Licenses for CombatMUD and InfectEngine.                     #
#####################################################################################
# InfectEngine Copyright (c) 2010-2014 David Simmerson                              #
# CombatMUD Copyright (c) 2007-2014 David Simmerson                                 #
#                                                                                   #
# Permission is hereby granted, A, to any person obtaining a copy                   #
# of this software and associated documentation files (the "Software"), to deal     #
# in the Software without restriction, including without limitation the rights      #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         #
# copies of the Software, and to permit persons to whom the Software is             #
# furnished to do so, subject to the following conditions:                          #
#                                                                                   #
# The above copyright notice and this permission notice shall be included in        #
# all copies or substantial portions of the Software.                               #
#                                                                                   #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN         #
# THE SOFTWARE.                                                                     #
###################################################################################*/

#include "Engine.h"

#define DIF(a,b) (~((~a)|(b)))

/*
 *  Verbose writes reset data in plain english into the comments
 *  section of the resets.  It makes areas considerably larger but
 *  may aid in debugging.
 */

/* #define VERBOSE */

/*****************************************************************************
 Name:		fix_string
 Purpose:	Returns a string without \r and ~.
 ****************************************************************************/
char *fix_string ( const char *str )
{
	static char strfix[MAX_STRING_LENGTH * 2];
	int i;
	int o;

	if ( str == NULL )
	{ return '\0'; }

	for ( o = i = 0; str[i + o] != '\0'; i++ ) {
		if ( str[i + o] == '\r' || str[i + o] == '~' )
		{ o++; }
		strfix[i] = str[i + o];
	}
	strfix[i] = '\0';
	return strfix;
}



/*****************************************************************************
 Name:		save_area_list
 Purpose:	Saves the listing of files to be loaded at startup.
 Called by:	cmd_asave(olc_save.c).
 ****************************************************************************/
void save_area_list()
{
	FILE *fp;
	Zone *pArea;
	extern HELP_AREA * had_list;
	HELP_AREA * ha;

	if ( ( fp = fopen ( Format ( "%s%s", WORLD_DIR, AREA_LIST ), "w" ) ) == NULL ) {
		log_hd ( LOG_ERROR, "Save_area_list: fopen" );
		ReportErrno ( "area.lst" );
	} else {
		/*
		 * Add any help files that need to be loaded at
		 * startup to this section.
		 */
		fprintf ( fp, "social.are\n" );   /* ROM OLC */

		for ( ha = had_list; ha; ha = ha->next )
			if ( ha->area == NULL )
			{ fprintf ( fp, "%s\n", ha->filename ); }

		for ( pArea = area_first; pArea; pArea = pArea->next ) {
			fprintf ( fp, "%s\n", pArea->file_name );
		}

		fprintf ( fp, "$\n" );
		fclose ( fp );
	}

	return;
}


/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 *
 * buf[] must hold at least 32+1 characters.
 *
 * -- Hugin
 */
char *fwrite_flag ( long flags, char buf[] )
{
	char offset;
	char *cp;

	buf[0] = '\0';

	if ( flags == 0 ) {
		strcpy ( buf, "0" );
		return buf;
	}

	/* 32 -- number of bits in a long */

	for ( offset = 0, cp = buf; offset < 32; offset++ )
		if ( flags & ( ( long ) 1 << offset ) ) {
			if ( offset <= 'Z' - 'A' )
			{ * ( cp++ ) = 'A' + offset; }
			else
			{ * ( cp++ ) = 'a' + offset - ( 'Z' - 'A' + 1 ); }
		}

	*cp = '\0';

	return buf;
}

void save_mobprogs ( FILE *fp, Zone *pArea )
{
	MPROG_CODE *pMprog;
	int i;

	fprintf ( fp, "#MOBPROGS\n" );

	for ( i = pArea->min_vnum; i <= pArea->max_vnum; i++ ) {
		if ( ( pMprog = get_mprog_index ( i ) ) != NULL ) {
			fprintf ( fp, "#%d\n", i );
			fprintf ( fp, "%s~\n", fix_string ( pMprog->code ) );
		}
	}

	fprintf ( fp, "#0\n\n" );
	return;
}

/*****************************************************************************
 Name:		save_mobile
 Purpose:	Save one mobile to file, new format -- Hugin
 Called by:	save_mobiles (below).
 ****************************************************************************/
void save_mobile ( FILE *fp, NPCData *pMobIndex )
{
	sh_int race = pMobIndex->race;
	MPROG_LIST *pMprog;
	char buf[MAX_STRING_LENGTH];
	long temp;

	fprintf ( fp, "#%d\n",	pMobIndex->vnum );
	fprintf ( fp, "%s~\n",	pMobIndex->player_name );
	fprintf ( fp, "%s~\n",	pMobIndex->short_descr );
	fprintf ( fp, "%s~\n",	fix_string ( pMobIndex->long_descr ) );
	fprintf ( fp, "%s~\n",	fix_string ( pMobIndex->description ) );
	fprintf ( fp, "%s~\n",	race_table[race].name );

	fprintf ( fp, "%s ",			print_flags ( pMobIndex->material_flags ) );
	fprintf ( fp, "%s ",			print_flags ( pMobIndex->random ) );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->act,		buf ) );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->affected_by,	buf ) );
	fprintf ( fp, "%d %d\n",	pMobIndex->alignment , pMobIndex->group );
	fprintf ( fp, "%d ",		pMobIndex->level );
	fprintf ( fp, "%d ",		pMobIndex->hitroll );
	fprintf ( fp, "%dd%d+%d ",	pMobIndex->hit[DICE_NUMBER],
			  pMobIndex->hit[DICE_TYPE],
			  pMobIndex->hit[DICE_BONUS] );
	fprintf ( fp, "%dd%d+%d ",	pMobIndex->mana[DICE_NUMBER],
			  pMobIndex->mana[DICE_TYPE],
			  pMobIndex->mana[DICE_BONUS] );
	fprintf ( fp, "%dd%d+%d ",	pMobIndex->damage[DICE_NUMBER],
			  pMobIndex->damage[DICE_TYPE],
			  pMobIndex->damage[DICE_BONUS] );
	fprintf ( fp, "%s\n",	attack_table[pMobIndex->dam_type].name );
	fprintf ( fp, "%d %d %d %d\n",
			  pMobIndex->ac[AC_PIERCE] / 10,
			  pMobIndex->ac[AC_BASH]   / 10,
			  pMobIndex->ac[AC_SLASH]  / 10,
			  pMobIndex->ac[AC_EXOTIC] / 10 );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->off_flags,  buf ) );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->imm_flags,  buf ) );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->res_flags,  buf ) );
	fprintf ( fp, "%s\n",	fwrite_flag ( pMobIndex->vuln_flags, buf ) );
	fprintf ( fp, "%s %s %s %ld\n",
			  position_table[pMobIndex->start_pos].short_name,
			  position_table[pMobIndex->default_pos].short_name,
			  sex_table[pMobIndex->sex].name,
			  pMobIndex->wealth );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->form,  buf ) );
	fprintf ( fp, "%s ",		fwrite_flag ( pMobIndex->parts, buf ) );

	fprintf ( fp, "%s ",		size_table[pMobIndex->size].name );

	if ( ( temp = DIF ( race_table[race].act, pMobIndex->act ) ) )
	{ fprintf ( fp, "F act %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].aff, pMobIndex->affected_by ) ) )
	{ fprintf ( fp, "F aff %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].off, pMobIndex->off_flags ) ) )
	{ fprintf ( fp, "F off %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].imm, pMobIndex->imm_flags ) ) )
	{ fprintf ( fp, "F imm %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].res, pMobIndex->res_flags ) ) )
	{ fprintf ( fp, "F res %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].vuln, pMobIndex->vuln_flags ) ) )
	{ fprintf ( fp, "F vul %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].form, pMobIndex->form ) ) )
	{ fprintf ( fp, "F for %s\n", fwrite_flag ( temp, buf ) ); }

	if ( ( temp = DIF ( race_table[race].parts, pMobIndex->parts ) ) )
	{ fprintf ( fp, "F par %s\n", fwrite_flag ( temp, buf ) ); }

	for ( pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next ) {
		fprintf ( fp, "M %s %d %s~\n",
				  mprog_type_to_name ( pMprog->trig_type ), pMprog->vnum,
				  pMprog->trig_phrase );
	}

	// -- repop percentage
	fprintf ( fp, "L %d\n", pMobIndex->repop_percent );

	return;
}


/*****************************************************************************
 Name:		save_mobiles
 Purpose:	Save #MOBILES secion of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_mobiles ( FILE *fp, Zone *pArea )
{
	int i;
	NPCData *pMob;

	fprintf ( fp, "#MOBILES\n" );

	for ( i = pArea->min_vnum; i <= pArea->max_vnum; i++ ) {
		if ( ( pMob = get_mob_index ( i ) ) )
		{ save_mobile ( fp, pMob ); }
	}

	fprintf ( fp, "#0\n\n\n\n" );
	return;
}





/*****************************************************************************
 Name:		save_object
 Purpose:	Save one object to file.
                new ROM format saving -- Hugin
 Called by:	save_objects (below).
 ****************************************************************************/
void save_object ( FILE *fp, ItemData *pObjIndex )
{
	char letter;
	Affect *pAf;
	DescriptionData *pEd;
	char buf[MAX_STRING_LENGTH];

	fprintf ( fp, "#%d\n",    pObjIndex->vnum );
	fprintf ( fp, "%s~\n",    pObjIndex->name );
	fprintf ( fp, "%s~\n",    pObjIndex->short_descr );
	fprintf ( fp, "%s~\n",    fix_string ( pObjIndex->description ) );
	fprintf ( fp, "%s ",      item_name ( pObjIndex->item_type ) );
	fprintf ( fp, "%s ",      fwrite_flag ( pObjIndex->extra_flags, buf ) );
	fprintf ( fp, "%s\n",     fwrite_flag ( pObjIndex->wear_flags,  buf ) );
	fprintf ( fp, "%s ", print_flags ( pObjIndex->material_flags ) );

	/*
	 *  Using fwrite_flag to write most values gives a strange
	 *  looking area file, consider making a case for each
	 *  item type later.
	 */

	switch ( pObjIndex->item_type ) {
		default:
			fprintf ( fp, "%s ",  fwrite_flag ( pObjIndex->value[0], buf ) );
			fprintf ( fp, "%s ",  fwrite_flag ( pObjIndex->value[1], buf ) );
			fprintf ( fp, "%s ",  fwrite_flag ( pObjIndex->value[2], buf ) );
			fprintf ( fp, "%s ",  fwrite_flag ( pObjIndex->value[3], buf ) );
			fprintf ( fp, "%s\n", fwrite_flag ( pObjIndex->value[4], buf ) );
			break;

		case ITEM_DRINK_CON:
		case ITEM_FOUNTAIN:
			fprintf ( fp, "%d %d '%s' %d %d\n",
					  pObjIndex->value[0],
					  pObjIndex->value[1],
					  liq_table[pObjIndex->value[2]].liq_name,
					  pObjIndex->value[3],
					  pObjIndex->value[4] );
			break;

		case ITEM_CONTAINER:
			fprintf ( fp, "%d %s %d %d %d\n",
					  pObjIndex->value[0],
					  fwrite_flag ( pObjIndex->value[1], buf ),
					  pObjIndex->value[2],
					  pObjIndex->value[3],
					  pObjIndex->value[4] );
			break;

		case ITEM_WEAPON:
			fprintf ( fp, "%s %d %d %s %s\n",
					  weapon_name ( pObjIndex->value[0] ),
					  pObjIndex->value[1],
					  pObjIndex->value[2],
					  attack_table[pObjIndex->value[3]].name,
					  fwrite_flag ( pObjIndex->value[4], buf ) );
			break;

		case ITEM_PILL:
		case ITEM_POTION:
		case ITEM_SCROLL:
			fprintf ( fp, "%d '%s' '%s' '%s' '%s'\n",
					  pObjIndex->value[0] > 0 ? /* no negative numbers */
					  pObjIndex->value[0]
					  : 0,
					  pObjIndex->value[1] != -1 ?
					  skill_table[pObjIndex->value[1]].name
					  : "",
					  pObjIndex->value[2] != -1 ?
					  skill_table[pObjIndex->value[2]].name
					  : "",
					  pObjIndex->value[3] != -1 ?
					  skill_table[pObjIndex->value[3]].name
					  : "",
					  pObjIndex->value[4] != -1 ?
					  skill_table[pObjIndex->value[4]].name
					  : "" );
			break;

		case ITEM_STAFF:
		case ITEM_WAND:
			fprintf ( fp, "%d %d %d '%s' %d\n",
					  pObjIndex->value[0],
					  pObjIndex->value[1],
					  pObjIndex->value[2],
					  pObjIndex->value[3] != -1 ?
					  skill_table[pObjIndex->value[3]].name :
					  "",
					  pObjIndex->value[4] );
			break;
	}

	fprintf ( fp, "%d ", pObjIndex->level );
	fprintf ( fp, "%d ", pObjIndex->weight );
	fprintf ( fp, "%d ", pObjIndex->cost );

	if ( pObjIndex->condition >  90 ) { letter = 'P'; }
	else if ( pObjIndex->condition >  75 ) { letter = 'G'; }
	else if ( pObjIndex->condition >  50 ) { letter = 'A'; }
	else if ( pObjIndex->condition >  25 ) { letter = 'W'; }
	else if ( pObjIndex->condition >  10 ) { letter = 'D'; }
	else if ( pObjIndex->condition >   0 ) { letter = 'B'; }
	else                                   { letter = 'R'; }

	fprintf ( fp, "%c\n", letter );

	for ( pAf = pObjIndex->affected; pAf; pAf = pAf->next ) {
		if ( pAf->where == TO_OBJECT || pAf->bitvector == 0 )
		{ fprintf ( fp, "A\n%d %d\n",  pAf->location, pAf->modifier ); }
		else {
			fprintf ( fp, "F\n" );

			switch ( pAf->where ) {
				case TO_AFFECTS:
					fprintf ( fp, "A " );
					break;
				case TO_IMMUNE:
					fprintf ( fp, "I " );
					break;
				case TO_RESIST:
					fprintf ( fp, "R " );
					break;
				case TO_VULN:
					fprintf ( fp, "V " );
					break;
				default:
					log_hd ( LOG_ERROR,  Format ( "olc_save: Invalid Affect->where: %d", pAf->where ) );
					break;
			}

			fprintf ( fp, "%d %d %s\n", pAf->location, pAf->modifier,
					  fwrite_flag ( pAf->bitvector, buf ) );
		}
	}

	for ( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next ) {
		fprintf ( fp, "E\n%s~\n%s~\n", pEd->keyword,
				  fix_string ( pEd->description ) );
	}

	fprintf ( fp, "R %d %d %d %d %d %d\n",
			  pObjIndex->requirements[SIZ_REQ],
			  pObjIndex->requirements[STR_REQ],
			  pObjIndex->requirements[DEX_REQ],
			  pObjIndex->requirements[CON_REQ],
			  pObjIndex->requirements[INT_REQ],
			  pObjIndex->requirements[WIS_REQ] );

	// -- store our repop percentage
	fprintf ( fp, "L %d\n", pObjIndex->repop_percent );

	return;
}




/*****************************************************************************
 Name:		save_objects
 Purpose:	Save #OBJECTS section of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_objects ( FILE *fp, Zone *pArea )
{
	int i;
	ItemData *pObj;

	fprintf ( fp, "#OBJECTS\n" );

	for ( i = pArea->min_vnum; i <= pArea->max_vnum; i++ ) {
		if ( ( pObj = get_obj_index ( i ) ) )
		{ save_object ( fp, pObj ); }
	}

	fprintf ( fp, "#0\n\n\n\n" );
	return;
}





/*****************************************************************************
 Name:		save_rooms
 Purpose:	Save #ROOMS section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_rooms ( FILE *fp, Zone *pArea )
{
	RoomData *pRoomIndex;
	DescriptionData *pEd;
	Exit *pExit;
	int iHash;
	int door;

	fprintf ( fp, "#ROOMS\n" );
	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next ) {
			if ( pRoomIndex->area == pArea ) {
				fprintf ( fp, "#%d\n",		pRoomIndex->vnum );
				fprintf ( fp, "%s~\n",		pRoomIndex->name );
				fprintf ( fp, "%s~\n",		fix_string ( pRoomIndex->description ) );
				fprintf ( fp, "0 " );
				fprintf ( fp, "%d ",		pRoomIndex->room_flags );
				fprintf ( fp, "%d\n",		pRoomIndex->sector_type );

				for ( pEd = pRoomIndex->extra_descr; pEd;
						pEd = pEd->next ) {
					fprintf ( fp, "E\n%s~\n%s~\n", pEd->keyword,
							  fix_string ( pEd->description ) );
				}
				for ( door = 0; door < MAX_DIR; door++ ) {	/* I hate this! */
					if ( ( pExit = pRoomIndex->exit[door] )
							&& pExit->u1.to_room ) {
						int locks = 0;

						/* HACK : TO PREVENT EX_LOCKED etc without EX_ISDOOR
						   to stop booting the mud */
						if ( IS_SET ( pExit->rs_flags, EX_CLOSED )
								||   IS_SET ( pExit->rs_flags, EX_LOCKED )
								||   IS_SET ( pExit->rs_flags, EX_PICKPROOF )
								||   IS_SET ( pExit->rs_flags, EX_NOPASS )
								||   IS_SET ( pExit->rs_flags, EX_EASY )
								||   IS_SET ( pExit->rs_flags, EX_HARD )
								||   IS_SET ( pExit->rs_flags, EX_INFURIATING )
								||   IS_SET ( pExit->rs_flags, EX_NOCLOSE )
								||   IS_SET ( pExit->rs_flags, EX_NOLOCK ) )
						{ SET_BIT ( pExit->rs_flags, EX_ISDOOR ); }
						else
						{ REMOVE_BIT ( pExit->rs_flags, EX_ISDOOR ); }

						/* THIS SUCKS but it's backwards compatible */
						/* NOTE THAT EX_NOCLOSE NOLOCK etc aren't being saved */
						if ( IS_SET ( pExit->rs_flags, EX_ISDOOR )
								&& ( !IS_SET ( pExit->rs_flags, EX_PICKPROOF ) )
								&& ( !IS_SET ( pExit->rs_flags, EX_NOPASS ) ) )
						{ locks = 1; }
						if ( IS_SET ( pExit->rs_flags, EX_ISDOOR )
								&& ( IS_SET ( pExit->rs_flags, EX_PICKPROOF ) )
								&& ( !IS_SET ( pExit->rs_flags, EX_NOPASS ) ) )
						{ locks = 2; }
						if ( IS_SET ( pExit->rs_flags, EX_ISDOOR )
								&& ( !IS_SET ( pExit->rs_flags, EX_PICKPROOF ) )
								&& ( IS_SET ( pExit->rs_flags, EX_NOPASS ) ) )
						{ locks = 3; }
						if ( IS_SET ( pExit->rs_flags, EX_ISDOOR )
								&& ( IS_SET ( pExit->rs_flags, EX_PICKPROOF ) )
								&& ( IS_SET ( pExit->rs_flags, EX_NOPASS ) ) )
						{ locks = 4; }

						fprintf ( fp, "D%d\n",      pExit->orig_door );
						fprintf ( fp, "%s~\n",      fix_string ( pExit->description ) );
						fprintf ( fp, "%s~\n",      pExit->keyword );
						fprintf ( fp, "%d %d %d\n", locks,
								  pExit->key,
								  pExit->u1.to_room->vnum );
					}
				}
				if ( pRoomIndex->mana_rate != 100 || pRoomIndex->heal_rate != 100 )
					fprintf ( fp, "M %d H %d\n", pRoomIndex->mana_rate,
							  pRoomIndex->heal_rate );
				if ( pRoomIndex->clan > 0 )
				{ fprintf ( fp, "C %s~\n" , clan_table[pRoomIndex->clan].name ); }

				if ( !IS_NULLSTR ( pRoomIndex->owner ) )
				{ fprintf ( fp, "O %s~\n" , pRoomIndex->owner ); }

				fprintf ( fp, "S\n" );
			}
		}
	}
	fprintf ( fp, "#0\n\n\n\n" );
	return;
}



/*****************************************************************************
 Name:		save_specials
 Purpose:	Save #SPECIALS section of area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_specials ( FILE *fp, Zone *pArea )
{
	int iHash;
	NPCData *pMobIndex;

	fprintf ( fp, "#SPECIALS\n" );

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next ) {
			if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun ) {
#if defined( VERBOSE )
				fprintf ( fp, "M %d %s Load to: %s\n", pMobIndex->vnum,
						  spec_name ( pMobIndex->spec_fun ),
						  pMobIndex->short_descr );
#else
				fprintf ( fp, "M %d %s\n", pMobIndex->vnum,
						  spec_name ( pMobIndex->spec_fun ) );
#endif
			}
		}
	}

	fprintf ( fp, "S\n\n\n\n" );
	return;
}



/*
 * This function is obsolete.  It it not needed but has been left here
 * for historical reasons.  It is used currently for the same reason.
 *
 * I don't think it's obsolete in ROM -- Hugin.
 */
void save_door_resets ( FILE *fp, Zone *pArea )
{
	int iHash;
	RoomData *pRoomIndex;
	Exit *pExit;
	int door;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next ) {
			if ( pRoomIndex->area == pArea ) {
				for ( door = 0; door < MAX_DIR; door++ ) {
					if ( ( pExit = pRoomIndex->exit[door] )
							&& pExit->u1.to_room
							&& ( IS_SET ( pExit->rs_flags, EX_CLOSED )
								 || IS_SET ( pExit->rs_flags, EX_LOCKED ) ) )
#if defined( VERBOSE )
						fprintf ( fp, "D 0 %d %d %d The %s door of %s is %s\n",
								  pRoomIndex->vnum,
								  pExit->orig_door,
								  IS_SET ( pExit->rs_flags, EX_LOCKED ) ? 2 : 1,
								  dir_name[ pExit->orig_door ],
								  pRoomIndex->name,
								  IS_SET ( pExit->rs_flags, EX_LOCKED ) ? "closed and locked"
								  : "closed" );
#endif
#if !defined( VERBOSE )
					fprintf ( fp, "D 0 %d %d %d\n",
							  pRoomIndex->vnum,
							  pExit->orig_door,
							  IS_SET ( pExit->rs_flags, EX_LOCKED ) ? 2 : 1 );
#endif
				}
			}
		}
	}
	return;
}




/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_resets ( FILE *fp, Zone *pArea )
{
	Reset *pReset;
	NPCData *pLastMob = NULL;
	RoomData *pRoom;
	ItemData *pLastObj = NULL;
	int iHash;

	fprintf ( fp, "#RESETS\n" );

	save_door_resets ( fp, pArea );

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next ) {
			if ( pRoom->area == pArea ) {
				for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next ) {
					switch ( pReset->command ) {
						default:
							log_hd ( LOG_ERROR, Format ( "Save_resets: bad command %c.", pReset->command ) );
							break;

						case 'M':
							pLastMob = get_mob_index ( pReset->arg1 );
							fprintf ( fp, "M 0 %d %d %d %d\n",
									  pReset->arg1,
									  pReset->arg2,
									  pReset->arg3,
									  pReset->arg4 );
							break;

						case 'O':
							pLastObj = get_obj_index ( pReset->arg1 );

							// -- we log it, but we do not crash or suicide here to prevent save file corruption.
							if ( !pLastObj ) {
								log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "%s: NULL item index(o) for vnum %d", __FUNCTION__, pReset->arg1 ) );
							}
							pRoom = get_room_index ( pReset->arg3 );
							fprintf ( fp, "O 0 %d 0 %d\n",
									  pReset->arg1,
									  pReset->arg3 );
							break;

						case 'P':
							pLastObj = get_obj_index ( pReset->arg1 );

							// -- we log it, but we do not crash or suicide here to prevent save file corruption.
							if ( !pLastObj ) {
								log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "%s: NULL item index(p) for vnum %d", __FUNCTION__, pReset->arg1 ) );
							}
							fprintf ( fp, "P 0 %d %d %d %d\n",
									  pReset->arg1,
									  pReset->arg2,
									  pReset->arg3,
									  pReset->arg4 );
							break;

						case 'G':
							fprintf ( fp, "G 0 %d 0\n", pReset->arg1 );
							if ( !pLastMob ) {
								log_hd ( LOG_ERROR, Format ( "Save_resets: !NO_MOB! in [%s]", pArea->file_name ) );
							}
							break;

						case 'E':
							fprintf ( fp, "E 0 %d 0 %d\n",
									  pReset->arg1,
									  pReset->arg3 );
							if ( !pLastMob ) {
								log_hd ( LOG_ERROR, Format ( "Save_resets: !NO_MOB! in [%s]", pArea->file_name ) );
							}
							break;

						case 'D':
							break;

						case 'R':
							pRoom = get_room_index ( pReset->arg1 );
							fprintf ( fp, "R 0 %d %d\n",
									  pReset->arg1,
									  pReset->arg2 );
							break;
					}
				}
			}	/* End if correct area */
		}	/* End for pRoom */
	}	/* End for iHash */
	fprintf ( fp, "S\n\n\n\n" );
	return;
}



/*****************************************************************************
 Name:		save_shops
 Purpose:	Saves the #SHOPS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_shops ( FILE *fp, Zone *pArea )
{
	SHOP_DATA *pShopIndex;
	NPCData *pMobIndex;
	int iTrade;
	int iHash;

	fprintf ( fp, "#SHOPS\n" );

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next ) {
			if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop ) {
				pShopIndex = pMobIndex->pShop;

				fprintf ( fp, "%d ", pShopIndex->keeper );
				for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ ) {
					if ( pShopIndex->buy_type[iTrade] != 0 ) {
						fprintf ( fp, "%d ", pShopIndex->buy_type[iTrade] );
					} else
					{ fprintf ( fp, "0 " ); }
				}
				fprintf ( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
				fprintf ( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
			}
		}
	}

	fprintf ( fp, "0\n\n\n\n" );
	return;
}

void save_helps ( FILE *fp, HELP_AREA *ha )
{
	HELP_DATA *help = ha->first;

	fprintf ( fp, "#HELPS\n" );

	for ( ; help; help = help->next_area ) {
		fprintf ( fp, "%d %s~\n", help->level, help->keyword );
		fprintf ( fp, "%s~\n\n", fix_string ( help->text ) );
	}

	fprintf ( fp, "-1 $~\n\n" );

	ha->changed = FALSE;

	return;
}

void save_other_helps ( Creature *ch )
{
	extern HELP_AREA * had_list;
	HELP_AREA *ha;
	FILE *fp;

	for ( ha = had_list; ha; ha = ha->next )
		if ( ha->changed == TRUE ) {
			fp = fopen ( Format ( "%s%s", WORLD_DIR, ha->filename ), "w" );

			if ( !fp ) {
				ReportErrno ( ha->filename );
				return;
			}

			save_helps ( fp, ha );

			if ( ch )
			{ printf_to_char ( ch, "%s\n\r", ha->filename ); }

			fprintf ( fp, "#$\n" );
			fclose ( fp );
		}

	return;
}

/*****************************************************************************
 Name:		save_area
 Purpose:	Save an area, note that this format is new.
 Called by:	cmd_asave(olc_save.c).
 ****************************************************************************/
void save_area ( Zone *pArea )
{
	FILE *fp;

	fclose ( fpReserve );
	if ( ! ( fp = fopen ( Format ( "%s%s", WORLD_DIR, pArea->file_name ), "w" ) ) ) {
		log_hd ( LOG_ERROR, Format ( "save_area: unable to open %s for writing", pArea->file_name ) );
		ReportErrno ( pArea->file_name );
	}

	fprintf ( fp, "#AREADATA\n" );
	fprintf ( fp, "Name %s~\n",        pArea->name );
	fprintf ( fp, "Builders %s~\n",        fix_string ( pArea->builders ) );
	fprintf ( fp, "VNUMs %d %d\n",      pArea->min_vnum, pArea->max_vnum );
	fprintf ( fp, "Credits %s~\n",	 pArea->credits );
	fprintf ( fp, "Security %d\n",         pArea->security );
	fprintf ( fp, "End\n\n\n\n" );

	save_mobiles ( fp, pArea );
	save_objects ( fp, pArea );
	save_rooms ( fp, pArea );
	save_specials ( fp, pArea );
	save_resets ( fp, pArea );
	save_shops ( fp, pArea );
	save_mobprogs ( fp, pArea );

	if ( pArea->helps && pArea->helps->first )
	{ save_helps ( fp, pArea->helps ); }

	fprintf ( fp, "#$\n" );

	fclose ( fp );
	fpReserve = fopen ( NULL_FILE, "r" );
	return;
}


/*****************************************************************************
 Name:		cmd_asave
 Purpose:	Entry point for saving area data.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
DefineCommand ( cmd_asave )
{
	char arg1 [MAX_INPUT_LENGTH];
	Zone *pArea;
	int value, sec;

	value = sec = 0;

	if ( !ch )       /* Do an autosave */
	{ sec = 9; }
	else if ( !IS_NPC ( ch ) )
	{ sec = ch->pcdata->security; }
	else
	{ sec = 0; }

	/*    {
		save_area_list();
		for( pArea = area_first; pArea; pArea = pArea->next )
		{
		    save_area( pArea );
		    REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
		}
		return;
	    } */

	smash_tilde ( argument );
	strcpy ( arg1, argument );

	if ( arg1[0] == '\0' ) {
		if ( ch ) {
			writeBuffer ( "Syntax:\n\r", ch );
			writeBuffer ( "  asave <vnum>   - saves a particular area\n\r",	ch );
			writeBuffer ( "  asave list     - saves the area.lst file\n\r",	ch );
			writeBuffer ( "  asave area     - saves the area being edited\n\r",	ch );
			writeBuffer ( "  asave changed  - saves all changed zones\n\r",	ch );
			writeBuffer ( "  asave world    - saves the world! (db dump)\n\r",	ch );
			writeBuffer ( "\n\r", ch );
		}

		return;
	}

	/* Snarf the value (which need not be numeric). */
	value = atoi ( arg1 );
	if ( ! ( pArea = get_area_data ( value ) ) && is_number ( arg1 ) ) {
		if ( ch )
		{ writeBuffer ( "That area does not exist.\n\r", ch ); }
		return;
	}

	/* Save area of given vnum. */
	/* ------------------------ */
	if ( is_number ( arg1 ) ) {
		if ( ch && !IS_BUILDER ( ch, pArea ) ) {
			writeBuffer ( "You are not a builder for this area.\n\r", ch );
			return;
		}

		save_area_list();
		save_area ( pArea );

		return;
	}

	/* Save the world, only authorized areas. */
	/* -------------------------------------- */
	if ( !str_cmp ( "world", arg1 ) ) {
		save_area_list();
		for ( pArea = area_first; pArea; pArea = pArea->next ) {
			/* Builder must be assigned this area. */
			if ( ch && !IS_BUILDER ( ch, pArea ) )
			{ continue; }

			save_area ( pArea );
			REMOVE_BIT ( pArea->area_flags, AREA_CHANGED );
		}

		if ( ch )
		{ writeBuffer ( "You saved the world.\n\r", ch ); }

		save_other_helps ( NULL );

		return;
	}

	/* Save changed areas, only authorized areas. */
	/* ------------------------------------------ */
	if ( !str_cmp ( "changed", arg1 ) ) {
		char buf[MAX_INPUT_LENGTH];

		save_area_list();

		if ( ch )
		{ writeBuffer ( "Saved zones:\n\r", ch ); }
		else
		{ log_hd ( LOG_BASIC, "Saved zones:" ); }

		sprintf ( buf, "None.\n\r" );

		for ( pArea = area_first; pArea; pArea = pArea->next ) {
			/* Builder must be assigned this area. */
			if ( ch && !IS_BUILDER ( ch, pArea ) )
			{ continue; }

			/* Save changed areas. */
			if ( IS_SET ( pArea->area_flags, AREA_CHANGED ) ) {
				save_area ( pArea );
				sprintf ( buf, "%24s - '%s'", pArea->name, pArea->file_name );
				if ( ch ) {
					writeBuffer ( buf, ch );
					writeBuffer ( "\n\r", ch );
				} else
				{ log_hd ( LOG_BASIC, buf ); }
				REMOVE_BIT ( pArea->area_flags, AREA_CHANGED );
			}
		}

		save_other_helps ( ch );

		if ( !str_cmp ( buf, "None.\n\r" ) ) {
			if ( ch )
			{ writeBuffer ( buf, ch ); }
			else
			{ log_hd ( LOG_BASIC, "None." ); }
		}
		return;
	}

	/* Save the area.lst file. */
	/* ----------------------- */
	if ( !str_cmp ( arg1, "list" ) ) {
		save_area_list();
		return;
	}

	/* Save area being edited, if authorized. */
	/* -------------------------------------- */
	if ( !str_cmp ( arg1, "area" ) ) {
		if ( !ch || !ch->desc )
		{ return; }

		/* Is character currently editing. */
		if ( ch->desc->editor == ED_NONE ) {
			writeBuffer ( "You are not editing an area, "
						  "therefore an area vnum is required.\n\r", ch );
			return;
		}

		/* Find the area to save. */
		switch ( ch->desc->editor ) {
			case ED_AREA:
				pArea = ( Zone * ) ch->desc->pEdit;
				break;
			case ED_ROOM:
				pArea = ch->in_room->area;
				break;
			case ED_OBJECT:
				pArea = ( ( ItemData * ) ch->desc->pEdit )->area;
				break;
			case ED_MOBILE:
				pArea = ( ( NPCData * ) ch->desc->pEdit )->area;
				break;
			case ED_HELP:
				writeBuffer ( "Saving helps: ", ch );
				save_other_helps ( ch );
				return;
			default:
				pArea = ch->in_room->area;
				break;
		}

		if ( !IS_BUILDER ( ch, pArea ) ) {
			writeBuffer ( "You are not a builder for this area.\n\r", ch );
			return;
		}

		save_area_list();
		save_area ( pArea );
		REMOVE_BIT ( pArea->area_flags, AREA_CHANGED );
		writeBuffer ( "Area saved.\n\r", ch );
		return;
	}

	/* Show correct syntax. */
	/* -------------------- */
	if ( ch )
	{ cmd_function ( ch, cmd_asave, "" ); }

	return;
}
