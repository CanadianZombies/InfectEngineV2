/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *                                                                         *
 ***************************************************************************/


struct	mob_cmd_type {
	const char * 	name;
	CmdData *		cmd_fun;
};

/* the command table itself */
extern	const	struct	mob_cmd_type	mob_cmd_table	[];

/*
 * Command functions.
 * Defined in mob_cmds.c
 */
Command (	cmd_mpasound	);
Command (	cmd_mpgecho	);
Command (	cmd_mpzecho	);
Command (	cmd_mpkill	);
Command (	cmd_mpassist	);
Command (	cmd_mpjunk	);
Command (	cmd_mpechoaround	);
Command (	cmd_mpecho	);
Command (	cmd_mpechoat	);
Command (	cmd_mpmload	);
Command (	cmd_mpoload	);
Command (	cmd_mppurge	);
Command (	cmd_mpgoto	);
Command (	cmd_mpat		);
Command (	cmd_mptransfer	);
Command (	cmd_mpgtransfer	);
Command (	cmd_mpforce	);
Command (	cmd_mpgforce	);
Command (	cmd_mpvforce	);
Command (	cmd_mpcast	);
Command (	cmd_mpdamage	);
Command (	cmd_mpremember	);
Command (	cmd_mpforget	);
Command (	cmd_mpdelay	);
Command (	cmd_mpcancel	);
Command (	cmd_mpcall	);
Command (	cmd_mpflee	);
Command (	cmd_mpotransfer	);
Command (	cmd_mpremove	);
