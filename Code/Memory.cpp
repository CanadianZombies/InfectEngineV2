/***************************************************************************
 *  File: mem.c                                                            *
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



#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

/*
 * Globals
 */
extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          int                     top_room;
extern		int			top_mprog_index;

RoomData *room_index_free;
NPCData *mob_index_free;
ItemData *obj_index_free;
SHOP_DATA *shop_free;
Reset*reset_free;
Zone *area_free;
Exit *exit_free;

void	recycle_extra_descr	args ( ( DescriptionData *pExtra ) );
void	recycle_affect		args ( ( Affect *af ) );
void	recycle_mprog              args ( ( MPROG_LIST *mp ) );


Reset *new_reset_data ( void )
{
	Reset *pReset;

	if ( !reset_free ) {
		ALLOC_DATA ( pReset, Reset, 1 );
		top_reset++;
	} else {
		pReset          =   reset_free;
		reset_free      =   reset_free->next;
	}

	pReset->next        =   NULL;
	pReset->command     =   'X';
	pReset->arg1        =   0;
	pReset->arg2        =   0;
	pReset->arg3        =   0;
	pReset->arg4	=   0;

	return pReset;
}



void recycle_reset_data ( Reset *pReset )
{
	pReset->next            = reset_free;
	reset_free              = pReset;
	return;
}



Zone *new_area ( void )
{
	Zone *pArea;
	char buf[MAX_INPUT_LENGTH];

	if ( !area_free ) {
		ALLOC_DATA ( pArea, Zone, 1 );
		top_area++;
	} else {
		pArea       =   area_free;
		area_free   =   area_free->next;
	}

	pArea->next             =   NULL;
	pArea->name             =   assign_string ( "New area" );
	/*    pArea->recall           =   ROOM_VNUM_TEMPLE;      ROM OLC */
	pArea->area_flags       =   AREA_ADDED;
	pArea->security         =   1;
	pArea->builders         =   assign_string ( "None" );
	pArea->min_vnum            =   0;
	pArea->max_vnum            =   0;
	pArea->age              =   0;
	pArea->nplayer          =   0;
	pArea->empty            =   TRUE;              /* ROM patch */
	sprintf ( buf, "area%d.are", pArea->vnum );
	pArea->file_name        =   assign_string ( buf );
	pArea->vnum             =   top_area - 1;

	return pArea;
}



void recycle_area ( Zone *pArea )
{
	PURGE_DATA ( pArea->name );
	PURGE_DATA ( pArea->file_name );
	PURGE_DATA ( pArea->builders );
	PURGE_DATA ( pArea->credits );

	pArea->next         =   area_free->next;
	area_free           =   pArea;
	return;
}



Exit *new_exit ( void )
{
	Exit *pExit;

	if ( !exit_free ) {
		ALLOC_DATA ( pExit, Exit, 1 );
		top_exit++;
	} else {
		pExit           =   exit_free;
		exit_free       =   exit_free->next;
	}

	pExit->u1.to_room   =   NULL;                  /* ROM OLC */
	pExit->next         =   NULL;
	/*  pExit->vnum         =   0;                        ROM OLC */
	pExit->exit_info    =   0;
	pExit->key          =   0;
	pExit->keyword      =   &str_empty[0];
	pExit->description  =   &str_empty[0];
	pExit->rs_flags     =   0;

	return pExit;
}



void recycle_exit ( Exit *pExit )
{
	PURGE_DATA ( pExit->keyword );
	PURGE_DATA ( pExit->description );

	pExit->next         =   exit_free;
	exit_free           =   pExit;
	return;
}


RoomData *new_room_index ( void )
{
	RoomData *pRoom;
	int door;

	if ( !room_index_free ) {
		ALLOC_DATA ( pRoom, RoomData, 1 );
		top_room++;
	} else {
		pRoom           =   room_index_free;
		room_index_free =   room_index_free->next;
	}

	pRoom->next             =   NULL;
	pRoom->people           =   NULL;
	pRoom->contents         =   NULL;
	pRoom->extra_descr      =   NULL;
	pRoom->area             =   NULL;

	for ( door = 0; door < MAX_DIR; door++ )
	{ pRoom->exit[door]   =   NULL; }

	pRoom->name             =   &str_empty[0];
	pRoom->description      =   &str_empty[0];
	pRoom->owner	    =	&str_empty[0];
	pRoom->vnum             =   0;
	pRoom->room_flags       =   0;
	pRoom->light            =   0;
	pRoom->sector_type      =   0;
	pRoom->clan		    =	0;
	pRoom->heal_rate	    =   100;
	pRoom->mana_rate	    =   100;

	return pRoom;
}



void recycle_room_index ( RoomData *pRoom )
{
	int door;
	DescriptionData *pExtra;
	Reset *pReset;

	PURGE_DATA ( pRoom->name );
	PURGE_DATA ( pRoom->description );
	PURGE_DATA ( pRoom->owner );

	for ( door = 0; door < MAX_DIR; door++ ) {
		if ( pRoom->exit[door] )
		{ recycle_exit ( pRoom->exit[door] ); }
	}

	for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next ) {
		recycle_extra_descr ( pExtra );
	}

	for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next ) {
		recycle_reset_data ( pReset );
	}

	pRoom->next     =   room_index_free;
	room_index_free =   pRoom;
	return;
}

extern Affect *affect_free;


SHOP_DATA *new_shop ( void )
{
	SHOP_DATA *pShop;
	int buy;

	if ( !shop_free ) {
		ALLOC_DATA ( pShop, SHOP_DATA, 1 );
		top_shop++;
	} else {
		pShop           =   shop_free;
		shop_free       =   shop_free->next;
	}

	pShop->next         =   NULL;
	pShop->keeper       =   0;

	for ( buy = 0; buy < MAX_TRADE; buy++ )
	{ pShop->buy_type[buy]    =   0; }

	pShop->profit_buy   =   100;
	pShop->profit_sell  =   100;
	pShop->open_hour    =   0;
	pShop->close_hour   =   23;

	return pShop;
}



void recycle_shop ( SHOP_DATA *pShop )
{
	pShop->next = shop_free;
	shop_free   = pShop;
	return;
}



ItemData *new_obj_index ( void )
{
	ItemData *pObj;
	int value;

	if ( !obj_index_free ) {
		ALLOC_DATA ( pObj, ItemData, 1 );
		top_obj_index++;
	} else {
		pObj            =   obj_index_free;
		obj_index_free  =   obj_index_free->next;
	}

	pObj->next          =   NULL;
	pObj->extra_descr   =   NULL;
	pObj->affected      =   NULL;
	pObj->area          =   NULL;
	pObj->name          =   assign_string ( "no name" );
	pObj->short_descr   =   assign_string ( "(no short description)" );
	pObj->description   =   assign_string ( "(no description)" );
	pObj->vnum          =   0;
	pObj->item_type     =   ITEM_TRASH;
	pObj->extra_flags   =   0;
	pObj->wear_flags    =   0;
	pObj->count         =   0;
	pObj->weight        =   0;
	pObj->cost          =   0;
	pObj->material      =   assign_string ( "unknown" );     /* ROM */
	pObj->condition     =   100;                        /* ROM */
	for ( value = 0; value < 5; value++ )               /* 5 - ROM */
	{ pObj->value[value]  =   0; }

	pObj->new_format    = TRUE; /* ROM */

	return pObj;
}



void recycle_obj_index ( ItemData *pObj )
{
	DescriptionData *pExtra;
	Affect *pAf;

	PURGE_DATA ( pObj->name );
	PURGE_DATA ( pObj->short_descr );
	PURGE_DATA ( pObj->description );

	for ( pAf = pObj->affected; pAf; pAf = pAf->next ) {
		recycle_affect ( pAf );
	}

	for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next ) {
		recycle_extra_descr ( pExtra );
	}

	pObj->next              = obj_index_free;
	obj_index_free          = pObj;
	return;
}



NPCData *new_mob_index ( void )
{
	NPCData *pMob;

	if ( !mob_index_free ) {
		ALLOC_DATA ( pMob, NPCData, 1 );
		top_mob_index++;
	} else {
		pMob            =   mob_index_free;
		mob_index_free  =   mob_index_free->next;
	}

	pMob->next          =   NULL;
	pMob->spec_fun      =   NULL;
	pMob->pShop         =   NULL;
	pMob->area          =   NULL;
	pMob->player_name   =   assign_string ( "no name" );
	pMob->short_descr   =   assign_string ( "(no short description)" );
	pMob->long_descr    =   assign_string ( "(no long description)\n\r" );
	pMob->description   =   &str_empty[0];
	pMob->vnum          =   0;
	pMob->count         =   0;
	pMob->killed        =   0;
	pMob->sex           =   0;
	pMob->level         =   0;
	pMob->act           =   ACT_IS_NPC;
	pMob->affected_by   =   0;
	pMob->alignment     =   0;
	pMob->hitroll	=   0;
	pMob->race          =   race_lookup ( "human" ); /* - Hugin */
	pMob->form          =   0;           /* ROM patch -- Hugin */
	pMob->parts         =   0;           /* ROM patch -- Hugin */
	pMob->imm_flags     =   0;           /* ROM patch -- Hugin */
	pMob->res_flags     =   0;           /* ROM patch -- Hugin */
	pMob->vuln_flags    =   0;           /* ROM patch -- Hugin */
	pMob->material      =   assign_string ( "unknown" ); /* -- Hugin */
	pMob->off_flags     =   0;           /* ROM patch -- Hugin */
	pMob->size          =   SIZE_MEDIUM; /* ROM patch -- Hugin */
	pMob->ac[AC_PIERCE]	=   0;           /* ROM patch -- Hugin */
	pMob->ac[AC_BASH]	=   0;           /* ROM patch -- Hugin */
	pMob->ac[AC_SLASH]	=   0;           /* ROM patch -- Hugin */
	pMob->ac[AC_EXOTIC]	=   0;           /* ROM patch -- Hugin */
	pMob->hit[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
	pMob->hit[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
	pMob->hit[DICE_BONUS]	=   0;   /* ROM patch -- Hugin */
	pMob->mana[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
	pMob->mana[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
	pMob->mana[DICE_BONUS]	=   0;   /* ROM patch -- Hugin */
	pMob->damage[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
	pMob->damage[DICE_TYPE]	=   0;   /* ROM patch -- Hugin */
	pMob->damage[DICE_NUMBER]	=   0;   /* ROM patch -- Hugin */
	pMob->start_pos             =   POS_STANDING; /*  -- Hugin */
	pMob->default_pos           =   POS_STANDING; /*  -- Hugin */
	pMob->wealth                =   0;

	pMob->new_format            = TRUE;  /* ROM */

	return pMob;
}



void recycle_mob_index ( NPCData *pMob )
{
	PURGE_DATA ( pMob->player_name );
	PURGE_DATA ( pMob->short_descr );
	PURGE_DATA ( pMob->long_descr );
	PURGE_DATA ( pMob->description );
	recycle_mprog ( pMob->mprogs );

	recycle_shop ( pMob->pShop );

	pMob->next              = mob_index_free;
	mob_index_free          = pMob;
	return;
}

MPROG_CODE              *       mpcode_free;

MPROG_CODE *new_mpcode ( void )
{
	MPROG_CODE *NewCode;

	if ( !mpcode_free ) {
		ALLOC_DATA ( NewCode, MPROG_CODE, 1 );
		top_mprog_index++;
	} else {
		NewCode     = mpcode_free;
		mpcode_free = mpcode_free->next;
	}

	NewCode->vnum    = 0;
	NewCode->code    = assign_string ( "" );
	NewCode->next    = NULL;

	return NewCode;
}

void recycle_mpcode ( MPROG_CODE *pMcode )
{
	PURGE_DATA ( pMcode->code );
	pMcode->next = mpcode_free;
	mpcode_free  = pMcode;
	return;
}