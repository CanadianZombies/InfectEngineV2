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

int flag_lookup args ( ( const char *name, const struct flag_type *flag_table ) );

DefineCommand ( cmd_flag )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	char word[MAX_INPUT_LENGTH];
	Creature *victim;
	long *flag, old = 0, inew = 0, marked = 0, pos;
	char type;
	const struct flag_type *flag_table;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	argument = ChopC ( argument, arg3 );

	type = argument[0];

	if ( type == '=' || type == '-' || type == '+' )
	{ argument = ChopC ( argument, word ); }

	if ( arg1[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  flag mob  <name> <field> <flags>\n\r", ch );
		writeBuffer ( "  flag char <name> <field> <flags>\n\r", ch );
		writeBuffer ( "  mob  flags: act,aff,off,imm,res,vuln,form,part\n\r", ch );
		writeBuffer ( "  char flags: plr,comm,aff,imm,res,vuln,\n\r", ch );
		writeBuffer ( "  +: add flag, -: remove flag, = set equal to\n\r", ch );
		writeBuffer ( "  otherwise flag toggles the flags listed.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' ) {
		writeBuffer ( "What do you wish to set flags on?\n\r", ch );
		return;
	}

	if ( arg3[0] == '\0' ) {
		writeBuffer ( "You need to specify a flag to set.\n\r", ch );
		return;
	}

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Which flags do you wish to change?\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg1, "mob" ) || !str_prefix ( arg1, "char" ) ) {
		victim = get_char_world ( ch, arg2 );
		if ( victim == NULL ) {
			writeBuffer ( "You can't find them.\n\r", ch );
			return;
		}

		/* select a flag to set */
		if ( !str_prefix ( arg3, "act" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Use plr for PCs.\n\r", ch );
				return;
			}

			flag = &victim->act;
			flag_table = act_flags;
		}  else if ( !str_prefix ( arg3, "plr" ) ) {
			if ( IS_NPC ( victim ) ) {
				writeBuffer ( "Use act for NPCs.\n\r", ch );
				return;
			}

			flag = &victim->act;
			flag_table = plr_flags;
		}

		else if ( !str_prefix ( arg3, "aff" ) ) {
			flag = &victim->affected_by;
			flag_table = affect_flags;
		}

		else if ( !str_prefix ( arg3, "immunity" ) ) {
			flag = &victim->imm_flags;
			flag_table = imm_flags;
		}

		else if ( !str_prefix ( arg3, "resist" ) ) {
			flag = &victim->res_flags;
			flag_table = imm_flags;
		}

		else if ( !str_prefix ( arg3, "vuln" ) ) {
			flag = &victim->vuln_flags;
			flag_table = imm_flags;
		}

		else if ( !str_prefix ( arg3, "form" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Form can't be set on PCs.\n\r", ch );
				return;
			}

			flag = &victim->form;
			flag_table = form_flags;
		}

		else if ( !str_prefix ( arg3, "parts" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Parts can't be set on PCs.\n\r", ch );
				return;
			}

			flag = &victim->parts;
			flag_table = part_flags;
		}

		else if ( !str_prefix ( arg3, "comm" ) ) {
			if ( IS_NPC ( victim ) ) {
				writeBuffer ( "Comm can't be set on NPCs.\n\r", ch );
				return;
			}

			flag = &victim->comm;
			flag_table = comm_flags;
		}

		else {
			writeBuffer ( "That's not an acceptable flag.\n\r", ch );
			return;
		}

		old = *flag;
		victim->zone = NULL;

		if ( type != '=' )
		{ inew = old; }

		/* mark the words */
		for ( ; ; ) {
			argument = ChopC ( argument, word );

			if ( word[0] == '\0' )
			{ break; }

			pos = flag_lookup ( word, flag_table );

			if ( pos == NO_FLAG ) {
				writeBuffer ( "That flag doesn't exist!\n\r", ch );
				return;
			} else
			{ SET_BIT ( marked, pos ); }
		}

		for ( pos = 0; flag_table[pos].name != NULL; pos++ ) {
			if ( !flag_table[pos].settable && IS_SET ( old, flag_table[pos].bit ) ) {
				SET_BIT ( inew, flag_table[pos].bit );
				continue;
			}

			if ( IS_SET ( marked, flag_table[pos].bit ) ) {
				switch ( type ) {
					case '=':
					case '+':
						SET_BIT ( inew, flag_table[pos].bit );
						break;
					case '-':
						REMOVE_BIT ( inew, flag_table[pos].bit );
						break;
					default:
						if ( IS_SET ( inew, flag_table[pos].bit ) )
						{ REMOVE_BIT ( inew, flag_table[pos].bit ); }
						else
						{ SET_BIT ( inew, flag_table[pos].bit ); }
				}
			}
		}
		*flag = inew;
		return;
	}
}




