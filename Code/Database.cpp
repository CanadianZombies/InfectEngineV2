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


// -- for SkillNumbering
#ifndef Database_Cpp
#define Database_Cpp
	#include "Engine.h"
#undef Database_Cpp

#if !defined(macintosh)
extern	int	_filbuf		args ( ( FILE * ) );
#endif

#if !defined(OLD_RAND)
#if !defined(linux)
long random();
#endif
void srandom ( unsigned int );
int getpid();
time_t time ( time_t *tloc );
#endif


/* externals for counting purposes */
extern	Item	*obj_free;
extern	Creature	*char_free;
extern  Socket *descriptor_free;
extern	PlayerData		*pcdata_free;
extern  Affect	*affect_free;

/*
 * Globals.
 */
HELP_DATA *		help_first;
HELP_DATA *		help_last;

HELP_AREA *		had_list;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

MPROG_CODE *		mprog_list;

Creature *		char_list;
char *			help_greeting;
KILL_DATA		kill_table	[MAX_LEVEL];

Item *		object_list;
TIME_INFO_DATA		time_info;
EnvironmentDataData		weather_info;

/*
 * Locals.
 */
NPCData *	mob_index_hash		[MAX_KEY_HASH];
ItemData *	obj_index_hash		[MAX_KEY_HASH];
RoomData *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];

Zone *		area_first;
Zone *		area_last;
Zone *		current_area;

char *			string_space;
char *			top_string;
char			str_empty	[1];

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;
int                     top_vnum_room;		/* OLC */
int                     top_vnum_mob;		/* OLC */
int                     top_vnum_obj;		/* OLC */
int			top_mprog_index;	/* OLC */
int 			mobile_count = 0;
int			newmobs = 0;
int			newobjs = 0;

unsigned long 		connect_count = 0;


/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	1413120

int			nAllocString;
int			sAllocString;

/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];



/*
 * Local booting procedures.
*/
void    init_mm         args ( ( void ) );
void	load_area	args ( ( FILE *fp ) );
void    new_load_area   args ( ( FILE *fp ) );  /* OLC */
void	load_helps	args ( ( FILE *fp, char *fname ) );
void	load_old_mob	args ( ( FILE *fp ) );
void 	load_mobiles	args ( ( FILE *fp ) );
void	load_old_obj	args ( ( FILE *fp ) );
void 	load_objects	args ( ( FILE *fp ) );
void	load_resets	args ( ( FILE *fp ) );
void	load_rooms	args ( ( FILE *fp ) );
void	load_shops	args ( ( FILE *fp ) );
void 	load_socials	args ( ( FILE *fp ) );
void	load_specials	args ( ( FILE *fp ) );
void	load_notes	args ( ( void ) );
void	load_bans	args ( ( void ) );
void	load_mobprogs	args ( ( FILE *fp ) );

void	fix_exits	args ( ( void ) );
void    fix_mobprogs	args ( ( void ) );

void	reset_area	args ( ( Zone * pArea ) );
void    load_connections ();

/*
 * Big mama top level function.
 */
void boot_db ( void )
{
	log_hd ( LOG_ALL, "Booting up Databases..." );
	/*
	 * Init some data space stuff.
	 */
	{
		if ( ( string_space = ( char * ) calloc ( 1, MAX_STRING ) ) == NULL ) {
			log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "Boot_db: can't alloc %d string space.", MAX_STRING ) );
			exit ( 1 );
		}
		top_string	= string_space;
		fBootDb		= TRUE;
	}

	/*
	 * Init random number generator.
	 */
	{
		log_hd ( LOG_ALL, "Initiating Random Number Generator..." );
		init_mm( );
	}

	/*
	 * Set time and weather.
	 */
	{
		log_hd ( LOG_ALL, "Generating Environmental Data..." );
		long lhour, lday, lmonth;

		lhour		= ( current_time - 650336715 )
					  / ( PULSE_TICK / PULSE_PER_SECOND );
		time_info.hour	= lhour  % 24;
		lday		= lhour  / 24;
		time_info.day	= lday   % 35;
		lmonth		= lday   / 35;
		time_info.month	= lmonth % 17;
		time_info.year	= lmonth / 17;

		if ( time_info.hour <  5 ) { weather_info.sunlight = SUN_DARK; }
		else if ( time_info.hour <  6 ) { weather_info.sunlight = SUN_RISE; }
		else if ( time_info.hour < 19 ) { weather_info.sunlight = SUN_LIGHT; }
		else if ( time_info.hour < 20 ) { weather_info.sunlight = SUN_SET; }
		else                            { weather_info.sunlight = SUN_DARK; }

		weather_info.change	= 0;
		weather_info.mmhg	= 960;
		if ( time_info.month >= 7 && time_info.month <= 12 )
		{ weather_info.mmhg += number_range ( 1, 50 ); }
		else
		{ weather_info.mmhg += number_range ( 1, 80 ); }

		if ( weather_info.mmhg <=  980 ) { weather_info.sky = SKY_LIGHTNING; }
		else if ( weather_info.mmhg <= 1000 ) { weather_info.sky = SKY_RAINING; }
		else if ( weather_info.mmhg <= 1020 ) { weather_info.sky = SKY_CLOUDY; }
		else                                  { weather_info.sky = SKY_CLOUDLESS; }

	}

	/*
	 * Assign gsn's for skills which have them.
	 */
	{
		int sn;
		log_hd ( LOG_ALL, "Assigning Skill table GSN data" );
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].pgsn != NULL ) {
				log_hd ( LOG_ALL, Format ( "\tSkill: %s assigned to skill #%d...", skill_table[sn].name ? skill_table[sn].name : "Unknown Skill", sn ) );
				*skill_table[sn].pgsn = sn;
			}
		}
	}

	/*
	 * Read in all the area files.
	 */
	{
		FILE *fpList;
		log_hd ( LOG_ALL, "Attempting to load world district data..." );

		if ( ( fpList = fopen ( Format ( "%s%s", WORLD_DIR, AREA_LIST ), "r" ) ) == NULL ) {
			ReportErrno( AREA_LIST );
			exit ( 1 );
		}

		for ( ; ; ) {
			strcpy ( strArea, fread_word ( fpList ) );
			if ( strArea[0] == '$' )
			{ break; }

			if ( strArea[0] == '-' ) {
				fpArea = stdin;
			} else {
				if ( ( fpArea = fopen ( Format ( "%s%s", WORLD_DIR, strArea ), "r" ) ) == NULL ) {
					ReportErrno( strArea );
					exit ( 1 );
				}
			}

			current_area = NULL;

			log_hd ( LOG_ALL, Format ( "\tReading District: %s%s", WORLD_DIR, strArea ) );

			for ( ; ; ) {
				char *word;

				if ( fread_letter ( fpArea ) != '#' ) {
					log_hd ( LOG_ERROR, "Boot_db: # not found." );
					exit ( 1 );
				}

				word = fread_word ( fpArea );

				if ( word[0] == '$'               )                 { break; }
				else if ( !str_cmp ( word, "AREA"     ) ) { load_area    ( fpArea ); }
				/* OLC */     else if ( !str_cmp ( word, "AREADATA" ) ) { new_load_area ( fpArea ); }
				else if ( !str_cmp ( word, "HELPS"    ) ) { load_helps   ( fpArea, strArea ); }
				else if ( !str_cmp ( word, "MOBOLD"   ) ) { load_old_mob ( fpArea ); }
				else if ( !str_cmp ( word, "MOBILES"  ) ) { load_mobiles ( fpArea ); }
				else if ( !str_cmp ( word, "MOBPROGS" ) ) { load_mobprogs ( fpArea ); }
				else if ( !str_cmp ( word, "OBJOLD"   ) ) { load_old_obj ( fpArea ); }
				else if ( !str_cmp ( word, "OBJECTS"  ) ) { load_objects ( fpArea ); }
				else if ( !str_cmp ( word, "RESETS"   ) ) { load_resets  ( fpArea ); }
				else if ( !str_cmp ( word, "ROOMS"    ) ) { load_rooms   ( fpArea ); }
				else if ( !str_cmp ( word, "SHOPS"    ) ) { load_shops   ( fpArea ); }
				else if ( !str_cmp ( word, "SOCIALS"  ) ) { load_socials ( fpArea ); }
				else if ( !str_cmp ( word, "SPECIALS" ) ) { load_specials ( fpArea ); }
				else {
					log_hd ( LOG_ERROR,  "Boot_db: bad section name." );
					exit ( 1 );
				}
			}

			if ( fpArea != stdin )
			{ fclose ( fpArea ); }
			fpArea = NULL;
		}
		fclose ( fpList );
	}

	/*
	 * Fix up exits.
	 * Declare db booting over.
	 * Reset all areas once.
	 * Load up the songs, notes and ban files.
	 */
	{
		log_hd ( LOG_ALL, "Fixing automated Exits..." );
		fix_exits( );

		log_hd ( LOG_ALL, "Fixing MUD scripts..." );
		fix_mobprogs( );

		log_hd ( LOG_ALL, "Boot Status changed..." );
		fBootDb	= FALSE;

		log_hd ( LOG_ALL, "Attempting to Convert old-style objects to new format..." );
		convert_objects( );           /* ROM OLC */

		log_hd ( LOG_ALL, "Updating Districts..." );
		area_update( );

		log_hd ( LOG_ALL, "Loading global note system..." );
		load_notes( );

		log_hd ( LOG_ALL, "Loading Security related ban data..." );
		load_bans();

		log_hd ( LOG_ALL, "Loading connection count data..." );
		load_connections();

		log_hd ( LOG_ALL, "Loading JukeBOX data..." );
		load_songs();
	}

	log_hd ( LOG_ALL, "Database booting has completed..." );
	return;
}


void load_connections()
{
	FILE *fh = NULL;
	if ( ( fh = fopen ( CONNECTED_DAT, "r" ) ) != NULL ) {
		fread ( &connect_count, sizeof ( unsigned long int ), 1, fh );
		fclose ( fh );
	}
}


/*
 * Snarf an 'area' header line.
 */
void load_area ( FILE *fp )
{
	Zone *pArea;

	ALLOC_DATA ( pArea, Zone, 1 );

	pArea->file_name	= fread_string ( fp );

	pArea->area_flags   = AREA_LOADING;         /* OLC */
	pArea->security     = 9;                    /* OLC */ /* 9 -- Hugin */
	pArea->builders     = assign_string ( "None" );   /* OLC */
	pArea->vnum         = top_area;             /* OLC */

	pArea->name		= fread_string ( fp );
	pArea->credits	= fread_string ( fp );
	pArea->min_vnum	= fread_number ( fp );
	pArea->max_vnum	= fread_number ( fp );
	pArea->age		= 15;
	pArea->nplayer	= 0;
	pArea->empty	= FALSE;

	if ( !area_first )
	{ area_first = pArea; }
	if ( area_last ) {
		area_last->next = pArea;
		REMOVE_BIT ( area_last->area_flags, AREA_LOADING );     /* OLC */
	}
	area_last		= pArea;
	pArea->next		= NULL;
	current_area	= pArea;

	top_area++;
	return;
}

/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
	if ( !str_cmp( word, literal ) )    \
	{                                   \
		field  = value;                 \
		fMatch = TRUE;                  \
		break;                          \
	}

#define SKEY( string, field )                       \
	if ( !str_cmp( word, string ) )     \
	{                                   \
		PURGE_DATA( field );           \
		field = fread_string( fp );     \
		fMatch = TRUE;                  \
		break;                          \
	}



/* OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void new_load_area ( FILE *fp )
{
	Zone *pArea;
	char      *word;
	bool      fMatch;

	ALLOC_DATA ( pArea, Zone, 1 );

	pArea->age          = 15;
	pArea->nplayer      = 0;
	pArea->file_name     = assign_string ( strArea );
	pArea->vnum         = top_area;
	pArea->name         = assign_string ( "New Area" );
	pArea->builders     = assign_string ( "" );
	pArea->security     = 9;                    /* 9 -- Hugin */
	pArea->min_vnum        = 0;
	pArea->max_vnum        = 0;
	pArea->area_flags   = 0;
	/*  pArea->recall       = ROOM_VNUM_TEMPLE;        ROM OLC */

	for ( ; ; ) {
		word   = feof ( fp ) ? ( char * ) "End" : fread_word ( fp );
		fMatch = FALSE;

		switch ( UPPER ( word[0] ) ) {
			case 'N':
				SKEY ( "Name", pArea->name );
				break;
			case 'S':
				KEY ( "Security", pArea->security, fread_number ( fp ) );
				break;
			case 'V':
				if ( !str_cmp ( word, "VNUMs" ) ) {
					pArea->min_vnum = fread_number ( fp );
					pArea->max_vnum = fread_number ( fp );
					fMatch = true;
				}
				break;
			case 'E':
				if ( !str_cmp ( word, "End" ) ) {
					fMatch = TRUE;
					if ( area_first == NULL )
					{ area_first = pArea; }
					if ( area_last  != NULL )
					{ area_last->next = pArea; }
					area_last	= pArea;
					pArea->next	= NULL;
					current_area	= pArea;
					top_area++;

					return;
				}
				break;
			case 'B':
				SKEY ( "Builders", pArea->builders );
				break;
			case 'C':
				SKEY ( "Credits", pArea->credits );
				break;
		}

		if ( !fMatch ) {
			log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "Unknown word in district %s, word: %s", strArea, !IS_NULLSTR ( word ) ? word : "{NULL WORD}" ) );
			SUICIDE;
		}

	}
}

/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum ( int vnum )
{
	if ( area_last->min_vnum == 0 || area_last->max_vnum == 0 )
	{ area_last->min_vnum = area_last->max_vnum = vnum; }

	if ( vnum != URANGE ( area_last->min_vnum, vnum, area_last->max_vnum ) ) {
		if ( vnum < area_last->min_vnum )
		{ area_last->min_vnum = vnum; }
		else
		{ area_last->max_vnum = vnum; }
	}
	return;
}

/*
 * Snarf a help section.
 */
void load_helps ( FILE *fp, char *fname )
{
	HELP_DATA *pHelp;
	int level;
	char *keyword;

	for ( ; ; ) {
		HELP_AREA * had;

		level		= fread_number ( fp );
		keyword		= fread_string ( fp );

		if ( keyword[0] == '$' )
		{ break; }

		if ( !had_list ) {
			had			= new_had ();
			had->filename		= assign_string ( fname );
			had->area		= current_area;
			if ( current_area )
			{ current_area->helps	= had; }
			had_list		= had;
		} else if ( str_cmp ( fname, had_list->filename ) ) {
			had			= new_had ();
			had->filename		= assign_string ( fname );
			had->area		= current_area;
			if ( current_area )
			{ current_area->helps	= had; }
			had->next		= had_list;
			had_list		= had;
		} else
		{ had			= had_list; }

		pHelp		= new_help( );
		pHelp->level	= level;
		pHelp->keyword	= keyword;
		pHelp->text	= fread_string ( fp );

		if ( !str_cmp ( pHelp->keyword, "greeting" ) )
		{ help_greeting = pHelp->text; }

		if ( help_first == NULL )
		{ help_first = pHelp; }
		if ( help_last  != NULL )
		{ help_last->next = pHelp; }

		help_last		= pHelp;
		pHelp->next		= NULL;

		if ( !had->first )
		{ had->first	= pHelp; }
		if ( !had->last )
		{ had->last	= pHelp; }

		had->last->next_area	= pHelp;
		had->last		= pHelp;
		pHelp->next_area	= NULL;

		top_help++;
	}

	return;
}



/*
 * Snarf a mob section.  old style
 */
void load_old_mob ( FILE *fp )
{
	NPCData *pMobIndex;
	/* for race updating */
	int race;
	char name[MAX_STRING_LENGTH];

	if ( !area_last ) { /* OLC */
		log_hd ( LOG_ERROR, "Load_mobiles: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		sh_int vnum;
		char letter;
		int iHash;

		letter				= fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_mobiles: # not found." );
			exit ( 1 );
		}

		vnum				= fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_mob_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_mobiles: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pMobIndex, NPCData, 1 );

		pMobIndex->vnum			= vnum;
		pMobIndex->area                 = area_last;               /* OLC */
		pMobIndex->new_format		= FALSE;
		pMobIndex->player_name		= fread_string ( fp );
		pMobIndex->short_descr		= fread_string ( fp );
		pMobIndex->long_descr		= fread_string ( fp );
		pMobIndex->description		= fread_string ( fp );

		pMobIndex->long_descr[0]	= UPPER ( pMobIndex->long_descr[0] );
		pMobIndex->description[0]	= UPPER ( pMobIndex->description[0] );

		pMobIndex->act			= fread_flag ( fp ) | ACT_IS_NPC;
		pMobIndex->affected_by		= fread_flag ( fp );
		pMobIndex->pShop		= NULL;
		pMobIndex->alignment		= fread_number ( fp );
		letter				= fread_letter ( fp );
		pMobIndex->level		= fread_number ( fp );

		/*
		 * The unused stuff is for imps who want to use the old-style
		 * stats-in-files method.
		 */
		fread_number ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		/* 'd'		*/		  fread_letter ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		/* '+'		*/		  fread_letter ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		/* 'd'		*/		  fread_letter ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		/* '+'		*/		  fread_letter ( fp );	/* Unused */
		fread_number ( fp );	/* Unused */
		pMobIndex->wealth               = fread_number ( fp ) / 20;
		/* xp can't be used! */		  fread_number ( fp );	/* Unused */
		pMobIndex->start_pos		= fread_number ( fp );	/* Unused */
		pMobIndex->default_pos		= fread_number ( fp );	/* Unused */

		if ( pMobIndex->start_pos < POS_SLEEPING )
		{ pMobIndex->start_pos = POS_STANDING; }
		if ( pMobIndex->default_pos < POS_SLEEPING )
		{ pMobIndex->default_pos = POS_STANDING; }

		/*
		 * Back to meaningful values.
		 */
		pMobIndex->sex			= fread_number ( fp );

		/* compute the race BS */
		one_argument ( pMobIndex->player_name, name );

		if ( name[0] == '\0' || ( race =  race_lookup ( name ) ) == 0 ) {
			/* fill in with blanks */
			pMobIndex->race = race_lookup ( "human" );
			pMobIndex->off_flags = OFF_DODGE | OFF_DISARM | OFF_TRIP | ASSIST_VNUM;
			pMobIndex->imm_flags = 0;
			pMobIndex->res_flags = 0;
			pMobIndex->vuln_flags = 0;
			pMobIndex->form = FORM_EDIBLE | FORM_SENTIENT | FORM_BIPED | FORM_MAMMAL;
			pMobIndex->parts = PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART |
							   PART_BRAINS | PART_GUTS;
		} else {
			pMobIndex->race = race;
			pMobIndex->off_flags = OFF_DODGE | OFF_DISARM | OFF_TRIP | ASSIST_RACE |
								   race_table[race].off;
			pMobIndex->imm_flags = race_table[race].imm;
			pMobIndex->res_flags = race_table[race].res;
			pMobIndex->vuln_flags = race_table[race].vuln;
			pMobIndex->form = race_table[race].form;
			pMobIndex->parts = race_table[race].parts;
		}

		if ( letter != 'S' ) {
			log_hd ( LOG_ERROR, Format ( "Load_mobiles: vnum %d non-S.", vnum ) );
			exit ( 1 );
		}

		convert_mobile ( pMobIndex );                          /* ROM OLC */

		iHash			= vnum % MAX_KEY_HASH;
		pMobIndex->next		= mob_index_hash[iHash];
		mob_index_hash[iHash]	= pMobIndex;
		top_mob_index++;
		top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;  /* OLC */
		assign_area_vnum ( vnum );                                 /* OLC */
		kill_table[URANGE ( 0, pMobIndex->level, MAX_LEVEL - 1 )].number++;
	}

	return;
}

/*
 * Snarf an obj section.  old style
 */
void load_old_obj ( FILE *fp )
{
	ItemData *pObjIndex;

	if ( !area_last ) { /* OLC */
		log_hd ( LOG_ERROR, "Load_objects: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		sh_int vnum;
		char letter;
		int iHash;

		letter				= fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_objects: # not found." );
			exit ( 1 );
		}

		vnum				= fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_obj_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_objects: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pObjIndex, ItemData, 1 );

		pObjIndex->vnum			= vnum;
		pObjIndex->area                 = area_last;            /* OLC */
		pObjIndex->new_format		= FALSE;
		pObjIndex->reset_num	 	= 0;
		pObjIndex->name			= fread_string ( fp );
		pObjIndex->short_descr		= fread_string ( fp );
		pObjIndex->description		= fread_string ( fp );
		/* Action description */	  fread_string ( fp );

		pObjIndex->short_descr[0]	= LOWER ( pObjIndex->short_descr[0] );
		pObjIndex->description[0]	= UPPER ( pObjIndex->description[0] );
		pObjIndex->material		= assign_string ( "" );

		pObjIndex->item_type		= fread_number ( fp );
		pObjIndex->extra_flags		= fread_flag ( fp );
		pObjIndex->wear_flags		= fread_flag ( fp );
		pObjIndex->value[0]		= fread_number ( fp );
		pObjIndex->value[1]		= fread_number ( fp );
		pObjIndex->value[2]		= fread_number ( fp );
		pObjIndex->value[3]		= fread_number ( fp );
		pObjIndex->value[4]		= 0;
		pObjIndex->level		= 0;
		pObjIndex->condition 		= 100;
		pObjIndex->weight		= fread_number ( fp );
		pObjIndex->cost			= fread_number ( fp );	/* Unused */
		/* Cost per day */		  fread_number ( fp );


		if ( pObjIndex->item_type == ITEM_WEAPON ) {
			if ( is_name ( "two", pObjIndex->name )
					||  is_name ( "two-handed", pObjIndex->name )
					||  is_name ( "claymore", pObjIndex->name ) )
			{ SET_BIT ( pObjIndex->value[4], WEAPON_TWO_HANDS ); }
		}

		for ( ; ; ) {
			char letter;

			letter = fread_letter ( fp );

			if ( letter == 'A' ) {
				Affect *paf;

				ALLOC_DATA ( paf, Affect, 1 );

				paf->where		= TO_OBJECT;
				paf->type		= -1;
				paf->level		= 20; /* RT temp fix */
				paf->duration		= -1;
				paf->location		= fread_number ( fp );
				paf->modifier		= fread_number ( fp );
				paf->bitvector		= 0;
				paf->next		= pObjIndex->affected;
				pObjIndex->affected	= paf;
				top_affect++;
			}

			else if ( letter == 'E' ) {
				DescriptionData *ed;

				ALLOC_DATA ( ed, DescriptionData, 1 );
				ed->keyword		= fread_string ( fp );
				ed->description		= fread_string ( fp );
				ed->next		= pObjIndex->extra_descr;
				pObjIndex->extra_descr	= ed;
				top_ed++;
			}

			else {
				ungetc ( letter, fp );
				break;
			}
		}

		/* fix armors */
		if ( pObjIndex->item_type == ITEM_ARMOR ) {
			pObjIndex->value[1] = pObjIndex->value[0];
			pObjIndex->value[2] = pObjIndex->value[1];
		}

		/*
		 * Translate spell "slot numbers" to internal "skill numbers."
		 */
		switch ( pObjIndex->item_type ) {
			case ITEM_PILL:
			case ITEM_POTION:
			case ITEM_SCROLL:
				pObjIndex->value[1] = slot_lookup ( pObjIndex->value[1] );
				pObjIndex->value[2] = slot_lookup ( pObjIndex->value[2] );
				pObjIndex->value[3] = slot_lookup ( pObjIndex->value[3] );
				pObjIndex->value[4] = slot_lookup ( pObjIndex->value[4] );
				break;

			case ITEM_STAFF:
			case ITEM_WAND:
				pObjIndex->value[3] = slot_lookup ( pObjIndex->value[3] );
				break;
		}

		iHash			= vnum % MAX_KEY_HASH;
		pObjIndex->next		= obj_index_hash[iHash];
		obj_index_hash[iHash]	= pObjIndex;
		top_obj_index++;
		top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;   /* OLC */
		assign_area_vnum ( vnum );                                  /* OLC */
	}

	return;
}

/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset ( RoomData *pR, Reset *pReset )
{
	Reset *pr;

	if ( !pR )
	{ return; }

	pr = pR->reset_last;

	if ( !pr ) {
		pR->reset_first = pReset;
		pR->reset_last  = pReset;
	} else {
		pR->reset_last->next = pReset;
		pR->reset_last       = pReset;
		pR->reset_last->next = NULL;
	}

	/*    top_reset++; no estamos asignando memoria!!!! */

	return;
}

/*
 * Snarf a reset section.
 */
void load_resets ( FILE *fp )
{
	Reset *pReset;
	Exit *pexit;
	RoomData *pRoomIndex;
	int rVnum = -1;

	if ( !area_last ) {
		log_hd ( LOG_ERROR, "Load_resets: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		char letter;

		if ( ( letter = fread_letter ( fp ) ) == 'S' )
		{ break; }

		if ( letter == '*' ) {
			fread_to_eol ( fp );
			continue;
		}

		pReset		= new_reset_data();
		pReset->command	= letter;
		/* if_flag */	  fread_number ( fp );
		pReset->arg1	= fread_number ( fp );
		pReset->arg2	= fread_number ( fp );
		pReset->arg3	= ( letter == 'G' || letter == 'R' )
						  ? 0 : fread_number ( fp );
		pReset->arg4	= ( letter == 'P' || letter == 'M' )
						  ? fread_number ( fp ) : 0;
		fread_to_eol ( fp );

		switch ( pReset->command ) {
			case 'M':
			case 'O':
				rVnum = pReset->arg3;
				break;

			case 'P':
			case 'G':
			case 'E':
				break;

			case 'D':
				pRoomIndex = get_room_index ( ( rVnum = pReset->arg1 ) );
				if ( pReset->arg2 < 0
						||   pReset->arg2 >= MAX_DIR
						|| !pRoomIndex
						|| ! ( pexit = pRoomIndex->exit[pReset->arg2] )
						|| !IS_SET ( pexit->rs_flags, EX_ISDOOR ) ) {
					log_hd ( LOG_ERROR, Format ( "Load_resets: 'D': exit %d, room %d not door.", pReset->arg2, pReset->arg1 ) );
					exit ( 1 );
				}

				switch ( pReset->arg3 ) {
					default:
						log_hd ( LOG_ERROR, Format ( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3 ) );
						break;
					case 0:
						break;
					case 1:
						SET_BIT ( pexit->rs_flags, EX_CLOSED );
						SET_BIT ( pexit->exit_info, EX_CLOSED );
						break;
					case 2:
						SET_BIT ( pexit->rs_flags, EX_CLOSED | EX_LOCKED );
						SET_BIT ( pexit->exit_info, EX_CLOSED | EX_LOCKED );
						break;
				}
				break;

			case 'R':
				rVnum = pReset->arg1;
				break;
		}

		if ( rVnum == -1 ) {
			log_hd ( LOG_ERROR, "load_resets : rVnum == -1" );
			exit ( 1 );
		}

		if ( pReset->command != 'D' )
		{ new_reset ( get_room_index ( rVnum ), pReset ); }
		else
		{ recycle_reset_data ( pReset ); }
	}

	return;
}

/*
 * Snarf a room section.
 */
void load_rooms ( FILE *fp )
{
	RoomData *pRoomIndex;

	if ( area_last == NULL ) {
		log_hd ( LOG_ERROR, "Load_resets: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		sh_int vnum;
		char letter;
		int door;
		int iHash;

		letter				= fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_rooms: # not found." );
			exit ( 1 );
		}

		vnum				= fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_room_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_rooms: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pRoomIndex, RoomData, 1 );

		pRoomIndex->owner		= assign_string ( "" );
		pRoomIndex->people		= NULL;
		pRoomIndex->contents		= NULL;
		pRoomIndex->extra_descr		= NULL;
		pRoomIndex->area		= area_last;
		pRoomIndex->vnum		= vnum;
		pRoomIndex->name		= fread_string ( fp );
		pRoomIndex->description		= fread_string ( fp );
		/* Area number */		  fread_number ( fp );
		pRoomIndex->room_flags		= fread_flag ( fp );
		/* horrible hack */
		if ( 3000 <= vnum && vnum < 3400 )
		{ SET_BIT ( pRoomIndex->room_flags, ROOM_LAW ); }
		pRoomIndex->sector_type		= fread_number ( fp );
		pRoomIndex->light		= 0;
		for ( door = 0; door <= 5; door++ )
		{ pRoomIndex->exit[door] = NULL; }

		/* defaults */
		pRoomIndex->heal_rate = 100;
		pRoomIndex->mana_rate = 100;

		for ( ; ; ) {
			letter = fread_letter ( fp );

			if ( letter == 'S' )
			{ break; }

			if ( letter == 'H' ) /* healing room */
			{ pRoomIndex->heal_rate = fread_number ( fp ); }

			else if ( letter == 'M' ) /* mana room */
			{ pRoomIndex->mana_rate = fread_number ( fp ); }

			else if ( letter == 'C' ) { /* clan */
				if ( pRoomIndex->clan ) {
					log_hd ( LOG_ERROR, "Load_rooms: duplicate clan fields." );
					exit ( 1 );
				}
				pRoomIndex->clan = clan_lookup ( fread_string ( fp ) );
			}


			else if ( letter == 'D' ) {
				Exit *pexit;
				int locks;

				door = fread_number ( fp );
				if ( door < 0 || door > 5 ) {
					log_hd ( LOG_ERROR, Format ( "Fread_rooms: vnum %d has bad door number.", vnum ) );
					exit ( 1 );
				}

				ALLOC_DATA ( pexit, Exit, 1 );

				pexit->description	= fread_string ( fp );
				pexit->keyword		= fread_string ( fp );
				pexit->exit_info	= 0;
				pexit->rs_flags         = 0;                    /* OLC */
				locks			= fread_number ( fp );
				pexit->key		= fread_number ( fp );
				pexit->u1.vnum		= fread_number ( fp );
				pexit->orig_door	= door;			/* OLC */

				switch ( locks ) {
					case 1:
						pexit->exit_info = EX_ISDOOR;
						pexit->rs_flags  = EX_ISDOOR;
						break;
					case 2:
						pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;
						pexit->rs_flags  = EX_ISDOOR | EX_PICKPROOF;
						break;
					case 3:
						pexit->exit_info = EX_ISDOOR | EX_NOPASS;
						pexit->rs_flags  = EX_ISDOOR | EX_NOPASS;
						break;
					case 4:
						pexit->exit_info = EX_ISDOOR | EX_NOPASS | EX_PICKPROOF;
						pexit->rs_flags  = EX_ISDOOR | EX_NOPASS | EX_PICKPROOF;
						break;
				}

				pRoomIndex->exit[door]	= pexit;
				top_exit++;
			} else if ( letter == 'E' ) {
				DescriptionData *ed;

				ALLOC_DATA ( ed, DescriptionData, 1 );

				ed->keyword		= fread_string ( fp );
				ed->description		= fread_string ( fp );
				ed->next		= pRoomIndex->extra_descr;
				pRoomIndex->extra_descr	= ed;
				top_ed++;
			}

			else if ( letter == 'O' ) {
				if ( pRoomIndex->owner[0] != '\0' ) {
					log_hd ( LOG_ERROR, "Load_rooms: duplicate owner." );
					exit ( 1 );
				}

				pRoomIndex->owner = fread_string ( fp );
			}

			else {
				log_hd ( LOG_ERROR, Format ( "Load_rooms: vnum %d has flag not 'DES'.", vnum ) );
				exit ( 1 );
			}
		}

		iHash			= vnum % MAX_KEY_HASH;
		pRoomIndex->next	= room_index_hash[iHash];
		room_index_hash[iHash]	= pRoomIndex;
		top_room++;
		top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; /* OLC */
		assign_area_vnum ( vnum );                                   /* OLC */
	}

	return;
}



/*
 * Snarf a shop section.
 */
void load_shops ( FILE *fp )
{
	SHOP_DATA *pShop;

	for ( ; ; ) {
		NPCData *pMobIndex;
		int iTrade;

		ALLOC_DATA ( pShop, SHOP_DATA, 1 );

		pShop->keeper		= fread_number ( fp );
		if ( pShop->keeper == 0 )
		{ break; }
		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		{ pShop->buy_type[iTrade]	= fread_number ( fp ); }
		pShop->profit_buy	= fread_number ( fp );
		pShop->profit_sell	= fread_number ( fp );
		pShop->open_hour	= fread_number ( fp );
		pShop->close_hour	= fread_number ( fp );
		fread_to_eol ( fp );
		pMobIndex		= get_mob_index ( pShop->keeper );
		pMobIndex->pShop	= pShop;

		if ( shop_first == NULL )
		{ shop_first = pShop; }
		if ( shop_last  != NULL )
		{ shop_last->next = pShop; }

		shop_last	= pShop;
		pShop->next	= NULL;
		top_shop++;
	}

	return;
}


/*
 * Snarf spec proc declarations.
 */
void load_specials ( FILE *fp )
{
	for ( ; ; ) {
		NPCData *pMobIndex;
		char letter;

		switch ( letter = fread_letter ( fp ) ) {
			default:
				log_hd ( LOG_ERROR, Format ( "Load_specials: letter '%c' not *MS.", letter ) );
				exit ( 1 );

			case 'S':
				return;

			case '*':
				break;

			case 'M':
				pMobIndex		= get_mob_index	( fread_number ( fp ) );
				pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
				if ( pMobIndex->spec_fun == 0 ) {
					log_hd ( LOG_ERROR, Format ( "Load_specials: 'M': vnum %d.", pMobIndex->vnum ) );
					exit ( 1 );
				}
				break;
		}

		fread_to_eol ( fp );
	}
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits ( void )
{
	extern const sh_int rev_dir [];
	RoomData *pRoomIndex;
	RoomData *to_room;
	Exit *pexit;
	Exit *pexit_rev;
	Reset *pReset;
	RoomData *iLastRoom, *iLastObj;
	int iHash;
	int door;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoomIndex  = room_index_hash[iHash];
				pRoomIndex != NULL;
				pRoomIndex  = pRoomIndex->next ) {
			bool fexit;

			iLastRoom = iLastObj = NULL;

			/* OLC : nuevo chequeo de resets */
			for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next ) {
				switch ( pReset->command ) {
					default:
						log_hd ( LOG_ERROR, Format ( "fix_exits : cuarto %d con reset cmd %c", pRoomIndex->vnum, pReset->command ) );
						exit ( 1 );
						break;

					case 'M':
						get_mob_index ( pReset->arg1 );
						iLastRoom = get_room_index ( pReset->arg3 );
						break;

					case 'O':
						get_obj_index ( pReset->arg1 );
						iLastObj = get_room_index ( pReset->arg3 );
						break;

					case 'P':
						get_obj_index ( pReset->arg1 );
						if ( iLastObj == NULL ) {
							log_hd ( LOG_ERROR, Format ( "fix_exits : reset en cuarto %d con iLastObj NULL", pRoomIndex->vnum ) );
							exit ( 1 );
						}
						break;

					case 'G':
					case 'E':
						get_obj_index ( pReset->arg1 );
						if ( iLastRoom == NULL ) {
							log_hd ( LOG_ERROR, Format ( "fix_exits : reset en cuarto %d con iLastRoom NULL", pRoomIndex->vnum ) );
							exit ( 1 );
						}
						iLastObj = iLastRoom;
						break;

					case 'D':
						log_hd ( LOG_ERROR, "Oldstyle-Exit-Reset(D) -- ???" );
						break;

					case 'R':
						get_room_index ( pReset->arg1 );
						if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ) {
							log_hd ( LOG_ERROR, Format ( "fix_exits : reset en cuarto %d con arg2 %d >= MAX_DIR",
														 pRoomIndex->vnum, pReset->arg2 ) );
							exit ( 1 );
						}
						break;
				} /* switch */
			} /* for */

			fexit = FALSE;
			for ( door = 0; door <= 5; door++ ) {
				if ( ( pexit = pRoomIndex->exit[door] ) != NULL ) {
					if ( pexit->u1.vnum <= 0
							|| get_room_index ( pexit->u1.vnum ) == NULL )
					{ pexit->u1.to_room = NULL; }
					else {
						fexit = TRUE;
						pexit->u1.to_room = get_room_index ( pexit->u1.vnum );
					}
				}
			}
			if ( !fexit )
			{ SET_BIT ( pRoomIndex->room_flags, ROOM_NO_MOB ); }
		}
	}

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pRoomIndex  = room_index_hash[iHash];
				pRoomIndex != NULL;
				pRoomIndex  = pRoomIndex->next ) {
			for ( door = 0; door <= 5; door++ ) {
				if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
						&&   ( to_room   = pexit->u1.to_room            ) != NULL
						&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
						&&   pexit_rev->u1.to_room != pRoomIndex
						&&   ( pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299 ) ) {
					log_hd ( LOG_ERROR, Format ( "Fix_exits: %d:%d -> %d:%d -> %d.",
												 pRoomIndex->vnum, door,
												 to_room->vnum,    rev_dir[door],
												 ( pexit_rev->u1.to_room == NULL )
												 ? 0 : pexit_rev->u1.to_room->vnum ) );
				}
			}
		}
	}

	return;
}

/*
 * Load mobprogs section
 */
void load_mobprogs ( FILE *fp )
{
	MPROG_CODE *pMprog;

	if ( area_last == NULL ) {
		log_hd ( LOG_ERROR, "Load_mobprogs: no #AREA seen yet." );
		exit ( 1 );
	}

	for ( ; ; ) {
		sh_int vnum;
		char letter;

		letter		  = fread_letter ( fp );
		if ( letter != '#' ) {
			log_hd ( LOG_ERROR, "Load_mobprogs: # not found." );
			exit ( 1 );
		}

		vnum		 = fread_number ( fp );
		if ( vnum == 0 )
		{ break; }

		fBootDb = FALSE;
		if ( get_mprog_index ( vnum ) != NULL ) {
			log_hd ( LOG_ERROR, Format ( "Load_mobprogs: vnum %d duplicated.", vnum ) );
			exit ( 1 );
		}
		fBootDb = TRUE;

		ALLOC_DATA ( pMprog, MPROG_CODE, 1 );

		pMprog->vnum  	= vnum;
		pMprog->code  	= fread_string ( fp );
		if ( mprog_list == NULL )
		{ mprog_list = pMprog; }
		else {
			pMprog->next = mprog_list;
			mprog_list 	= pMprog;
		}
		top_mprog_index++;
	}
	return;
}

/*
 *  Translate mobprog vnums pointers to real code
 */
void fix_mobprogs ( void )
{
	NPCData *pMobIndex;
	MPROG_LIST        *list;
	MPROG_CODE        *prog;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ) {
		for ( pMobIndex   = mob_index_hash[iHash];
				pMobIndex   != NULL;
				pMobIndex   = pMobIndex->next ) {
			for ( list = pMobIndex->mprogs; list != NULL; list = list->next ) {
				if ( ( prog = get_mprog_index ( list->vnum ) ) != NULL )
				{ list->code = prog->code; }
				else {
					log_hd ( LOG_ERROR, Format ( "Fix_mobprogs: code vnum %d not found.", list->vnum ) );
					exit ( 1 );
				}
			}
		}
	}
}


/*
 * Repopulate areas periodically.
 */
void area_update ( void )
{
	Zone *pArea;

	for ( pArea = area_first; pArea != NULL; pArea = pArea->next ) {
		if ( ++pArea->age < 3 )
		{ continue; }

		/*
		 * Check age and reset.
		 * Note: Mud School resets every 3 minutes (not 15).
		 */
		if ( ( !pArea->empty && ( pArea->nplayer == 0 || pArea->age >= 15 ) )
				||    pArea->age >= 31 ) {
			RoomData *pRoomIndex;

			reset_area ( pArea );
			wiznet ( Format ( "%s has just been reset.", pArea->name ), NULL, NULL, WIZ_RESETS, 0, 0 );

			pArea->age = number_range ( 0, 3 );


			pRoomIndex = get_room_index ( ROOM_VNUM_SCHOOL );
			if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
			{ pArea->age = 15 - 2; }
			else if ( pArea->nplayer == 0 )
			{ pArea->empty = TRUE; }

			log_hd ( LOG_DEBUG, Format ( "DISTRICT: Reset district %s, new district age: %d", pArea->name, pArea->age ) );

		}
	}

	return;
}

/* OLC
 * Reset one room.  Called by reset_area and olc.
 */
void reset_room ( RoomData *pRoom )
{
	Reset  *pReset;
	Creature   *pMob;
	Creature	*mob;
	Item    *pObj;
	Creature   *LastMob = NULL;
	Item    *LastObj = NULL;
	int iExit;
	int level = 0;
	bool last;

	if ( !pRoom )
	{ return; }

	pMob        = NULL;
	last        = FALSE;

	for ( iExit = 0;  iExit < MAX_DIR;  iExit++ ) {
		Exit *pExit;
		if ( ( pExit = pRoom->exit[iExit] )
				/*  && !IS_SET( pExit->exit_info, EX_BASHED )   ROM OLC */ ) {
			pExit->exit_info = pExit->rs_flags;
			if ( ( pExit->u1.to_room != NULL )
					&& ( ( pExit = pExit->u1.to_room->exit[rev_dir[iExit]] ) ) ) {
				/* nail the other side */
				pExit->exit_info = pExit->rs_flags;
			}
		}
	}

	for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next ) {
		NPCData  *pMobIndex;
		ItemData  *pObjIndex;
		ItemData  *pObjToIndex;
		RoomData *pRoomIndex;
		int count, limit = 0;

		switch ( pReset->command ) {
			default:
				log_hd ( LOG_ERROR, Format ( "Reset_room: bad command %c.", pReset->command ) );
				break;

			case 'M':
				if ( ! ( pMobIndex = get_mob_index ( pReset->arg1 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'M': bad vnum %d.", pReset->arg1 ) );
					continue;
				}

				if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) == NULL ) {
					log_hd ( LOG_ERROR, Format ( "Reset_area: 'R': bad vnum %d.", pReset->arg3 ) );
					continue;
				}
				if ( pMobIndex->count >= pReset->arg2 ) {
					last = FALSE;
					break;
				}
				/* */
				count = 0;
				for ( mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room )
					if ( mob->pIndexData == pMobIndex ) {
						count++;
						if ( count >= pReset->arg4 ) {
							last = FALSE;
							break;
						}
					}

				if ( count >= pReset->arg4 )
				{ break; }

				/* */

				pMob = create_mobile ( pMobIndex );

				/*
				 * Some more hard coding.
				 */
				if ( room_is_dark ( pRoom ) )
				{ SET_BIT ( pMob->affected_by, AFF_INFRARED ); }

				/*
				 * Pet shop mobiles get ACT_PET set.
				 */
				{
					RoomData *pRoomIndexPrev;

					pRoomIndexPrev = get_room_index ( pRoom->vnum - 1 );
					if ( pRoomIndexPrev
							&& IS_SET ( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
					{ SET_BIT ( pMob->act, ACT_PET ); }
				}

				char_to_room ( pMob, pRoom );

				LastMob = pMob;
				level  = URANGE ( 0, pMob->level - 2, LEVEL_HERO - 1 ); /* -1 ROM */
				last = TRUE;
				break;

			case 'O':
				if ( ! ( pObjIndex = get_obj_index ( pReset->arg1 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'O' 1 : bad vnum %d", pReset->arg1 ) );
					log_hd ( LOG_ERROR, Format ( "\t%d %d %d %d", pReset->arg1, pReset->arg2, pReset->arg3,
												 pReset->arg4 ) );
					continue;
				}

				if ( ! ( pRoomIndex = get_room_index ( pReset->arg3 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'O' 2 : bad vnum %d.", pReset->arg3 ) );
					log_hd ( LOG_ERROR, Format ( "\t%d %d %d %d", pReset->arg1, pReset->arg2, pReset->arg3,
												 pReset->arg4 ) );
					continue;
				}

				if ( pRoom->area->nplayer > 0
						|| count_obj_list ( pObjIndex, pRoom->contents ) > 0 ) {
					last = FALSE;
					break;
				}

				pObj = create_object ( pObjIndex,             /* UMIN - ROM OLC */
									   UMIN ( number_fuzzy ( level ), LEVEL_HERO - 1 ) );
				pObj->cost = 0;
				obj_to_room ( pObj, pRoom );
				last = TRUE;
				break;

			case 'P':
				if ( ! ( pObjIndex = get_obj_index ( pReset->arg1 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'P': bad vnum %d.", pReset->arg1 ) );
					continue;
				}

				if ( ! ( pObjToIndex = get_obj_index ( pReset->arg3 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'P': bad vnum %d.", pReset->arg3 ) );
					continue;
				}

				if ( pReset->arg2 > 50 ) /* old format */
				{ limit = 6; }
				else if ( pReset->arg2 == -1 ) /* no limit */
				{ limit = 999; }
				else
				{ limit = pReset->arg2; }

				if ( pRoom->area->nplayer > 0
						|| ( LastObj = get_obj_type ( pObjToIndex ) ) == NULL
						|| ( LastObj->in_room == NULL && !last )
						|| ( pObjIndex->count >= limit /* && number_range(0,4) != 0 */ )
						|| ( count = count_obj_list ( pObjIndex, LastObj->contains ) ) > pReset->arg4  ) {
					last = FALSE;
					break;
				}
				/* lastObj->level  -  ROM */

				while ( count < pReset->arg4 ) {
					pObj = create_object ( pObjIndex, number_fuzzy ( LastObj->level ) );
					obj_to_obj ( pObj, LastObj );
					count++;
					if ( pObjIndex->count >= limit )
					{ break; }
				}

				/* fix object lock state! */
				LastObj->value[1] = LastObj->pIndexData->value[1];
				last = TRUE;
				break;

			case 'G':
			case 'E':
				if ( ! ( pObjIndex = get_obj_index ( pReset->arg1 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'E' or 'G': bad vnum %d.", pReset->arg1 ) );
					continue;
				}

				if ( !last )
				{ break; }

				if ( !LastMob ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'E' or 'G': null mob for vnum %d.",
												 pReset->arg1 ) );
					last = FALSE;
					break;
				}

				if ( LastMob->pIndexData->pShop ) { /* Shop-keeper? */
					int olevel = 0, i, j;

					if ( !pObjIndex->new_format )
						switch ( pObjIndex->item_type ) {
							default:
								olevel = 0;
								break;
							case ITEM_PILL:
							case ITEM_POTION:
							case ITEM_SCROLL:
								olevel = 53;
								for ( i = 1; i < 5; i++ ) {
									if ( pObjIndex->value[i] > 0 ) {
										for ( j = 0; j < MAX_CLASS; j++ ) {
											olevel = UMIN ( olevel,
															skill_table[pObjIndex->value[i]].
															skill_level[j] );
										}
									}
								}

								olevel = UMAX ( 0, ( olevel * 3 / 4 ) - 2 );
								break;

							case ITEM_WAND:
								olevel = number_range ( 10, 20 );
								break;
							case ITEM_STAFF:
								olevel = number_range ( 15, 25 );
								break;
							case ITEM_ARMOR:
								olevel = number_range (  5, 15 );
								break;
							/* ROM patch weapon, treasure */
							case ITEM_WEAPON:
								olevel = number_range (  5, 15 );
								break;
							case ITEM_TREASURE:
								olevel = number_range ( 10, 20 );
								break;

#if 0 /* envy version */
							case ITEM_WEAPON:
								if ( pReset->command == 'G' )
								{ olevel = number_range ( 5, 15 ); }
								else
								{ olevel = number_fuzzy ( level ); }
#endif /* envy version */

								break;
						}

					pObj = create_object ( pObjIndex, olevel );
					SET_BIT ( pObj->extra_flags, ITEM_INVENTORY ); /* ROM OLC */

#if 0 /* envy version */
					if ( pReset->command == 'G' )
					{ SET_BIT ( pObj->extra_flags, ITEM_INVENTORY ); }
#endif /* envy version */

				} else { /* ROM OLC else version */
					int limit;
					if ( pReset->arg2 > 50 ) /* old format */
					{ limit = 6; }
					else if ( pReset->arg2 == -1 || pReset->arg2 == 0 )  /* no limit */
					{ limit = 999; }
					else
					{ limit = pReset->arg2; }

					if ( pObjIndex->count < limit || number_range ( 0, 4 ) == 0 ) {
						pObj = create_object ( pObjIndex,
											   UMIN ( number_fuzzy ( level ), LEVEL_HERO - 1 ) );
						/* error message if it is too high */
						if ( pObj->level > LastMob->level + 3
								||  ( pObj->item_type == ITEM_WEAPON
									  &&   pReset->command == 'E'
									  &&   pObj->level < LastMob->level - 5 && pObj->level < 45 ) )
							fprintf ( stderr,
									  "Err: obj %s (%d) -- %d, mob %s (%d) -- %d\n",
									  pObj->short_descr, pObj->pIndexData->vnum, pObj->level,
									  LastMob->short_descr, LastMob->pIndexData->vnum, LastMob->level );
					} else
					{ break; }
				}

#if 0 /* envy else version */
				else {
					pObj = create_object ( pObjIndex, number_fuzzy ( level ) );
				}
#endif /* envy else version */

				obj_to_char ( pObj, LastMob );
				if ( pReset->command == 'E' )
				{ equip_char ( LastMob, pObj, pReset->arg3 ); }
				last = TRUE;
				break;

			case 'D':
				break;

			case 'R':
				if ( ! ( pRoomIndex = get_room_index ( pReset->arg1 ) ) ) {
					log_hd ( LOG_ERROR, Format ( "Reset_room: 'R': bad vnum %d.", pReset->arg1 ) );
					continue;
				}

				{
					Exit *pExit;
					int d0;
					int d1;

					for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ ) {
						d1                   = number_range ( d0, pReset->arg2 - 1 );
						pExit                = pRoomIndex->exit[d0];
						pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
						pRoomIndex->exit[d1] = pExit;
					}
				}
				break;
		}
	}

	return;
}

/* OLC
 * Reset one area.
 */
void reset_area ( Zone *pArea )
{
	RoomData *pRoom;
	int  vnum;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ) {
		if ( ( pRoom = get_room_index ( vnum ) ) )
		{ reset_room ( pRoom ); }
	}

	return;
}

/*
 * Create an instance of a mobile.
 */
Creature *create_mobile ( NPCData *pMobIndex )
{
	Creature *mob;
	int i;
	Affect af;

	mobile_count++;

	if ( pMobIndex == NULL ) {
		log_hd ( LOG_ERROR, "Create_mobile: NULL pMobIndex." );
		exit ( 1 );
	}

	mob = new_char();

	mob->pIndexData	= pMobIndex;

	mob->name		= assign_string ( pMobIndex->player_name );   /* OLC */
	mob->short_descr	= assign_string ( pMobIndex->short_descr );   /* OLC */
	mob->long_descr	= assign_string ( pMobIndex->long_descr );    /* OLC */
	mob->description	= assign_string ( pMobIndex->description );   /* OLC */
	mob->id		= get_mob_id();
	mob->spec_fun	= pMobIndex->spec_fun;
	mob->prompt		= NULL;
	mob->mprog_target   = NULL;

	if ( pMobIndex->wealth == 0 ) {
		mob->silver = 0;
		mob->gold   = 0;
	} else {
		long wealth;

		wealth = number_range ( pMobIndex->wealth / 2, 3 * pMobIndex->wealth / 2 );
		mob->gold = number_range ( wealth / 200, wealth / 100 );
		mob->silver = wealth - ( mob->gold * 100 );
	}

	if ( pMobIndex->new_format )
		/* load in new style */
	{
		/* read from prototype */
		mob->group		= pMobIndex->group;
		mob->act 		= pMobIndex->act;
		mob->comm		= COMM_NOCHANNELS | COMM_NOSHOUT | COMM_NOTELL;
		mob->affected_by	= pMobIndex->affected_by;
		mob->alignment		= pMobIndex->alignment;
		mob->level		= pMobIndex->level;
		mob->hitroll		= pMobIndex->hitroll;
		mob->damroll		= pMobIndex->damage[DICE_BONUS];
		mob->max_hit		= dice ( pMobIndex->hit[DICE_NUMBER],
									 pMobIndex->hit[DICE_TYPE] )
							  + pMobIndex->hit[DICE_BONUS];
		mob->hit		= mob->max_hit;
		mob->max_mana		= dice ( pMobIndex->mana[DICE_NUMBER],
									 pMobIndex->mana[DICE_TYPE] )
							  + pMobIndex->mana[DICE_BONUS];
		mob->mana		= mob->max_mana;
		mob->damage[DICE_NUMBER] = pMobIndex->damage[DICE_NUMBER];
		mob->damage[DICE_TYPE]	= pMobIndex->damage[DICE_TYPE];
		mob->dam_type		= pMobIndex->dam_type;
		if ( mob->dam_type == 0 )
			switch ( number_range ( 1, 3 ) ) {
				case ( 1 ) :
					mob->dam_type = 3;
					break;  /* slash */
				case ( 2 ) :
					mob->dam_type = 7;
					break;  /* pound */
				case ( 3 ) :
					mob->dam_type = 11;
					break;  /* pierce */
			}
		for ( i = 0; i < 4; i++ )
		{ mob->armor[i]	= pMobIndex->ac[i]; }
		mob->off_flags		= pMobIndex->off_flags;
		mob->imm_flags		= pMobIndex->imm_flags;
		mob->res_flags		= pMobIndex->res_flags;
		mob->vuln_flags		= pMobIndex->vuln_flags;
		mob->start_pos		= pMobIndex->start_pos;
		mob->default_pos	= pMobIndex->default_pos;
		mob->sex		= pMobIndex->sex;
		if ( mob->sex == 3 ) /* random sex */
		{ mob->sex = number_range ( 1, 2 ); }
		mob->race		= pMobIndex->race;
		mob->form		= pMobIndex->form;
		mob->parts		= pMobIndex->parts;
		mob->size		= pMobIndex->size;
		mob->material		= assign_string ( pMobIndex->material );

		/* computed on the spot */

		for ( i = 0; i < MAX_STATS; i ++ )
		{ mob->perm_stat[i] = UMIN ( 25, 11 + mob->level / 4 ); }

		if ( IS_SET ( mob->act, ACT_WARRIOR ) ) {
			mob->perm_stat[STAT_STR] += 3;
			mob->perm_stat[STAT_INT] -= 1;
			mob->perm_stat[STAT_CON] += 2;
		}

		if ( IS_SET ( mob->act, ACT_THIEF ) ) {
			mob->perm_stat[STAT_DEX] += 3;
			mob->perm_stat[STAT_INT] += 1;
			mob->perm_stat[STAT_WIS] -= 1;
		}

		if ( IS_SET ( mob->act, ACT_CLERIC ) ) {
			mob->perm_stat[STAT_WIS] += 3;
			mob->perm_stat[STAT_DEX] -= 1;
			mob->perm_stat[STAT_STR] += 1;
		}

		if ( IS_SET ( mob->act, ACT_MAGE ) ) {
			mob->perm_stat[STAT_INT] += 3;
			mob->perm_stat[STAT_STR] -= 1;
			mob->perm_stat[STAT_DEX] += 1;
		}

		if ( IS_SET ( mob->off_flags, OFF_FAST ) )
		{ mob->perm_stat[STAT_DEX] += 2; }

		mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
		mob->perm_stat[STAT_CON] += ( mob->size - SIZE_MEDIUM ) / 2;

		/* let's get some spell action */
		if ( IS_AFFECTED ( mob, AFF_SANCTUARY ) ) {
			af.where	 = TO_AFFECTS;
			af.type      = skill_lookup ( "sanctuary" );
			af.level     = mob->level;
			af.duration  = -1;
			af.location  = APPLY_NONE;
			af.modifier  = 0;
			af.bitvector = AFF_SANCTUARY;
			affect_to_char ( mob, &af );
		}

		if ( IS_AFFECTED ( mob, AFF_HASTE ) ) {
			af.where	 = TO_AFFECTS;
			af.type      = skill_lookup ( "haste" );
			af.level     = mob->level;
			af.duration  = -1;
			af.location  = APPLY_DEX;
			af.modifier  = 1 + ( mob->level >= 18 ) + ( mob->level >= 25 ) +
						   ( mob->level >= 32 );
			af.bitvector = AFF_HASTE;
			affect_to_char ( mob, &af );
		}

		if ( IS_AFFECTED ( mob, AFF_PROTECT_EVIL ) ) {
			af.where	 = TO_AFFECTS;
			af.type	 = skill_lookup ( "protection evil" );
			af.level	 = mob->level;
			af.duration	 = -1;
			af.location	 = APPLY_SAVES;
			af.modifier	 = -1;
			af.bitvector = AFF_PROTECT_EVIL;
			affect_to_char ( mob, &af );
		}

		if ( IS_AFFECTED ( mob, AFF_PROTECT_GOOD ) ) {
			af.where	 = TO_AFFECTS;
			af.type      = skill_lookup ( "protection good" );
			af.level     = mob->level;
			af.duration  = -1;
			af.location  = APPLY_SAVES;
			af.modifier  = -1;
			af.bitvector = AFF_PROTECT_GOOD;
			affect_to_char ( mob, &af );
		}
	} else { /* read in old format and convert */
		mob->act		= pMobIndex->act;
		mob->affected_by	= pMobIndex->affected_by;
		mob->alignment		= pMobIndex->alignment;
		mob->level		= pMobIndex->level;
		mob->hitroll		= pMobIndex->hitroll;
		mob->damroll		= 0;
		mob->max_hit		= mob->level * 8 + number_range (
								  mob->level * mob->level / 4,
								  mob->level * mob->level );
		mob->max_hit *= .9;
		mob->hit		= mob->max_hit;
		mob->max_mana		= 100 + dice ( mob->level, 10 );
		mob->mana		= mob->max_mana;
		switch ( number_range ( 1, 3 ) ) {
			case ( 1 ) :
				mob->dam_type = 3;
				break;  /* slash */
			case ( 2 ) :
				mob->dam_type = 7;
				break;  /* pound */
			case ( 3 ) :
				mob->dam_type = 11;
				break;  /* pierce */
		}
		for ( i = 0; i < 3; i++ )
		{ mob->armor[i]	= interpolate ( mob->level, 100, -100 ); }
		mob->armor[3]		= interpolate ( mob->level, 100, 0 );
		mob->race		= pMobIndex->race;
		mob->off_flags		= pMobIndex->off_flags;
		mob->imm_flags		= pMobIndex->imm_flags;
		mob->res_flags		= pMobIndex->res_flags;
		mob->vuln_flags		= pMobIndex->vuln_flags;
		mob->start_pos		= pMobIndex->start_pos;
		mob->default_pos	= pMobIndex->default_pos;
		mob->sex		= pMobIndex->sex;
		mob->form		= pMobIndex->form;
		mob->parts		= pMobIndex->parts;
		mob->size		= SIZE_MEDIUM;
		mob->material		= ( char * ) "";

		for ( i = 0; i < MAX_STATS; i ++ )
		{ mob->perm_stat[i] = 11 + mob->level / 4; }
	}

	mob->position = mob->start_pos;


	/* link the mob to the world list */
	mob->next		= char_list;
	char_list		= mob;
	pMobIndex->count++;
	return mob;
}

/* duplicate a mobile exactly -- except inventory */
void clone_mobile ( Creature *parent, Creature *clone )
{
	int i;
	Affect *paf;

	if ( parent == NULL || clone == NULL || !IS_NPC ( parent ) )
	{ return; }

	/* start fixing values */
	clone->name 	= assign_string ( parent->name );
	clone->version	= parent->version;
	clone->short_descr	= assign_string ( parent->short_descr );
	clone->long_descr	= assign_string ( parent->long_descr );
	clone->description	= assign_string ( parent->description );
	clone->group	= parent->group;
	clone->sex		= parent->sex;
	clone->archetype	= parent->archetype;
	clone->race		= parent->race;
	clone->level	= parent->level;
	clone->trust	= 0;
	clone->timer	= parent->timer;
	clone->wait		= parent->wait;
	clone->hit		= parent->hit;
	clone->max_hit	= parent->max_hit;
	clone->mana		= parent->mana;
	clone->max_mana	= parent->max_mana;
	clone->move		= parent->move;
	clone->max_move	= parent->max_move;
	clone->gold		= parent->gold;
	clone->silver	= parent->silver;
	clone->exp		= parent->exp;
	clone->act		= parent->act;
	clone->comm		= parent->comm;
	clone->imm_flags	= parent->imm_flags;
	clone->res_flags	= parent->res_flags;
	clone->vuln_flags	= parent->vuln_flags;
	clone->invis_level	= parent->invis_level;
	clone->affected_by	= parent->affected_by;
	clone->position	= parent->position;
	clone->practice	= parent->practice;
	clone->train	= parent->train;
	clone->saving_throw	= parent->saving_throw;
	clone->alignment	= parent->alignment;
	clone->hitroll	= parent->hitroll;
	clone->damroll	= parent->damroll;
	clone->wimpy	= parent->wimpy;
	clone->form		= parent->form;
	clone->parts	= parent->parts;
	clone->size		= parent->size;
	clone->material	= assign_string ( parent->material );
	clone->off_flags	= parent->off_flags;
	clone->dam_type	= parent->dam_type;
	clone->start_pos	= parent->start_pos;
	clone->default_pos	= parent->default_pos;
	clone->spec_fun	= parent->spec_fun;

	for ( i = 0; i < 4; i++ )
	{ clone->armor[i]	= parent->armor[i]; }

	for ( i = 0; i < MAX_STATS; i++ ) {
		clone->perm_stat[i]	= parent->perm_stat[i];
		clone->mod_stat[i]	= parent->mod_stat[i];
	}

	for ( i = 0; i < 3; i++ )
	{ clone->damage[i]	= parent->damage[i]; }

	/* now add the affects */
	for ( paf = parent->affected; paf != NULL; paf = paf->next )
	{ affect_to_char ( clone, paf ); }

}




/*
 * Create an instance of an object.
 */
Item *create_object ( ItemData *pObjIndex, int level )
{
	Affect *paf;
	Item *obj;
	int i;

	if ( pObjIndex == NULL ) {
		log_hd ( LOG_ERROR, "Create_object: NULL pObjIndex." );
		exit ( 1 );
	}

	obj = new_obj();

	obj->pIndexData	= pObjIndex;
	obj->in_room	= NULL;
	obj->enchanted	= FALSE;

	if ( pObjIndex->new_format )
	{ obj->level = pObjIndex->level; }
	else
	{ obj->level		= UMAX ( 0, level ); }
	obj->wear_loc	= -1;

	obj->name		= assign_string ( pObjIndex->name );          /* OLC */
	obj->short_descr	= assign_string ( pObjIndex->short_descr );   /* OLC */
	obj->description	= assign_string ( pObjIndex->description );   /* OLC */
	obj->material	= assign_string ( pObjIndex->material );
	obj->item_type	= pObjIndex->item_type;
	obj->extra_flags	= pObjIndex->extra_flags;
	obj->wear_flags	= pObjIndex->wear_flags;
	obj->value[0]	= pObjIndex->value[0];
	obj->value[1]	= pObjIndex->value[1];
	obj->value[2]	= pObjIndex->value[2];
	obj->value[3]	= pObjIndex->value[3];
	obj->value[4]	= pObjIndex->value[4];
	obj->weight		= pObjIndex->weight;

	if ( level == -1 || pObjIndex->new_format )
	{ obj->cost	= pObjIndex->cost; }
	else
		obj->cost	= number_fuzzy ( 10 )
					  * number_fuzzy ( level ) * number_fuzzy ( level );

	/*
	 * Mess with object properties.
	 */
	switch ( obj->item_type ) {
		default:
			log_hd ( LOG_ERROR, Format ( "Read_object: vnum %d bad type.", pObjIndex->vnum ) );
			break;

		case ITEM_LIGHT:
			if ( obj->value[2] == 999 )
			{ obj->value[2] = -1; }
			break;

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
		case ITEM_PORTAL:
			if ( !pObjIndex->new_format )
			{ obj->cost /= 5; }
			break;

		case ITEM_TREASURE:
		case ITEM_WARP_STONE:
		case ITEM_ROOM_KEY:
		case ITEM_GEM:
		case ITEM_JEWELRY:
			break;

		case ITEM_JUKEBOX:
			for ( i = 0; i < 5; i++ )
			{ obj->value[i] = -1; }
			break;

		case ITEM_SCROLL:
			if ( level != -1 && !pObjIndex->new_format )
			{ obj->value[0]	= number_fuzzy ( obj->value[0] ); }
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			if ( level != -1 && !pObjIndex->new_format ) {
				obj->value[0]	= number_fuzzy ( obj->value[0] );
				obj->value[1]	= number_fuzzy ( obj->value[1] );
				obj->value[2]	= obj->value[1];
			}
			if ( !pObjIndex->new_format )
			{ obj->cost *= 2; }
			break;

		case ITEM_WEAPON:
			if ( level != -1 && !pObjIndex->new_format ) {
				obj->value[1] = number_fuzzy ( number_fuzzy ( 1 * level / 4 + 2 ) );
				obj->value[2] = number_fuzzy ( number_fuzzy ( 3 * level / 4 + 6 ) );
			}
			break;

		case ITEM_ARMOR:
			if ( level != -1 && !pObjIndex->new_format ) {
				obj->value[0]	= number_fuzzy ( level / 5 + 3 );
				obj->value[1]	= number_fuzzy ( level / 5 + 3 );
				obj->value[2]	= number_fuzzy ( level / 5 + 3 );
			}
			break;

		case ITEM_POTION:
		case ITEM_PILL:
			if ( level != -1 && !pObjIndex->new_format )
			{ obj->value[0] = number_fuzzy ( number_fuzzy ( obj->value[0] ) ); }
			break;

		case ITEM_MONEY:
			if ( !pObjIndex->new_format )
			{ obj->value[0]	= obj->cost; }
			break;
	}

	for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
		if ( paf->location == APPLY_SPELL_AFFECT )
		{ affect_to_obj ( obj, paf ); }

	obj->next		= object_list;
	object_list		= obj;
	pObjIndex->count++;

	return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object ( Item *parent, Item *clone )
{
	int i;
	Affect *paf;
	DescriptionData *ed, *ed_new;

	if ( parent == NULL || clone == NULL )
	{ return; }

	/* start fixing the object */
	clone->name 	= assign_string ( parent->name );
	clone->short_descr 	= assign_string ( parent->short_descr );
	clone->description	= assign_string ( parent->description );
	clone->item_type	= parent->item_type;
	clone->extra_flags	= parent->extra_flags;
	clone->wear_flags	= parent->wear_flags;
	clone->weight	= parent->weight;
	clone->cost		= parent->cost;
	clone->level	= parent->level;
	clone->condition	= parent->condition;
	clone->material	= assign_string ( parent->material );
	clone->timer	= parent->timer;

	for ( i = 0;  i < 5; i ++ )
	{ clone->value[i]	= parent->value[i]; }

	/* affects */
	clone->enchanted	= parent->enchanted;

	for ( paf = parent->affected; paf != NULL; paf = paf->next )
	{ affect_to_obj ( clone, paf ); }

	/* extended desc */
	for ( ed = parent->extra_descr; ed != NULL; ed = ed->next ) {
		ed_new                  = new_extra_descr();
		ed_new->keyword    	= assign_string ( ed->keyword );
		ed_new->description     = assign_string ( ed->description );
		ed_new->next           	= clone->extra_descr;
		clone->extra_descr  	= ed_new;
	}

}



/*
 * Clear a new character.
 */
void clear_char ( Creature *ch )
{
	static Creature ch_zero;
	int i;

	*ch				= ch_zero;
	ch->name			= &str_empty[0];
	ch->short_descr		= &str_empty[0];
	ch->long_descr		= &str_empty[0];
	ch->description		= &str_empty[0];
	ch->prompt                  = &str_empty[0];
	ch->logon			= current_time;
	ch->lines			= PAGELEN;
	for ( i = 0; i < 4; i++ )
	{ ch->armor[i]		= 100; }
	ch->position		= POS_STANDING;
	ch->hit			= 20;
	ch->max_hit			= 20;
	ch->mana			= 100;
	ch->max_mana		= 100;
	ch->move			= 100;
	ch->max_move		= 100;
	ch->on			= NULL;
	for ( i = 0; i < MAX_STATS; i ++ ) {
		ch->perm_stat[i] = 13;
		ch->mod_stat[i] = 0;
	}
	return;
}

/*
 * Get an extra description from a list.
 */
char *get_extra_descr ( const char *name, DescriptionData *ed )
{
	for ( ; ed != NULL; ed = ed->next ) {
		if ( is_name ( ( char * ) name, ed->keyword ) )
		{ return ed->description; }
	}
	return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
NPCData *get_mob_index ( int vnum )
{
	NPCData *pMobIndex;

	for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
			pMobIndex != NULL;
			pMobIndex  = pMobIndex->next ) {
		if ( pMobIndex->vnum == vnum )
		{ return pMobIndex; }
	}

	if ( fBootDb ) {
		log_hd ( LOG_ERROR, Format ( "Get_mob_index: bad vnum %d.", vnum ) );
		exit ( 1 );
	}

	return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
ItemData *get_obj_index ( int vnum )
{
	ItemData *pObjIndex;

	for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
			pObjIndex != NULL;
			pObjIndex  = pObjIndex->next ) {
		if ( pObjIndex->vnum == vnum )
		{ return pObjIndex; }
	}

	if ( fBootDb ) {
		log_hd ( LOG_ERROR, Format ( "Get_obj_index: bad vnum %d.", vnum ) );
		exit ( 1 );
	}

	return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
RoomData *get_room_index ( int vnum )
{
	RoomData *pRoomIndex;

	for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
			pRoomIndex != NULL;
			pRoomIndex  = pRoomIndex->next ) {
		if ( pRoomIndex->vnum == vnum )
		{ return pRoomIndex; }
	}

	if ( fBootDb ) {
		log_hd ( LOG_ERROR, Format ( "Get_room_index: bad vnum %d.", vnum ) );
		exit ( 1 );
	}

	return NULL;
}

MPROG_CODE *get_mprog_index ( int vnum )
{
	MPROG_CODE *prg;
	for ( prg = mprog_list; prg; prg = prg->next ) {
		if ( prg->vnum == vnum )
		{ return ( prg ); }
	}
	return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter ( FILE *fp )
{
	char c;

	do {
		c = getc ( fp );
	} while ( isspace ( c ) );

	return c;
}



/*
 * Read a number from a file.
 */
int fread_number ( FILE *fp )
{
	int number;
	bool sign;
	char c;

	do {
		c = getc ( fp );
	} while ( isspace ( c ) );

	number = 0;

	sign   = FALSE;
	if ( c == '+' ) {
		c = getc ( fp );
	} else if ( c == '-' ) {
		sign = TRUE;
		c = getc ( fp );
	}

	if ( !isdigit ( c ) ) {
		log_hd ( LOG_ERROR, Format ( "Fread_number: bad format (%c).", c ) );
		exit ( 1 );
	}

	while ( isdigit ( c ) ) {
		number = number * 10 + c - '0';
		c      = getc ( fp );
	}

	if ( sign )
	{ number = 0 - number; }

	if ( c == '|' )
	{ number += fread_number ( fp ); }
	else if ( c != ' ' )
	{ ungetc ( c, fp ); }

	return number;
}

long fread_flag ( FILE *fp )
{
	int number;
	char c;
	bool negative = FALSE;

	do {
		c = getc ( fp );
	} while ( isspace ( c ) );

	if ( c == '-' ) {
		negative = TRUE;
		c = getc ( fp );
	}

	number = 0;

	if ( !isdigit ( c ) ) {
		while ( ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' ) ) {
			number += flag_convert ( c );
			c = getc ( fp );
		}
	}

	while ( isdigit ( c ) ) {
		number = number * 10 + c - '0';
		c = getc ( fp );
	}

	if ( c == '|' )
	{ number += fread_flag ( fp ); }

	else if  ( c != ' ' )
	{ ungetc ( c, fp ); }

	if ( negative )
	{ return -1 * number; }

	return number;
}

long flag_convert ( char letter )
{
	long bitsum = 0;
	char i;

	if ( 'A' <= letter && letter <= 'Z' ) {
		bitsum = 1;
		for ( i = letter; i > 'A'; i-- )
		{ bitsum *= 2; }
	} else if ( 'a' <= letter && letter <= 'z' ) {
		bitsum = 67108864; /* 2^26 */
		for ( i = letter; i > 'a'; i -- )
		{ bitsum *= 2; }
	}

	return bitsum;
}




/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 *   this function takes 40% to 50% of boot-up time.
 */
char *fread_string ( FILE *fp )
{
	char *plast;
	char c;

	plast = top_string + sizeof ( char * );
	if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] ) {
		log_hd ( LOG_ERROR, Format ( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING ) );
		exit ( 1 );
	}

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do {
		c = getc ( fp );
	} while ( isspace ( c ) );

	if ( ( *plast++ = c ) == '~' )
	{ return &str_empty[0]; }

	for ( ;; ) {
		/*
		 * Back off the char type lookup,
		 *   it was too dirty for portability.
		 *   -- Furey
		 */

		switch ( *plast = getc ( fp ) ) {
			default:
				plast++;
				break;

			case EOF:
				/* temp fix */
				log_hd ( LOG_ERROR, "Fread_string: EOF" );
				return NULL;
				/* exit( 1 ); */
				break;

			case '\n':
				plast++;
				*plast++ = '\r';
				break;

			case '\r':
				break;

			case '~':
				plast++;
				{
					union {
						char *	pc;
						char	rgc[sizeof ( char * )];
					} u1;
					int ic;
					int iHash;
					char *pHash;
					char *pHashPrev;
					char *pString;

					plast[-1] = '\0';
					iHash     = UMIN ( MAX_KEY_HASH - 1, plast - 1 - top_string );
					for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev ) {
						for ( ic = 0; ic < ( signed int ) sizeof ( char * ); ic++ )
						{ u1.rgc[ic] = pHash[ic]; }
						pHashPrev = u1.pc;
						pHash    += sizeof ( char * );

						if ( top_string[sizeof ( char * )] == pHash[0]
								&&   !strcmp ( top_string + sizeof ( char * ) + 1, pHash + 1 ) )
						{ return pHash; }
					}

					if ( fBootDb ) {
						pString		= top_string;
						top_string		= plast;
						u1.pc		= string_hash[iHash];
						for ( ic = 0; ic < ( signed int ) sizeof ( char * ); ic++ )
						{ pString[ic] = u1.rgc[ic]; }
						string_hash[iHash]	= pString;

						nAllocString += 1;
						sAllocString += top_string - pString;
						return pString + sizeof ( char * );
					} else {
						return assign_string ( top_string + sizeof ( char * ) );
					}
				}
		}
	}
}

char *fread_string_eol ( FILE *fp )
{
	static bool char_special[256 - EOF];
	char *plast;
	char c;

	if ( char_special[EOF - EOF] != TRUE ) {
		char_special[EOF -  EOF] = TRUE;
		char_special['\n' - EOF] = TRUE;
		char_special['\r' - EOF] = TRUE;
	}

	plast = top_string + sizeof ( char * );
	if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] ) {
		log_hd ( LOG_ERROR, Format ( "Fread_string_eol: MAX_STRING %d exceeded.", MAX_STRING ) );
		exit ( 1 );
	}

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do {
		c = getc ( fp );
	} while ( isspace ( c ) );

	if ( ( *plast++ = c ) == '\n' )
	{ return &str_empty[0]; }

	for ( ;; ) {
		if ( !char_special[ ( *plast++ = getc ( fp ) ) - EOF ] )
		{ continue; }

		switch ( plast[-1] ) {
			default:
				break;

			case EOF:
				log_hd ( LOG_ERROR, Format ( "Fread_string_eol  EOF" ) );
				exit ( 1 );
				break;

			case '\n':
			case '\r': {
					union {
						char *      pc;
						char        rgc[sizeof ( char * )];
					} u1;
					int ic;
					int iHash;
					char *pHash;
					char *pHashPrev;
					char *pString;

					plast[-1] = '\0';
					iHash     = UMIN ( MAX_KEY_HASH - 1, plast - 1 - top_string );
					for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev ) {
						for ( ic = 0; ic < ( signed int ) sizeof ( char * ); ic++ )
						{ u1.rgc[ic] = pHash[ic]; }
						pHashPrev = u1.pc;
						pHash    += sizeof ( char * );

						if ( top_string[sizeof ( char * )] == pHash[0]
								&&   !strcmp ( top_string + sizeof ( char * ) + 1, pHash + 1 ) )
						{ return pHash; }
					}

					if ( fBootDb ) {
						pString             = top_string;
						top_string          = plast;
						u1.pc               = string_hash[iHash];
						for ( ic = 0; ic < ( signed int ) sizeof ( char * ); ic++ )
						{ pString[ic] = u1.rgc[ic]; }
						string_hash[iHash]  = pString;

						nAllocString += 1;
						sAllocString += top_string - pString;
						return pString + sizeof ( char * );
					} else {
						return assign_string ( top_string + sizeof ( char * ) );
					}
				}
		}
	}
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol ( FILE *fp )
{
	char c;

	do {
		c = getc ( fp );
	} while ( c != '\n' && c != '\r' );

	do {
		c = getc ( fp );
	} while ( c == '\n' || c == '\r' );

	ungetc ( c, fp );
	return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word ( FILE *fp )
{
	static char word[MAX_INPUT_LENGTH];
	char *pword;
	char cEnd;

	do {
		cEnd = getc ( fp );
	} while ( isspace ( cEnd ) );

	if ( cEnd == '\'' || cEnd == '"' ) {
		pword   = word;
	} else {
		word[0] = cEnd;
		pword   = word + 1;
		cEnd    = ' ';
	}

	for ( ; pword < word + MAX_INPUT_LENGTH; pword++ ) {
		*pword = getc ( fp );
		if ( cEnd == ' ' ? isspace ( *pword ) : *pword == cEnd ) {
			if ( cEnd == ' ' )
			{ ungetc ( *pword, fp ); }
			*pword = '\0';
			return word;
		}
	}

	log_hd ( LOG_ERROR, Format ( "Fread_word: word too long (%s)", !IS_NULLSTR ( word ) ? word : "{NULL}" ) );
	exit ( 1 );
	return NULL;
}


/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *assign_string ( const char *str )
{
	char *str_new;

	if ( str[0] == '\0' )
	{ return &str_empty[0]; }

	if ( str >= string_space && str < top_string )
	{ return ( char * ) str; }

	ALLOC_DATA ( str_new, char, strlen ( str ) + 1 );

	strcpy ( str_new, str );
	return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void freeSharedString ( const char *pstr )
{
	if ( pstr == NULL
			||   pstr == &str_empty[0]
			|| ( pstr >= string_space && pstr < top_string ) )
	{ return; }

	ACTUAL_PURGE ( pstr );
	return;
}



DefineCommand ( cmd_areas )
{
	Zone *pArea1;
	Zone *pArea2;
	int iArea;
	int iAreaHalf;

	if ( argument[0] != '\0' ) {
		writeBuffer ( "No argument is used with this command.\n\r", ch );
		return;
	}

	iAreaHalf = ( top_area + 1 ) / 2;
	pArea1    = area_first;
	pArea2    = area_first;
	for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	{ pArea2 = pArea2->next; }

	for ( iArea = 0; iArea < iAreaHalf; iArea++ ) {
		writeBuffer ( Format ( "%-39s%-39s\n\r", pArea1->name, ( pArea2 != NULL ) ? pArea2->name : "" ), ch );
		pArea1 = pArea1->next;
		if ( pArea2 != NULL )
		{ pArea2 = pArea2->next; }
	}

	return;
}

DefineCommand ( cmd_memory )
{
	char buf[MAX_STRING_LENGTH];

	sprintf ( buf, "Affects %5d\n\r", top_affect    );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Areas   %5d\n\r", top_area      );
	writeBuffer ( buf, ch );
	sprintf ( buf, "ExDes   %5d\n\r", top_ed        );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Exits   %5d\n\r", top_exit      );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Helps   %5d\n\r", top_help      );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Socials %5d\n\r", social_count  );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Mobs    %5d(%d new format)\n\r", top_mob_index, newmobs );
	writeBuffer ( buf, ch );
	sprintf ( buf, "(in use)%5d\n\r", mobile_count  );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Objs    %5d(%d new format)\n\r", top_obj_index, newobjs );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Resets  %5d\n\r", top_reset     );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Rooms   %5d\n\r", top_room      );
	writeBuffer ( buf, ch );
	sprintf ( buf, "Shops   %5d\n\r", top_shop      );
	writeBuffer ( buf, ch );

	sprintf ( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
			  nAllocString, sAllocString, MAX_STRING );
	writeBuffer ( buf, ch );
	return;
}

DefineCommand ( cmd_dump )
{
	int count, count2, num_pcs, aff_count;
	Creature *fch;
	NPCData *pMobIndex;
	PlayerData *pc;
	Item *obj;
	ItemData *pObjIndex;
	RoomData *room;
	Exit *exit;
	Socket *d;
	Affect *af;
	FILE *fp;
	int vnum, nMatch = 0;

	/* open file */
	fclose ( fpReserve );
	fp = fopen ( "mem.dmp", "w" );

	/* report use of data structures */

	num_pcs = 0;
	aff_count = 0;

	/* mobile prototypes */
	fprintf ( fp, "MobProt	%4d (%8ld bytes)\n",
			  top_mob_index, top_mob_index * ( sizeof ( *pMobIndex ) ) );

	/* mobs */
	count = 0;
	count2 = 0;
	for ( fch = char_list; fch != NULL; fch = fch->next ) {
		count++;
		if ( fch->pcdata != NULL )
		{ num_pcs++; }
		for ( af = fch->affected; af != NULL; af = af->next )
		{ aff_count++; }
	}
	for ( fch = char_free; fch != NULL; fch = fch->next )
	{ count2++; }

	fprintf ( fp, "Mobs	%4d (%8ld bytes), %2d free (%ld bytes)\n",
			  count, count * ( sizeof ( *fch ) ), count2, count2 * ( sizeof ( *fch ) ) );

	/* pcdata */
	count = 0;
	for ( pc = pcdata_free; pc != NULL; pc = pc->next )
	{ count++; }

	fprintf ( fp, "Pcdata	%4d (%8ld bytes), %2d free (%ld bytes)\n",
			  num_pcs, num_pcs * ( sizeof ( *pc ) ), count, count * ( sizeof ( *pc ) ) );

	/* descriptors */
	count = 0;
	count2 = 0;
	for ( d = socket_list; d != NULL; d = d->next )
	{ count++; }
	for ( d = descriptor_free; d != NULL; d = d->next )
	{ count2++; }

	fprintf ( fp, "Descs	%4d (%8ld bytes), %2d free (%ld bytes)\n",
			  count, count * ( sizeof ( *d ) ), count2, count2 * ( sizeof ( *d ) ) );

	/* object prototypes */
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
		if ( ( pObjIndex = get_obj_index ( vnum ) ) != NULL ) {
			for ( af = pObjIndex->affected; af != NULL; af = af->next )
			{ aff_count++; }
			nMatch++;
		}

	fprintf ( fp, "ObjProt	%4d (%8ld bytes)\n",
			  top_obj_index, top_obj_index * ( sizeof ( *pObjIndex ) ) );


	/* objects */
	count = 0;
	count2 = 0;
	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		count++;
		for ( af = obj->affected; af != NULL; af = af->next )
		{ aff_count++; }
	}
	for ( obj = obj_free; obj != NULL; obj = obj->next )
	{ count2++; }

	fprintf ( fp, "Objs	%4d (%8ld bytes), %2d free (%ld bytes)\n",
			  count, count * ( sizeof ( *obj ) ), count2, count2 * ( sizeof ( *obj ) ) );

	/* affects */
	count = 0;
	for ( af = affect_free; af != NULL; af = af->next )
	{ count++; }

	fprintf ( fp, "Affects	%4d (%8ld bytes), %2d free (%ld bytes)\n",
			  aff_count, aff_count * ( sizeof ( *af ) ), count, count * ( sizeof ( *af ) ) );

	/* rooms */
	fprintf ( fp, "Rooms	%4d (%8ld bytes)\n",
			  top_room, top_room * ( sizeof ( *room ) ) );

	/* exits */
	fprintf ( fp, "Exits	%4d (%8ld bytes)\n",
			  top_exit, top_exit * ( sizeof ( *exit ) ) );

	fclose ( fp );

	/* start printing out mobile data */
	fp = fopen ( "mob.dmp", "w" );

	fprintf ( fp, "\nMobile Analysis\n" );
	fprintf ( fp,  "---------------\n" );
	nMatch = 0;
	for ( vnum = 0; nMatch < top_mob_index; vnum++ ) {
		if ( ( pMobIndex = get_mob_index ( vnum ) ) != NULL ) {
			nMatch++;
			fprintf ( fp, "#%-4d %3d active %3d killed     %s\n",
					  pMobIndex->vnum, pMobIndex->count,
					  pMobIndex->killed, pMobIndex->short_descr );
		}
	}
	fclose ( fp );

	/* start printing out object data */
	fp = fopen ( "obj.dmp", "w" );

	fprintf ( fp, "\nObject Analysis\n" );
	fprintf ( fp,  "---------------\n" );
	nMatch = 0;
	for ( vnum = 0; nMatch < top_obj_index; vnum++ ) {
		if ( ( pObjIndex = get_obj_index ( vnum ) ) != NULL ) {
			nMatch++;
			fprintf ( fp, "#%-4d %3d active %3d reset      %s\n",
					  pObjIndex->vnum, pObjIndex->count,
					  pObjIndex->reset_num, pObjIndex->short_descr );
		}
	}
	/* close file */
	fclose ( fp );
	fpReserve = fopen ( NULL_FILE, "r" );
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy ( int number )
{
	switch ( number_bits ( 2 ) ) {
		case 0:
			number -= 1;
			break;
		case 3:
			number += 1;
			break;
	}

	return UMAX ( 1, number );
}



/*
 * Generate a random number.
 */
int number_range ( int from, int to )
{
	int power;
	int number;

	if ( from == 0 && to == 0 )
	{ return 0; }

	if ( ( to = to - from + 1 ) <= 1 )
	{ return from; }

	for ( power = 2; power < to; power <<= 1 )
		;

	while ( ( number = number_mm() & ( power - 1 ) ) >= to )
		;

	return from + number;
}

/*
 * Generate a percentile roll.
 */
int number_percent ( void )
{
	int percent;

	while ( ( percent = number_mm() & ( 128 - 1 ) ) > 99 )
		;

	return 1 + percent;
}

/*
 * Generate a random door.
 */
int number_door ( void )
{
	int door;

	while ( ( door = number_mm() & ( 8 - 1 ) ) > 5 )
		;

	return door;
}

int number_bits ( int width )
{
	return number_mm( ) & ( ( 1 << width ) - 1 );
}

void init_mm( )
{
	srandom ( time ( NULL ) ^getpid() );
	return;
}



long number_mm ( void )
{
	return random() >> 6;
}


/*
 * Roll some dice.
 */
int dice ( int number, int size )
{
	int idice;
	int sum;

	switch ( size ) {
		case 0:
			return 0;
		case 1:
			return number;
	}

	for ( idice = 0, sum = 0; idice < number; idice++ )
	{ sum += number_range ( 1, size ); }

	return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate ( int level, int value_00, int value_32 )
{
	return value_00 + level * ( value_32 - value_00 ) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde ( const char *str )
{
	char *mstr = ( char * ) str;
	for ( ; *mstr != '\0'; mstr++ ) {
		if ( *mstr == '~' )
		{ *mstr = '-'; }
	}

	str = mstr;
	return;
}

/*
 * Returns an initial-capped string.
 */
char *capitalize ( const char *str )
{
	static char strcap[MAX_STRING_LENGTH];
	int i;

	for ( i = 0; str[i] != '\0'; i++ )
	{ strcap[i] = LOWER ( str[i] ); }
	strcap[i] = '\0';
	strcap[0] = UPPER ( strcap[0] );
	return strcap;
}


/*
 * Append a string to a file.
 */
void append_file ( Creature *ch, const char *file, const char *str )
{
	FILE *fp;

	if ( IS_NPC ( ch ) || str[0] == '\0' )
	{ return; }

	fclose ( fpReserve );
	if ( ( fp = fopen ( file, "a" ) ) == NULL ) {
		ReportErrno ( file );
		writeBuffer ( "Could not open the file!\n\r", ch );
	} else {
		fprintf ( fp, "[%5d] %s: %s\n",
				  ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
		fclose ( fp );
	}

	fpReserve = fopen ( NULL_FILE, "r" );
	return;
}

/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain ( void )
{
	return;
}

#endif


