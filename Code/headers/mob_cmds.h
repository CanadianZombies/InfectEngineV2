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
