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

struct flag_stat_type {
	const struct flag_type *structure;
	bool stat;
};



/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table catagorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] = {
	/*  {	structure		stat	}, */
	{	area_flags,		FALSE	},
	{   sex_flags,		TRUE	},
	{   exit_flags,		FALSE	},
	{   door_resets,		TRUE	},
	{   room_flags,		FALSE	},
	{   sector_flags,		TRUE	},
	{   type_flags,		TRUE	},
	{   extra_flags,		FALSE	},
	{   wear_flags,		FALSE	},
	{   act_flags,		FALSE	},
	{   affect_flags,		FALSE	},
	{   apply_flags,		TRUE	},
	{   wear_loc_flags,		TRUE	},
	{   wear_loc_strings,	TRUE	},
	{   container_flags,	FALSE	},
	{ material_flags,	FALSE },

	/* ROM specific flags: */

	{   form_flags,             FALSE   },
	{   part_flags,             FALSE   },
	{   ac_type,                TRUE    },
	{   size_flags,             TRUE    },
	{   position_flags,         TRUE    },
	{   off_flags,              FALSE   },
	{   imm_flags,              FALSE   },
	{   res_flags,              FALSE   },
	{   vuln_flags,             FALSE   },
	{   weapon_archetype,           TRUE    },
	{   weapon_type2,            FALSE   },
	{   apply_types,		TRUE	},
	{   0,			0	}
};



/*****************************************************************************
 Name:		is_stat( table )
 Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat ( const struct flag_type *flag_table )
{
	int flag;

	for ( flag = 0; flag_stat_table[flag].structure; flag++ ) {
		if ( flag_stat_table[flag].structure == flag_table
				&& flag_stat_table[flag].stat )
		{ return TRUE; }
	}
	return FALSE;
}

/*****************************************************************************
 Name:		flag_value( table, flag )
 Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
 Called by:	olc.c and olc_act.c.
 ****************************************************************************/
int flag_value ( const struct flag_type *flag_table, const char *argument )
{
	char word[MAX_INPUT_LENGTH];
	int  bit;
	int  marked = 0;
	bool found = FALSE;

	if ( is_stat ( flag_table ) )
	{ return flag_lookup ( argument, flag_table ); }

	/*
	 * Accept multiple flags.
	 */
	for ( ; ; ) {
		argument = ChopC ( argument, word );

		if ( word[0] == '\0' )
		{ break; }

		if ( ( bit = flag_lookup ( word, flag_table ) ) != NO_FLAG ) {
			SET_BIT ( marked, bit );
			found = TRUE;
		}
	}

	if ( found )
	{ return marked; }
	else
	{ return NO_FLAG; }
}



/*****************************************************************************
 Name:		flag_string( table, flags/stat )
 Purpose:	Returns string with name(s) of the flags or stat entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
const char *flag_string ( const struct flag_type *flag_table, int bits )
{
	static char buf[2][512];
	static int cnt = 0;
	int  flag;

	if ( ++cnt > 1 )
	{ cnt = 0; }

	buf[cnt][0] = '\0';

	for ( flag = 0; flag_table[flag].name != NULL; flag++ ) {
		if ( !is_stat ( flag_table ) && IS_SET ( bits, flag_table[flag].bit ) ) {
			strcat ( buf[cnt], " " );
			strcat ( buf[cnt], flag_table[flag].name );
		} else if ( flag_table[flag].bit == bits ) {
			strcat ( buf[cnt], " " );
			strcat ( buf[cnt], flag_table[flag].name );
			break;
		}
	}
	return ( buf[cnt][0] != '\0' ) ? buf[cnt] + 1 : "none";
}
