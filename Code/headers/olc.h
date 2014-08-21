/***************************************************************************
 *  File: olc.h                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */


/*
 * Version Data because we are sad like this.
 */
#define VERSION	"    CombatMUD(V7) District Development Studio\n\r"
#define AUTHOR	"    Developed by David Simmerson\r\n"
#define DATE	"    \r\n"
#define CREDITS "    Ported to InfectEngineV2 August 2014"



/*
 * New typedefs.
 */
typedef	bool OLC_FUN		args ( ( Creature *ch, const char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun


/* Command procedures needed ROM OLC */
Command (    cmd_help    );
DECLARE_SPELL_FUN ( spell_null );





/*
 * Connected states for editor.
 */
#define ED_NONE		0
#define ED_AREA		1
#define ED_ROOM		2
#define ED_OBJECT	3
#define ED_MOBILE	4
#define ED_MPCODE	5
#define ED_HELP		6



/*
 * Interpreter Prototypes
 */
void    aedit           args ( ( Creature *ch, const char *argument ) );
void    redit           args ( ( Creature *ch, const char *argument ) );
void    medit           args ( ( Creature *ch, const char *argument ) );
void    oedit           args ( ( Creature *ch, const char *argument ) );
void	mpedit		args ( ( Creature *ch, const char *argument ) );
void	hedit		args ( ( Creature *, const char * ) );


/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */



/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type {
	const char * 	name;
	OLC_FUN *		olc_fun;
};



/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type {
	const char * 	name;
	CmdData *		cmd_fun;
};



/*
 * Utils.
 */
Zone *get_vnum_area	args ( ( int vnum ) );
Zone *get_area_data	args ( ( int vnum ) );
int flag_value			args ( ( const struct flag_type *flag_table,
								 const char *argument ) );
const char *flag_string		args ( ( const struct flag_type *flag_table,
									 int bits ) );
void add_reset			args ( ( RoomData *room,
								 Reset *pReset, int index ) );



/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	hedit_table[];


/*
 * Editor Commands.
 */
Command ( cmd_aedit        );
Command ( cmd_redit        );
Command ( cmd_oedit        );
Command ( cmd_medit        );
Command ( cmd_mpedit	);
Command ( cmd_hedit	);

/*
 * General Functions
 */
bool show_commands		args ( ( Creature *ch, const char *argument ) );
bool show_help			args ( ( Creature *ch, const char *argument ) );
bool edit_done			args ( ( Creature *ch ) );
bool show_version		args ( ( Creature *ch, const char *argument ) );



/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN ( aedit_show		);
DECLARE_OLC_FUN ( aedit_create		);
DECLARE_OLC_FUN ( aedit_name		);
DECLARE_OLC_FUN ( aedit_file		);
DECLARE_OLC_FUN ( aedit_age		);
/* DECLARE_OLC_FUN( aedit_recall	);       ROM OLC */
DECLARE_OLC_FUN ( aedit_reset		);
DECLARE_OLC_FUN ( aedit_security		);
DECLARE_OLC_FUN ( aedit_builder		);
DECLARE_OLC_FUN ( aedit_vnum		);
DECLARE_OLC_FUN ( aedit_lvnum		);
DECLARE_OLC_FUN ( aedit_uvnum		);
DECLARE_OLC_FUN ( aedit_credits		);


/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN ( redit_show		);
DECLARE_OLC_FUN ( redit_create		);
DECLARE_OLC_FUN ( redit_name		);
DECLARE_OLC_FUN ( redit_desc		);
DECLARE_OLC_FUN ( redit_ed		);
DECLARE_OLC_FUN ( redit_format		);
DECLARE_OLC_FUN ( redit_north		);
DECLARE_OLC_FUN ( redit_south		);
DECLARE_OLC_FUN ( redit_east		);
DECLARE_OLC_FUN ( redit_west		);
DECLARE_OLC_FUN ( redit_up		);
DECLARE_OLC_FUN ( redit_down		);
DECLARE_OLC_FUN ( redit_mreset		);
DECLARE_OLC_FUN ( redit_oreset		);
DECLARE_OLC_FUN ( redit_mlist		);
DECLARE_OLC_FUN ( redit_rlist		);
DECLARE_OLC_FUN ( redit_olist		);
DECLARE_OLC_FUN ( redit_mshow		);
DECLARE_OLC_FUN ( redit_oshow		);
DECLARE_OLC_FUN ( redit_heal		);
DECLARE_OLC_FUN ( redit_mana		);
DECLARE_OLC_FUN ( redit_clan		);
DECLARE_OLC_FUN ( redit_owner		);
DECLARE_OLC_FUN ( redit_room		);
DECLARE_OLC_FUN ( redit_sector		);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN ( oedit_show		);
DECLARE_OLC_FUN ( oedit_create		);
DECLARE_OLC_FUN ( oedit_name		);
DECLARE_OLC_FUN ( oedit_short		);
DECLARE_OLC_FUN ( oedit_long		);
DECLARE_OLC_FUN ( oedit_addaffect	);
DECLARE_OLC_FUN ( oedit_addapply		);
DECLARE_OLC_FUN ( oedit_delaffect	);
DECLARE_OLC_FUN ( oedit_value0		);
DECLARE_OLC_FUN ( oedit_value1		);
DECLARE_OLC_FUN ( oedit_value2		);
DECLARE_OLC_FUN ( oedit_value3		);
DECLARE_OLC_FUN ( oedit_value4		); /* ROM */
DECLARE_OLC_FUN ( oedit_weight		);
DECLARE_OLC_FUN ( oedit_cost		);
DECLARE_OLC_FUN ( oedit_ed		);

DECLARE_OLC_FUN ( oedit_extra            ); /* ROM */
DECLARE_OLC_FUN ( oedit_wear             ); /* ROM */
DECLARE_OLC_FUN ( oedit_type             ); /* ROM */
DECLARE_OLC_FUN ( oedit_affect           ); /* ROM */
DECLARE_OLC_FUN ( oedit_material		); /* ROM */
DECLARE_OLC_FUN ( oedit_level            ); /* ROM */
DECLARE_OLC_FUN ( oedit_condition        ); /* ROM */

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN ( medit_show		);
DECLARE_OLC_FUN ( medit_create		);
DECLARE_OLC_FUN ( medit_name		);
DECLARE_OLC_FUN ( medit_short		);
DECLARE_OLC_FUN ( medit_long		);
DECLARE_OLC_FUN ( medit_shop		);
DECLARE_OLC_FUN ( medit_desc		);
DECLARE_OLC_FUN ( medit_level		);
DECLARE_OLC_FUN ( medit_align		);
DECLARE_OLC_FUN ( medit_spec		);

DECLARE_OLC_FUN ( medit_sex		); /* ROM */
DECLARE_OLC_FUN ( medit_act		); /* ROM */
DECLARE_OLC_FUN ( medit_affect		); /* ROM */
DECLARE_OLC_FUN ( medit_ac		); /* ROM */
DECLARE_OLC_FUN ( medit_form		); /* ROM */
DECLARE_OLC_FUN ( medit_part		); /* ROM */
DECLARE_OLC_FUN ( medit_imm		); /* ROM */
DECLARE_OLC_FUN ( medit_res		); /* ROM */
DECLARE_OLC_FUN ( medit_vuln		); /* ROM */
DECLARE_OLC_FUN ( medit_material		); /* ROM */
DECLARE_OLC_FUN ( medit_off		); /* ROM */
DECLARE_OLC_FUN ( medit_size		); /* ROM */
DECLARE_OLC_FUN ( medit_hitdice		); /* ROM */
DECLARE_OLC_FUN ( medit_manadice		); /* ROM */
DECLARE_OLC_FUN ( medit_damdice		); /* ROM */
DECLARE_OLC_FUN ( medit_race		); /* ROM */
DECLARE_OLC_FUN ( medit_position		); /* ROM */
DECLARE_OLC_FUN ( medit_gold		); /* ROM */
DECLARE_OLC_FUN ( medit_hitroll		); /* ROM */
DECLARE_OLC_FUN ( medit_damtype		); /* ROM */
DECLARE_OLC_FUN ( medit_group		); /* ROM */
DECLARE_OLC_FUN ( medit_addmprog		); /* ROM */
DECLARE_OLC_FUN ( medit_delmprog		); /* ROM */

/* Mobprog editor */
DECLARE_OLC_FUN ( mpedit_create		);
DECLARE_OLC_FUN ( mpedit_code		);
DECLARE_OLC_FUN ( mpedit_show		);
DECLARE_OLC_FUN ( mpedit_list		);

/* Editor de helps */
DECLARE_OLC_FUN ( hedit_keyword		);
DECLARE_OLC_FUN ( hedit_text		);
DECLARE_OLC_FUN ( hedit_new		);
DECLARE_OLC_FUN ( hedit_level		);
DECLARE_OLC_FUN ( hedit_delete		);
DECLARE_OLC_FUN ( hedit_show		);
DECLARE_OLC_FUN ( hedit_list		);

/*
 * Macros
 */
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (NPCData *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (ItemData *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (Zone *)Ch->desc->pEdit )
#define EDIT_MPCODE(Ch, Code)   ( Code = (MPROG_CODE*)Ch->desc->pEdit )


/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	DescriptionData
Reset	*new_reset_data		args ( ( void ) );
void		recycle_reset_data		args ( ( Reset *pReset ) );
Zone	*new_area		args ( ( void ) );
void		recycle_area		args ( ( Zone *pArea ) );
Exit	*new_exit		args ( ( void ) );
void		recycle_exit		args ( ( Exit *pExit ) );
ED 		*new_extra_descr	args ( ( void ) );
void		recycle_extra_descr	args ( ( ED *pExtra ) );
RoomData *new_room_index		args ( ( void ) );
void		recycle_room_index		args ( ( RoomData *pRoom ) );
Affect	*new_affect		args ( ( void ) );
void		recycle_affect		args ( ( Affect* pAf ) );
SHOP_DATA	*new_shop		args ( ( void ) );
void		recycle_shop		args ( ( SHOP_DATA *pShop ) );
ItemData	*new_obj_index		args ( ( void ) );
void		recycle_obj_index		args ( ( ItemData *pObj ) );
NPCData	*new_mob_index		args ( ( void ) );
void		recycle_mob_index		args ( ( NPCData *pMob ) );
#undef	ED

void		show_liqlist		args ( ( Creature *ch ) );
void		show_damlist		args ( ( Creature *ch ) );

const char *		mprog_type_to_name	args ( ( int type ) );
MPROG_LIST      *new_mprog              args ( ( void ) );
void            recycle_mprog              args ( ( MPROG_LIST *mp ) );
MPROG_CODE	*new_mpcode		args ( ( void ) );
void		recycle_mpcode		args ( ( MPROG_CODE *pMcode ) );
