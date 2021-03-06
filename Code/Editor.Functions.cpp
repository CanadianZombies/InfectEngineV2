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

const char * mprog_type_to_name ( int type );

#define ALT_FLAGVALUE_SET( _blargh, _table, _arg )		\
	{							\
		int blah = flag_value( _table, _arg );		\
		_blargh = (blah == NO_FLAG) ? 0 : blah;		\
	}

#define ALT_FLAGVALUE_TOGGLE( _blargh, _table, _arg )		\
	{							\
		int blah = flag_value( _table, _arg );		\
		_blargh ^= (blah == NO_FLAG) ? 0 : blah;	\
	}

/* Return TRUE if area changed, FALSE if not. */
#define REDIT( fun )		bool fun( Creature *ch, const char *argument )
#define OEDIT( fun )		bool fun( Creature *ch, const char *argument )
#define MEDIT( fun )		bool fun( Creature *ch, const char *argument )
#define AEDIT( fun )		bool fun( Creature *ch, const char *argument )



struct olc_help_type {
	const char *command;
	const void *structure;
	const char *desc;
};



bool show_version ( Creature *ch, const char *argument )
{
	writeBuffer ( VERSION, ch );
	writeBuffer ( "\n\r", ch );
	writeBuffer ( AUTHOR, ch );
	writeBuffer ( "\n\r", ch );
	writeBuffer ( DATE, ch );
	writeBuffer ( "\n\r", ch );
	writeBuffer ( CREDITS, ch );
	writeBuffer ( "\n\r", ch );

	return FALSE;
}

/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] = {
	{	"area",		area_flags,	 "Area attributes."		 },
	{	"room",		room_flags,	 "Room attributes."		 },
	{	"sector",	sector_flags,	 "Sector types, terrain."	 },
	{	"exit",		exit_flags,	 "Exit types."			 },
	{	"type",		type_flags,	 "Types of objects."		 },
	{	"extra",	extra_flags,	 "Object attributes."		 },
	{	"wear",		wear_flags,	 "Where to wear object."	 },
	{	"spec",		spec_table,	 "Available special programs." 	 },
	{	"sex",		sex_flags,	 "Sexes."			 },
	{	"act",		act_flags,	 "Mobile attributes."		 },
	{	"affect",	affect_flags,	 "Mobile affects."		 },
	{	"wear-loc",	wear_loc_flags,	 "Where mobile wears object."	 },
	{	"spells",	skill_table,	 "Names of current spells." 	 },
	{	"container",	container_flags, "Container status."		 },

	/* ROM specific bits: */
	{	"material",	material_flags,	"Types of materials."	},
	{ "random", random_eq_flags,  "Random Item Generation flags. " },
	{	"armor",	ac_type,	 "Ac for different attacks."	 },
	{   "apply",	apply_flags,	 "Apply flags"			 },
	{	"form",		form_flags,	 "Mobile body form."	         },
	{	"part",		part_flags,	 "Mobile body parts."		 },
	{	"imm",		imm_flags,	 "Mobile immunity."		 },
	{	"res",		res_flags,	 "Mobile resistance."	         },
	{	"vuln",		vuln_flags,	 "Mobile vulnerability."	 },
	{	"off",		off_flags,	 "Mobile offensive behaviour."	 },
	{	"size",		size_flags,	 "Mobile size."			 },
	{   "position",     position_flags,  "Mobile positions."             },
	{   "warchetype",       weapon_archetype,    "Weapon archetype."                 },
	{   "wtype",        weapon_type2,    "Special weapon type."          },
	{	"portal",	portal_flags,	 "Portal types."		 },
	{	"furniture",	furniture_flags, "Furniture types."		 },
	{   "liquid",	liq_table,	 "Liquid types."		 },
	{	"apptype",	apply_types,	 "Apply types."			 },
	{	"weapon",	attack_table,	 "Weapon types."		 },
	{	"mprog",	mprog_flags,	 "MobProgram flags."		 },
	{	NULL,		NULL,		 NULL				 }
};



/*****************************************************************************
 Name:		show_flag_cmds
 Purpose:	Displays settable flags and stats.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds ( Creature *ch, const struct flag_type *flag_table )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int  flag;
	int  col;

	buf1[0] = '\0';
	col = 0;
	for ( flag = 0; flag_table[flag].name != NULL; flag++ ) {
		if ( flag_table[flag].settable ) {
			sprintf ( buf, "%-19.18s", flag_table[flag].name );
			strcat ( buf1, buf );
			if ( ++col % 4 == 0 )
			{ strcat ( buf1, "\n\r" ); }
		}
	}

	if ( col % 4 != 0 )
	{ strcat ( buf1, "\n\r" ); }

	writeBuffer ( buf1, ch );
	return;
}


/*****************************************************************************
 Name:		show_skill_cmds
 Purpose:	Displays all skill functions.
 		Does remove those damn immortal commands from the list.
 		Could be improved by:
 		(1) Adding a check for a particular archetype.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds ( Creature *ch, int tar )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH * 2 ];
	int  sn;
	int  col;

	buf1[0] = '\0';
	col = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ ) {
		if ( !skill_table[sn].name )
		{ break; }

		if ( SameString ( skill_table[sn].name, "reserved" )
				|| skill_table[sn].spell_fun == spell_null )
		{ continue; }

		if ( tar == -1 || skill_table[sn].target == tar ) {
			sprintf ( buf, "%-19.18s", skill_table[sn].name );
			strcat ( buf1, buf );
			if ( ++col % 4 == 0 )
			{ strcat ( buf1, "\n\r" ); }
		}
	}

	if ( col % 4 != 0 )
	{ strcat ( buf1, "\n\r" ); }

	writeBuffer ( buf1, ch );
	return;
}



/*****************************************************************************
 Name:		show_spec_cmds
 Purpose:	Displays settable special functions.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds ( Creature *ch )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int  spec;
	int  col;

	buf1[0] = '\0';
	col = 0;
	writeBuffer ( "Preceed special functions with 'spec_'\n\r\n\r", ch );
	for ( spec = 0; spec_table[spec].function != NULL; spec++ ) {
		sprintf ( buf, "%-19.18s", &spec_table[spec].name[5] );
		strcat ( buf1, buf );
		if ( ++col % 4 == 0 )
		{ strcat ( buf1, "\n\r" ); }
	}

	if ( col % 4 != 0 )
	{ strcat ( buf1, "\n\r" ); }

	writeBuffer ( buf1, ch );
	return;
}



/*****************************************************************************
 Name:		show_help
 Purpose:	Displays help for many tables used in OLC.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_help ( Creature *ch, const char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char spell[MAX_INPUT_LENGTH];
	int cnt;

	argument = ChopC ( argument, arg );
	ChopC ( argument, spell );

	/*
	 * Display syntax.
	 */
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:  ? [command]\n\r\n\r", ch );
		writeBuffer ( "[command]  [description]\n\r", ch );
		for ( cnt = 0; help_table[cnt].command != NULL; cnt++ ) {
			sprintf ( buf, "%-10.10s -%s\n\r",
					  capitalize ( help_table[cnt].command ),
					  help_table[cnt].desc );
			writeBuffer ( buf, ch );
		}
		return FALSE;
	}

	/*
	 * Find the command, show changeable data.
	 * ---------------------------------------
	 */
	for ( cnt = 0; help_table[cnt].command != NULL; cnt++ ) {
		if (  arg[0] == help_table[cnt].command[0]
				&& !str_prefix ( arg, help_table[cnt].command ) ) {
			if ( help_table[cnt].structure == spec_table ) {
				show_spec_cmds ( ch );
				return FALSE;
			} else if ( help_table[cnt].structure == liq_table ) {
				show_liqlist ( ch );
				return FALSE;
			} else if ( help_table[cnt].structure == attack_table ) {
				show_damlist ( ch );
				return FALSE;
			} else if ( help_table[cnt].structure == skill_table ) {

				if ( spell[0] == '\0' ) {
					writeBuffer ( "Syntax:  ? spells "
								  "[ignore/attack/defend/self/object/all]\n\r", ch );
					return FALSE;
				}

				if ( !str_prefix ( spell, "all" ) )
				{ show_skill_cmds ( ch, -1 ); }
				else if ( !str_prefix ( spell, "ignore" ) )
				{ show_skill_cmds ( ch, TAR_IGNORE ); }
				else if ( !str_prefix ( spell, "attack" ) )
				{ show_skill_cmds ( ch, TAR_CHAR_OFFENSIVE ); }
				else if ( !str_prefix ( spell, "defend" ) )
				{ show_skill_cmds ( ch, TAR_CHAR_DEFENSIVE ); }
				else if ( !str_prefix ( spell, "self" ) )
				{ show_skill_cmds ( ch, TAR_CHAR_SELF ); }
				else if ( !str_prefix ( spell, "object" ) )
				{ show_skill_cmds ( ch, TAR_OBJ_INV ); }
				else
					writeBuffer ( "Syntax:  ? spell "
								  "[ignore/attack/defend/self/object/all]\n\r", ch );

				return FALSE;
			} else {
				show_flag_cmds ( ch, ( const flag_type* ) help_table[cnt].structure );
				return FALSE;
			}
		}
	}

	show_help ( ch, "" );
	return FALSE;
}

REDIT ( redit_rlist )
{
	RoomData	*pRoomIndex;
	Zone		*pArea;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buf1;
	char		arg  [ MAX_INPUT_LENGTH    ];
	bool found;
	int vnum;
	int  col = 0;

	ChopC ( argument, arg );

	pArea = IN_ROOM ( ch )->area;
	buf1 = new_buf();
	/*    buf1[0] = '\0'; */
	found   = FALSE;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ) {
		if ( ( pRoomIndex = get_room_index ( vnum ) ) ) {
			found = TRUE;
			sprintf ( buf, "[%5d] %-17.16s",
					  vnum, pRoomIndex->name ? capitalize ( pRoomIndex->name ) : "[No Name]" );
			add_buf ( buf1, buf );
			if ( ++col % 3 == 0 )
			{ add_buf ( buf1, "\n\r" ); }
		}
	}

	if ( !found ) {
		writeBuffer ( "Room(s) not found in this area.\n\r", ch );
		return FALSE;
	}

	if ( col % 3 != 0 )
	{ add_buf ( buf1, "\n\r" ); }

	writePage ( buf_string ( buf1 ), ch );
	recycle_buf ( buf1 );
	return FALSE;
}

REDIT ( redit_mlist )
{
	NPCData	*pMobIndex;
	Zone		*pArea;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buf1;
	char		arg  [ MAX_INPUT_LENGTH    ];
	bool fAll, found;
	int vnum;
	int  col = 0;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:  mlist <all/name>\n\r", ch );
		return FALSE;
	}

	buf1 = new_buf();
	pArea = IN_ROOM ( ch )->area;
	/*    buf1[0] = '\0'; */
	fAll    = SameString ( arg, "all" );
	found   = FALSE;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ) {
		if ( ( pMobIndex = get_mob_index ( vnum ) ) != NULL ) {
			if ( fAll || is_name ( arg, pMobIndex->player_name ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %-17.16s",
						  pMobIndex->vnum, pMobIndex->short_descr ? capitalize ( pMobIndex->short_descr ) : "[No Name]" );
				add_buf ( buf1, buf );
				if ( ++col % 3 == 0 )
				{ add_buf ( buf1, "\n\r" ); }
			}
		}
	}

	if ( !found ) {
		writeBuffer ( "Mobile(s) not found in this area.\n\r", ch );
		return FALSE;
	}

	if ( col % 3 != 0 )
	{ add_buf ( buf1, "\n\r" ); }

	writePage ( buf_string ( buf1 ), ch );
	recycle_buf ( buf1 );
	return FALSE;
}



REDIT ( redit_olist )
{
	ItemData	*pObjIndex;
	Zone		*pArea;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buf1;
	char		arg  [ MAX_INPUT_LENGTH    ];
	bool fAll, found;
	int vnum;
	int  col = 0;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:  olist <all/name/item_type>\n\r", ch );
		return FALSE;
	}

	pArea = IN_ROOM ( ch )->area;
	buf1 = new_buf();
	/*    buf1[0] = '\0'; */
	fAll    = SameString ( arg, "all" );
	found   = FALSE;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ) {
		if ( ( pObjIndex = get_obj_index ( vnum ) ) ) {
			if ( fAll || is_name ( arg, pObjIndex->name )
					|| flag_value ( type_flags, arg ) == pObjIndex->item_type ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %-17.16s",
						  pObjIndex->vnum, pObjIndex->short_descr ? capitalize ( pObjIndex->short_descr ) : "[No Name]" );
				add_buf ( buf1, buf );
				if ( ++col % 3 == 0 )
				{ add_buf ( buf1, "\n\r" ); }
			}
		}
	}

	if ( !found ) {
		writeBuffer ( "Object(s) not found in this area.\n\r", ch );
		return FALSE;
	}

	if ( col % 3 != 0 )
	{ add_buf ( buf1, "\n\r" ); }

	writePage ( buf_string ( buf1 ), ch );
	recycle_buf ( buf1 );
	return FALSE;
}



REDIT ( redit_mshow )
{
	NPCData *pMob;
	int value;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  mshow <vnum>\n\r", ch );
		return FALSE;
	}

	if ( !is_number ( argument ) ) {
		writeBuffer ( "REdit: That is an invalid mob vnum.\n\r", ch );
		return FALSE;
	}

	if ( is_number ( argument ) ) {
		value = atoi ( argument );
		if ( ! ( pMob = get_mob_index ( value ) ) ) {
			writeBuffer ( "REdit:  That mobile does not exist.\n\r", ch );
			return FALSE;
		}

		ch->desc->pEdit = ( void * ) pMob;
	}

	medit_show ( ch, argument );
	ch->desc->pEdit = ( void * ) IN_ROOM ( ch );
	return FALSE;
}



REDIT ( redit_oshow )
{
	ItemData *pObj;
	int value;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  oshow <vnum>\n\r", ch );
		return FALSE;
	}

	if ( !is_number ( argument ) ) {
		writeBuffer ( "REdit: That is in invalid object vnum.\n\r", ch );
		return FALSE;
	}

	if ( is_number ( argument ) ) {
		value = atoi ( argument );
		if ( ! ( pObj = get_obj_index ( value ) ) ) {
			writeBuffer ( "REdit:  That object does not exist.\n\r", ch );
			return FALSE;
		}

		ch->desc->pEdit = ( void * ) pObj;
	}

	oedit_show ( ch, argument );
	ch->desc->pEdit = ( void * ) IN_ROOM ( ch );
	return FALSE;
}



/*****************************************************************************
 Name:		check_range( lower vnum, upper vnum )
 Purpose:	Ensures the range spans only one area.
 Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range ( int lower, int upper )
{
	Zone *pArea;
	int cnt = 0;

	for ( pArea = area_first; pArea; pArea = pArea->next ) {
		/*
		 * lower < area < upper
		 */
		if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper )
				||   ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
		{ ++cnt; }

		if ( cnt > 1 )
		{ return FALSE; }
	}
	return TRUE;
}



Zone *get_vnum_area ( int vnum )
{
	Zone *pArea;

	for ( pArea = area_first; pArea; pArea = pArea->next ) {
		if ( vnum >= pArea->min_vnum
				&& vnum <= pArea->max_vnum )
		{ return pArea; }
	}

	return 0;
}



/*
 * Area Editor Functions.
 */
AEDIT ( aedit_show )
{
	Zone *pArea;
	char buf  [MAX_STRING_LENGTH];

	EDIT_AREA ( ch, pArea );

	sprintf ( buf, "Name:     [%5d] %s\n\r", pArea->vnum, pArea->name );
	writeBuffer ( buf, ch );

	sprintf ( buf, "File:     %s\n\r", pArea->file_name );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Vnums:    [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Age:      [%d]\n\r",	pArea->age );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Players:  [%d]\n\r", pArea->nplayer );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Security: [%d]\n\r", pArea->security );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Builders: [%s]\n\r", pArea->builders );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Credits : [%s]\n\r", pArea->credits );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Flags:    [%s]\n\r", flag_string ( area_flags, pArea->area_flags ) );
	writeBuffer ( buf, ch );

	return FALSE;
}



AEDIT ( aedit_reset )
{
	Zone *pArea;

	EDIT_AREA ( ch, pArea );

	reset_area ( pArea );
	writeBuffer ( "Area reset.\n\r", ch );

	return FALSE;
}



AEDIT ( aedit_create )
{
	Zone *pArea;

	pArea               =   new_area();
	area_last->next     =   pArea;
	area_last		=   pArea;	/* Thanks, Walker. */
	ch->desc->pEdit     =   ( void * ) pArea;

	SET_BIT ( pArea->area_flags, AREA_ADDED );
	writeBuffer ( "Area Created.\n\r", ch );
	return FALSE;
}



AEDIT ( aedit_name )
{
	Zone *pArea;

	EDIT_AREA ( ch, pArea );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:   name [$name]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pArea->name );
	pArea->name = assign_string ( argument );

	writeBuffer ( "Name set.\n\r", ch );
	return TRUE;
}

AEDIT ( aedit_credits )
{
	Zone *pArea;

	EDIT_AREA ( ch, pArea );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:   credits [$credits]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pArea->credits );
	pArea->credits = assign_string ( argument );

	writeBuffer ( "Credits set.\n\r", ch );
	return TRUE;
}


AEDIT ( aedit_file )
{
	Zone *pArea;
	char file[MAX_STRING_LENGTH];
	int i, length;

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, file );	/* Forces Lowercase */

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  filename [$file]\n\r", ch );
		return FALSE;
	}

	/*
	 * Simple Syntax Check.
	 */
	length = strlen ( argument );
	if ( length > 8 ) {
		writeBuffer ( "No more than eight characters allowed.\n\r", ch );
		return FALSE;
	}

	/*
	 * Allow only letters and numbers.
	 */
	for ( i = 0; i < length; i++ ) {
		if ( !isalnum ( file[i] ) ) {
			writeBuffer ( "Only letters and numbers are valid.\n\r", ch );
			return FALSE;
		}
	}

	PURGE_DATA ( pArea->file_name );
	strcat ( file, ".are" );
	pArea->file_name = assign_string ( file );

	writeBuffer ( "Filename set.\n\r", ch );
	return TRUE;
}



AEDIT ( aedit_age )
{
	Zone *pArea;
	char age[MAX_STRING_LENGTH];

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, age );

	if ( !is_number ( age ) || age[0] == '\0' ) {
		writeBuffer ( "Syntax:  age [#xage]\n\r", ch );
		return FALSE;
	}

	pArea->age = atoi ( age );

	writeBuffer ( "Age set.\n\r", ch );
	return TRUE;
}

AEDIT ( aedit_security )
{
	Zone *pArea;
	char sec[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int  value;

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, sec );

	if ( !is_number ( sec ) || sec[0] == '\0' ) {
		writeBuffer ( "Syntax:  security [#xlevel]\n\r", ch );
		return FALSE;
	}

	value = atoi ( sec );

	if ( value > ch->pcdata->security || value < 0 ) {
		if ( ch->pcdata->security != 0 ) {
			sprintf ( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
			writeBuffer ( buf, ch );
		} else
		{ writeBuffer ( "Security is 0 only.\n\r", ch ); }
		return FALSE;
	}

	pArea->security = value;

	writeBuffer ( "Security set.\n\r", ch );
	return TRUE;
}



AEDIT ( aedit_builder )
{
	Zone *pArea;
	char name[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, name );

	if ( name[0] == '\0' ) {
		writeBuffer ( "Syntax:  builder [$name]  -toggles builder\n\r", ch );
		writeBuffer ( "Syntax:  builder All      -allows everyone\n\r", ch );
		return FALSE;
	}

	name[0] = UPPER ( name[0] );

	if ( strstr ( pArea->builders, name ) != '\0' ) {
		pArea->builders = string_replace ( pArea->builders, name, "\0" );
		pArea->builders = string_unpad ( pArea->builders );

		if ( pArea->builders[0] == '\0' ) {
			PURGE_DATA ( pArea->builders );
			pArea->builders = assign_string ( "None" );
		}
		writeBuffer ( "Builder removed.\n\r", ch );
		return TRUE;
	} else {
		buf[0] = '\0';
		if ( strstr ( pArea->builders, "None" ) != '\0' ) {
			pArea->builders = string_replace ( pArea->builders, "None", "\0" );
			pArea->builders = string_unpad ( pArea->builders );
		}

		if ( pArea->builders[0] != '\0' ) {
			strcat ( buf, pArea->builders );
			strcat ( buf, " " );
		}
		strcat ( buf, name );
		PURGE_DATA ( pArea->builders );
		pArea->builders = string_proper ( assign_string ( buf ) );

		writeBuffer ( "Builder added.\n\r", ch );
		writeBuffer ( pArea->builders, ch );
		return TRUE;
	}

	return FALSE;
}



AEDIT ( aedit_vnum )
{
	Zone *pArea;
	char lower[MAX_STRING_LENGTH];
	char upper[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA ( ch, pArea );

	argument = ChopC ( argument, lower );
	ChopC ( argument, upper );

	if ( !is_number ( lower ) || lower[0] == '\0'
			|| !is_number ( upper ) || upper[0] == '\0' ) {
		writeBuffer ( "Syntax:  vnum [#xlower] [#xupper]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = atoi ( lower ) ) > ( iupper = atoi ( upper ) ) ) {
		writeBuffer ( "AEdit:  Upper must be larger then lower.\n\r", ch );
		return FALSE;
	}

	if ( !check_range ( atoi ( lower ), atoi ( upper ) ) ) {
		writeBuffer ( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area ( ilower )
			&& get_vnum_area ( ilower ) != pArea ) {
		writeBuffer ( "AEdit:  Lower vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->min_vnum = ilower;
	writeBuffer ( "Lower vnum set.\n\r", ch );

	if ( get_vnum_area ( iupper )
			&& get_vnum_area ( iupper ) != pArea ) {
		writeBuffer ( "AEdit:  Upper vnum already assigned.\n\r", ch );
		return TRUE;	/* The lower value has been set. */
	}

	pArea->max_vnum = iupper;
	writeBuffer ( "Upper vnum set.\n\r", ch );

	return TRUE;
}



AEDIT ( aedit_lvnum )
{
	Zone *pArea;
	char lower[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, lower );

	if ( !is_number ( lower ) || lower[0] == '\0' ) {
		writeBuffer ( "Syntax:  min_vnum [#xlower]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = atoi ( lower ) ) > ( iupper = pArea->max_vnum ) ) {
		writeBuffer ( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
		return FALSE;
	}

	if ( !check_range ( ilower, iupper ) ) {
		writeBuffer ( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area ( ilower )
			&& get_vnum_area ( ilower ) != pArea ) {
		writeBuffer ( "AEdit:  Lower vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->min_vnum = ilower;
	writeBuffer ( "Lower vnum set.\n\r", ch );
	return TRUE;
}



AEDIT ( aedit_uvnum )
{
	Zone *pArea;
	char upper[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA ( ch, pArea );

	ChopC ( argument, upper );

	if ( !is_number ( upper ) || upper[0] == '\0' ) {
		writeBuffer ( "Syntax:  max_vnum [#xupper]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi ( upper ) ) ) {
		writeBuffer ( "AEdit:  Upper must be larger then lower.\n\r", ch );
		return FALSE;
	}

	if ( !check_range ( ilower, iupper ) ) {
		writeBuffer ( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area ( iupper )
			&& get_vnum_area ( iupper ) != pArea ) {
		writeBuffer ( "AEdit:  Upper vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->max_vnum = iupper;
	writeBuffer ( "Upper vnum set.\n\r", ch );

	return TRUE;
}



/*
 * Room Editor Functions.
 */
REDIT ( redit_show )
{
	RoomData	*pRoom;
	char		buf  [MAX_STRING_LENGTH];
	char		buf1 [2 * MAX_STRING_LENGTH];
	Item		*obj;
	Creature		*rch;
	int			door;
	bool		fcnt;

	EDIT_ROOM ( ch, pRoom );

	buf1[0] = '\0';

	sprintf ( buf, "Description:\n\r%s", pRoom->description );
	strcat ( buf1, buf );

	sprintf ( buf, "Name:       [%s]\n\rArea:       [%5d] %s\n\r",
			  pRoom->name, pRoom->area->vnum, pRoom->area->name );
	strcat ( buf1, buf );

	sprintf ( buf, "Vnum:       [%5d]\n\rSector:     [%s]\n\r",
			  pRoom->vnum, flag_string ( sector_flags, pRoom->sector_type ) );
	strcat ( buf1, buf );

	sprintf ( buf, "Room flags: [%s]\n\r",
			  flag_string ( room_flags, pRoom->room_flags ) );
	strcat ( buf1, buf );

	if ( pRoom->heal_rate != 100 || pRoom->mana_rate != 100 ) {
		sprintf ( buf, "Health rec: [%d]\n\rMana rec  : [%d]\n\r",
				  pRoom->heal_rate , pRoom->mana_rate );
		strcat ( buf1, buf );
	}

	if ( pRoom->clan > 0 ) {
		sprintf ( buf, "Clan      : [%d] %s\n\r",
				  pRoom->clan,
				  clan_table[pRoom->clan].name );
		strcat ( buf1, buf );
	}

	if ( !IS_NULLSTR ( pRoom->owner ) ) {
		sprintf ( buf, "Owner     : [%s]\n\r", pRoom->owner );
		strcat ( buf1, buf );
	}

	if ( pRoom->extra_descr ) {
		DescriptionData *ed;

		strcat ( buf1, "Desc Kwds:  [" );
		for ( ed = pRoom->extra_descr; ed; ed = ed->next ) {
			strcat ( buf1, ed->keyword );
			if ( ed->next )
			{ strcat ( buf1, " " ); }
		}
		strcat ( buf1, "]\n\r" );
	}

	strcat ( buf1, "Characters: [" );
	fcnt = FALSE;
	for ( rch = pRoom->people; rch; rch = rch->next_in_room ) {
		ChopC ( rch->name, buf );
		strcat ( buf1, buf );
		strcat ( buf1, " " );
		fcnt = TRUE;
	}

	if ( fcnt ) {
		int end;

		end = strlen ( buf1 ) - 1;
		buf1[end] = ']';
		strcat ( buf1, "\n\r" );
	} else
	{ strcat ( buf1, "none]\n\r" ); }

	strcat ( buf1, "Objects:    [" );
	fcnt = FALSE;
	for ( obj = pRoom->contents; obj; obj = obj->next_content ) {
		ChopC ( obj->name, buf );
		strcat ( buf1, buf );
		strcat ( buf1, " " );
		fcnt = TRUE;
	}

	if ( fcnt ) {
		int end;

		end = strlen ( buf1 ) - 1;
		buf1[end] = ']';
		strcat ( buf1, "\n\r" );
	} else
	{ strcat ( buf1, "none]\n\r" ); }

	for ( door = 0; door < MAX_DIR; door++ ) {
		Exit *pexit;

		if ( ( pexit = pRoom->exit[door] ) ) {
			char word[MAX_INPUT_LENGTH];
			char reset_state[MAX_STRING_LENGTH];
			char *state;
			int i, length;

			sprintf ( buf, "-%-5s to [%5d] Key: [%5d] ",
					  capitalize ( dir_name[door] ),
					  pexit->u1.to_room ? pexit->u1.to_room->vnum : 0,      /* ROM OLC */
					  pexit->key );
			strcat ( buf1, buf );

			/*
			 * Format up the exit info.
			 * Capitalize all flags that are not part of the reset info.
			 */
			strcpy ( reset_state, flag_string ( exit_flags, pexit->rs_flags ) );
			state = ( char * ) flag_string ( exit_flags, pexit->exit_info );
			strcat ( buf1, " Exit flags: [" );
			for ( ; ; ) {
				state = ChopC ( state, word );

				if ( word[0] == '\0' ) {
					int end;

					end = strlen ( buf1 ) - 1;
					buf1[end] = ']';
					strcat ( buf1, "\n\r" );
					break;
				}

				if ( str_infix ( word, reset_state ) ) {
					length = strlen ( word );
					for ( i = 0; i < length; i++ )
					{ word[i] = UPPER ( word[i] ); }
				}
				strcat ( buf1, word );
				strcat ( buf1, " " );
			}

			if ( pexit->keyword && pexit->keyword[0] != '\0' ) {
				sprintf ( buf, "Kwds: [%s]\n\r", pexit->keyword );
				strcat ( buf1, buf );
			}
			if ( pexit->description && pexit->description[0] != '\0' ) {
				sprintf ( buf, "%s", pexit->description );
				strcat ( buf1, buf );
			}
		}
	}

	writeBuffer ( buf1, ch );
	return FALSE;
}




/* Local function. */
bool change_exit ( Creature *ch, const char *argument, int door )
{
	RoomData *pRoom;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int  value;

	EDIT_ROOM ( ch, pRoom );

	/*
	 * Set the exit flags, needs full argument.
	 * ----------------------------------------
	 */
	if ( ( value = flag_value ( exit_flags, argument ) ) != NO_FLAG ) {
		RoomData *pToRoom;
		int rev;                                    /* ROM OLC */

		if ( !pRoom->exit[door] ) {
			writeBuffer ( "There is no exit in that direction.\n\r", ch );
			return FALSE;
		}

		/*
		 * This room.
		 */
		TOGGLE_BIT ( pRoom->exit[door]->rs_flags,  value );
		/* Don't toggle exit_info because it can be changed by players. */
		pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

		/*
		 * Connected room.
		 */
		pToRoom = pRoom->exit[door]->u1.to_room;     /* ROM OLC */
		rev = rev_dir[door];

		if ( pToRoom->exit[rev] != NULL ) {
			pToRoom->exit[rev]->rs_flags = pRoom->exit[door]->rs_flags;
			pToRoom->exit[rev]->exit_info = pRoom->exit[door]->exit_info;
		}

		writeBuffer ( "Exit flag toggled.\n\r", ch );
		return TRUE;
	}

	/*
	 * Now parse the arguments.
	 */
	argument = ChopC ( argument, command );
	ChopC ( argument, arg );

	if ( command[0] == '\0' && argument[0] == '\0' ) {	/* Move command. */
		move_char ( ch, door, TRUE );                   /* ROM OLC */
		return FALSE;
	}

	if ( command[0] == '?' ) {
		cmd_function ( ch, &cmd_help, "EXIT" );
		return FALSE;
	}

	if ( SameString ( command, "delete" ) ) {
		RoomData *pToRoom;
		int rev;                                     /* ROM OLC */

		if ( !pRoom->exit[door] ) {
			writeBuffer ( "REdit:  Cannot delete a null exit.\n\r", ch );
			return FALSE;
		}

		/*
		 * Remove ToRoom Exit.
		 */
		rev = rev_dir[door];
		pToRoom = pRoom->exit[door]->u1.to_room;       /* ROM OLC */

		if ( pToRoom->exit[rev] ) {
			recycle_exit ( pToRoom->exit[rev] );
			pToRoom->exit[rev] = NULL;
		}

		/*
		 * Remove this exit.
		 */
		recycle_exit ( pRoom->exit[door] );
		pRoom->exit[door] = NULL;

		writeBuffer ( "Exit unlinked.\n\r", ch );
		return TRUE;
	}

	if ( SameString ( command, "link" ) ) {
		Exit *pExit;
		RoomData *toRoom;

		if ( arg[0] == '\0' || !is_number ( arg ) ) {
			writeBuffer ( "Syntax:  [direction] link [vnum]\n\r", ch );
			return FALSE;
		}

		value = atoi ( arg );

		if ( ! ( toRoom = get_room_index ( value ) ) ) {
			writeBuffer ( "REdit:  Cannot link to non-existant room.\n\r", ch );
			return FALSE;
		}

		if ( !IS_BUILDER ( ch, toRoom->area ) ) {
			writeBuffer ( "REdit:  Cannot link to that area.\n\r", ch );
			return FALSE;
		}

		if ( toRoom->exit[rev_dir[door]] ) {
			writeBuffer ( "REdit:  Remote side's exit already exists.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{ pRoom->exit[door] = new_exit(); }

		pRoom->exit[door]->u1.to_room = toRoom;
		pRoom->exit[door]->orig_door = door;

		door                    = rev_dir[door];
		pExit                   = new_exit();
		pExit->u1.to_room       = pRoom;
		pExit->orig_door	= door;
		toRoom->exit[door]       = pExit;

		writeBuffer ( "Two-way link established.\n\r", ch );
		return TRUE;
	}

	if ( SameString ( command, "dig" ) ) {
		char buf[MAX_STRING_LENGTH];

		if ( arg[0] == '\0' || !is_number ( arg ) ) {
			writeBuffer ( "Syntax: [direction] dig <vnum>\n\r", ch );
			return FALSE;
		}

		redit_create ( ch, arg );
		sprintf ( buf, "link %s", arg );
		change_exit ( ch, buf, door );
		return TRUE;
	}

	if ( SameString ( command, "room" ) ) {
		RoomData *toRoom;

		if ( arg[0] == '\0' || !is_number ( arg ) ) {
			writeBuffer ( "Syntax:  [direction] room [vnum]\n\r", ch );
			return FALSE;
		}

		value = atoi ( arg );

		if ( ! ( toRoom = get_room_index ( value ) ) ) {
			writeBuffer ( "REdit:  Cannot link to non-existant room.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{ pRoom->exit[door] = new_exit(); }

		pRoom->exit[door]->u1.to_room = toRoom;    /* ROM OLC */
		pRoom->exit[door]->orig_door = door;

		writeBuffer ( "One-way link established.\n\r", ch );
		return TRUE;
	}

	if ( SameString ( command, "key" ) ) {
		ItemData *key;

		if ( arg[0] == '\0' || !is_number ( arg ) ) {
			writeBuffer ( "Syntax:  [direction] key [vnum]\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] ) {
			writeBuffer ( "There is no exit in that direction.\n\r", ch );
			return FALSE;
		}

		value = atoi ( arg );

		if ( ! ( key = get_obj_index ( value ) ) ) {
			writeBuffer ( "REdit:  Key doesn't exist.\n\r", ch );
			return FALSE;
		}

		if ( key->item_type != ITEM_KEY ) {
			writeBuffer ( "REdit:  Your key's item type must be KEY, for obvious reasons.\n\r", ch );
			return FALSE;
		}

		pRoom->exit[door]->key = value;

		writeBuffer ( "Exit key set.\n\r", ch );
		return TRUE;
	}

	if ( SameString ( command, "name" ) ) {
		if ( arg[0] == '\0' ) {
			writeBuffer ( "Syntax:  [direction] name [string]\n\r", ch );
			writeBuffer ( "         [direction] name none\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] ) {
			writeBuffer ( "There is no exit in that direction.\n\r", ch );
			return FALSE;
		}

		PURGE_DATA ( pRoom->exit[door]->keyword );

		if ( !SameString ( arg, "none" ) )
		{ pRoom->exit[door]->keyword = assign_string ( arg ); }
		else
		{ pRoom->exit[door]->keyword = assign_string ( "" ); }

		writeBuffer ( "Exit name set.\n\r", ch );
		return TRUE;
	}

	if ( !str_prefix ( command, "description" ) ) {
		if ( arg[0] == '\0' ) {
			if ( !pRoom->exit[door] ) {
				writeBuffer ( "There is no exit in that direction.\n\r", ch );
				return FALSE;
			}

			EnterStringEditor ( ch, &pRoom->exit[door]->description );
			return TRUE;
		}

		writeBuffer ( "Syntax:  [direction] desc\n\r", ch );
		return FALSE;
	}

	return FALSE;
}



REDIT ( redit_north )
{
	if ( change_exit ( ch, argument, DIR_NORTH ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_south )
{
	if ( change_exit ( ch, argument, DIR_SOUTH ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_east )
{
	if ( change_exit ( ch, argument, DIR_EAST ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_west )
{
	if ( change_exit ( ch, argument, DIR_WEST ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_up )
{
	if ( change_exit ( ch, argument, DIR_UP ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_down )
{
	if ( change_exit ( ch, argument, DIR_DOWN ) )
	{ return TRUE; }

	return FALSE;
}



REDIT ( redit_ed )
{
	RoomData *pRoom;
	DescriptionData *ed;
	char command[MAX_INPUT_LENGTH];
	char keyword[MAX_INPUT_LENGTH];

	EDIT_ROOM ( ch, pRoom );

	argument = ChopC ( argument, command );
	ChopC ( argument, keyword );

	if ( command[0] == '\0' || keyword[0] == '\0' ) {
		writeBuffer ( "Syntax:  ed add [keyword]\n\r", ch );
		writeBuffer ( "         ed edit [keyword]\n\r", ch );
		writeBuffer ( "         ed delete [keyword]\n\r", ch );
		writeBuffer ( "         ed format [keyword]\n\r", ch );
		return FALSE;
	}

	if ( SameString ( command, "add" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed add [keyword]\n\r", ch );
			return FALSE;
		}

		ed			=   new_extra_descr();
		ed->keyword		=   assign_string ( keyword );
		ed->description		=   assign_string ( "" );
		ed->next		=   pRoom->extra_descr;
		pRoom->extra_descr	=   ed;

		EnterStringEditor ( ch, &ed->description );

		return TRUE;
	}


	if ( SameString ( command, "edit" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed edit [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
		}

		if ( !ed ) {
			writeBuffer ( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		EnterStringEditor ( ch, &ed->description );

		return TRUE;
	}


	if ( SameString ( command, "delete" ) ) {
		DescriptionData *ped = NULL;

		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed delete [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
			ped = ed;
		}

		if ( !ed ) {
			writeBuffer ( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		if ( !ped )
		{ pRoom->extra_descr = ed->next; }
		else
		{ ped->next = ed->next; }

		recycle_extra_descr ( ed );

		writeBuffer ( "Extra description deleted.\n\r", ch );
		return TRUE;
	}


	if ( SameString ( command, "format" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed format [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
		}

		if ( !ed ) {
			writeBuffer ( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		ed->description = StringEditor_FormatString ( ed->description );

		writeBuffer ( "Extra description formatted.\n\r", ch );
		return TRUE;
	}

	redit_ed ( ch, "" );
	return FALSE;
}



REDIT ( redit_create )
{
	Zone *pArea;
	RoomData *pRoom;
	int value;
	int iHash;

	EDIT_ROOM ( ch, pRoom );

	value = atoi ( argument );

	if ( argument[0] == '\0' || value <= 0 ) {
		writeBuffer ( "Syntax:  create [vnum > 0]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area ( value );
	if ( !pArea ) {
		writeBuffer ( "REdit:  That vnum is not assigned an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER ( ch, pArea ) ) {
		writeBuffer ( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
		return FALSE;
	}

	if ( get_room_index ( value ) ) {
		writeBuffer ( "REdit:  Room vnum already exists.\n\r", ch );
		return FALSE;
	}

	pRoom			= new_room_index();
	pRoom->area			= pArea;
	pRoom->vnum			= value;

	if ( value > top_vnum_room )
	{ top_vnum_room = value; }

	iHash			= value % MAX_KEY_HASH;
	pRoom->next			= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoom;
	ch->desc->pEdit		= ( void * ) pRoom;

	writeBuffer ( "Room created.\n\r", ch );
	return TRUE;
}



REDIT ( redit_name )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  name [name]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pRoom->name );
	pRoom->name = assign_string ( argument );

	writeBuffer ( "Name set.\n\r", ch );
	return TRUE;
}



REDIT ( redit_desc )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	if ( argument[0] == '\0' ) {
		EnterStringEditor ( ch, &pRoom->description );
		return TRUE;
	}

	writeBuffer ( "Syntax:  desc\n\r", ch );
	return FALSE;
}

REDIT ( redit_heal )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	if ( is_number ( argument ) ) {
		pRoom->heal_rate = atoi ( argument );
		writeBuffer ( "Heal rate set.\n\r", ch );
		return TRUE;
	}

	writeBuffer ( "Syntax : heal <#xnumber>\n\r", ch );
	return FALSE;
}

REDIT ( redit_mana )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	if ( is_number ( argument ) ) {
		pRoom->mana_rate = atoi ( argument );
		writeBuffer ( "Mana rate set.\n\r", ch );
		return TRUE;
	}

	writeBuffer ( "Syntax : mana <#xnumber>\n\r", ch );
	return FALSE;
}

REDIT ( redit_clan )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	pRoom->clan = clan_lookup ( argument );

	writeBuffer ( "Clan set.\n\r", ch );
	return TRUE;
}

REDIT ( redit_format )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	pRoom->description = StringEditor_FormatString ( pRoom->description );

	writeBuffer ( "String formatted.\n\r", ch );
	return TRUE;
}



REDIT ( redit_mreset )
{
	RoomData	*pRoom;
	NPCData	*pMobIndex;
	Creature		*newmob;
	char		arg [ MAX_INPUT_LENGTH ];
	char		arg2 [ MAX_INPUT_LENGTH ];

	Reset		*pReset;
	char		output [ MAX_STRING_LENGTH ];

	EDIT_ROOM ( ch, pRoom );

	argument = ChopC ( argument, arg );
	argument = ChopC ( argument, arg2 );

	if ( arg[0] == '\0' || !is_number ( arg ) ) {
		writeBuffer ( "Syntax:  mreset <vnum> <max #x> <mix #x>\n\r", ch );
		return FALSE;
	}

	if ( ! ( pMobIndex = get_mob_index ( atoi ( arg ) ) ) ) {
		writeBuffer ( "REdit: No mobile has that vnum.\n\r", ch );
		return FALSE;
	}

	if ( pMobIndex->area != pRoom->area ) {
		writeBuffer ( "REdit: No such mobile in this area.\n\r", ch );
		return FALSE;
	}

	/*
	 * Create the mobile reset.
	 */
	pReset              = new_reset_data();
	pReset->command	= 'M';
	pReset->arg1	= pMobIndex->vnum;
	pReset->arg2	= is_number ( arg2 ) ? atoi ( arg2 ) : MAX_MOB;
	pReset->arg3	= pRoom->vnum;
	pReset->arg4	= is_number ( argument ) ? atoi ( argument ) : 1;
	add_reset ( pRoom, pReset, 0/* Last slot*/ );

	/*
	 * Create the mobile.
	 */
	newmob = create_mobile ( pMobIndex );
	char_to_room ( newmob, pRoom );

	sprintf ( output, "%s (%d) has been loaded and added to resets.\n\r"
			  "There will be a maximum of %d loaded to this room.\n\r",
			  capitalize ( pMobIndex->short_descr ),
			  pMobIndex->vnum,
			  pReset->arg2 );
	writeBuffer ( output, ch );
	act ( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
	return TRUE;
}



struct wear_type {
	int	wear_loc;
	int	wear_bit;
};



const struct wear_type wear_table[] = {
	{	WEAR_NONE,	ITEM_TAKE		},
	{	WEAR_LIGHT,	ITEM_LIGHT		},
	{	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
	{	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
	{	WEAR_NECK_1,	ITEM_WEAR_NECK		},
	{	WEAR_NECK_2,	ITEM_WEAR_NECK		},
	{	WEAR_BODY,	ITEM_WEAR_BODY		},
	{	WEAR_HEAD,	ITEM_WEAR_HEAD		},
	{	WEAR_LEGS,	ITEM_WEAR_LEGS		},
	{	WEAR_FEET,	ITEM_WEAR_FEET		},
	{	WEAR_HANDS,	ITEM_WEAR_HANDS		},
	{	WEAR_ARMS,	ITEM_WEAR_ARMS		},
	{	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
	{	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
	{	WEAR_WAIST,	ITEM_WEAR_WAIST		},
	{	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
	{	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
	{	WEAR_WIELD,	ITEM_WIELD		},
	{	WEAR_HOLD,	ITEM_HOLD		},
	{	NO_FLAG,	NO_FLAG			}
};



/*****************************************************************************
 Name:		wear_loc
 Purpose:	Returns the location of the bit that matches the count.
 		1 = first match, 2 = second match etc.
 Called by:	oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc ( int bits, int count )
{
	int flag;

	for ( flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++ ) {
		if ( IS_SET ( bits, wear_table[flag].wear_bit ) && --count < 1 )
		{ return wear_table[flag].wear_loc; }
	}

	return NO_FLAG;
}



/*****************************************************************************
 Name:		wear_bit
 Purpose:	Converts a wear_loc into a bit.
 Called by:	redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit ( int loc )
{
	int flag;

	for ( flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++ ) {
		if ( loc == wear_table[flag].wear_loc )
		{ return wear_table[flag].wear_bit; }
	}

	return 0;
}



REDIT ( redit_oreset )
{
	RoomData	*pRoom;
	ItemData	*pObjIndex;
	Item		*newobj;
	Item		*to_obj;
	Creature		*to_mob;
	char		arg1 [ MAX_INPUT_LENGTH ];
	char		arg2 [ MAX_INPUT_LENGTH ];
	int			olevel = 0;

	Reset		*pReset;
	char		output [ MAX_STRING_LENGTH ];

	EDIT_ROOM ( ch, pRoom );

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number ( arg1 ) ) {
		writeBuffer ( "Syntax:  oreset <vnum> <args>\n\r", ch );
		writeBuffer ( "        -no_args               = into room\n\r", ch );
		writeBuffer ( "        -<obj_name>            = into obj\n\r", ch );
		writeBuffer ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
		return FALSE;
	}

	if ( ! ( pObjIndex = get_obj_index ( atoi ( arg1 ) ) ) ) {
		writeBuffer ( "REdit: No object has that vnum.\n\r", ch );
		return FALSE;
	}

	if ( pObjIndex->area != pRoom->area ) {
		writeBuffer ( "REdit: No such object in this area.\n\r", ch );
		return FALSE;
	}

	/*
	 * Load into room.
	 */
	if ( arg2[0] == '\0' ) {
		pReset		= new_reset_data();
		pReset->command	= 'O';
		pReset->arg1	= pObjIndex->vnum;
		pReset->arg2	= 0;
		pReset->arg3	= pRoom->vnum;
		pReset->arg4	= 0;
		add_reset ( pRoom, pReset, 0/* Last slot*/ );

		newobj = create_object ( pObjIndex, Math::instance().fuzzy ( olevel ) );
		obj_to_room ( newobj, pRoom );

		sprintf ( output, "%s (%d) has been loaded and added to resets.\n\r",
				  capitalize ( pObjIndex->short_descr ),
				  pObjIndex->vnum );
		writeBuffer ( output, ch );
	} else
		/*
		 * Load into object's inventory.
		 */
		if ( argument[0] == '\0'
				&& ( ( to_obj = get_obj_list ( ch, arg2, pRoom->contents ) ) != NULL ) ) {
			pReset		= new_reset_data();
			pReset->command	= 'P';
			pReset->arg1	= pObjIndex->vnum;
			pReset->arg2	= 0;
			pReset->arg3	= to_obj->pIndexData->vnum;
			pReset->arg4	= 1;
			add_reset ( pRoom, pReset, 0/* Last slot*/ );

			newobj = create_object ( pObjIndex, Math::instance().fuzzy ( olevel ) );
			newobj->cost = 0;
			obj_to_obj ( newobj, to_obj );

			sprintf ( output, "%s (%d) has been loaded into "
					  "%s (%d) and added to resets.\n\r",
					  capitalize ( newobj->short_descr ),
					  newobj->pIndexData->vnum,
					  to_obj->short_descr,
					  to_obj->pIndexData->vnum );
			writeBuffer ( output, ch );
		} else
			/*
			 * Load into mobile's inventory.
			 */
			if ( ( to_mob = get_char_room ( ch, arg2 ) ) != NULL ) {
				int	wear_loc;

				/*
				 * Make sure the location on mobile is valid.
				 */
				if ( ( wear_loc = flag_value ( wear_loc_flags, argument ) ) == NO_FLAG ) {
					writeBuffer ( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
					return FALSE;
				}

				/*
				 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
				 */
				if ( !IS_SET ( pObjIndex->wear_flags, wear_bit ( wear_loc ) ) ) {
					sprintf ( output,
							  "%s (%d) has wear flags: [%s]\n\r",
							  capitalize ( pObjIndex->short_descr ),
							  pObjIndex->vnum,
							  flag_string ( wear_flags, pObjIndex->wear_flags ) );
					writeBuffer ( output, ch );
					return FALSE;
				}

				/*
				 * Can't load into same position.
				 */
				if ( get_eq_char ( to_mob, wear_loc ) ) {
					writeBuffer ( "REdit:  Object already equipped.\n\r", ch );
					return FALSE;
				}

				pReset		= new_reset_data();
				pReset->arg1	= pObjIndex->vnum;
				pReset->arg2	= wear_loc;
				if ( pReset->arg2 == WEAR_NONE )
				{ pReset->command = 'G'; }
				else
				{ pReset->command = 'E'; }
				pReset->arg3	= wear_loc;

				add_reset ( pRoom, pReset, 0/* Last slot*/ );

				olevel  = URANGE ( 0, to_mob->level - 2, LEVEL_HERO );
				newobj = create_object ( pObjIndex, Math::instance().fuzzy ( olevel ) );

				if ( to_mob->pIndexData->pShop ) {	/* Shop-keeper? */
					switch ( pObjIndex->item_type ) {
						default:
							olevel = 0;
							break;
						case ITEM_PILL:
							olevel = Math::instance().range (  0, 10 );
							break;
						case ITEM_POTION:
							olevel = Math::instance().range (  0, 10 );
							break;
						case ITEM_SCROLL:
							olevel = Math::instance().range (  5, 15 );
							break;
						case ITEM_WAND:
							olevel = Math::instance().range ( 10, 20 );
							break;
						case ITEM_STAFF:
							olevel = Math::instance().range ( 15, 25 );
							break;
						case ITEM_ARMOR:
							olevel = Math::instance().range (  5, 15 );
							break;
						case ITEM_WEAPON:
							if ( pReset->command == 'G' )
							{ olevel = Math::instance().range ( 5, 15 ); }
							else
							{ olevel = Math::instance().fuzzy ( olevel ); }
							break;
					}

					newobj = create_object ( pObjIndex, olevel );
					if ( pReset->arg2 == WEAR_NONE )
					{ SET_BIT ( newobj->extra_flags, ITEM_INVENTORY ); }
				} else
				{ newobj = create_object ( pObjIndex, Math::instance().fuzzy ( olevel ) ); }

				obj_to_char ( newobj, to_mob );
				if ( pReset->command == 'E' )
				{ equip_char ( to_mob, newobj, pReset->arg3 ); }

				sprintf ( output, "%s (%d) has been loaded "
						  "%s of %s (%d) and added to resets.\n\r",
						  capitalize ( pObjIndex->short_descr ),
						  pObjIndex->vnum,
						  flag_string ( wear_loc_strings, pReset->arg3 ),
						  to_mob->short_descr,
						  to_mob->pIndexData->vnum );
				writeBuffer ( output, ch );
			} else {	/* Display Syntax */
				writeBuffer ( "REdit:  That mobile isn't here.\n\r", ch );
				return FALSE;
			}

	act ( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
	return TRUE;
}


//-- show the values of the item you are editing.
void show_obj_values ( Creature *ch, ItemData *obj )
{
	char buf[MAX_STRING_LENGTH];

	switch ( obj->item_type ) {
		default:	/* No values. */
			break;

		case ITEM_LIGHT:
			if ( obj->value[2] == -1 || obj->value[2] == 999 ) /* ROM OLC */
			{ sprintf ( buf, "[v2] Light:  Infinite[-1]\n\r" ); }
			else
			{ sprintf ( buf, "[v2] Light:  [%d]\n\r", obj->value[2] ); }
			writeBuffer ( buf, ch );
			break;

		case ITEM_SOURCE:
			sprintf ( buf,	        "[v0] to hp:   [%d]\n\r"
					  "[v1] to mn:   [%d]\n\r"
					  "[v2] to mv:   [%d]\n\r",
					  obj->value[0],
					  obj->value[1],
					  obj->value[2] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_TREASURECHEST:
			writeBuffer ( Format (
							  "[v0] Silver:       [%d]\n\r"
							  "[v1] Flags:        [%s]\n\r"
							  "[v2] Lock Diff: %s [%d]\n\r"
							  "[v3] Trap Type     [%d]\n\r"
							  "[v4] Trap Dmg      [%d]\n\r",
							  obj->value[0],
							  flag_string ( container_flags, obj->value[1] ),
							  obj->value[2],
							  obj->value[3],
							  obj->value[4] ), ch );
			break;

		case ITEM_LOCKPICK:
			writeBuffer ( Format (
							  "[v0] Strength:     [%d] 1-100\n\r",
							  obj->value[0] ), ch );
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			sprintf ( buf,
					  "[v0] Level:          [%d]\n\r"
					  "[v1] Charges Total:  [%d]\n\r"
					  "[v2] Charges Left:   [%d]\n\r"
					  "[v3] Spell:          %s\n\r",
					  obj->value[0],
					  obj->value[1],
					  obj->value[2],
					  obj->value[3] != -1 ? skill_table[obj->value[3]].name
					  : "none" );
			writeBuffer ( buf, ch );
			break;

		case ITEM_PORTAL:
			sprintf ( buf,
					  "[v0] Charges:        [%d]\n\r"
					  "[v1] Exit Flags:     %s\n\r"
					  "[v2] Portal Flags:   %s\n\r"
					  "[v3] Goes to (vnum): [%d]\n\r",
					  obj->value[0],
					  flag_string ( exit_flags, obj->value[1] ),
					  flag_string ( portal_flags , obj->value[2] ),
					  obj->value[3] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_FURNITURE:
			sprintf ( buf,
					  "[v0] Max people:      [%d]\n\r"
					  "[v1] Max weight:      [%d]\n\r"
					  "[v2] Furniture Flags: %s\n\r"
					  "[v3] Heal bonus:      [%d]\n\r"
					  "[v4] Mana bonus:      [%d]\n\r",
					  obj->value[0],
					  obj->value[1],
					  flag_string ( furniture_flags, obj->value[2] ),
					  obj->value[3],
					  obj->value[4] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			sprintf ( buf,
					  "[v0] Level:  [%d]\n\r"
					  "[v1] Spell:  %s\n\r"
					  "[v2] Spell:  %s\n\r"
					  "[v3] Spell:  %s\n\r"
					  "[v4] Spell:  %s\n\r",
					  obj->value[0],
					  obj->value[1] != -1 ? skill_table[obj->value[1]].name
					  : "none",
					  obj->value[2] != -1 ? skill_table[obj->value[2]].name
					  : "none",
					  obj->value[3] != -1 ? skill_table[obj->value[3]].name
					  : "none",
					  obj->value[4] != -1 ? skill_table[obj->value[4]].name
					  : "none" );
			writeBuffer ( buf, ch );
			break;

		/* ARMOR for ROM */

		case ITEM_ARMOR:
			sprintf ( buf,
					  "[v0] Ac pierce       [%d]\n\r"
					  "[v1] Ac bash         [%d]\n\r"
					  "[v2] Ac slash        [%d]\n\r"
					  "[v3] Ac exotic       [%d]\n\r",
					  obj->value[0],
					  obj->value[1],
					  obj->value[2],
					  obj->value[3] );
			writeBuffer ( buf, ch );
			break;

		/* WEAPON changed in ROM: */
		/* I had to split the output here, I have no idea why, but it helped -- Hugin */
		/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
		case ITEM_WEAPON:
			sprintf ( buf, "[v0] Weapon archetype:   %s\n\r",
					  flag_string ( weapon_archetype, obj->value[0] ) );
			writeBuffer ( buf, ch );
			sprintf ( buf, "[v1] Number of dice: [%d]\n\r", obj->value[1] );
			writeBuffer ( buf, ch );
			sprintf ( buf, "[v2] Type of dice:   [%d]\n\r", obj->value[2] );
			writeBuffer ( buf, ch );
			sprintf ( buf, "[v3] Type:           %s\n\r",
					  attack_table[obj->value[3]].name );
			writeBuffer ( buf, ch );
			sprintf ( buf, "[v4] Special type:   %s\n\r",
					  flag_string ( weapon_type2,  obj->value[4] ) );
			writeBuffer ( buf, ch );
			break;

		case ITEM_CONTAINER:
			sprintf ( buf,
					  "[v0] Weight:     [%d kg]\n\r"
					  "[v1] Flags:      [%s]\n\r"
					  "[v2] Key:     %s [%d]\n\r"
					  "[v3] Capacity    [%d]\n\r"
					  "[v4] Weight Mult [%d]\n\r",
					  obj->value[0],
					  flag_string ( container_flags, obj->value[1] ),
					  get_obj_index ( obj->value[2] )
					  ? get_obj_index ( obj->value[2] )->short_descr
					  : "none",
					  obj->value[2],
					  obj->value[3],
					  obj->value[4] );
			writeBuffer ( buf, ch );
			break;

		case ITEM_DRINK_CON:
			sprintf ( buf,
					  "[v0] Liquid Total: [%d]\n\r"
					  "[v1] Liquid Left:  [%d]\n\r"
					  "[v2] Liquid:       %s\n\r"
					  "[v3] Poisoned:     %s\n\r",
					  obj->value[0],
					  obj->value[1],
					  liq_table[obj->value[2]].liq_name,
					  obj->value[3] != 0 ? "Yes" : "No" );
			writeBuffer ( buf, ch );
			break;

		case ITEM_FOUNTAIN:
			sprintf ( buf,
					  "[v0] Liquid Total: [%d]\n\r"
					  "[v1] Liquid Left:  [%d]\n\r"
					  "[v2] Liquid:	    %s\n\r",
					  obj->value[0],
					  obj->value[1],
					  liq_table[obj->value[2]].liq_name );
			writeBuffer ( buf, ch );
			break;

		case ITEM_FOOD:
			sprintf ( buf,
					  "[v0] Food hours: [%d]\n\r"
					  "[v1] Full hours: [%d]\n\r"
					  "[v3] Poisoned:   %s\n\r",
					  obj->value[0],
					  obj->value[1],
					  obj->value[3] != 0 ? "Yes" : "No" );
			writeBuffer ( buf, ch );
			break;

		case ITEM_MONEY:
			sprintf ( buf, "[v0] Gold:   [%d]\n\r", obj->value[0] );
			writeBuffer ( buf, ch );
			break;
	}

	return;
}



bool set_obj_values ( Creature *ch, ItemData *pObj, int value_num, const char *argument )
{
	switch ( pObj->item_type ) {
		default:
			break;

		case ITEM_LIGHT:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_LIGHT" );
					return FALSE;
				case 2:
					writeBuffer ( "HOURS OF LIGHT SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
			}
			break;

		case ITEM_SOURCE:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help,  "ITEM_SOURCE" );
					return FALSE;
				case 0:
					writeBuffer ( "HP modifier SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "MN modifier SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "MV modifier SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
			}
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_STAFF_WAND" );
					return FALSE;
				case 0:
					writeBuffer ( "SPELL LEVEL SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
				case 3:
					writeBuffer ( "SPELL TYPE SET.\n\r", ch );
					pObj->value[3] = skill_lookup ( argument );
					break;
			}
			break;

		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_SCROLL_POTION_PILL" );
					return FALSE;
				case 0:
					writeBuffer ( "SPELL LEVEL SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "SPELL TYPE 1 SET.\n\r\n\r", ch );
					pObj->value[1] = skill_lookup ( argument );
					break;
				case 2:
					writeBuffer ( "SPELL TYPE 2 SET.\n\r\n\r", ch );
					pObj->value[2] = skill_lookup ( argument );
					break;
				case 3:
					writeBuffer ( "SPELL TYPE 3 SET.\n\r\n\r", ch );
					pObj->value[3] = skill_lookup ( argument );
					break;
				case 4:
					writeBuffer ( "SPELL TYPE 4 SET.\n\r\n\r", ch );
					pObj->value[4] = skill_lookup ( argument );
					break;
			}
			break;

		/* ARMOR for ROM: */

		case ITEM_ARMOR:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_ARMOR" );
					return FALSE;
				case 0:
					writeBuffer ( "AC PIERCE SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "AC BASH SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "AC SLASH SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
				case 3:
					writeBuffer ( "AC EXOTIC SET.\n\r\n\r", ch );
					pObj->value[3] = atoi ( argument );
					break;
			}
			break;

		/* WEAPONS changed in ROM */

		case ITEM_WEAPON:
			switch ( value_num ) {
				default:
					cmd_function ( ch, cmd_help, "ITEM_WEAPON" );
					return FALSE;
				case 0:
					writeBuffer ( "WEAPON CLASS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET ( pObj->value[0], weapon_archetype, argument );
					break;
				case 1:
					writeBuffer ( "NUMBER OF DICE SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "TYPE OF DICE SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
				case 3:
					writeBuffer ( "WEAPON TYPE SET.\n\r\n\r", ch );
					pObj->value[3] = attack_lookup ( argument );
					break;
				case 4:
					writeBuffer ( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
					ALT_FLAGVALUE_TOGGLE ( pObj->value[4], weapon_type2, argument );
					break;
			}
			break;

		case ITEM_PORTAL:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_PORTAL" );
					return FALSE;

				case 0:
					writeBuffer ( "CHARGES SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "EXIT FLAGS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET ( pObj->value[1], exit_flags, argument );
					break;
				case 2:
					writeBuffer ( "PORTAL FLAGS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET ( pObj->value[2], portal_flags, argument );
					break;
				case 3:
					writeBuffer ( "EXIT VNUM SET.\n\r\n\r", ch );
					pObj->value[3] = atoi ( argument );
					break;
			}
			break;

		case ITEM_FURNITURE:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_FURNITURE" );
					return FALSE;

				case 0:
					writeBuffer ( "NUMBER OF PEOPLE SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "MAX WEIGHT SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "FURNITURE FLAGS TOGGLED.\n\r\n\r", ch );
					ALT_FLAGVALUE_TOGGLE ( pObj->value[2], furniture_flags, argument );
					break;
				case 3:
					writeBuffer ( "HEAL BONUS SET.\n\r\n\r", ch );
					pObj->value[3] = atoi ( argument );
					break;
				case 4:
					writeBuffer ( "MANA BONUS SET.\n\r\n\r", ch );
					pObj->value[4] = atoi ( argument );
					break;
			}
			break;

		case ITEM_CONTAINER:
			switch ( value_num ) {
					int value;

				default:
					cmd_function ( ch, &cmd_help, "ITEM_CONTAINER" );
					return FALSE;
				case 0:
					writeBuffer ( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					if ( ( value = flag_value ( container_flags, argument ) ) != NO_FLAG )
					{ TOGGLE_BIT ( pObj->value[1], value ); }
					else {
						cmd_function ( ch, &cmd_help, "ITEM_CONTAINER" );
						return FALSE;
					}
					writeBuffer ( "CONTAINER TYPE SET.\n\r\n\r", ch );
					break;
				case 2:
					if ( atoi ( argument ) != 0 ) {
						if ( !get_obj_index ( atoi ( argument ) ) ) {
							writeBuffer ( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
							return FALSE;
						}

						if ( get_obj_index ( atoi ( argument ) )->item_type != ITEM_KEY ) {
							writeBuffer ( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
							return FALSE;
						}
					}
					writeBuffer ( "CONTAINER KEY SET.\n\r\n\r", ch );
					pObj->value[2] = atoi ( argument );
					break;
				case 3:
					writeBuffer ( "CONTAINER MAX WEIGHT SET.\n\r", ch );
					pObj->value[3] = atoi ( argument );
					break;
				case 4:
					writeBuffer ( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
					pObj->value[4] = atoi ( argument );
					break;
			}
			break;

		case ITEM_DRINK_CON:
			switch ( value_num ) {
				default:
					cmd_function ( ch, cmd_help, "ITEM_DRINK" );
					/* OLC		    cmd_function(ch, cmd_help, "liquids" );    */
					return FALSE;
				case 0:
					writeBuffer ( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "LIQUID TYPE SET.\n\r\n\r", ch );
					pObj->value[2] = ( liq_lookup ( argument ) != -1 ?
									   liq_lookup ( argument ) : 0 );
					break;
				case 3:
					writeBuffer ( "POISON VALUE TOGGLED.\n\r\n\r", ch );
					pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
					break;
			}
			break;

		case ITEM_FOUNTAIN:
			switch ( value_num ) {
				default:
					cmd_function ( ch, &cmd_help, "ITEM_FOUNTAIN" );
					/* OLC		    cmd_function(ch, &cmd_help, "liquids" );    */
					return FALSE;
				case 0:
					writeBuffer ( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 2:
					writeBuffer ( "LIQUID TYPE SET.\n\r\n\r", ch );
					pObj->value[2] = ( liq_lookup ( argument ) != -1 ?
									   liq_lookup ( argument ) : 0 );
					break;
			}
			break;

		case ITEM_FOOD:
			switch ( value_num ) {
				default:
					cmd_function ( ch, cmd_help, "ITEM_FOOD" );
					return FALSE;
				case 0:
					writeBuffer ( "HOURS OF FOOD SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "HOURS OF FULL SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
				case 3:
					writeBuffer ( "POISON VALUE TOGGLED.\n\r\n\r", ch );
					pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
					break;
			}
			break;

		case ITEM_MONEY:
			switch ( value_num ) {
				default:
					cmd_function ( ch, cmd_help, "ITEM_MONEY" );
					return FALSE;
				case 0:
					writeBuffer ( "GOLD AMOUNT SET.\n\r\n\r", ch );
					pObj->value[0] = atoi ( argument );
					break;
				case 1:
					writeBuffer ( "SILVER AMOUNT SET.\n\r\n\r", ch );
					pObj->value[1] = atoi ( argument );
					break;
			}
			break;
	}

	show_obj_values ( ch, pObj );

	return TRUE;
}

void show_obj_requirements ( Creature *ch, ItemData *obj )
{
	writeBuffer ( "Requirements:\n\r", ch );
	writeBuffer ( Format (	"[r0] Size:          [%d]\n\r"
							"[r1] Strength:      [%d]\n\r"
							"[r2] Dexterity:     [%d]\n\r"
							"[r3] Constitution:  [%d]\n\r"
							"[r4] Inteligence:   [%d]\n\r"
							"[r5] Wisdom:        [%d]\n\r",
							obj->requirements[SIZ_REQ], obj->requirements[STR_REQ],
							obj->requirements[DEX_REQ], obj->requirements[CON_REQ],
							obj->requirements[INT_REQ], obj->requirements[WIS_REQ] ), ch );

	writeBuffer ( "\n\r", ch );
	return;
}

OEDIT ( oedit_req0 )
{
	char buf[MSL];
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r0 [number]\n\r", ch );
		writeBuffer ( "For numbers use:\n\r", ch );

		sprintf ( buf, "%d: Tiny\n\r%d: Small\n\r%d: Medium\n\r%d: Large\n\r%d: Huge\n\r%d: Giant\n\r%d: All\n\r",	SIZE_TINY, SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE, SIZE_HUGE, SIZE_GIANT, SIZE_MAGIC );
		writeBuffer ( buf, ch );
		return FALSE;
	}

	pObj->requirements[SIZ_REQ] = atoi ( argument );
	writeBuffer ( "Size Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_req1 )
{
	ItemData *pObj;
	EDIT_OBJ ( ch, pObj );
	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r1 [number]\n\r", ch );
		return FALSE;
	}
	pObj->requirements[STR_REQ] = atoi ( argument );
	writeBuffer ( "Strength Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_req2 )
{
	ItemData *pObj;
	EDIT_OBJ ( ch, pObj );
	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r2 [number]\n\r", ch );
		return FALSE;
	}
	pObj->requirements[DEX_REQ] = atoi ( argument );
	writeBuffer ( "Dexterity Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_req3 )
{
	ItemData *pObj;
	EDIT_OBJ ( ch, pObj );

	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r3 [number]\n\r", ch );
		return FALSE;
	}

	pObj->requirements[CON_REQ] = atoi ( argument );
	writeBuffer ( "Constitution Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_req4 )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );
	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r4 [number]\n\r", ch );
		return FALSE;
	}
	pObj->requirements[INT_REQ] = atoi ( argument );
	writeBuffer ( "Intelligence Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_req5 )
{
	ItemData *pObj;
	EDIT_OBJ ( ch, pObj );

	if ( IS_NULLSTR ( argument ) || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  r5 [number]\n\r", ch );
		return FALSE;
	}
	pObj->requirements[WIS_REQ] = atoi ( argument );
	writeBuffer ( "Wis Requirement set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_show )
{
	ItemData *pObj;
	char buf[MAX_STRING_LENGTH];
	Affect *paf;
	int cnt;

	EDIT_OBJ ( ch, pObj );

	sprintf ( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
			  pObj->name,
			  !pObj->area ? -1        : pObj->area->vnum,
			  !pObj->area ? "No Area" : pObj->area->name );
	writeBuffer ( buf, ch );


	sprintf ( buf, "Vnum:        [%5d]\n\rType:        [%s]\n\r",
			  pObj->vnum,
			  flag_string ( type_flags, pObj->item_type ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Level:       [%5d]\n\rRepop:      [%5d]\r\n", pObj->level, pObj->repop_percent );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Wear flags:  [%s]\n\r",
			  flag_string ( wear_flags, pObj->wear_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Extra flags: [%s]\n\r",
			  flag_string ( extra_flags, pObj->extra_flags ) );
	writeBuffer ( buf, ch );

	writeBuffer ( Format ( "Material Bits: [%s]\r\n", material_bit_name ( pObj->material_flags ) ), ch );

	sprintf ( buf, "Condition:   [%5d]\n\r",              /* ROM */
			  pObj->condition );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Weight:      [%5d]\n\rCost:        [%5d]\n\r",
			  pObj->weight, pObj->cost );
	writeBuffer ( buf, ch );

	show_obj_requirements ( ch, pObj );

	if ( pObj->extra_descr ) {
		DescriptionData *ed;

		writeBuffer ( "Ex desc kwd: ", ch );

		for ( ed = pObj->extra_descr; ed; ed = ed->next ) {
			writeBuffer ( "[", ch );
			writeBuffer ( ed->keyword, ch );
			writeBuffer ( "]", ch );
		}

		writeBuffer ( "\n\r", ch );
	}

	sprintf ( buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r",
			  pObj->short_descr, pObj->description );
	writeBuffer ( buf, ch );

	for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next ) {
		if ( cnt == 0 ) {
			writeBuffer ( "Number Modifier Affects\n\r", ch );
			writeBuffer ( "------ -------- -------\n\r", ch );
		}
		sprintf ( buf, "[%4d] %-8d %s\n\r", cnt,
				  paf->modifier,
				  flag_string ( apply_flags, paf->location ) );
		writeBuffer ( buf, ch );
		cnt++;
	}

	show_obj_values ( ch, pObj );

	return FALSE;
}


/*
 * Need to issue warning if flag isn't valid. -- does so now -- Hugin.
 */
OEDIT ( oedit_addaffect )
{
	int value;
	ItemData *pObj;
	Affect *pAf;
	char loc[MAX_STRING_LENGTH];
	char mod[MAX_STRING_LENGTH];

	EDIT_OBJ ( ch, pObj );

	argument = ChopC ( argument, loc );
	ChopC ( argument, mod );

	if ( loc[0] == '\0' || mod[0] == '\0' || !is_number ( mod ) ) {
		writeBuffer ( "Syntax:  addaffect [location] [#xmod]\n\r", ch );
		return FALSE;
	}

	if ( ( value = flag_value ( apply_flags, loc ) ) == NO_FLAG ) { /* Hugin */
		writeBuffer ( "Valid affects are:\n\r", ch );
		show_help ( ch, "apply" );
		return FALSE;
	}

	pAf             =   new_affect();
	pAf->location   =   value;
	pAf->modifier   =   atoi ( mod );
	pAf->where	    =   TO_OBJECT;
	pAf->type       =   -1;
	pAf->duration   =   -1;
	pAf->bitvector  =   0;
	pAf->level      =	pObj->level;
	pAf->next       =   pObj->affected;
	pObj->affected  =   pAf;

	writeBuffer ( "Affect added.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_addapply )
{
	int value, bv, typ;
	ItemData *pObj;
	Affect *pAf;
	char loc[MAX_STRING_LENGTH];
	char mod[MAX_STRING_LENGTH];
	char type[MAX_STRING_LENGTH];
	char bvector[MAX_STRING_LENGTH];

	EDIT_OBJ ( ch, pObj );

	argument = ChopC ( argument, type );
	argument = ChopC ( argument, loc );
	argument = ChopC ( argument, mod );
	ChopC ( argument, bvector );

	if ( type[0] == '\0' || ( typ = flag_value ( apply_types, type ) ) == NO_FLAG ) {
		writeBuffer ( "Invalid apply type. Valid apply types are:\n\r", ch );
		show_help ( ch, "apptype" );
		return FALSE;
	}

	if ( loc[0] == '\0' || ( value = flag_value ( apply_flags, loc ) ) == NO_FLAG ) {
		writeBuffer ( "Valid applys are:\n\r", ch );
		show_help ( ch, "apply" );
		return FALSE;
	}

	if ( bvector[0] == '\0' || ( bv = flag_value ( bitvector_type[typ].table, bvector ) ) == NO_FLAG ) {
		writeBuffer ( "Invalid bitvector type.\n\r", ch );
		writeBuffer ( "Valid bitvector types are:\n\r", ch );
		show_help ( ch, ( char * ) bitvector_type[typ].help );
		return FALSE;
	}

	if ( mod[0] == '\0' || !is_number ( mod ) ) {
		writeBuffer ( "Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
		return FALSE;
	}

	pAf             =   new_affect();
	pAf->location   =   value;
	pAf->modifier   =   atoi ( mod );
	pAf->where	    =   apply_types[typ].bit;
	pAf->type	    =	-1;
	pAf->duration   =   -1;
	pAf->bitvector  =   bv;
	pAf->level      =	pObj->level;
	pAf->next       =   pObj->affected;
	pObj->affected  =   pAf;

	writeBuffer ( "Apply added.\n\r", ch );
	return TRUE;
}

/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
OEDIT ( oedit_delaffect )
{
	ItemData *pObj;
	Affect *pAf;
	Affect *pAf_next;
	char affect[MAX_STRING_LENGTH];
	int  value;
	int  cnt = 0;

	EDIT_OBJ ( ch, pObj );

	ChopC ( argument, affect );

	if ( !is_number ( affect ) || affect[0] == '\0' ) {
		writeBuffer ( "Syntax:  delaffect [#xaffect]\n\r", ch );
		return FALSE;
	}

	value = atoi ( affect );

	if ( value < 0 ) {
		writeBuffer ( "Only non-negative affect-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( ! ( pAf = pObj->affected ) ) {
		writeBuffer ( "OEdit:  Non-existant affect.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 ) {	/* First case: Remove first affect */
		pAf = pObj->affected;
		pObj->affected = pAf->next;
		recycle_affect ( pAf );
	} else {	/* Affect to remove is not the first */
		while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
		{ pAf = pAf_next; }

		if ( pAf_next ) {	/* See if it's the next affect */
			pAf->next = pAf_next->next;
			recycle_affect ( pAf_next );
		} else {                             /* Doesn't exist */
			writeBuffer ( "No such affect.\n\r", ch );
			return FALSE;
		}
	}

	writeBuffer ( "Affect removed.\n\r", ch );
	return TRUE;
}



OEDIT ( oedit_name )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  name [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pObj->name );
	pObj->name = assign_string ( argument );

	writeBuffer ( "Name set.\n\r", ch );
	return TRUE;
}



OEDIT ( oedit_short )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  short [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pObj->short_descr );
	pObj->short_descr = assign_string ( argument );
	pObj->short_descr[0] = LOWER ( pObj->short_descr[0] );

	writeBuffer ( "Short description set.\n\r", ch );
	return TRUE;
}



OEDIT ( oedit_long )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  long [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pObj->description );
	pObj->description = assign_string ( argument );
	pObj->description[0] = UPPER ( pObj->description[0] );

	writeBuffer ( "Long description set.\n\r", ch );
	return TRUE;
}



bool set_value ( Creature *ch, ItemData *pObj, const char *argument, int value )
{
	if ( argument[0] == '\0' ) {
		set_obj_values ( ch, pObj, -1, "" );    /* '\0' changed to "" -- Hugin */
		return FALSE;
	}

	if ( set_obj_values ( ch, pObj, value, argument ) )
	{ return TRUE; }

	return FALSE;
}



/*****************************************************************************
 Name:		oedit_values
 Purpose:	Finds the object and sets its value.
 Called by:	The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values ( Creature *ch, const char *argument, int value )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( set_value ( ch, pObj, argument, value ) )
	{ return TRUE; }

	return FALSE;
}


OEDIT ( oedit_value0 )
{
	if ( oedit_values ( ch, argument, 0 ) )
	{ return TRUE; }

	return FALSE;
}



OEDIT ( oedit_value1 )
{
	if ( oedit_values ( ch, argument, 1 ) )
	{ return TRUE; }

	return FALSE;
}



OEDIT ( oedit_value2 )
{
	if ( oedit_values ( ch, argument, 2 ) )
	{ return TRUE; }

	return FALSE;
}



OEDIT ( oedit_value3 )
{
	if ( oedit_values ( ch, argument, 3 ) )
	{ return TRUE; }

	return FALSE;
}



OEDIT ( oedit_value4 )
{
	if ( oedit_values ( ch, argument, 4 ) )
	{ return TRUE; }

	return FALSE;
}



OEDIT ( oedit_weight )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  weight [number]\n\r", ch );
		return FALSE;
	}

	pObj->weight = atoi ( argument );

	writeBuffer ( "Weight set.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_cost )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  cost [number]\n\r", ch );
		return FALSE;
	}

	pObj->cost = atoi ( argument );

	writeBuffer ( "Cost set.\n\r", ch );
	return TRUE;
}



OEDIT ( oedit_create )
{
	ItemData *pObj;
	Zone *pArea;
	int  value;
	int  iHash;

	value = atoi ( argument );
	if ( argument[0] == '\0' || value == 0 ) {
		writeBuffer ( "Syntax:  oedit create [vnum]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area ( value );
	if ( !pArea ) {
		writeBuffer ( "OEdit:  That vnum is not assigned an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER ( ch, pArea ) ) {
		writeBuffer ( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
		return FALSE;
	}

	if ( get_obj_index ( value ) ) {
		writeBuffer ( "OEdit:  Object vnum already exists.\n\r", ch );
		return FALSE;
	}

	pObj			= new_obj_index();
	pObj->vnum			= value;
	pObj->area			= pArea;

	if ( value > top_vnum_obj )
	{ top_vnum_obj = value; }

	iHash			= value % MAX_KEY_HASH;
	pObj->next			= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObj;
	ch->desc->pEdit		= ( void * ) pObj;

	writeBuffer ( "Object Created.\n\r", ch );
	return TRUE;
}



OEDIT ( oedit_ed )
{
	ItemData *pObj;
	DescriptionData *ed;
	char command[MAX_INPUT_LENGTH];
	char keyword[MAX_INPUT_LENGTH];

	EDIT_OBJ ( ch, pObj );

	argument = ChopC ( argument, command );
	ChopC ( argument, keyword );

	if ( command[0] == '\0' ) {
		writeBuffer ( "Syntax:  ed add [keyword]\n\r", ch );
		writeBuffer ( "         ed delete [keyword]\n\r", ch );
		writeBuffer ( "         ed edit [keyword]\n\r", ch );
		writeBuffer ( "         ed format [keyword]\n\r", ch );
		return FALSE;
	}

	if ( SameString ( command, "add" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed add [keyword]\n\r", ch );
			return FALSE;
		}

		ed                  =   new_extra_descr();
		ed->keyword         =   assign_string ( keyword );
		ed->next            =   pObj->extra_descr;
		pObj->extra_descr   =   ed;

		EnterStringEditor ( ch, &ed->description );

		return TRUE;
	}

	if ( SameString ( command, "edit" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed edit [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
		}

		if ( !ed ) {
			writeBuffer ( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		EnterStringEditor ( ch, &ed->description );

		return TRUE;
	}

	if ( SameString ( command, "delete" ) ) {
		DescriptionData *ped = NULL;

		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed delete [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
			ped = ed;
		}

		if ( !ed ) {
			writeBuffer ( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		if ( !ped )
		{ pObj->extra_descr = ed->next; }
		else
		{ ped->next = ed->next; }

		recycle_extra_descr ( ed );

		writeBuffer ( "Extra description deleted.\n\r", ch );
		return TRUE;
	}


	if ( SameString ( command, "format" ) ) {
		if ( keyword[0] == '\0' ) {
			writeBuffer ( "Syntax:  ed format [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next ) {
			if ( is_name ( keyword, ed->keyword ) )
			{ break; }
		}

		if ( !ed ) {
			writeBuffer ( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		ed->description = StringEditor_FormatString ( ed->description );

		writeBuffer ( "Extra description formatted.\n\r", ch );
		return TRUE;
	}

	oedit_ed ( ch, "" );
	return FALSE;
}


/* ROM object functions : */

OEDIT ( oedit_extra )     /* Moved out of oedit() due to naming conflicts -- Hugin */
{
	ItemData *pObj;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_OBJ ( ch, pObj );

		if ( ( value = flag_value ( extra_flags, argument ) ) != NO_FLAG ) {
			TOGGLE_BIT ( pObj->extra_flags, value );

			writeBuffer ( "Extra flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax:  extra [flag]\n\r"
				  "Type '? extra' for a list of flags.\n\r", ch );
	return FALSE;
}


OEDIT ( oedit_wear )     /* Moved out of oedit() due to naming conflicts -- Hugin */
{
	ItemData *pObj;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_OBJ ( ch, pObj );

		if ( ( value = flag_value ( wear_flags, argument ) ) != NO_FLAG ) {
			TOGGLE_BIT ( pObj->wear_flags, value );

			writeBuffer ( "Wear flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax:  wear [flag]\n\r"
				  "Type '? wear' for a list of flags.\n\r", ch );
	return FALSE;
}


OEDIT ( autoweapon )
{
	ItemData *pObj;
	Affect *pAf, *pAfN;
	int dice, size, bonus;
	double avg;

	EDIT_OBJ ( ch, pObj );
	if ( pObj->item_type != ITEM_WEAPON ) {
		return FALSE;
	}
	if ( pObj->level < 1 ) {
		return FALSE;
	}
	bonus = UMAX ( 0, pObj->level / 10 - 1 );
	/* adjust this next line to change the avg dmg your weapons will get! */
	avg = ( pObj->level * .76 );
	dice = ( pObj->level / 10 + 1 );
	size = dice / 2;
	/* loop through dice sizes until we find that the Next Math::instance().dice size's avg
	will be too high... ie, find the "best fit" */
	for ( size = dice / 2 ; dice * ( size + 2 ) / 2 < avg ; size++ )
	{ }

	dice = UMAX ( 1, dice );
	size = UMAX ( 2, size );

	switch ( pObj->value[0] ) {
		default:
		case WEAPON_EXOTIC:
		case WEAPON_SWORD:
			break;
		case WEAPON_DAGGER:
			dice = UMAX ( 1, dice - 1 );
			size = UMAX ( 2, size - 1 );
			break;
		case WEAPON_SPEAR:
		case WEAPON_POLEARM:
			size++;
			break;
		case WEAPON_MACE:
		case WEAPON_AXE:
			size = UMAX ( 2, size - 1 );
			break;
		case WEAPON_FLAIL:
		case WEAPON_WHIP:
			dice = UMAX ( 1, dice - 1 );
			break;
	}
	dice = UMAX ( 1, dice );
	size = UMAX ( 2, size );


	pObj->cost = 25 * ( size * ( dice + 1 ) ) + 20 * bonus + 20 * pObj->level;
	pObj->weight = pObj->level + 1;
	pObj->value[1] = dice;
	pObj->value[2] = size;

	for ( pAf = pObj->affected; pAf; pAf = pAfN ) {
		pAfN = pAf->next;
		recycle_affect ( pAf );
	}
	pObj->affected = NULL;

	if ( bonus > 0 ) {
		pAf = new_affect();
		pAf->location = APPLY_DAMROLL;
		pAf->modifier = bonus;
		pAf->where = TO_OBJECT;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->bitvector = 0;
		pAf->level = pObj->level;
		pAf->next = pObj->affected;
		pObj->affected = pAf;

		pAf = new_affect();
		pAf->location = APPLY_HITROLL;
		pAf->modifier = bonus;
		pAf->where = TO_OBJECT;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->bitvector = 0;
		pAf->level = pObj->level;
		pAf->next = pObj->affected;
		pObj->affected = pAf;
	}
	writeBuffer ( "\aRExperimental values set for this weapon.\an\r\n", ch );
	return TRUE;
}

OEDIT ( autoarmor )
{
	ItemData *pObj;
	int size;

	EDIT_OBJ ( ch, pObj );
	if ( pObj->item_type != ITEM_ARMOR ) {
		return FALSE;
	}
	if ( pObj->level < 1 ) {
		return FALSE;
	}
	size = UMAX ( 1, pObj->level / 2.8 + 1 );
	pObj->weight = pObj->level + 1;
	pObj->cost = pObj->level ^ 2 * 2;
	pObj->value[0] = size;
	pObj->value[1] = size;
	pObj->value[2] = size;
	pObj->value[3] = ( size - 1 );
	writeBuffer ( "\aRExperimental values for armor have been set.\r\n", ch );
	return TRUE;
}

OEDIT ( oedit_type )     /* Moved out of oedit() due to naming conflicts -- Hugin */
{
	ItemData *pObj;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_OBJ ( ch, pObj );

		if ( ( value = flag_value ( type_flags, argument ) ) != NO_FLAG ) {
			pObj->item_type = value;

			writeBuffer ( "Type set.\n\r", ch );

			/*
			 * Clear the values.
			 */
			pObj->value[0] = 0;
			pObj->value[1] = 0;
			pObj->value[2] = 0;
			pObj->value[3] = 0;
			pObj->value[4] = 0;     /* ROM */

			if ( pObj->item_type == ITEM_WEAPON ) {
				autoweapon ( ch, "" );
			}
			if ( pObj->item_type == ITEM_ARMOR ) {
				autoarmor ( ch, "" );
			}

			return TRUE;
		}
	}

	writeBuffer ( "Syntax:  type [flag]\n\r"
				  "Type '? type' for a list of flags.\n\r", ch );
	return FALSE;
}

OEDIT ( oedit_material )
{
	ItemData *pObj;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_OBJ ( ch, pObj );

		if ( ( value = flag_value ( material_flags, argument ) ) != NO_FLAG ) {
			pObj->material_flags = value;
			writeBuffer ( "Material toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: material [material-name]\n\r"
				  "Type '? material' for a list of materials.\n\r", ch );
	return FALSE;
}

OEDIT ( oedit_level )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  level [number]\n\r", ch );
		return FALSE;
	}

	pObj->level = atoi ( argument );

	if ( pObj->item_type == ITEM_WEAPON ) {
		autoweapon ( ch, "" );
	}

	if ( pObj->item_type == ITEM_ARMOR ) {
		autoarmor ( ch, "" );
	}

	writeBuffer ( "Level set.\r\n", ch );
	return TRUE;
}

OEDIT ( oedit_repop )
{
	ItemData *pObj;

	EDIT_OBJ ( ch, pObj );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  repop [number]\n\r", ch );
		return FALSE;
	}

	int p = atoi ( argument );
	if ( p < 0 || p > 100 ) {
		writeBuffer ( "Repop percentage must be between 1 and 100\r\n", ch );
		return false;
	}

	pObj->repop_percent = atoi ( argument );

	writeBuffer ( "Repop Percentage set.\r\n", ch );
	return TRUE;
}

OEDIT ( oedit_condition )
{
	ItemData *pObj;
	int value;

	if ( argument[0] != '\0'
			&& ( value = atoi ( argument ) ) >= 0
			&& ( value <= 100 ) ) {
		EDIT_OBJ ( ch, pObj );

		pObj->condition = value;
		writeBuffer ( "Condition set.\n\r", ch );

		return TRUE;
	}

	writeBuffer ( "Syntax:  condition [number]\n\r"
				  "Where number can range from 0 (ruined) to 100 (perfect).\n\r",
				  ch );
	return FALSE;
}





/*
 * Mobile Editor Functions.
 */
MEDIT ( medit_show )
{
	NPCData *pMob;
	char buf[MAX_STRING_LENGTH];
	MPROG_LIST *list;

	EDIT_MOB ( ch, pMob );

	sprintf ( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\rRepop:        [%5d]",
			  pMob->player_name,
			  !pMob->area ? -1        : pMob->area->vnum,
			  !pMob->area ? "No Area" : pMob->area->name,
			  pMob->repop_percent );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Act:         [%s]\n\r",
			  flag_string ( act_flags, pMob->act ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Vnum:        [%5d] Sex:   [%s]   Race: [%s]\n\r",
			  pMob->vnum,
			  pMob->sex == SEX_MALE    ? "male   " :
			  pMob->sex == SEX_FEMALE  ? "female " :
			  pMob->sex == 3           ? "random " : "neutral",
			  race_table[pMob->race].name );
	writeBuffer ( buf, ch );

	sprintf ( buf,
			  "Level:       [%2d]    Align: [%4d]      Hitroll: [%2d] Dam Type:    [%s]\n\r",
			  pMob->level,	pMob->alignment,
			  pMob->hitroll,	attack_table[pMob->dam_type].name );
	writeBuffer ( buf, ch );

	if ( pMob->group ) {
		sprintf ( buf, "Group:       [%5d]\n\r", pMob->group );
		writeBuffer ( buf, ch );
	}


	writeBuffer ( Format ( "Material Bits: [%s]\r\n", material_bit_name ( pMob->material_flags ) ), ch );

	writeBuffer ( Format ( "Random Bits: [%s]\r\n", random_eq_bit_name ( pMob->random ) ), ch );

	sprintf ( buf, "Hitdice:    [%2dd%-3d+%4d] ",
			  pMob->hit[DICE_NUMBER],
			  pMob->hit[DICE_TYPE],
			  pMob->hit[DICE_BONUS] );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Damage dice: [%2dd%-3d+%4d] ",
			  pMob->damage[DICE_NUMBER],
			  pMob->damage[DICE_TYPE],
			  pMob->damage[DICE_BONUS] );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Mana dice:   [%2dd%-3d+%4d]\n\r",
			  pMob->mana[DICE_NUMBER],
			  pMob->mana[DICE_TYPE],
			  pMob->mana[DICE_BONUS] );
	writeBuffer ( buf, ch );

	/* ROM values end */

	sprintf ( buf, "Affected by: [%s]\n\r",
			  flag_string ( affect_flags, pMob->affected_by ) );
	writeBuffer ( buf, ch );

	/* ROM values: */

	sprintf ( buf, "Armor:       [pierce: %d  bash: %d  slash: %d  magic: %d]\n\r",
			  pMob->ac[AC_PIERCE], pMob->ac[AC_BASH],
			  pMob->ac[AC_SLASH], pMob->ac[AC_EXOTIC] );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Form:        [%s]\n\r",
			  flag_string ( form_flags, pMob->form ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Parts:       [%s]\n\r",
			  flag_string ( part_flags, pMob->parts ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Imm:         [%s]\n\r",
			  flag_string ( imm_flags, pMob->imm_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Res:         [%s]\n\r",
			  flag_string ( res_flags, pMob->res_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Vuln:        [%s]\n\r",
			  flag_string ( vuln_flags, pMob->vuln_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Off:         [%s]\n\r",
			  flag_string ( off_flags,  pMob->off_flags ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Size:        [%s]\n\r",
			  flag_string ( size_flags, pMob->size ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Start pos.   [%s]\n\r",
			  flag_string ( position_flags, pMob->start_pos ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Default pos  [%s]\n\r",
			  flag_string ( position_flags, pMob->default_pos ) );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Wealth:      [%5ld]\n\r",
			  pMob->wealth );
	writeBuffer ( buf, ch );

	/* ROM values end */

	if ( pMob->spec_fun ) {
		sprintf ( buf, "Spec fun:    [%s]\n\r",  spec_name ( pMob->spec_fun ) );
		writeBuffer ( buf, ch );
	}

	sprintf ( buf, "Short descr: %s\n\rLong descr:\n\r%s",
			  pMob->short_descr,
			  pMob->long_descr );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Description:\n\r%s", pMob->description );
	writeBuffer ( buf, ch );

	if ( pMob->pShop ) {
		SHOP_DATA *pShop;
		int iTrade;

		pShop = pMob->pShop;

		sprintf ( buf,
				  "Shop data for [%5d]:\n\r"
				  "  Markup for purchaser: %d%%\n\r"
				  "  Markdown for seller:  %d%%\n\r",
				  pShop->keeper, pShop->profit_buy, pShop->profit_sell );
		writeBuffer ( buf, ch );
		sprintf ( buf, "  Hours: %d to %d.\n\r",
				  pShop->open_hour, pShop->close_hour );
		writeBuffer ( buf, ch );

		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ ) {
			if ( pShop->buy_type[iTrade] != 0 ) {
				if ( iTrade == 0 ) {
					writeBuffer ( "  Number Trades Type\n\r", ch );
					writeBuffer ( "  ------ -----------\n\r", ch );
				}
				sprintf ( buf, "  [%4d] %s\n\r", iTrade,
						  flag_string ( type_flags, pShop->buy_type[iTrade] ) );
				writeBuffer ( buf, ch );
			}
		}
	}

	if ( pMob->mprogs ) {
		int cnt;

		snprintf ( buf, sizeof ( buf ), "\n\rMOBPrograms for [%5d]:\n\r", pMob->vnum );
		writeBuffer ( buf, ch );

		for ( cnt = 0, list = pMob->mprogs; list; list = list->next ) {
			if ( cnt == 0 ) {
				writeBuffer ( " Number Vnum Trigger Phrase\n\r", ch );
				writeBuffer ( " ------ ---- ------- ------\n\r", ch );
			}

			snprintf ( buf, sizeof ( buf ), "[%5d] %4d %7s %s\n\r", cnt,
					   list->vnum, mprog_type_to_name ( list->trig_type ),
					   list->trig_phrase );
			writeBuffer ( buf, ch );
			cnt++;
		}
	}

	return FALSE;
}



MEDIT ( medit_create )
{
	NPCData *pMob;
	Zone *pArea;
	int  value;
	int  iHash;

	value = atoi ( argument );
	if ( argument[0] == '\0' || value == 0 ) {
		writeBuffer ( "Syntax:  medit create [vnum]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area ( value );

	if ( !pArea ) {
		writeBuffer ( "MEdit:  That vnum is not assigned an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER ( ch, pArea ) ) {
		writeBuffer ( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
		return FALSE;
	}

	if ( get_mob_index ( value ) ) {
		writeBuffer ( "MEdit:  Mobile vnum already exists.\n\r", ch );
		return FALSE;
	}

	pMob			= new_mob_index();
	pMob->vnum			= value;
	pMob->area			= pArea;

	if ( value > top_vnum_mob )
	{ top_vnum_mob = value; }

	pMob->act			= ACT_IS_NPC;
	iHash			= value % MAX_KEY_HASH;
	pMob->next			= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMob;
	ch->desc->pEdit		= ( void * ) pMob;

	writeBuffer ( "Mobile Created.\n\r", ch );
	return TRUE;
}



MEDIT ( medit_spec )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  spec [special function]\n\r", ch );
		return FALSE;
	}


	if ( SameString ( argument, "none" ) ) {
		pMob->spec_fun = NULL;

		writeBuffer ( "Spec removed.\n\r", ch );
		return TRUE;
	}

	if ( spec_lookup ( argument ) ) {
		pMob->spec_fun = spec_lookup ( argument );
		writeBuffer ( "Spec set.\n\r", ch );
		return TRUE;
	}

	writeBuffer ( "MEdit: No such special function.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_damtype )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  damtype [damage message]\n\r", ch );
		writeBuffer ( "For a list of damage types, type '? weapon'.\n\r", ch );
		return FALSE;
	}

	pMob->dam_type = attack_lookup ( argument );
	writeBuffer ( "Damage type set.\n\r", ch );
	return TRUE;
}


MEDIT ( medit_align )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  alignment [number]\n\r", ch );
		return FALSE;
	}

	pMob->alignment = atoi ( argument );

	writeBuffer ( "Alignment set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_material )
{
	NPCData *pObj;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pObj );

		if ( ( value = flag_value ( material_flags, argument ) ) != NO_FLAG ) {
			pObj->material_flags = value;
			writeBuffer ( "Material toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: material [material-name]\n\r"
				  "Type '? material' for a list of materials.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_repop )
{
	NPCData *pObj;

	EDIT_MOB ( ch, pObj );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  repop [number]\n\r", ch );
		return FALSE;
	}

	int p = atoi ( argument );
	if ( p < 0 || p > 100 ) {
		writeBuffer ( "Repop percentage must be between 1 and 100\r\n", ch );
		return false;
	}

	pObj->repop_percent = atoi ( argument );

	writeBuffer ( "Repop Percentage set.\r\n", ch );
	return TRUE;
}

MEDIT ( medit_level )
{
	NPCData *pMob;
	int argfun;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  level [number]\n\r", ch );
		return FALSE;
	}

	argfun = atoi ( argument );

	pMob->level = argfun;

	pMob->hit[DICE_NUMBER] = ( argfun );
	pMob->hit[DICE_TYPE] = ( argfun * 2 );
	pMob->hit[DICE_BONUS] = 1;
	pMob->damage[DICE_NUMBER] = ( argfun );
	pMob->damage[DICE_TYPE] = ( argfun / 20 );
	pMob->damage[DICE_BONUS] = Math::instance().range ( 1, 4 );
	pMob->hitroll = ( argfun * 2 / 6 );
	pMob->wealth = ( argfun * 10 );

	pMob->ac[AC_PIERCE] = ( argfun - ( argfun * 2 ) * 2 );
	pMob->ac[AC_BASH] = ( argfun - ( argfun * 2 ) * 2 );
	pMob->ac[AC_SLASH] = ( argfun - ( argfun * 2 ) * 2 );
	pMob->ac[AC_EXOTIC] = ( argfun - ( argfun * 2 ) * 2 );

	writeBuffer ( "Level set : Experimental Values set.\r\n", ch );
	return TRUE;
}



MEDIT ( medit_desc )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		EnterStringEditor ( ch, &pMob->description );
		return TRUE;
	}

	writeBuffer ( "Syntax:  desc    - line edit\n\r", ch );
	return FALSE;
}




MEDIT ( medit_long )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  long [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pMob->long_descr );

	pMob->long_descr = assign_string ( Format ( "%s\r\n", argument ) );
	pMob->long_descr[0] = UPPER ( pMob->long_descr[0]  );

	writeBuffer ( "Long description set.\n\r", ch );
	return TRUE;
}



MEDIT ( medit_short )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  short [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pMob->short_descr );
	pMob->short_descr = assign_string ( argument );

	writeBuffer ( "Short description set.\n\r", ch );
	return TRUE;
}



MEDIT ( medit_name )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  name [string]\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pMob->player_name );
	pMob->player_name = assign_string ( argument );

	writeBuffer ( "Name set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_shop )
{
	NPCData *pMob;
	char command[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, command );
	argument = ChopC ( argument, arg1 );

	EDIT_MOB ( ch, pMob );

	if ( command[0] == '\0' ) {
		writeBuffer ( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
		writeBuffer ( "         shop profit [#xbuying%] [#xselling%]\n\r", ch );
		writeBuffer ( "         shop type [#x0-4] [item type]\n\r", ch );
		writeBuffer ( "         shop assign\n\r", ch );
		writeBuffer ( "         shop remove\n\r", ch );
		return FALSE;
	}


	if ( SameString ( command, "hours" ) ) {
		if ( arg1[0] == '\0' || !is_number ( arg1 )
				|| argument[0] == '\0' || !is_number ( argument ) ) {
			writeBuffer ( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pShop ) {
			writeBuffer ( "MEdit:  You must first create the shop ('shop assign').\n\r", ch );
			return FALSE;
		}

		pMob->pShop->open_hour = atoi ( arg1 );
		pMob->pShop->close_hour = atoi ( argument );

		writeBuffer ( "Shop hours set.\n\r", ch );
		return TRUE;
	}


	if ( SameString ( command, "profit" ) ) {
		if ( arg1[0] == '\0' || !is_number ( arg1 )
				|| argument[0] == '\0' || !is_number ( argument ) ) {
			writeBuffer ( "Syntax:  shop profit [#xbuying%] [#xselling%]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pShop ) {
			writeBuffer ( "MEdit:  You must first create a shop ('shop assign').\n\r", ch );
			return FALSE;
		}

		pMob->pShop->profit_buy     = atoi ( arg1 );
		pMob->pShop->profit_sell    = atoi ( argument );

		writeBuffer ( "Shop profit set.\n\r", ch );
		return TRUE;
	}


	if ( SameString ( command, "type" ) ) {
		char buf[MAX_INPUT_LENGTH];
		int value;

		if ( arg1[0] == '\0' || !is_number ( arg1 )
				|| argument[0] == '\0' ) {
			writeBuffer ( "Syntax:  shop type [#x0-4] [item type]\n\r", ch );
			return FALSE;
		}

		if ( atoi ( arg1 ) >= MAX_TRADE ) {
			sprintf ( buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE );
			writeBuffer ( buf, ch );
			return FALSE;
		}

		if ( !pMob->pShop ) {
			writeBuffer ( "MEdit: You must first create a shop ('shop assign').\n\r", ch );
			return FALSE;
		}

		if ( ( value = flag_value ( type_flags, argument ) ) == NO_FLAG ) {
			writeBuffer ( "MEdit:  That type of item is not known.\n\r", ch );
			return FALSE;
		}

		pMob->pShop->buy_type[atoi ( arg1 )] = value;

		writeBuffer ( "Shop type set.\n\r", ch );
		return TRUE;
	}

	/* shop assign && shop delete by Phoenix */

	if ( !str_prefix ( command, "assign" ) ) {
		if ( pMob->pShop ) {
			writeBuffer ( "Mob already has a shop assigned to it.\n\r", ch );
			return FALSE;
		}

		pMob->pShop		= new_shop();
		if ( !shop_first )
		{ shop_first	= pMob->pShop; }
		if ( shop_last )
		{ shop_last->next	= pMob->pShop; }
		shop_last		= pMob->pShop;

		pMob->pShop->keeper	= pMob->vnum;

		writeBuffer ( "New shop assigned to mobile.\n\r", ch );
		return TRUE;
	}

	if ( !str_prefix ( command, "remove" ) ) {
		SHOP_DATA *pShop;

		pShop		= pMob->pShop;
		pMob->pShop	= NULL;

		if ( pShop == shop_first ) {
			if ( !pShop->next ) {
				shop_first = NULL;
				shop_last = NULL;
			} else
			{ shop_first = pShop->next; }
		} else {
			SHOP_DATA *ipShop;

			for ( ipShop = shop_first; ipShop; ipShop = ipShop->next ) {
				if ( ipShop->next == pShop ) {
					if ( !pShop->next ) {
						shop_last = ipShop;
						shop_last->next = NULL;
					} else
					{ ipShop->next = pShop->next; }
				}
			}
		}

		recycle_shop ( pShop );

		writeBuffer ( "Mobile is no longer a shopkeeper.\n\r", ch );
		return TRUE;
	}

	medit_shop ( ch, "" );
	return FALSE;
}


/* ROM medit functions: */


MEDIT ( medit_sex )         /* Moved out of medit() due to naming conflicts -- Hugin */
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( sex_flags, argument ) ) != NO_FLAG ) {
			pMob->sex = value;

			writeBuffer ( "Sex set.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: sex [sex]\n\r"
				  "Type '? sex' for a list of flags.\n\r", ch );
	return FALSE;
}


MEDIT ( medit_act )         /* Moved out of medit() due to naming conflicts -- Hugin */
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( act_flags, argument ) ) != NO_FLAG ) {
			pMob->act ^= value;
			SET_BIT ( pMob->act, ACT_IS_NPC );

			writeBuffer ( "Act flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: act [flag]\n\r"
				  "Type '? act' for a list of flags.\n\r", ch );
	return FALSE;
}


MEDIT ( medit_affect )     /* Moved out of medit() due to naming conflicts -- Hugin */
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( affect_flags, argument ) ) != NO_FLAG ) {
			pMob->affected_by ^= value;

			writeBuffer ( "Affect flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: affect [flag]\n\r"
				  "Type '? affect' for a list of flags.\n\r", ch );
	return FALSE;
}



MEDIT ( medit_ac )
{
	NPCData *pMob;
	char arg[MAX_INPUT_LENGTH];
	int pierce, bash, slash, exotic;

	do { /* So that I can use break and send the syntax in one place */
		if ( argument[0] == '\0' )  { break; }

		EDIT_MOB ( ch, pMob );
		argument = ChopC ( argument, arg );

		if ( !is_number ( arg ) )  { break; }
		pierce = atoi ( arg );
		argument = ChopC ( argument, arg );

		if ( arg[0] != '\0' ) {
			if ( !is_number ( arg ) )  { break; }
			bash = atoi ( arg );
			argument = ChopC ( argument, arg );
		} else
		{ bash = pMob->ac[AC_BASH]; }

		if ( arg[0] != '\0' ) {
			if ( !is_number ( arg ) )  { break; }
			slash = atoi ( arg );
			argument = ChopC ( argument, arg );
		} else
		{ slash = pMob->ac[AC_SLASH]; }

		if ( arg[0] != '\0' ) {
			if ( !is_number ( arg ) )  { break; }
			exotic = atoi ( arg );
		} else
		{ exotic = pMob->ac[AC_EXOTIC]; }

		pMob->ac[AC_PIERCE] = pierce;
		pMob->ac[AC_BASH]   = bash;
		pMob->ac[AC_SLASH]  = slash;
		pMob->ac[AC_EXOTIC] = exotic;

		writeBuffer ( "Ac set.\n\r", ch );
		return TRUE;
	} while ( FALSE );    /* Just do it once.. */

	writeBuffer ( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
				  "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_form )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( form_flags, argument ) ) != NO_FLAG ) {
			pMob->form ^= value;
			writeBuffer ( "Form toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: form [flags]\n\r"
				  "Type '? form' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_random )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( random_eq_flags, argument ) ) != NO_FLAG ) {
			pMob->random ^= value;
			writeBuffer ( "Random Item Type toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: random [flags]\n\r"
				  "Type '? random' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_part )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( part_flags, argument ) ) != NO_FLAG ) {
			pMob->parts ^= value;
			writeBuffer ( "Parts toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: part [flags]\n\r"
				  "Type '? part' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_imm )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( imm_flags, argument ) ) != NO_FLAG ) {
			pMob->imm_flags ^= value;
			writeBuffer ( "Immunity toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: imm [flags]\n\r"
				  "Type '? imm' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_res )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( res_flags, argument ) ) != NO_FLAG ) {
			pMob->res_flags ^= value;
			writeBuffer ( "Resistance toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: res [flags]\n\r"
				  "Type '? res' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_vuln )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( vuln_flags, argument ) ) != NO_FLAG ) {
			pMob->vuln_flags ^= value;
			writeBuffer ( "Vulnerability toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: vuln [flags]\n\r"
				  "Type '? vuln' for a list of flags.\n\r", ch );
	return FALSE;
}


MEDIT ( medit_off )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( off_flags, argument ) ) != NO_FLAG ) {
			pMob->off_flags ^= value;
			writeBuffer ( "Offensive behaviour toggled.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: off [flags]\n\r"
				  "Type '? off' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_size )
{
	NPCData *pMob;
	int value;

	if ( argument[0] != '\0' ) {
		EDIT_MOB ( ch, pMob );

		if ( ( value = flag_value ( size_flags, argument ) ) != NO_FLAG ) {
			pMob->size = value;
			writeBuffer ( "Size set.\n\r", ch );
			return TRUE;
		}
	}

	writeBuffer ( "Syntax: size [size]\n\r"
				  "Type '? size' for a list of sizes.\n\r", ch );
	return FALSE;
}

MEDIT ( medit_hitdice )
{
	static char syntax[] = "Syntax:  hitdice <number> d <type> + <bonus>\n\r";
	char *num, *type, *bonus, *cp;
	NPCData *pMob;

	char *sarg = ( char * ) argument;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	num = cp = sarg;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) )  { * ( cp++ ) = '\0'; }

	type = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) ) { * ( cp++ ) = '\0'; }

	bonus = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	if ( *cp != '\0' ) { *cp = '\0'; }

	if ( ( !is_number ( num   ) || atoi ( num   ) < 1 )
			||   ( !is_number ( type  ) || atoi ( type  ) < 1 )
			||   ( !is_number ( bonus ) || atoi ( bonus ) < 0 ) ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	pMob->hit[DICE_NUMBER] = atoi ( num   );
	pMob->hit[DICE_TYPE]   = atoi ( type  );
	pMob->hit[DICE_BONUS]  = atoi ( bonus );

	writeBuffer ( "Hitdice set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_manadice )
{
	static char syntax[] = "Syntax:  manadice <number> d <type> + <bonus>\n\r";
	char *num, *type, *bonus, *cp;
	NPCData *pMob;
	char *sarg = ( char * ) argument;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	num = cp = sarg;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) )  { * ( cp++ ) = '\0'; }

	type = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) ) { * ( cp++ ) = '\0'; }

	bonus = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	if ( *cp != '\0' ) { *cp = '\0'; }

	if ( ! ( is_number ( num ) && is_number ( type ) && is_number ( bonus ) ) ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	if ( ( !is_number ( num   ) || atoi ( num   ) < 1 )
			||   ( !is_number ( type  ) || atoi ( type  ) < 1 )
			||   ( !is_number ( bonus ) || atoi ( bonus ) < 0 ) ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	pMob->mana[DICE_NUMBER] = atoi ( num   );
	pMob->mana[DICE_TYPE]   = atoi ( type  );
	pMob->mana[DICE_BONUS]  = atoi ( bonus );

	writeBuffer ( "Manadice set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_damdice )
{
	static char syntax[] = "Syntax:  damdice <number> d <type> + <bonus>\n\r";
	char *num, *type, *bonus, *cp;
	NPCData *pMob;

	char *sarg = ( char * ) argument;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	num = cp = sarg;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) )  { * ( cp++ ) = '\0'; }

	type = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	while ( *cp != '\0' && !isdigit ( *cp ) ) { * ( cp++ ) = '\0'; }

	bonus = cp;

	while ( isdigit ( *cp ) ) { ++cp; }
	if ( *cp != '\0' ) { *cp = '\0'; }

	if ( ! ( is_number ( num ) && is_number ( type ) && is_number ( bonus ) ) ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	if ( ( !is_number ( num   ) || atoi ( num   ) < 1 )
			||   ( !is_number ( type  ) || atoi ( type  ) < 1 )
			||   ( !is_number ( bonus ) || atoi ( bonus ) < 0 ) ) {
		writeBuffer ( syntax, ch );
		return FALSE;
	}

	pMob->damage[DICE_NUMBER] = atoi ( num   );
	pMob->damage[DICE_TYPE]   = atoi ( type  );
	pMob->damage[DICE_BONUS]  = atoi ( bonus );

	writeBuffer ( "Damdice set.\n\r", ch );
	return TRUE;
}


MEDIT ( medit_race )
{
	NPCData *pMob;
	int race;

	if ( argument[0] != '\0'
			&& ( race = race_lookup ( argument ) ) != 0 ) {
		EDIT_MOB ( ch, pMob );

		pMob->race = race;
		pMob->act	  |= race_table[race].act;
		pMob->affected_by |= race_table[race].aff;
		pMob->off_flags   |= race_table[race].off;
		pMob->imm_flags   |= race_table[race].imm;
		pMob->res_flags   |= race_table[race].res;
		pMob->vuln_flags  |= race_table[race].vuln;
		pMob->form        |= race_table[race].form;
		pMob->parts       |= race_table[race].parts;

		writeBuffer ( "Race set.\n\r", ch );
		return TRUE;
	}

	if ( argument[0] == '?' ) {
		char buf[MAX_STRING_LENGTH];

		writeBuffer ( "Available races are:", ch );

		for ( race = 0; race_table[race].name != NULL; race++ ) {
			if ( ( race % 3 ) == 0 )
			{ writeBuffer ( "\n\r", ch ); }
			sprintf ( buf, " %-15s", race_table[race].name );
			writeBuffer ( buf, ch );
		}

		writeBuffer ( "\n\r", ch );
		return FALSE;
	}

	writeBuffer ( "Syntax:  race [race]\n\r"
				  "Type 'race ?' for a list of races.\n\r", ch );
	return FALSE;
}


MEDIT ( medit_position )
{
	NPCData *pMob;
	char arg[MAX_INPUT_LENGTH];
	int value;

	argument = ChopC ( argument, arg );

	switch ( arg[0] ) {
		default:
			break;

		case 'S':
		case 's':
			if ( str_prefix ( arg, "start" ) )
			{ break; }

			if ( ( value = flag_value ( position_flags, argument ) ) == NO_FLAG )
			{ break; }

			EDIT_MOB ( ch, pMob );

			pMob->start_pos = value;
			writeBuffer ( "Start position set.\n\r", ch );
			return TRUE;

		case 'D':
		case 'd':
			if ( str_prefix ( arg, "default" ) )
			{ break; }

			if ( ( value = flag_value ( position_flags, argument ) ) == NO_FLAG )
			{ break; }

			EDIT_MOB ( ch, pMob );

			pMob->default_pos = value;
			writeBuffer ( "Default position set.\n\r", ch );
			return TRUE;
	}

	writeBuffer ( "Syntax:  position [start/default] [position]\n\r"
				  "Type '? position' for a list of positions.\n\r", ch );
	return FALSE;
}


MEDIT ( medit_gold )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  wealth [number]\n\r", ch );
		return FALSE;
	}

	pMob->wealth = atoi ( argument );

	writeBuffer ( "Wealth set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_hitroll )
{
	NPCData *pMob;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' || !is_number ( argument ) ) {
		writeBuffer ( "Syntax:  hitroll [number]\n\r", ch );
		return FALSE;
	}

	pMob->hitroll = atoi ( argument );

	writeBuffer ( "Hitroll set.\n\r", ch );
	return TRUE;
}

void show_liqlist ( Creature *ch )
{
	int liq;
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();

	for ( liq = 0; liq_table[liq].liq_name != NULL; liq++ ) {
		if ( ( liq % 21 ) == 0 )
		{ add_buf ( buffer, "Name                 Color          Proof Full Thirst Food Ssize\n\r" ); }

		snprintf ( buf, sizeof ( buf ), "%-20s %-14s %5d %4d %6d %4d %5d\n\r",
				   liq_table[liq].liq_name, liq_table[liq].liq_color,
				   liq_table[liq].liq_affect[0], liq_table[liq].liq_affect[1],
				   liq_table[liq].liq_affect[2], liq_table[liq].liq_affect[3],
				   liq_table[liq].liq_affect[4] );
		add_buf ( buffer, buf );
	}

	writePage ( buf_string ( buffer ), ch );
	recycle_buf ( buffer );

	return;
}

void show_damlist ( Creature *ch )
{
	int att;
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();

	for ( att = 0; attack_table[att].name != NULL; att++ ) {
		if ( ( att % 21 ) == 0 )
		{ add_buf ( buffer, "Name                 Noun\n\r" ); }

		snprintf ( buf, sizeof ( buf ), "%-20s %-20s\n\r",
				   attack_table[att].name, attack_table[att].noun );
		add_buf ( buffer, buf );
	}

	writePage ( buf_string ( buffer ), ch );
	recycle_buf ( buffer );

	return;
}

MEDIT ( medit_group )
{
	NPCData *pMob;
	NPCData *pMTemp;
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int temp;
	BUFFER *buffer;
	bool found = FALSE;

	EDIT_MOB ( ch, pMob );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax: group [number]\n\r", ch );
		writeBuffer ( "        group show [number]\n\r", ch );
		return FALSE;
	}

	if ( is_number ( argument ) ) {
		pMob->group = atoi ( argument );
		writeBuffer ( "Group set.\n\r", ch );
		return TRUE;
	}

	argument = ChopC ( argument, arg );

	if ( !strcmp ( arg, "show" ) && is_number ( argument ) ) {
		if ( atoi ( argument ) == 0 ) {
			writeBuffer ( "Are you crazy?\n\r", ch );
			return FALSE;
		}

		buffer = new_buf ();

		for ( temp = 0; temp < 65536; temp++ ) {
			pMTemp = get_mob_index ( temp );
			if ( pMTemp && ( pMTemp->group == atoi ( argument ) ) ) {
				found = TRUE;
				sprintf ( buf, "[%5d] %s\n\r", pMTemp->vnum, pMTemp->player_name );
				add_buf ( buffer, buf );
			}
		}

		if ( found )
		{ writePage ( buf_string ( buffer ), ch ); }
		else
		{ writeBuffer ( "No mobs in that group.\n\r", ch ); }

		recycle_buf ( buffer );
		return FALSE;
	}

	return FALSE;
}

REDIT ( redit_owner )
{
	RoomData *pRoom;

	EDIT_ROOM ( ch, pRoom );

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Syntax:  owner [owner]\n\r", ch );
		writeBuffer ( "         owner none\n\r", ch );
		return FALSE;
	}

	PURGE_DATA ( pRoom->owner );
	if ( SameString ( argument, "none" ) )
	{ pRoom->owner = assign_string ( "" ); }
	else
	{ pRoom->owner = assign_string ( argument ); }

	writeBuffer ( "Owner set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_addmprog )
{
	int value;
	NPCData *pMob;
	MPROG_LIST *list;
	MPROG_CODE *code;
	char trigger[MAX_STRING_LENGTH];
	char phrase[MAX_STRING_LENGTH];
	char num[MAX_STRING_LENGTH];

	EDIT_MOB ( ch, pMob );
	argument = ChopC ( argument, num );
	argument = ChopC ( argument, trigger );
	argument = ChopC ( argument, phrase );

	if ( !is_number ( num ) || trigger[0] == '\0' || phrase[0] == '\0' ) {
		writeBuffer ( "Syntax:   addmprog [vnum] [trigger] [phrase]\n\r", ch );
		return FALSE;
	}

	if ( ( value = flag_value ( mprog_flags, trigger ) ) == NO_FLAG ) {
		writeBuffer ( "Valid flags are:\n\r", ch );
		show_help ( ch, "mprog" );
		return FALSE;
	}

	if ( ( code = get_mprog_index ( atoi ( num ) ) ) == NULL ) {
		writeBuffer ( "No such MOBProgram.\n\r", ch );
		return FALSE;
	}

	list                  = new_mprog();
	list->vnum            = atoi ( num );
	list->trig_type       = value;
	list->trig_phrase     = assign_string ( phrase );
	list->code            = code->code;
	SET_BIT ( pMob->mprog_flags, value );
	list->next            = pMob->mprogs;
	pMob->mprogs          = list;

	writeBuffer ( "Mprog Added.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_delmprog )
{
	NPCData *pMob;
	MPROG_LIST *list;
	MPROG_LIST *list_next;
	char mprog[MAX_STRING_LENGTH];
	int value;
	int cnt = 0;

	EDIT_MOB ( ch, pMob );

	ChopC ( argument, mprog );
	if ( !is_number ( mprog ) || mprog[0] == '\0' ) {
		writeBuffer ( "Syntax:  delmprog [#mprog]\n\r", ch );
		return FALSE;
	}

	value = atoi ( mprog );

	if ( value < 0 ) {
		writeBuffer ( "Only non-negative mprog-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( ! ( list = pMob->mprogs ) ) {
		writeBuffer ( "MEdit:  Non existant mprog.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 ) {
		REMOVE_BIT ( pMob->mprog_flags, pMob->mprogs->trig_type );
		list = pMob->mprogs;
		pMob->mprogs = list->next;
		recycle_mprog ( list );
	} else {
		while ( ( list_next = list->next ) && ( ++cnt < value ) )
		{ list = list_next; }

		if ( list_next ) {
			REMOVE_BIT ( pMob->mprog_flags, list_next->trig_type );
			list->next = list_next->next;
			recycle_mprog ( list_next );
		} else {
			writeBuffer ( "No such mprog.\n\r", ch );
			return FALSE;
		}
	}

	writeBuffer ( "Mprog removed.\n\r", ch );
	return TRUE;
}

REDIT ( redit_room )
{
	RoomData *room;
	int value;

	EDIT_ROOM ( ch, room );

	if ( ( value = flag_value ( room_flags, argument ) ) == NO_FLAG ) {
		writeBuffer ( "Syntax: room [flag(s)]\n\r", ch );
		return FALSE;
	}

	TOGGLE_BIT ( room->room_flags, value );
	writeBuffer ( "Room flags toggled.\n\r", ch );
	return TRUE;
}

REDIT ( redit_sector )
{
	RoomData *room;
	int value;

	EDIT_ROOM ( ch, room );

	if ( ( value = flag_value ( sector_flags, argument ) ) == NO_FLAG ) {
		writeBuffer ( "Syntax: sector [type]\n\r", ch );
		return FALSE;
	}

	room->sector_type = value;
	writeBuffer ( "Sector type set.\n\r", ch );

	return TRUE;
}
