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

extern int flag_lookup args ( ( const char *name, const struct flag_type *flag_table ) );

/* values for db2.c */
struct		social_type	social_table		[MAX_SOCIALS];
int		social_count;

/* snarf a socials file */
void load_socials ( FILE *fp )
{
	for ( ; ; ) {
		struct social_type social;
		char *temp;
		/* clear social */
		social.char_no_arg = NULL;
		social.others_no_arg = NULL;
		social.char_found = NULL;
		social.others_found = NULL;
		social.vict_found = NULL;
		social.char_not_found = NULL;
		social.char_auto = NULL;
		social.others_auto = NULL;

		temp = fread_word ( fp );
		if ( !strcmp ( temp, "#0" ) )
		{ return; }  /* done */
#if defined(social_debug)
		else
		{ fprintf ( stderr, "%s\n\r", temp ); }
#endif

		strcpy ( social.name, temp );
		fread_to_eol ( fp );

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.char_no_arg = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.char_no_arg = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.others_no_arg = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.others_no_arg = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.char_found = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.char_found = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.others_found = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.others_found = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.vict_found = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.vict_found = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.char_not_found = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.char_not_found = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.char_auto = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.char_auto = temp; }

		temp = fread_string_eol ( fp );
		if ( !strcmp ( temp, "$" ) )
		{ social.others_auto = NULL; }
		else if ( !strcmp ( temp, "#" ) ) {
			social_table[social_count] = social;
			social_count++;
			continue;
		} else
		{ social.others_auto = temp; }

		social_table[social_count] = social;
		social_count++;
	}
	return;
}






/*
 * Snarf a mob section.  new style
 */
void load_mobiles ( FILE *fp )
{
	NPCData *pMobIndex;

	if ( !area_last ) { /* OLC */
		log_hd ( LOG_ERROR, "Load_mobiles: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		sh_int vnum;
		char letter;
		int iHash;

		letter                          = fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_mobiles: # not found." );
			exit ( 1 );
		}

		vnum                            = fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_mob_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_mobiles: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pMobIndex, NPCData, 1 );

		pMobIndex->vnum                 = vnum;
		pMobIndex->area                 = area_last;               /* OLC */
		pMobIndex->new_format		= TRUE;
		newmobs++;
		pMobIndex->player_name          = fread_string ( fp );
		pMobIndex->short_descr          = fread_string ( fp );
		pMobIndex->long_descr           = fread_string ( fp );
		pMobIndex->description          = fread_string ( fp );
		pMobIndex->race		 	= race_lookup ( fread_string ( fp ) );

		pMobIndex->long_descr[0]        = UPPER ( pMobIndex->long_descr[0] );
		pMobIndex->description[0]       = UPPER ( pMobIndex->description[0] );

		pMobIndex->material_flags				= fread_flag ( fp );
		pMobIndex->random								= fread_flag ( fp );
		pMobIndex->act                  = fread_flag ( fp ) | ACT_IS_NPC
										  | race_table[pMobIndex->race].act;
		pMobIndex->affected_by          = fread_flag ( fp )
										  | race_table[pMobIndex->race].aff;
		pMobIndex->pShop                = NULL;
		pMobIndex->alignment            = fread_number ( fp );
		pMobIndex->group                = fread_number ( fp );

		pMobIndex->level                = fread_number ( fp );
		pMobIndex->hitroll              = fread_number ( fp );

		/* read hit dice */
		pMobIndex->hit[DICE_NUMBER]     = fread_number ( fp );
		/* 'd'          */                fread_letter ( fp );
		pMobIndex->hit[DICE_TYPE]   	= fread_number ( fp );
		/* '+'          */                fread_letter ( fp );
		pMobIndex->hit[DICE_BONUS]      = fread_number ( fp );

		/* read mana dice */
		pMobIndex->mana[DICE_NUMBER]	= fread_number ( fp );
		fread_letter ( fp );
		pMobIndex->mana[DICE_TYPE]	= fread_number ( fp );
		fread_letter ( fp );
		pMobIndex->mana[DICE_BONUS]	= fread_number ( fp );

		/* read damage dice */
		pMobIndex->damage[DICE_NUMBER]	= fread_number ( fp );
		fread_letter ( fp );
		pMobIndex->damage[DICE_TYPE]	= fread_number ( fp );
		fread_letter ( fp );
		pMobIndex->damage[DICE_BONUS]	= fread_number ( fp );
		pMobIndex->dam_type		= attack_lookup ( fread_word ( fp ) );

		/* read armor archetype */
		pMobIndex->ac[AC_PIERCE]	= fread_number ( fp ) * 10;
		pMobIndex->ac[AC_BASH]		= fread_number ( fp ) * 10;
		pMobIndex->ac[AC_SLASH]		= fread_number ( fp ) * 10;
		pMobIndex->ac[AC_EXOTIC]	= fread_number ( fp ) * 10;

		/* read flags and add in data from the race table */
		pMobIndex->off_flags		= fread_flag ( fp )
									  | race_table[pMobIndex->race].off;
		pMobIndex->imm_flags		= fread_flag ( fp )
									  | race_table[pMobIndex->race].imm;
		pMobIndex->res_flags		= fread_flag ( fp )
									  | race_table[pMobIndex->race].res;
		pMobIndex->vuln_flags		= fread_flag ( fp )
									  | race_table[pMobIndex->race].vuln;

		/* vital statistics */
		pMobIndex->start_pos		= position_lookup ( fread_word ( fp ) );
		pMobIndex->default_pos		= position_lookup ( fread_word ( fp ) );
		pMobIndex->sex			= sex_lookup ( fread_word ( fp ) );

		pMobIndex->wealth		= fread_number ( fp );

		pMobIndex->form			= fread_flag ( fp )
								  | race_table[pMobIndex->race].form;
		pMobIndex->parts		= fread_flag ( fp )
								  | race_table[pMobIndex->race].parts;
		/* size */
		CHECK_POS ( pMobIndex->size, size_lookup ( fread_word ( fp ) ), "size" );
		/*	pMobIndex->size			= size_lookup(fread_word(fp)); */

		while ( true ) {
			letter = fread_letter ( fp );

			if ( letter == 'F' ) {
				char *word;
				long vector;

				word                    = fread_word ( fp );
				vector			= fread_flag ( fp );

				if ( !str_prefix ( word, "act" ) )
				{ REMOVE_BIT ( pMobIndex->act, vector ); }
				else if ( !str_prefix ( word, "aff" ) )
				{ REMOVE_BIT ( pMobIndex->affected_by, vector ); }
				else if ( !str_prefix ( word, "off" ) )
				{ REMOVE_BIT ( pMobIndex->off_flags, vector ); }
				else if ( !str_prefix ( word, "imm" ) )
				{ REMOVE_BIT ( pMobIndex->imm_flags, vector ); }
				else if ( !str_prefix ( word, "res" ) )
				{ REMOVE_BIT ( pMobIndex->res_flags, vector ); }
				else if ( !str_prefix ( word, "vul" ) )
				{ REMOVE_BIT ( pMobIndex->vuln_flags, vector ); }
				else if ( !str_prefix ( word, "for" ) )
				{ REMOVE_BIT ( pMobIndex->form, vector ); }
				else if ( !str_prefix ( word, "par" ) )
				{ REMOVE_BIT ( pMobIndex->parts, vector ); }
				else {
					log_hd ( LOG_ERROR, "Flag remove: flag not found." );
					exit ( 1 );
				}
			} else if ( letter == 'M' ) {
				MPROG_LIST *pMprog;
				char *word;
				int trigger = 0;

				ALLOC_DATA ( pMprog, MPROG_LIST, 1 );

				word   		    = fread_word ( fp );
				if ( ( trigger = flag_lookup ( word, mprog_flags ) ) == NO_FLAG ) {
					log_hd ( LOG_ERROR, "MOBprogs: invalid trigger." );
					exit ( 1 );
				}
				SET_BIT ( pMobIndex->mprog_flags, trigger );
				pMprog->trig_type   = trigger;
				pMprog->vnum        = fread_number ( fp );
				pMprog->trig_phrase = fread_string ( fp );
				pMprog->next        = pMobIndex->mprogs;
				pMobIndex->mprogs   = pMprog;
			} else if ( letter == 'L' ) {
				pMobIndex->repop_percent = fread_number ( fp );
			} else {
				ungetc ( letter, fp );
				break;
			}
		}

		iHash                   = vnum % MAX_KEY_HASH;
		pMobIndex->next         = mob_index_hash[iHash];
		mob_index_hash[iHash]   = pMobIndex;
		top_mob_index++;
		top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;  /* OLC */
		assign_area_vnum ( vnum );                                 /* OLC */
		kill_table[URANGE ( 0, pMobIndex->level, MAX_LEVEL - 1 )].number++;
	}

	return;
}

/*
 * Snarf an obj section. new style
 */
void load_objects ( FILE *fp )
{
	ItemData *pObjIndex;

	if ( !area_last ) { /* OLC */
		log_hd ( LOG_ERROR, "Load_objects: no #AREA seen yet." );
		exit ( 1 );
	}

	while ( true ) {
		sh_int vnum;
		char letter;
		int iHash;

		letter                          = fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_objects: # not found." );
			exit ( 1 );
		}

		vnum                            = fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_obj_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_objects: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pObjIndex, ItemData, 1 );

		pObjIndex->vnum                 = vnum;
		pObjIndex->area                 = area_last;            /* OLC */
		pObjIndex->new_format           = TRUE;
		pObjIndex->reset_num		= 0;
		newobjs++;
		pObjIndex->name                 = fread_string ( fp );
		pObjIndex->short_descr          = fread_string ( fp );
		pObjIndex->description          = fread_string ( fp );

		CHECK_POS ( pObjIndex->item_type, item_lookup ( fread_word ( fp ) ), "item_type" );
		pObjIndex->extra_flags          = fread_flag ( fp );
		pObjIndex->wear_flags           = fread_flag ( fp );
		pObjIndex->material_flags       = fread_flag ( fp );
		switch ( pObjIndex->item_type ) {
			case ITEM_WEAPON:
				pObjIndex->value[0]		= weapon_type ( fread_word ( fp ) );
				pObjIndex->value[1]		= fread_number ( fp );
				pObjIndex->value[2]		= fread_number ( fp );
				pObjIndex->value[3]		= attack_lookup ( fread_word ( fp ) );
				pObjIndex->value[4]		= fread_flag ( fp );
				break;
			case ITEM_CONTAINER:
				pObjIndex->value[0]		= fread_number ( fp );
				pObjIndex->value[1]		= fread_flag ( fp );
				pObjIndex->value[2]		= fread_number ( fp );
				pObjIndex->value[3]		= fread_number ( fp );
				pObjIndex->value[4]		= fread_number ( fp );
				break;
			case ITEM_DRINK_CON:
			case ITEM_FOUNTAIN:
				pObjIndex->value[0]         = fread_number ( fp );
				pObjIndex->value[1]         = fread_number ( fp );
				CHECK_POS ( pObjIndex->value[2], liq_lookup ( fread_word ( fp ) ), "liq_lookup" );
				pObjIndex->value[3]         = fread_number ( fp );
				pObjIndex->value[4]         = fread_number ( fp );
				break;
			case ITEM_WAND:
			case ITEM_STAFF:
				pObjIndex->value[0]		= fread_number ( fp );
				pObjIndex->value[1]		= fread_number ( fp );
				pObjIndex->value[2]		= fread_number ( fp );
				pObjIndex->value[3]		= skill_lookup ( fread_word ( fp ) );
				pObjIndex->value[4]		= fread_number ( fp );
				break;
			case ITEM_POTION:
			case ITEM_PILL:
			case ITEM_SCROLL:
				pObjIndex->value[0]		= fread_number ( fp );
				pObjIndex->value[1]		= skill_lookup ( fread_word ( fp ) );
				pObjIndex->value[2]		= skill_lookup ( fread_word ( fp ) );
				pObjIndex->value[3]		= skill_lookup ( fread_word ( fp ) );
				pObjIndex->value[4]		= skill_lookup ( fread_word ( fp ) );
				break;
			default:
				pObjIndex->value[0]             = fread_flag ( fp );
				pObjIndex->value[1]             = fread_flag ( fp );
				pObjIndex->value[2]             = fread_flag ( fp );
				pObjIndex->value[3]             = fread_flag ( fp );
				pObjIndex->value[4]		    = fread_flag ( fp );
				break;
		}
		pObjIndex->level		= fread_number ( fp );
		pObjIndex->weight               = fread_number ( fp );
		pObjIndex->cost                 = fread_number ( fp );

		/* condition */
		letter 				= fread_letter ( fp );
		switch ( letter ) {
			case ( 'P' ) :
				pObjIndex->condition = 100;
				break;
			case ( 'G' ) :
				pObjIndex->condition =  90;
				break;
			case ( 'A' ) :
				pObjIndex->condition =  75;
				break;
			case ( 'W' ) :
				pObjIndex->condition =  50;
				break;
			case ( 'D' ) :
				pObjIndex->condition =  25;
				break;
			case ( 'B' ) :
				pObjIndex->condition =  10;
				break;
			case ( 'R' ) :
				pObjIndex->condition =   0;
				break;
			default:
				pObjIndex->condition = 100;
				break;
		}

		while ( true ) {
			char letter;

			letter = fread_letter ( fp );

			if ( letter == 'A' ) {
				Affect *paf;

				ALLOC_DATA ( paf, Affect, 1 );

				paf->where		= TO_OBJECT;
				paf->type               = -1;
				paf->level              = pObjIndex->level;
				paf->duration           = -1;
				paf->location           = fread_number ( fp );
				paf->modifier           = fread_number ( fp );
				paf->bitvector          = 0;
				paf->next               = pObjIndex->affected;
				pObjIndex->affected     = paf;
				top_affect++;
			}

			else if ( letter == 'F' ) {
				Affect *paf;

				ALLOC_DATA ( paf, Affect, 1 );

				letter 			= fread_letter ( fp );
				switch ( letter ) {
					case 'A':
						paf->where          = TO_AFFECTS;
						break;
					case 'I':
						paf->where		= TO_IMMUNE;
						break;
					case 'R':
						paf->where		= TO_RESIST;
						break;
					case 'V':
						paf->where		= TO_VULN;
						break;
					default:
						log_hd ( LOG_ERROR, "Load_objects: Bad where on flag set." );
						exit ( 1 );
				}
				paf->type               = -1;
				paf->level              = pObjIndex->level;
				paf->duration           = -1;
				paf->location           = fread_number ( fp );
				paf->modifier           = fread_number ( fp );
				paf->bitvector          = fread_flag ( fp );
				paf->next               = pObjIndex->affected;
				pObjIndex->affected     = paf;
				top_affect++;
			}

			else if ( letter == 'E' ) {
				DescriptionData *ed;

				ALLOC_DATA ( ed, DescriptionData, 1 );

				ed->keyword             = fread_string ( fp );
				ed->description         = fread_string ( fp );
				ed->next                = pObjIndex->extra_descr;
				pObjIndex->extra_descr  = ed;
				top_ed++;
			} else if ( letter == 'R' ) {
				pObjIndex->requirements[SIZ_REQ]	= fread_number ( fp );
				pObjIndex->requirements[STR_REQ]	= fread_number ( fp );
				pObjIndex->requirements[DEX_REQ]	= fread_number ( fp );
				pObjIndex->requirements[CON_REQ]	= fread_number ( fp );
				pObjIndex->requirements[INT_REQ]	= fread_number ( fp );
				pObjIndex->requirements[WIS_REQ]	= fread_number ( fp );
			} else if ( letter == 'L' ) {
				pObjIndex->repop_percent = fread_number ( fp );
			} else {
				ungetc ( letter, fp );
				break;
			}
		}

		iHash                   = vnum % MAX_KEY_HASH;
		pObjIndex->next         = obj_index_hash[iHash];
		obj_index_hash[iHash]   = pObjIndex;
		top_obj_index++;
		top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;   /* OLC */
		assign_area_vnum ( vnum );                                  /* OLC */
	}

	return;
}

/*****************************************************************************
 Name:	        convert_objects
 Purpose:	Converts all old format objects to new format
 Called by:	boot_db (db.c).
 Note:          Loops over all resets to find the level of the mob
                loaded before the object to determine the level of
                the object.
		It might be better to update the levels in load_resets().
		This function is not pretty.. Sorry about that :)
 Author:        Hugin
 ****************************************************************************/
void convert_objects ( void )
{
	int vnum;
	Zone  *pArea;
	Reset *pReset;
	NPCData *pMob = NULL;
	ItemData *pObj;
	RoomData *pRoom;

	if ( newobjs == top_obj_index ) { return; } /* all objects in new format */

	for ( pArea = area_first; pArea; pArea = pArea->next ) {
		for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ) {
			if ( ! ( pRoom = get_room_index ( vnum ) ) ) { continue; }

			for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next ) {
				switch ( pReset->command ) {
					case 'M':
						if ( ! ( pMob = get_mob_index ( pReset->arg1 ) ) )
						{ log_hd ( LOG_ERROR, Format ( "Convert_objects: 'M': bad vnum %d.", pReset->arg1 ) ); }
						break;

					case 'O':
						if ( ! ( pObj = get_obj_index ( pReset->arg1 ) ) ) {
							log_hd ( LOG_ERROR, Format ( "Convert_objects: 'O': bad vnum %d.", pReset->arg1 ) );
							break;
						}

						if ( pObj->new_format )
						{ continue; }

						if ( !pMob ) {
							log_hd ( LOG_ERROR, "Convert_objects: 'O': No mob reset yet." );
							break;
						}

						pObj->level = pObj->level < 1 ? pMob->level - 2
									  : UMIN ( pObj->level, pMob->level - 2 );
						break;

					case 'P': {
							ItemData *pObj, *pObjTo;

							if ( ! ( pObj = get_obj_index ( pReset->arg1 ) ) ) {
								log_hd ( LOG_ERROR, Format ( "Convert_objects: 'P': bad vnum %d.", pReset->arg1 ) );
								break;
							}

							if ( pObj->new_format )
							{ continue; }

							if ( ! ( pObjTo = get_obj_index ( pReset->arg3 ) ) ) {
								log_hd ( LOG_ERROR, Format ( "Convert_objects: 'P': bad vnum %d.", pReset->arg3 ) );
								break;
							}

							pObj->level = pObj->level < 1 ? pObjTo->level
										  : UMIN ( pObj->level, pObjTo->level );
						}
						break;

					case 'G':
					case 'E':
						if ( ! ( pObj = get_obj_index ( pReset->arg1 ) ) ) {
							log_hd ( LOG_ERROR, Format ( "Convert_objects: 'E' or 'G': bad vnum %d.", pReset->arg1 ) );
							break;
						}

						if ( !pMob ) {
							log_hd ( LOG_ERROR, Format ( "Convert_objects: 'E' or 'G': null mob for vnum %d.",
														 pReset->arg1 ) );
							break;
						}

						if ( pObj->new_format )
						{ continue; }

						if ( pMob->pShop ) {
							switch ( pObj->item_type ) {
								default:
									pObj->level = UMAX ( 0, pObj->level );
									break;
								case ITEM_PILL:
								case ITEM_POTION:
									pObj->level = UMAX ( 5, pObj->level );
									break;
								case ITEM_SCROLL:
								case ITEM_ARMOR:
								case ITEM_WEAPON:
									pObj->level = UMAX ( 10, pObj->level );
									break;
								case ITEM_WAND:
								case ITEM_TREASURE:
									pObj->level = UMAX ( 15, pObj->level );
									break;
								case ITEM_STAFF:
									pObj->level = UMAX ( 20, pObj->level );
									break;
							}
						} else
							pObj->level = pObj->level < 1 ? pMob->level
										  : UMIN ( pObj->level, pMob->level );
						break;
				} /* switch ( pReset->command ) */
			}
		}
	}

	/* do the conversion: */

	for ( pArea = area_first; pArea ; pArea = pArea->next )
		for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
			if ( ( pObj = get_obj_index ( vnum ) ) )
				if ( !pObj->new_format )
				{ convert_object ( pObj ); }

	return;
}



/*****************************************************************************
 Name:		convert_object
 Purpose:	Converts an old_format obj to new_format
 Called by:	convert_objects (db2.c).
 Note:          Dug out of create_obj (db.c)
 Author:        Hugin
 ****************************************************************************/
void convert_object ( ItemData *pObjIndex )
{
	int level;
	int number, type;  /* for dice-conversion */

	if ( !pObjIndex || pObjIndex->new_format ) { return; }

	level = pObjIndex->level;

	pObjIndex->level    = UMAX ( 0, pObjIndex->level ); /* just to be sure */
	pObjIndex->cost     = 10 * level;

	switch ( pObjIndex->item_type ) {
		default:
			log_hd ( LOG_ERROR, Format ( "convert_object: vnum %d bad type.", pObjIndex->item_type ) );
			break;

		case ITEM_LIGHT:
		case ITEM_TREASURE:
		case ITEM_FURNITURE:
		case ITEM_TRASH:
		case ITEM_CONTAINER:
		case ITEM_DRINK_CON:
		case ITEM_KEY:
		case ITEM_FOOD:
		case ITEM_BOAT:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
		case ITEM_FOUNTAIN:
		case ITEM_MAP:
		case ITEM_CLOTHING:
		case ITEM_SCROLL:
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			pObjIndex->value[2] = pObjIndex->value[1];
			break;

		case ITEM_WEAPON:

			/*
			 * The conversion below is based on the values generated
			 * in one_hit() (fight.c).  Since I don't want a lvl 50
			 * weapon to do 15d3 damage, the min value will be below
			 * the one in one_hit, and to make up for it, I've made
			 * the max value higher.
			 * (I don't want 15d2 because this will hardly ever roll
			 * 15 or 30, it will only roll damage close to 23.
			 * I can't do 4d8+11, because one_hit there is no dice-
			 * bounus value to set...)
			 *
			 * The conversion below gives:

			 level:   dice      min      max      mean
			   1:     1d8      1( 2)    8( 7)     5( 5)
			   2:     2d5      2( 3)   10( 8)     6( 6)
			   3:     2d5      2( 3)   10( 8)     6( 6)
			   5:     2d6      2( 3)   12(10)     7( 7)
			  10:     4d5      4( 5)   20(14)    12(10)
			  20:     5d5      5( 7)   25(21)    15(14)
			  30:     5d7      5(10)   35(29)    20(20)
			  50:     5d11     5(15)   55(44)    30(30)

			 */

			number = UMIN ( level / 4 + 1, 5 );
			type   = ( level + 7 ) / number;

			pObjIndex->value[1] = number;
			pObjIndex->value[2] = type;
			break;

		case ITEM_ARMOR:
			pObjIndex->value[0] = level / 5 + 3;
			pObjIndex->value[1] = pObjIndex->value[0];
			pObjIndex->value[2] = pObjIndex->value[0];
			break;

		case ITEM_POTION:
		case ITEM_PILL:
			break;

		case ITEM_MONEY:
			pObjIndex->value[0] = pObjIndex->cost;
			break;
	}

	pObjIndex->new_format = TRUE;
	++newobjs;

	return;
}




/*****************************************************************************
 Name:		convert_mobile
 Purpose:	Converts an old_format mob into new_format
 Called by:	load_old_mob (db.c).
 Note:          Dug out of create_mobile (db.c)
 Author:        Hugin
 ****************************************************************************/
void convert_mobile ( NPCData *pMobIndex )
{
	int i;
	int type, number, bonus;
	int level;

	if ( !pMobIndex || pMobIndex->new_format ) { return; }

	level = pMobIndex->level;

	pMobIndex->act              |= ACT_WARRIOR;

	/*
	 * Calculate hit dice.  Gives close to the hitpoints
	 * of old format mobs created with create_mobile()  (db.c)
	 * A high number of dice makes for less variance in mobiles
	 * hitpoints.
	 * (might be a good idea to reduce the max number of dice)
	 *
	 * The conversion below gives:

	   level:     dice         min         max        diff       mean
	     1:       1d2+6       7(  7)     8(   8)     1(   1)     8(   8)
	 2:       1d3+15     16( 15)    18(  18)     2(   3)    17(  17)
	 3:       1d6+24     25( 24)    30(  30)     5(   6)    27(  27)
	 5:      1d17+42     43( 42)    59(  59)    16(  17)    51(  51)
	10:      3d22+96     99( 95)   162( 162)    63(  67)   131(    )
	15:     5d30+161    166(159)   311( 311)   145( 150)   239(    )
	30:    10d61+416    426(419)  1026(1026)   600( 607)   726(    )
	50:    10d169+920   930(923)  2610(2610)  1680(1688)  1770(    )

	The values in parenthesis give the values generated in create_mobile.
	    Diff = max - min.  Mean is the arithmetic mean.
	(hmm.. must be some roundoff error in my calculations.. smurfette got
	 1d6+23 hp at level 3 ? -- anyway.. the values above should be
	 approximately right..)
	 */
	type   = level * level * 27 / 40;
	number = UMIN ( type / 40 + 1, 10 ); /* how do they get 11 ??? */
	type   = UMAX ( 2, type / number );
	bonus  = UMAX ( 0, level * ( 8 + level ) * .9 - number * type );

	pMobIndex->hit[DICE_NUMBER]    = number;
	pMobIndex->hit[DICE_TYPE]      = type;
	pMobIndex->hit[DICE_BONUS]     = bonus;

	pMobIndex->mana[DICE_NUMBER]   = level;
	pMobIndex->mana[DICE_TYPE]     = 10;
	pMobIndex->mana[DICE_BONUS]    = 100;

	/*
	 * Calculate dam dice.  Gives close to the damage
	 * of old format mobs in damage()  (fight.c)
	 */
	type   = level * 7 / 4;
	number = UMIN ( type / 8 + 1, 5 );
	type   = UMAX ( 2, type / number );
	bonus  = UMAX ( 0, level * 9 / 4 - number * type );

	pMobIndex->damage[DICE_NUMBER] = number;
	pMobIndex->damage[DICE_TYPE]   = type;
	pMobIndex->damage[DICE_BONUS]  = bonus;

	switch ( number_range ( 1, 3 ) ) {
		case ( 1 ) :
			pMobIndex->dam_type =  3;
			break;  /* slash  */
		case ( 2 ) :
			pMobIndex->dam_type =  7;
			break;  /* pound  */
		case ( 3 ) :
			pMobIndex->dam_type = 11;
			break;  /* pierce */
	}

	for ( i = 0; i < 3; i++ )
	{ pMobIndex->ac[i]         = interpolate ( level, 100, -100 ); }
	pMobIndex->ac[3]             = interpolate ( level, 100, 0 );  /* exotic */

	pMobIndex->wealth           /= 100;
	pMobIndex->size              = SIZE_MEDIUM;

	pMobIndex->new_format        = TRUE;
	++newmobs;

	return;
}
