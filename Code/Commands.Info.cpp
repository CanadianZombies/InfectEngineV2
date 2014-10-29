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

const char *	where_name	[] = {
	"\aw{\aYo\aw}\aY--=\ag[\aoLight Source\aB:     ",
	"\aw{\aYo\aw}\aY--=\ag[\aoRing Finger L\aB:    ",
	"\aw{\aYo\aw}\aY--=\ag[\aoRing Finger R\aB:    ",
	"\aw{\aYo\aw}\aY--=\ag[\aoNeck\aB:             ",
	"\aw{\aYo\aw}\aY--=\ag[\aoNeck\aB:             ",
	"\aw{\aYo\aw}\aY--=\ag[\aoUpper Body\aB:       ",
	"\aw{\aYo\aw}\aY--=\ag[\aoHeadwear\aB:         ",
	"\aw{\aYo\aw}\aY--=\ag[\aoLegs\aB:             ",
	"\aw{\aYo\aw}\aY--=\ag[\aoFeet\aB:             ",
	"\aw{\aYo\aw}\aY--=\ag[\aoHands\aB:            ",
	"\aw{\aYo\aw}\aY--=\ag[\aoArms\aB:             ",
	"\aw{\aYo\aw}\aY--=\ag[\aoShield\aB:           ",
	"\aw{\aYo\aw}\aY--=\ag[\aoCovering Body\aB:    ",
	"\aw{\aYo\aw}\aY--=\ag[\aoWaist\aB:            ",
	"\aw{\aYo\aw}\aY--=\ag[\aoWrist L\aB:          ",
	"\aw{\aYo\aw}\aY--=\ag[\aoWrist R\aB:          ",
	"\aw{\aYo\aw}\aY--=\ag[\aoWeapon\aB:           ",
	"\aw{\aYo\aw}\aY--=\ag[\aoHolding\aB:          ",
	"\aw{\aYo\aw}\aY--=\ag[\aoHovering\aB:         ",
};


/* for  keeping track of the player count */
int max_on = 0;

/*
 * Local functions.
 */
char *	format_obj_to_char	args ( ( Item *obj, Creature *ch,
									 bool fShort ) );
void	show_list_to_char	args ( ( Item *list, Creature *ch,
									 bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args ( ( Creature *victim, Creature *ch ) );
void	show_char_to_char_1	args ( ( Creature *victim, Creature *ch ) );
void	show_char_to_char	args ( ( Creature *list, Creature *ch ) );
bool	check_blind		args ( ( Creature *ch ) );



char *format_obj_to_char ( Item *obj, Creature *ch, bool fShort )
{
	static char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if ( ( fShort && ( IS_NULLSTR ( obj->short_descr ) ) ) ||  ( IS_NULLSTR ( obj->description ) ) )
	{ return buf; }

	if ( IS_OBJ_STAT ( obj, ITEM_INVIS )     )   { strcat ( buf, "\ac(\aWInvis\ac) "     ); }
	if ( IS_AFFECTED ( ch, AFF_DETECT_EVIL )
			&& IS_OBJ_STAT ( obj, ITEM_EVIL )   )   { strcat ( buf, "\ac(\aRRed Aura\ac) "  ); }
	if ( IS_AFFECTED ( ch, AFF_DETECT_GOOD )
			&&  IS_OBJ_STAT ( obj, ITEM_BLESS ) )	      { strcat ( buf, "\ac(\aBBlue Aura\ac) "	); }
	if ( IS_AFFECTED ( ch, AFF_DETECT_MAGIC )
			&& IS_OBJ_STAT ( obj, ITEM_MAGIC )  )   { strcat ( buf, "\ac(\aMMagical\ac) "   ); }
	if ( IS_OBJ_STAT ( obj, ITEM_GLOW )      )   { strcat ( buf, "\ac(\aYGlowing\ac) "   ); }
	if ( IS_OBJ_STAT ( obj, ITEM_HUM )       )   { strcat ( buf, "\ac(\aOHumming\ac) "   ); }

	if ( fShort ) {
		if ( !IS_NULLSTR ( obj->short_descr ) )
		{ strcat ( buf, Format ( "\aW%s\an", obj->short_descr ) ); }
	} else {
		if ( !IS_NULLSTR ( obj->description ) ) {
			// -- fix the possible leaking colours glitch
			strcat ( buf, Format ( "\aW%s\an", obj->description ) );
		}
	}

	return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char ( Item *list, Creature *ch, bool fShort, bool fShowNothing )
{
	std::string output ( "" );
	char **prgpstrShow;
	int *prgnShow;
	char *pstrShow;
	Item *obj;
	int nShow;
	int iShow;
	int count;
	bool fCombine;

	if ( ch->desc == NULL )
	{ return; }

	count = 0;
	for ( obj = list; obj != NULL; obj = obj->next_content )
	{ count++; }

	ALLOC_DATA ( prgpstrShow, char *, count * sizeof ( char * ) );
	ALLOC_DATA ( prgnShow, int, count * sizeof ( int ) );

	nShow	= 0;

	/*
	 * Format the list of objects.
	 */
	for ( obj = list; obj != NULL; obj = obj->next_content ) {
		if ( obj->wear_loc == WEAR_NONE && can_see_obj ( ch, obj ) ) {
			pstrShow = format_obj_to_char ( obj, ch, fShort );

			fCombine = FALSE;

			if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) ) {
				/*
				 * Look for duplicates, case sensitive.
				 * Matches tend to be near end so run loop backwords.
				 */
				for ( iShow = nShow - 1; iShow >= 0; iShow-- ) {
					if ( !strcmp ( prgpstrShow[iShow], pstrShow ) ) {
						prgnShow[iShow]++;
						fCombine = TRUE;
						break;
					}
				}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if ( !fCombine ) {
				prgpstrShow [nShow] = assign_string ( pstrShow );
				prgnShow    [nShow] = 1;
				nShow++;
			}
		}
	}

	/*
	 * Output the formatted list.
	 */
	for ( iShow = 0; iShow < nShow; iShow++ ) {
		if ( prgpstrShow[iShow][0] == '\0' ) {
			PURGE_DATA ( prgpstrShow[iShow] );
			continue;
		}

		if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) ) {
			if ( prgnShow[iShow] != 1 ) {
				output.append ( Format ( "(%2d) ", prgnShow[iShow] ) );
			} else {
				output.append ( "     " );
			}
		}
		output.append ( prgpstrShow[iShow] );
		output.append ( "\n\r" );
		PURGE_DATA ( prgpstrShow[iShow] );
	}

	if ( fShowNothing && nShow == 0 ) {
		if ( IS_NPC ( ch ) || IS_SET ( ch->comm, COMM_COMBINE ) )
		{ writeBuffer ( "     ", ch ); }
		writeBuffer ( "Nothing.\n\r", ch );
	}
	writePage ( C_STR ( output ), ch );

	// -- cleanup!
	// -- PURGE_DATA will corrupt absolutely here.
	free ( ( void * ) prgpstrShow );
	free ( ( void * ) prgnShow );
	return;
}



void show_char_to_char_0 ( Creature *victim, Creature *ch )
{
	char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if ( IsStaff ( ch ) && !IS_NPC ( victim ) && victim->desc == NULL ) { strcat ( buf, "\ac{\aWLINKDEAD\ac} " ); }
	if ( IS_SET ( victim->comm, COMM_AFK	  )   ) { strcat ( buf, "\ac[\aBAFK\ac] "	     ); }
	if ( IS_AFFECTED ( victim, AFF_INVISIBLE )   ) { strcat ( buf, "\ac(\aCInvis\ac) "      ); }
	if ( victim->invis_level >= LEVEL_HERO    ) { strcat ( buf, "\ac(\aCWizi\ac) "	     ); }
	if ( IS_AFFECTED ( victim, AFF_HIDE )        ) { strcat ( buf, "\ac(\aoHide\ac) "       ); }
	if ( IS_AFFECTED ( victim, AFF_CHARM )       ) { strcat ( buf, "\ac(\aOCharmed\ac) "    ); }
	if ( IS_AFFECTED ( victim, AFF_PASS_DOOR )   ) { strcat ( buf, "\ac(\aWTranslucent\ac) " ); }
	if ( IS_AFFECTED ( victim, AFF_FAERIE_FIRE ) ) { strcat ( buf, "\ac(\aPPink Aura\ac) "  ); }
	if ( IS_EVIL ( victim )
			&&   IS_AFFECTED ( ch, AFF_DETECT_EVIL )     ) { strcat ( buf, "\ac(\aRRed Aura\ac) "   ); }
	if ( IS_GOOD ( victim )
			&&   IS_AFFECTED ( ch, AFF_DETECT_GOOD )     ) { strcat ( buf, "\ac(\aYGolden Aura\ac) " ); }
	if ( IS_AFFECTED ( victim, AFF_SANCTUARY )   ) { strcat ( buf, "\ac(\aWWhite Aura\ac) " ); }
	if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_KILLER ) )
	{ strcat ( buf, "\ac(\a[F111]KILLER\ac) "     ); }
	if ( !IS_NPC ( victim ) && IS_SET ( victim->act, PLR_THIEF  ) )
	{ strcat ( buf, "\ac(\a[F211]THIEF\ac) "      ); }

	if ( victim->position == victim->start_pos && !IS_NULLSTR ( victim->long_descr ) ) {
		strcat ( buf, Format ( "\an%s\an", victim->long_descr ) );
		writeBuffer ( buf, ch );
		return;
	}

	strcat ( buf, Format ( "\an%s\an", PERS ( victim, ch ) ) );

	if ( !IS_NPC ( victim ) && !IS_SET ( ch->comm, COMM_BRIEF )
			&&   victim->position == POS_STANDING && ch->on == NULL )
	{ strcat ( buf, Format ( "%s\an", victim->pcdata->title ) ); }

	switch ( victim->position ) {
		case POS_DEAD:
			strcat ( buf, " is DEAD!!" );
			break;
		case POS_MORTAL:
			strcat ( buf, " is mortally wounded." );
			break;
		case POS_INCAP:
			strcat ( buf, " is incapacitated." );
			break;
		case POS_STUNNED:
			strcat ( buf, " is lying here stunned." );
			break;
		case POS_SLEEPING:
			if ( victim->on != NULL ) {
				if ( IS_SET ( victim->on->value[2], SLEEP_AT ) ) {
					sprintf ( message, " is sleeping at %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else if ( IS_SET ( victim->on->value[2], SLEEP_ON ) ) {
					sprintf ( message, " is sleeping on %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else {
					sprintf ( message, " is sleeping in %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				}
			} else
			{ strcat ( buf, " is sleeping here." ); }
			break;
		case POS_RESTING:
			if ( victim->on != NULL ) {
				if ( IS_SET ( victim->on->value[2], REST_AT ) ) {
					sprintf ( message, " is resting at %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else if ( IS_SET ( victim->on->value[2], REST_ON ) ) {
					sprintf ( message, " is resting on %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else {
					sprintf ( message, " is resting in %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				}
			} else
			{ strcat ( buf, " is resting here." ); }
			break;
		case POS_SITTING:
			if ( victim->on != NULL ) {
				if ( IS_SET ( victim->on->value[2], SIT_AT ) ) {
					sprintf ( message, " is sitting at %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else if ( IS_SET ( victim->on->value[2], SIT_ON ) ) {
					sprintf ( message, " is sitting on %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else {
					sprintf ( message, " is sitting in %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				}
			} else
			{ strcat ( buf, " is sitting here." ); }
			break;
		case POS_STANDING:
			if ( victim->on != NULL ) {
				if ( IS_SET ( victim->on->value[2], STAND_AT ) ) {
					sprintf ( message, " is standing at %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else if ( IS_SET ( victim->on->value[2], STAND_ON ) ) {
					sprintf ( message, " is standing on %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				} else {
					sprintf ( message, " is standing in %s.",
							  victim->on->short_descr );
					strcat ( buf, message );
				}
			} else
			{ strcat ( buf, " is here." ); }
			break;
		case POS_FIGHTING:
			strcat ( buf, " is here, fighting " );
			if ( FIGHTING ( victim ) == NULL )
			{ strcat ( buf, "thin air??" ); }
			else if ( FIGHTING ( victim ) == ch )
			{ strcat ( buf, "YOU!" ); }
			else if ( IN_ROOM ( victim ) == FIGHTING ( victim )->in_room ) {
				strcat ( buf, PERS ( FIGHTING ( victim ), ch ) );
				strcat ( buf, "." );
			} else
			{ strcat ( buf, "someone who left??" ); }
			break;
	}

	strcat ( buf, "\n\r" );
	buf[0] = UPPER ( buf[0] );
	writeBuffer ( buf, ch );
	return;
}



void show_char_to_char_1 ( Creature *victim, Creature *ch )
{
	char buf[MAX_STRING_LENGTH];
	Item *obj;
	int iWear;
	int percent;
	bool found;

	if ( can_see ( victim, ch ) ) {
		if ( ch == victim )
		{ act ( "$n looks at $mself.", ch, NULL, NULL, TO_ROOM ); }
		else {
			act ( "$n looks at you.", ch, NULL, victim, TO_VICT    );
			act ( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
		}
	}

	if ( !IS_NULLSTR ( victim->description ) ) {
		writeBuffer ( victim->description, ch );
	} else {
		act ( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
	}

	if ( victim->max_hit > 0 )
	{ percent = ( 100 * victim->hit ) / victim->max_hit; }
	else
	{ percent = -1; }

	strcpy ( buf, PERS ( victim, ch ) );

	if ( percent >= 100 )
	{ strcat ( buf, " is in excellent condition.\n\r" ); }
	else if ( percent >= 90 )
	{ strcat ( buf, " has a few scratches.\n\r" ); }
	else if ( percent >= 75 )
	{ strcat ( buf, " has some small wounds and bruises.\n\r" ); }
	else if ( percent >=  50 )
	{ strcat ( buf, " has quite a few wounds.\n\r" ); }
	else if ( percent >= 30 )
	{ strcat ( buf, " has some big nasty wounds and scratches.\n\r" ); }
	else if ( percent >= 15 )
	{ strcat ( buf, " looks pretty hurt.\n\r" ); }
	else if ( percent >= 0 )
	{ strcat ( buf, " is in awful condition.\n\r" ); }
	else
	{ strcat ( buf, " is bleeding to death.\n\r" ); }

	buf[0] = UPPER ( buf[0] );
	writeBuffer ( buf, ch );

	found = FALSE;
	for ( iWear = 0; iWear < MAX_WEAR; iWear++ ) {
		if ( ( obj = get_eq_char ( victim, iWear ) ) != NULL
				&&   can_see_obj ( ch, obj ) ) {
			if ( !found ) {
				writeBuffer ( "\n\r", ch );
				act ( "$N is using:", ch, NULL, victim, TO_CHAR );
				found = TRUE;
			}
			writeBuffer ( where_name[iWear], ch );
			writeBuffer ( format_obj_to_char ( obj, ch, TRUE ), ch );
			writeBuffer ( "\n\r", ch );
		}
	}

	if ( victim != ch
			&&   !IS_NPC ( ch )
			&&   Math::instance().percent( ) < get_skill ( ch, gsn_peek ) ) {
		writeBuffer ( "\n\rYou peek at the inventory:\n\r", ch );
		check_improve ( ch, gsn_peek, TRUE, 4 );
		show_list_to_char ( victim->carrying, ch, TRUE, TRUE );
	}

	return;
}



void show_char_to_char ( Creature *list, Creature *ch )
{
	Creature *rch;

	for ( rch = list; rch != NULL; rch = rch->next_in_room ) {
		if ( rch == ch )
		{ continue; }

		if ( get_trust ( ch ) < rch->invis_level )
		{ continue; }

		if ( can_see ( ch, rch ) ) {
			show_char_to_char_0 ( rch, ch );
		} else if ( room_is_dark ( IN_ROOM ( ch ) )
					&&        IS_AFFECTED ( rch, AFF_INFRARED ) ) {
			writeBuffer ( "You see glowing red eyes watching YOU!\n\r", ch );
		}
	}

	return;
}



bool check_blind ( Creature *ch )
{

	if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) )
	{ return TRUE; }

	if ( IS_AFFECTED ( ch, AFF_BLIND ) ) {
		writeBuffer ( "You can't see a thing!\n\r", ch );
		return FALSE;
	}

	return TRUE;
}

/* changes your scroll */
DefineCommand ( cmd_scroll )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[100];
	int lines;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		if ( ch->lines == 0 )
		{ writeBuffer ( "You do not page long messages.\n\r", ch ); }
		else {
			snprintf ( buf, sizeof ( buf ), "You currently display %d lines per page.\n\r",
					   ch->lines + 2 );
			writeBuffer ( buf, ch );
		}
		return;
	}

	if ( !is_number ( arg ) ) {
		writeBuffer ( "You must provide a number.\n\r", ch );
		return;
	}

	lines = atoi ( arg );

	if ( lines == 0 ) {
		writeBuffer ( "Paging disabled.\n\r", ch );
		ch->lines = 0;
		return;
	}

	if ( lines < 10 || lines > 100 ) {
		writeBuffer ( "You must provide a reasonable number.\n\r", ch );
		return;
	}

	snprintf ( buf, sizeof ( buf ), "Scroll set to %d lines.\n\r", lines );
	writeBuffer ( buf, ch );
	ch->lines = lines - 2;
}

/* RT does socials */
DefineCommand ( cmd_socials )
{
	char buf[MAX_STRING_LENGTH];
	int iSocial;
	int col;

	col = 0;

	for ( iSocial = 0; !IS_NULLSTR ( social_table[iSocial].name ); iSocial++ ) {
		snprintf ( buf, sizeof ( buf ), "%-12s", social_table[iSocial].name );
		writeBuffer ( buf, ch );
		if ( ++col % 6 == 0 )
		{ writeBuffer ( "\n\r", ch ); }
	}

	if ( col % 6 != 0 )
	{ writeBuffer ( "\n\r", ch ); }
	return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

DefineCommand ( cmd_motd )
{
	cmd_function ( ch, &cmd_help, "motd" );
}

DefineCommand ( cmd_imotd )
{
	cmd_function ( ch, &cmd_help, "imotd" );
}

DefineCommand ( cmd_rules )
{
	cmd_function ( ch, &cmd_help, "rules" );
}

DefineCommand ( cmd_story )
{
	cmd_function ( ch, &cmd_help, "story" );
}

DefineCommand ( cmd_config )
{
	if ( IS_NPC ( ch ) ) { return; }

	if ( ch->queries.queryfunc != cmd_config ) {
		ch->queries.queryfunc = cmd_config;
		cmd = 0;
	}

	switch ( cmd ) {
		default:
		case 0:
			ch->queries.queryfunc = cmd_config;
			ch->queries.querycommand = 1;
			strcpy ( ch->queries.queryprompt, "Do you want to automatically assist others? {Y/n}: " );
			break;
		case 1:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOASSIST );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOASSIST );
			}
			ch->queries.querycommand = 2;
			strcpy ( ch->queries.queryprompt, "When you look do you want to see the exits from your room? {Y/n}: " );
			break;
		case 2:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOEXIT );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOEXIT );
			}
			ch->queries.querycommand = 3;
			strcpy ( ch->queries.queryprompt, "Do you want to loot gold automatically from corpses? {Y/n}: " );
			break;
		case 3:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOGOLD );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOGOLD );
			}
			ch->queries.querycommand = 4;
			strcpy ( ch->queries.queryprompt, "Do you want to automatically loot items from corpses? {Y/n}: " );
			break;
		case 4:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOLOOT );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOLOOT );
			}
			ch->queries.querycommand = 5;
			strcpy ( ch->queries.queryprompt, "Do you want to sacrifice corpses automatically? {Y/n}: " );
			break;
		case 5:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOSAC );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOSAC );
			}
			ch->queries.querycommand = 6;
			strcpy ( ch->queries.queryprompt, "Do you want to split your earnings automatically? {Y/n}: " );
			break;
		case 6:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->act, PLR_AUTOSPLIT );
			} else {
				REMOVE_BIT ( ch->act, PLR_AUTOSPLIT );
			}
			ch->queries.querycommand = 7;
			strcpy ( ch->queries.queryprompt, "Do you want to use compact display? {Y/n}: " );
			break;
		case 7:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->comm, COMM_COMPACT );
			} else {
				REMOVE_BIT ( ch->comm, COMM_COMPACT );
			}
			ch->queries.querycommand = 8;
			strcpy ( ch->queries.queryprompt, "Do you want to see your prompt? {Y/n}: " );
			break;
		case 8:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->comm, COMM_PROMPT );
			} else {
				REMOVE_BIT ( ch->comm, COMM_PROMPT );
			}
			ch->queries.querycommand = 9;
			strcpy ( ch->queries.queryprompt, "Do you want to automatically combine items into one display? {Y/n}: " );
			break;
		case 9:
			if ( argument[0] == 'y' || argument[0] == 'Y' ) {
				SET_BIT ( ch->comm, COMM_COMBINE );
			} else {
				REMOVE_BIT ( ch->comm, COMM_COMBINE );
			}
			ch->queries.queryfunc = NULL;
			ch->queries.querycommand = 0;

			// -- display our config status automatically.
			cmd_function ( ch, &cmd_configstatus, "" );
			break;
	}

}

DefineCommand ( cmd_configstatus )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	writeBuffer ( "\r\n\r\n\acYou are configured as follows: \r\n", ch );
	writeBuffer ( Format ( "\awYou %s\aw assist players.\r\n", IS_SET ( ch->act, PLR_AUTOASSIST ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw see exits.\r\n", IS_SET ( ch->act, PLR_AUTOEXIT ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw loot gold from corpses.\r\n", IS_SET ( ch->act, PLR_AUTOGOLD ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw loot items from corpses.\r\n", IS_SET ( ch->act, PLR_AUTOLOOT ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw sacrifice corpses.\r\n", IS_SET ( ch->act, PLR_AUTOSAC ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw automatically share your split of the looted gold.\r\n", IS_SET ( ch->act, PLR_AUTOSPLIT ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw see things in compact mode.\r\n", IS_SET ( ch->comm, COMM_COMPACT ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw see your prompt.\r\n", IS_SET ( ch->comm, COMM_PROMPT ) ? "\aYWILL" : "\aRWILL NOT" ), ch );
	writeBuffer ( Format ( "\awYou %s\aw see your iventory items in combined mode.\r\n", IS_SET ( ch->comm, COMM_COMBINE ) ? "\aYWILL" : "\aRWILL NOT" ), ch );

	if ( IS_SET ( ch->act, PLR_NOFOLLOW ) )
	{ writeBuffer ( "\aRYou do not welcome followers.\n\r", ch ); }
	else
	{ writeBuffer ( "\aYYou accept followers.\n\r", ch ); }

	writeBuffer ( "\an", ch );
	return;
}


DefineCommand ( cmd_brief )
{
	if ( IS_SET ( ch->comm, COMM_BRIEF ) ) {
		writeBuffer ( "Full descriptions activated.\n\r", ch );
		REMOVE_BIT ( ch->comm, COMM_BRIEF );
	} else {
		writeBuffer ( "Short descriptions activated.\n\r", ch );
		SET_BIT ( ch->comm, COMM_BRIEF );
	}
}

DefineCommand ( cmd_show )
{
	if ( IS_SET ( ch->comm, COMM_SHOW_AFFECTS ) ) {
		writeBuffer ( "Affects will no longer be shown in score.\n\r", ch );
		REMOVE_BIT ( ch->comm, COMM_SHOW_AFFECTS );
	} else {
		writeBuffer ( "Affects will now be shown in score.\n\r", ch );
		SET_BIT ( ch->comm, COMM_SHOW_AFFECTS );
	}
}

DefineCommand ( cmd_prompt )
{
	char buf[MAX_STRING_LENGTH];

	if ( IS_NULLSTR ( argument ) ) {
		if ( IS_SET ( ch->comm, COMM_PROMPT ) ) {
			writeBuffer ( "You will no longer see prompts.\n\r", ch );
			REMOVE_BIT ( ch->comm, COMM_PROMPT );
		} else {
			writeBuffer ( "You will now see prompts.\n\r", ch );
			SET_BIT ( ch->comm, COMM_PROMPT );
		}
		return;
	}

	if ( !strcmp ( argument, "all" ) )
	{ strcpy ( buf, "<%hhp %mm %vmv> " ); }
	else {
		char *sarg = ( char * ) argument;
		if ( strlen ( sarg ) > 50 )
		{ sarg[50] = '\0'; }
		strcpy ( buf, sarg );
		smash_tilde ( buf );
		if ( str_suffix ( "%c", buf ) )
		{ strcat ( buf, " " ); }

	}

	PURGE_DATA ( ch->prompt );
	ch->prompt = assign_string ( buf );
	snprintf ( buf, sizeof ( buf ), "Prompt set to %s\n\r", ch->prompt );
	writeBuffer ( buf, ch );
	return;
}

DefineCommand ( cmd_nofollow )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( IS_SET ( ch->act, PLR_NOFOLLOW ) ) {
		writeBuffer ( "You now accept followers.\n\r", ch );
		REMOVE_BIT ( ch->act, PLR_NOFOLLOW );
	} else {
		writeBuffer ( "You no longer accept followers.\n\r", ch );
		SET_BIT ( ch->act, PLR_NOFOLLOW );
		die_follower ( ch );
	}
}

DefineCommand ( cmd_look )
{
	char buf  [MAX_STRING_LENGTH];
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Exit *pexit;
	Creature *victim;
	Item *obj;
	char *pdesc;
	int door;
	int number, count;

	if ( ch->desc == NULL )
	{ return; }

	if ( ch->position < POS_SLEEPING ) {
		writeBuffer ( "You can't see anything but stars!\n\r", ch );
		return;
	}

	if ( ch->position == POS_SLEEPING ) {
		writeBuffer ( "You can't see anything, you're sleeping!\n\r", ch );
		return;
	}

	if ( !check_blind ( ch ) )
	{ return; }

	if ( !IS_NPC ( ch )
			&&   !IS_SET ( ch->act, PLR_HOLYLIGHT )
			&&   room_is_dark ( IN_ROOM ( ch ) ) ) {
		writeBuffer ( "It is pitch black ... \n\r", ch );
		show_char_to_char ( IN_ROOM ( ch )->people, ch );
		return;
	}

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	number = number_argument ( arg1, arg3 );
	count = 0;

	if ( arg1[0] == '\0' || SameString ( arg1, "auto" ) ) {
		/* 'look' or 'look auto' */
		writeBuffer ( Format ( "\a[F303][\a[F453]%s\a[F303]]\an", IN_ROOM ( ch )->name ), ch );

		if ( ( IsStaff ( ch ) && ( IS_NPC ( ch ) || IS_SET ( ch->act, PLR_HOLYLIGHT ) ) )
				||   IS_BUILDER ( ch, IN_ROOM ( ch )->area ) ) {
			writeBuffer ( Format ( "\ac{\aBRV: \aW%d\ac}\an", IN_ROOM ( ch )->vnum ), ch );
		}

		writeBuffer ( "\n\r", ch );

		if ( arg1[0] == '\0'
				|| ( !IS_NPC ( ch ) && !IS_SET ( ch->comm, COMM_BRIEF ) ) ) {
			writeBuffer ( "  ", ch );
			writeBuffer ( Format ( "\ag%s\an", IN_ROOM ( ch )->description ? IN_ROOM ( ch )->description : "No Room Description, please report to Omega" ), ch );
		}

		if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_AUTOEXIT ) ) {
			writeBuffer ( "\n\r", ch );
			cmd_function ( ch, &cmd_exits, "auto" );
		}

		show_list_to_char ( IN_ROOM ( ch )->contents, ch, FALSE, FALSE );
		show_char_to_char ( IN_ROOM ( ch )->people,   ch );
		return;
	}

	if ( SameString ( arg1, "i" ) || SameString ( arg1, "in" )  || SameString ( arg1, "on" ) ) {
		/* 'look in' */
		if ( arg2[0] == '\0' ) {
			writeBuffer ( "Look in what?\n\r", ch );
			return;
		}

		if ( ( obj = get_obj_here ( ch, arg2 ) ) == NULL ) {
			writeBuffer ( "You do not see that here.\n\r", ch );
			return;
		}

		switch ( obj->item_type ) {
			default:
				writeBuffer ( "That is not a container.\n\r", ch );
				break;

			case ITEM_DRINK_CON:
				if ( obj->value[1] <= 0 ) {
					writeBuffer ( "It is empty.\n\r", ch );
					break;
				}

				sprintf ( buf, "It's %sfilled with  a %s liquid.\n\r",
						  obj->value[1] <     obj->value[0] / 4
						  ? "less than half-" :
						  obj->value[1] < 3 * obj->value[0] / 4
						  ? "about half-"     : "more than half-",
						  liq_table[obj->value[2]].liq_color
						);

				writeBuffer ( buf, ch );
				break;

			case ITEM_TREASURECHEST:
				if ( IS_SET ( obj->value[1], CONT_CLOSED ) ) {
					writeBuffer ( "It is closed.\n\r", ch );
					break;
				}
				act ( "$p holds:", ch, obj, NULL, TO_CHAR );
				show_list_to_char ( obj->contains, ch, TRUE, TRUE );
				break;

			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
			case ITEM_CORPSE_PC:
				if ( IS_SET ( obj->value[1], CONT_CLOSED ) ) {
					writeBuffer ( "It is closed.\n\r", ch );
					break;
				}

				act ( "$p holds:", ch, obj, NULL, TO_CHAR );
				show_list_to_char ( obj->contains, ch, TRUE, TRUE );
				break;
		}
		return;
	}

	/*  -- Future development will require better syntax for looking at/in things in an attempt
	    -- to make us look a lot less like a standard DIKU derived mud but more like our own creation.
	    if(!SameString(arg1, "at"))
	    {
		writeBuffer("Syntax: look in <obj>\r\n", ch);
		writeBuffer("        look at <obj|victim>\r\n", ch);
		return;
	    } */

	if ( ( victim = get_char_room ( ch, arg1 ) ) != NULL ) {
		show_char_to_char_1 ( victim, ch );
		return;
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ) {
		if ( can_see_obj ( ch, obj ) ) {
			/* player can see object */
			pdesc = get_extra_descr ( arg3, obj->extra_descr );
			if ( pdesc != NULL ) {
				if ( ++count == number ) {
					writeBuffer ( pdesc, ch );
					return;
				} else { continue; }
			}

			pdesc = get_extra_descr ( arg3, obj->pIndexData->extra_descr );
			if ( pdesc != NULL ) {
				if ( ++count == number ) {
					writeBuffer ( pdesc, ch );
					return;
				} else { continue; }
			}

			if ( is_name ( arg3, obj->name ) ) {
				if ( ++count == number ) {
					writeBuffer ( obj->description, ch );
					writeBuffer ( "\n\r", ch );
					return;
				}
			}
		}
	}

	for ( obj = IN_ROOM ( ch )->contents; obj != NULL; obj = obj->next_content ) {
		if ( can_see_obj ( ch, obj ) ) {
			pdesc = get_extra_descr ( arg3, obj->extra_descr );
			if ( pdesc != NULL ) {
				if ( ++count == number ) {
					writeBuffer ( pdesc, ch );
					return;
				}
			}

			pdesc = get_extra_descr ( arg3, obj->pIndexData->extra_descr );
			if ( pdesc != NULL ) {
				if ( ++count == number ) {
					writeBuffer ( pdesc, ch );
					return;
				}
			}

			if ( is_name ( arg3, obj->name ) ) {
				if ( ++count == number ) {
					writeBuffer ( obj->description, ch );
					writeBuffer ( "\n\r", ch );
					return;
				}
			}
		}
	}

	pdesc = get_extra_descr ( arg3, IN_ROOM ( ch )->extra_descr );
	if ( pdesc != NULL ) {
		if ( ++count == number ) {
			writeBuffer ( pdesc, ch );
			return;
		}
	}

	if ( count > 0 && count != number ) {
		if ( count == 1 )
		{ snprintf ( buf, sizeof ( buf ), "You only see one %s here.\n\r", arg3 ); }
		else
		{ snprintf ( buf, sizeof ( buf ), "You only see %d of those here.\n\r", count ); }

		writeBuffer ( buf, ch );
		return;
	}

	if ( SameString ( arg1, "n" ) || SameString ( arg1, "north" ) ) { door = 0; }
	else if ( SameString ( arg1, "e" ) || SameString ( arg1, "east"  ) ) { door = 1; }
	else if ( SameString ( arg1, "s" ) || SameString ( arg1, "south" ) ) { door = 2; }
	else if ( SameString ( arg1, "w" ) || SameString ( arg1, "west"  ) ) { door = 3; }
	else if ( SameString ( arg1, "u" ) || SameString ( arg1, "up"    ) ) { door = 4; }
	else if ( SameString ( arg1, "d" ) || SameString ( arg1, "down"  ) ) { door = 5; }
	else {
		writeBuffer ( "You do not see that here.\n\r", ch );
		return;
	}

	/* 'look direction' */
	if ( ( pexit = IN_ROOM ( ch )->exit[door] ) == NULL ) {
		writeBuffer ( "Nothing special there.\n\r", ch );
		return;
	}

	if ( pexit->description != NULL && pexit->description[0] != '\0' )
	{ writeBuffer ( pexit->description, ch ); }
	else
	{ writeBuffer ( "Nothing special there.\n\r", ch ); }

	if ( pexit->keyword    != NULL
			&&   pexit->keyword[0] != '\0'
			&&   pexit->keyword[0] != ' ' ) {
		if ( IS_SET ( pexit->exit_info, EX_CLOSED ) ) {
			act ( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
		} else if ( IS_SET ( pexit->exit_info, EX_ISDOOR ) ) {
			act ( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
		}
	}

	return;
}

/* RT added back for the hell of it */
DefineCommand ( cmd_read )
{
	cmd_function ( ch, &cmd_look, argument );
	tail_chain();
}

DefineCommand ( cmd_examine )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Item *obj;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Examine what?\n\r", ch );
		return;
	}

	cmd_function ( ch, &cmd_look, arg );

	if ( ( obj = get_obj_here ( ch, arg ) ) != NULL ) {
		switch ( obj->item_type ) {
			default:
				break;

			case ITEM_JUKEBOX:
				cmd_function ( ch, &cmd_play, "list" );
				break;

			case ITEM_MONEY:
				if ( obj->value[0] == 0 ) {
					if ( obj->value[1] == 0 )
					{ snprintf ( buf, sizeof ( buf ), "Odd...there's no coins in the pile.\n\r" ); }
					else if ( obj->value[1] == 1 )
					{ snprintf ( buf, sizeof ( buf ), "Wow. One gold coin.\n\r" ); }
					else
						snprintf ( buf, sizeof ( buf ), "There are %d gold coins in the pile.\n\r",
								   obj->value[1] );
				} else if ( obj->value[1] == 0 ) {
					if ( obj->value[0] == 1 )
					{ snprintf ( buf, sizeof ( buf ), "Wow. One silver coin.\n\r" ); }
					else
						snprintf ( buf, sizeof ( buf ), "There are %d silver coins in the pile.\n\r",
								   obj->value[0] );
				} else
					snprintf ( buf, sizeof ( buf ),
							   "There are %d gold and %d silver coins in the pile.\n\r",
							   obj->value[1], obj->value[0] );
				writeBuffer ( buf, ch );
				break;

			case ITEM_DRINK_CON:
			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
			case ITEM_CORPSE_PC:
				snprintf ( buf, sizeof ( buf ), "in %s", argument );
				cmd_function ( ch, &cmd_look, buf );
		}
	}

	return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
DefineCommand ( cmd_exits )
{
	extern const char * dir_name[];
	char buf[MAX_STRING_LENGTH];
	Exit *pexit;
	bool found;
	bool fAuto;
	int door;

	fAuto  = SameString ( argument, "auto" );

	if ( !check_blind ( ch ) )
	{ return; }

	if ( fAuto )
	{ snprintf ( buf, sizeof ( buf ), "[Exits:" ); }
	else if ( IsStaff ( ch ) )
	{ snprintf ( buf, sizeof ( buf ), "Obvious exits from room %d:\n\r", IN_ROOM ( ch )->vnum ); }
	else
	{ snprintf ( buf, sizeof ( buf ), "Obvious exits:\n\r" ); }

	found = FALSE;
	for ( door = 0; door <= 5; door++ ) {
		if ( ( pexit = IN_ROOM ( ch )->exit[door] ) != NULL
				&&   pexit->u1.to_room != NULL
				&&   can_see_room ( ch, pexit->u1.to_room )
				&&   !IS_SET ( pexit->exit_info, EX_CLOSED ) ) {
			found = TRUE;
			if ( fAuto ) {
				strcat ( buf, " " );
				strcat ( buf, dir_name[door] );
			} else {
				sprintf ( buf + strlen ( buf ), "%-5s - %s",
						  capitalize ( dir_name[door] ),
						  room_is_dark ( pexit->u1.to_room )
						  ?  "Too dark to tell"
						  : pexit->u1.to_room->name
						);
				if ( IsStaff ( ch ) )
					sprintf ( buf + strlen ( buf ),
							  " (room %d)\n\r", pexit->u1.to_room->vnum );
				else
				{ sprintf ( buf + strlen ( buf ), "\n\r" ); }
			}
		}
	}

	if ( !found )
	{ strcat ( buf, fAuto ? " none" : "None.\n\r" ); }

	if ( fAuto )
	{ strcat ( buf, "]\n\r" ); }

	writeBuffer ( buf, ch );
	return;
}

DefineCommand ( cmd_worth )
{
	if ( IS_NPC ( ch ) ) {
		writeBuffer ( Format ( "You have %ld gold and %ld silver.\n\r",
							   ch->gold, ch->silver ), ch );
		return;
	}

	writeBuffer ( Format (
					  "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
					  ch->gold, ch->silver, ch->exp,
					  ( ch->level + 1 ) * exp_per_level ( ch, ch->pcdata->points ) - ch->exp ), ch );


	return;
}
char *outputStat ( Creature *ch, int lstat )
{
	std::string output ( "" );
	int statcount = 1;

	int top_stat = get_curr_stat ( ch, lstat );
	while ( statcount <= 25 ) {
		if ( statcount <= top_stat )
		{ output.append ( "\a[U9675/*]" ); }
		else
		{ output.append ( "\a[U9679/-]" ); }
		statcount++;
	}

	// -- return a char * instead of a const char *.
	return ( char * ) output.c_str();
}
DefineCommand ( cmd_score )
{
	int align = ch->alignment;

	if ( IS_NPC ( ch ) ) {
		writeBuffer ( "NPC's cannot see their sheet!\r\n", ch );
		return;
	}

	std::string buffer ( "" );
	// -- align colour will change the colour of various variables later on.
	char alignColour[10] = {'\0'};
	snprintf ( alignColour, sizeof ( alignColour ), "%s", align > 0 ? "\a[F154]" : "\a[F200]" );

	buffer.append ( Format ( "                                         %s_______________\r\n", alignColour ) );
	buffer.append ( Format ( "  \aC%8s %8s %8s            \a[F232]/\aY%-15s\a[F232]\\\r\n",
							 ch->pcdata->condition[COND_DRUNK]   > 10 ? "Drunk" : "",
							 ch->pcdata->condition[COND_THIRST] ==  0 ? "Thirsty" : "",
							 ch->pcdata->condition[COND_HUNGER]   ==  0 ? "Hungry" : "",
							 ch->name ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------------\a[F111]+ %-15s|\r\n", alignColour, " " ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------\a[F043]< \a[F210]About \a[F043]>%s--------------\a[F111]+\r\n", alignColour, alignColour ) );
	buffer.append ( Format ( "\a[F232]|\aWAge:               \a[F332]%-3dyrs   \aWLevel:       \a[F332]%2d            \a[F232]|\r\n", get_age ( ch ), ch->level ) );
	buffer.append ( Format ( "\a[F232]|\aWHours:           \a[F332]%-5d      \aWExp:      \a[F332]%5d            \a[F232]|\r\n", ( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600, ch->exp ) );
	buffer.append ( Format ( "\a[F232]|\aWRace:      \a[F332]%10s       \aWTNL:      \a[F332]%5d            \a[F232]|\r\n", race_table[ch->race].name, ( ( ch->level * 200 ) - ch->exp ) ) );
	buffer.append ( Format ( "\a[F232]|\aWArchetype: \a[F332]%10s       \aWCoward:   \a[F332]%5d            \a[F232]|\r\n", archetype_table[ch->archetype].name, ch->wimpy ) );
	buffer.append ( Format ( "\a[F232]|\aWGender:    \a[F332]%10s       \aWPosition: \a[F332]%10s       \a[F232]|\r\n", ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female", position_table[ch->position].name ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------\a[F043]<\a[F210]Statistics\a[F043]>%s-----------\a[F111]+\r\n", alignColour, alignColour ) );
	buffer.append ( Format ( "\a[F232]|\aWRating: \aWPierce: \a[F332]%5d \aWCrush:  \a[F332]%5d \aWHealth:  \aR%4d\aw/\aY%4d \a[F232]|\r\n", GET_AC ( ch, AC_PIERCE ), GET_AC ( ch, AC_BASH ), ch->hit, ch->max_hit ) );
	buffer.append ( Format ( "\a[F232]|        \aWSlash:  \a[F332]%5d \aWUnique: \a[F332]%5d \aWBreath:  \aR%4d\aw/\aY%4d \a[F232]|\r\n", GET_AC ( ch, AC_SLASH ), GET_AC ( ch, AC_EXOTIC ), ch->move, ch->max_move ) );
	buffer.append ( Format ( "\a[F232]|        \aWTo Hit: \a[F332]%5d \aWTo Dam: \a[F332]%5d \aWMagic:   \aR%4d\aw/\aY%4d \a[F232]|\r\n", GET_HITROLL ( ch ), GET_DAMROLL ( ch ), ch->mana, ch->max_mana ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------\a[F043]<\a[F210]Attributes\a[F043]>%s-----------\a[F111]+\r\n", alignColour, alignColour ) );
	buffer.append ( Format ( "\a[F232]|\aWST\a[F330]%25s \aWDE\a[F330]%25s\a[F232]|\r\n", outputStat ( ch, STAT_STR ), outputStat ( ch, STAT_DEX ) ) );
	buffer.append ( Format ( "\a[F232]|\aWIN\a[F330]%25s \aWWI\a[F330]%25s\a[F232]|\r\n", outputStat ( ch, STAT_INT ), outputStat ( ch, STAT_WIS ) ) );
	buffer.append ( Format ( "\a[F232]|\aWCO\a[F330]%25s   %25s\a[F232]|\r\n", outputStat ( ch, STAT_CON ), "" ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------\a[F043]<\a[F210]Loot\a[F043]>%s-----------------\a[F111]+\r\n", alignColour, alignColour ) );
	buffer.append ( Format ( "\a[F232]|\aWGold: \a[F332]%5d \aWSilver: \a[F332]%5d    \aWCarrying: \a[F332]%5d Items     \a[F232]|\r\n", ch->gold, ch->silver, ch->carry_number ) );
	buffer.append ( Format ( "\a[F232]|\aWWeighing: \a[F332]%5dlbs                                     \a[F232]|\r\n", get_carry_weight ( ch ) / 10 ) );
	buffer.append ( Format ( "\a[F111]+%s--------------------------------\a[F043]<\a[F210]==========\a[F043]>%s-----------\a[F111]+\r\n\an", alignColour, alignColour ) );
	writeBuffer ( C_STR ( buffer ), ch );

	/*
	 	-- practice and trains are being stripped out of the mud, skill points will be added
		   shortly to LEARN skills, only actual usage of skills will cause them to improve.
		"You have %d practices and %d training sessions.\n\r",  ch->practice, ch->train );
	*/

	// -- this will be removed completely and entirely re-done to be its own unique
	// -- and well thought out command, full of details and vital data.
	if ( IS_SET ( ch->comm, COMM_SHOW_AFFECTS ) )
	{ cmd_function ( ch, &cmd_affects, "" ); }
}

DefineCommand ( cmd_affects )
{
	Affect *paf, *paf_last = NULL;
	char buf[MAX_STRING_LENGTH];

	if ( ch->affected != NULL ) {
		writeBuffer ( "You are affected by the following spells:\n\r", ch );
		for ( paf = ch->affected; paf != NULL; paf = paf->next ) {
			if ( paf_last != NULL && paf->type == paf_last->type ) {
				if ( ch->level >= 20 )
				{ sprintf ( buf, "                      " ); }
				else
				{ continue; }
			} else
			{ sprintf ( buf, "Spell: %-15s", skill_table[paf->type].name ); }

			writeBuffer ( buf, ch );

			if ( ch->level >= 20 ) {
				sprintf ( buf,
						  ": modifies %s by %d ",
						  affect_loc_name ( paf->location ),
						  paf->modifier );
				writeBuffer ( buf, ch );
				if ( paf->duration == -1 )
				{ sprintf ( buf, "permanently" ); }
				else
				{ sprintf ( buf, "for %d hours", paf->duration ); }
				writeBuffer ( buf, ch );
			}

			writeBuffer ( "\n\r", ch );
			paf_last = paf;
		}
	} else
	{ writeBuffer ( "You are not affected by any spells.\n\r", ch ); }

	return;
}



const char *	day_name	[] = {
	"the Moon", "the Bull", "Deception", "Thunder", "Freedom",
	"the Great Gods", "the Sun"
};

const char *	month_name	[] = {
	"Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
	"the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
	"the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
	"the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

DefineCommand ( cmd_time )
{
	extern char str_boot_time[];
	char buf[MAX_STRING_LENGTH];
	const char *suf;
	int day;

	day     = time_info.day + 1;

	if ( day > 4 && day <  20 ) { suf = "th"; }
	else if ( day % 10 ==  1       ) { suf = "st"; }
	else if ( day % 10 ==  2       ) { suf = "nd"; }
	else if ( day % 10 ==  3       ) { suf = "rd"; }
	else                             { suf = "th"; }

	sprintf ( buf,
			  "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
			  ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
			  time_info.hour >= 12 ? "pm" : "am",
			  day_name[day % 7],
			  day, suf,
			  month_name[time_info.month] );
	writeBuffer ( buf, ch );
	snprintf ( buf, sizeof ( buf ), "ROM started up at %s\n\rThe system time is %s.\n\r",
			   str_boot_time,
			   ( char * ) ctime ( &current_time )
			 );

	writeBuffer ( buf, ch );
	return;
}

DefineCommand ( cmd_weather )
{
	static const char *sky_look[4] = {
		"cloudless",
		"cloudy",
		"rainy",
		"lit by flashes of lightning"
	};

	if ( !IS_OUTSIDE ( ch ) ) {
		writeBuffer ( "You can't see the weather indoors.\n\r", ch );
		return;
	}

	writeBuffer ( Format ( "The sky is %s and %s.\n\r",
						   sky_look[weather_info.sky],
						   weather_info.change >= 0
						   ? "a warm southerly breeze blows"
						   : "a cold northern gust blows"
						 ), ch );
	return;
}

DefineCommand ( cmd_help )
{
	HELP_DATA *pHelp;
	BUFFER *output;
	bool found = FALSE;
	char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
	int level;

	output = new_buf();

	if ( argument[0] == '\0' )
	{ argument = "summary"; }

	/* this parts handles help a b so that it returns help 'a b' */
	argall[0] = '\0';
	while ( argument[0] != '\0' ) {
		argument = ChopC ( argument, argone );
		if ( argall[0] != '\0' )
		{ strcat ( argall, " " ); }
		strcat ( argall, argone );
	}

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ) {
		level = ( pHelp->level < 0 ) ? -1 * pHelp->level - 1 : pHelp->level;

		if ( level > get_trust ( ch ) )
		{ continue; }

		if ( is_name ( argall, pHelp->keyword ) ) {
			/* add seperator if found */
			if ( found )
				add_buf ( output,
						  "\n\r============================================================\n\r\n\r" );
			if ( pHelp->level >= 0 && !SameString ( argall, "imotd" ) ) {
				add_buf ( output, pHelp->keyword );
				add_buf ( output, "\n\r" );
			}

			/*
			 * Strip leading '.' to allow initial blanks.
			 */
			if ( pHelp->text[0] == '.' )
			{ add_buf ( output, pHelp->text + 1 ); }
			else
			{ add_buf ( output, pHelp->text ); }
			found = TRUE;
			/* small hack :) */
			if ( ch->desc != NULL && ch->desc->connected != STATE_PLAYING
					&&  		    ch->desc->connected != STATE_GEN_GROUPS )
			{ break; }
		}
	}

	if ( !found )
	{ writeBuffer ( "No help on that word.\n\r", ch ); }
	else
	{ writePage ( buf_string ( output ), ch ); }
	recycle_buf ( output );
}

DefineCommand ( cmd_colours )
{
	int r, g, b, cnt;

	cnt = r = g = b = 0;

	writeBuffer ( Format ( "                  Colours of %s\n\r\n\r", "The Infected City" ), ch );

	writeBuffer ( "Initiating Tag for colours: ^^\n\r", ch );
	writeBuffer ( "                   Example: ^^R^RThis is bright red^n^^nThis is normal\n\r", ch );
	writeBuffer ( "                       256: ^^[F211]^[F211]Some odd colour^n^^nThis is normal\n\r", ch );
	writeBuffer ( "                     Reset: ^n^^n this will normalize all colours\n\r\n\r", ch );

	// -- new feature, disable/enable colours, we like showing off a bit.
	//	writeBuffer ( "To turn colour on/off type 'colours [on|off]'\n\r\n\r" );

	writeBuffer ( "Ansi Colours: \n\r", ch );
	writeBuffer ( "^rr ^RR ^bb ^BB ^cc ^CC ^gg ^GG ^yy ^YY ^oo ^OO ^mm ^MM ^ww ^WW ^pp ^PP ^n\an\n\r", ch );

	writeBuffer ( "\n\r256 Colour Support: \n\r", ch );
	for ( r = 0; r <= 5; r++ ) {
		for ( g = 0; g <= 5; g++ ) {
			for ( b = 0; b <= 5; b++ ) {
				writeBuffer ( Format ( "\a[f%d%d%d][F%d%d%d] ", r, g, b, r, g, b ), ch );
				if ( ++cnt == 10 ) {
					cnt = 0;
					writeBuffer ( "\n\r", ch );
				}
			}
		}
	}
	writeBuffer ( "\r\n", ch );
	/*
		if ( SameString ( argument, "on" ) ) {
			cr->bools.is_colourful = true;
			cr->writeBuffer ( "Colours are now enabled!\n\r" );
			END_COMMAND;
		}

		if ( SameString ( argument, "off" ) ) {
			cr->bools.is_colourful = false;
			cr->writeBuffer ( "Colours are now disabled.\n\r" );
			END_COMMAND;
		}

		cr->writeBuffer ( "Unknown option!\n\r" ); */
	return;
}


DefineCommand ( cmd_users )
{
	if ( atoi ( GetMSSP_Players() ) >= 50 ) {
		int col, cnt, staffcnt, jumps, afk;

		bool staffFirst = false;
		bool playerFirst = false;
		std::string output ( "" );

		col = cnt = staffcnt = jumps = afk = 0;

		output.append ( "\n\r" );
		// 65 across..
		output.append ( "\a[F341]****************************************************************\n\r"  );
		Creature *c, *cn;

		for ( c = char_list; c; c = cn ) {
			Creature *u = c;
			cn = c->next;

			if ( IS_NPC ( u ) )
			{ continue; }

			if ( IS_SET ( u->sflag, CR_STAFF ) ) {
				if ( !staffFirst ) {
					staffFirst = true;
					//					 ******************************************************************
					output.append ( "\a[F341]**                                                            **\n\r" );
					output.append ( "\a[F341]**          ***                \a[F540]Staff              \a[F341] ***        \a[F341]**\n\r" );
					output.append ( "\a[F341]**          ******************************************        \a[F341]**\n\r" );
					output.append ( "\a[F341]**                                                            **\n\r" );
				}
				if ( col == 0 )
				{ 	output.append ( "\a[F341]**" ); }
				output.append ( Format ( "\a[F115]%15s%5s", u->name, IS_SET ( u->comm, COMM_AFK ) ? afk_flag : "" ) );
				staffcnt++;
				if ( IS_SET ( u->comm, COMM_AFK ) )
				{ afk++; }
				if ( ++col == 3 ) {
					col = 0;
					output.append ( "\a[F341]**\n\r" );
				}
			}
		}
		if ( col > 0 ) {
			switch ( col ) {
				default:
					break;
				case 1:
					output.append ( Format ( "%40s\a[F341]**\n\r", "" ) );
					break;
				case 2:
					output.append ( Format ( "%20s\a[F341]**\n\r", "" ) );
					break;
			}
		}

		col = 0;
		jumps = 0;

		for ( c = char_list; c; c = cn ) {
			Creature *u = c;
			cn = c->next;

			if ( IS_NPC ( u ) )
			{ continue; }

			if ( IS_SET ( u->sflag, CR_STAFF ) )
			{ continue; }
			if ( !playerFirst ) {
				playerFirst = true;
				output.append ( "\a[F341]**                                                            **\n\r" );
				output.append ( "\a[F341]**          ***               \a[F540]Players              \a[F341]***        \a[F341]**\n\r" );
				output.append ( "\a[F341]**          ******************************************        \a[F341]**\n\r" );
				output.append ( "\a[F341]**                                                            **\n\r" );
			}
			cnt++;
			if ( col == 0 )
			{ 	output.append ( "\a[F341]**" ); }

			output.append ( Format ( "\a[F115]%15s%5s", u->name, IS_SET ( u->comm, COMM_AFK ) ? afk_flag : "" ) );
			if ( IS_SET ( u->comm, COMM_AFK ) )
			{ afk++; }
			if ( ++col == 3 ) {
				col = 0;
				output.append ( "\a[F341]**\n\r" );
			}
		}
		if ( col > 0 ) {
			switch ( col ) {
				default:
					break;
				case 1:
					output.append ( Format ( "%40s\a[F431]**\n\r", "" ) );
					break;
				case 2:
					output.append ( Format ( "%20s\a[F431]**\n\r", "" ) );
					break;
			}
		}

		output.append ( "\a[F341]**                                                            **\n\r" );
		output.append ( "\a[F341]****************************************************************\n\r" );
		output.append ( Format ( "\a[F301]There is currently \aR%d\a[F115] staff member(s) \a[F301]and \aR%d\a[F115] player(s)\a[F301] on %s\an.\n\r", staffcnt, cnt, "The Infected City" ) );

		if ( afk > 0 ) {
			output.append ( Format ( "\a[F301]Out of the \a[F115]%d \a[F301]users online, \a[F115]%d \a[F301]are AFK.\an\n\r", ( staffcnt + cnt ), afk ) );
		}

		writePage ( output.c_str(), ch );
		return;
	}

	writeBuffer ( "\n\r\n\r\a[F532]", ch );
	writeBuffer ( "                        _______ __          \n\r", ch );
	writeBuffer ( "                       |_     _|  |--.-----.   \n\r", ch );
	writeBuffer ( "                         |   | |     |  -__|   \n\r", ch );
	writeBuffer ( "                         |___| |__|__|_____|   \n\r", ch );
	writeBuffer ( " _______        ___            __            __      ______ __ __         \n\r", ch );
	writeBuffer ( "|_     _.-----.'  _.-----.----|  |_.-----.--|  |    |      |__|  |_.--.--.\n\r", ch );
	writeBuffer ( " _|   |_|     |   _|  -__|  __|   _|  -__|  _  |    |   ---|  |   _|  |  |\n\r", ch );
	writeBuffer ( "|_______|__|__|__| |_____|____|____|_____|_____|    |______|__|____|___  |\n\r", ch );
	writeBuffer ( "                                                                   |_____|\n\r\n\r^n", ch );

	int staff, players, ste, str, agi, inte;

	staff = players = ste = str = agi = inte = 0;

	writeBuffer ( "LV(Level)  ST(Survival Type) S(Sex)\n\r", ch );
	writeBuffer ( Format ( " LV   ST    S  %13s Status........\n\r", "Name" ), ch );
	writeBuffer ( "------------------------------------------------------------------------------\n\r", ch );
	Creature *c, *cn;

	for ( c = char_list; c; c = cn ) {
		cn = c->next;

		// -- reset every iteration.
		std::string flag_string ( "" );
		flag_string.clear();

		if ( c->level == 0 ) { continue; }

		if ( IS_NPC ( c ) ) { continue; }
		if ( IS_SET ( c->comm, COMM_AFK ) ) { flag_string.append ( " ^g{^YAFK^g}" ); }
		if ( c->fighting != NULL )   { flag_string.append ( " \a[F112]{\a[F500]COMBAT\a[F112]}" ); }
		if ( c->desc && c->desc->editor ) { flag_string.append ( " \ac[\aoOLC\ac]" ); }
		if ( IS_HERO ( c ) && c->level != MAX_LEVEL ) { flag_string.append ( " \ar**\a[F210]H\a[F211]er\a[F210]o\ar**" ); }
		if ( IS_HERO ( c ) && c->level == MAX_LEVEL ) { flag_string.append ( " \ar-\aR=\a[F211]{\a[F111]L\arE\aRGE\arN\a[F111]D\a[F211]}\aR=\ar- " ); }
		if ( IsStaff ( c ) ) { flag_string.append ( " \ac{ \a[F355]S\a[F211]TAF\a[F355]F \ac}" ); }
		std::string st ( "ALL" );
		ste++;

		/* Disabled until completely ported from InfectEngineV1
				switch ( c->integers.survival_type ) {
					default:
						st = "";
						break;
					case SURVIVAL_STEALTH:
						st = "STE";
						ste++;
						break;
					case SURVIVAL_STRENGTH:
						st = "STR";
						str++;
						break;
					case SURVIVAL_AGILITY:
						st = "AGI";
						agi++;
						break;
					case SURVIVAL_INTELLIGENCE:
						st = "INT";
						inte++;
						break;
				} */

		writeBuffer ( Format ( "\a[F333][\a[F451]%2d\a[F333]] \a[F333][\a[F451]%3s\a[F333]] \a[F333][\a[F451]%s\a[F333]] \a[F351]%13s %s^n\n\r", c->level, st.c_str(), c->sex == SEX_MALE ? "M" : "F",  c->name, flag_string.c_str() ), ch );
		if ( IS_SET ( c->sflag, CR_STAFF ) ) {
			staff++;
		} else {
			players++;
		}
	}

	writeBuffer ( Format ( "\n\r\a[F333][\a[F451]%d\a[F333]] Total Players \a[F333][\a[F451]%d\a[F333]] Total Staff \a[F333][\a[F451]%d\a[F333]]STE \a[F333][\a[F451]%d\a[F333]]STR \a[F333][\a[F451]%d\a[F333]]AGI \a[F333][\a[F451]%d\a[F333]]INT\an\n\r", players, staff, ste, str, agi, inte ), ch );

	/*
		if ( System.pDoubleExp )
		{ cr->writeBuffer ( "\a[F322]The Infected City is currently giving out double experience!^n\n\r" ); }
		if ( System.pDoubleDamage )
		{ cr->writeBuffer ( "\a[F322]The Infected City is currently giving out double damage!^n\n\r" ); }
		if ( System.pQuadDamage )
		{ cr->writeBuffer ( "\a[F322]The Infected City is currently giving out quad damage!^n\n\r" ); }
		if ( System.pFreeMove )
		{ cr->writeBuffer ( "\a[F322]The Infected City is currently giving out free movement!^n\n\r" ); }
		if ( System.pQuickMove )
		{ cr->writeBuffer ( "\a[F322]The Infected City is currently giving out quick movement!^n\n\r" ); }


		if ( System.pDayMsg ) {
			cr->writeBuffer ( Format ( "^Y%s^n\n\r", System.pDayMsg ) );
		} */
	return;
}

DefineCommand ( cmd_count )
{
	int count;
	Socket *d;
	char buf[MAX_STRING_LENGTH];

	count = 0;

	for ( d = socket_list; d != NULL; d = d->next )
		if ( d->connected == STATE_PLAYING && can_see ( ch, d->character ) )
		{ count++; }

	max_on = UMAX ( count, max_on );

	if ( max_on == count )
		snprintf ( buf, sizeof ( buf ), "There are %d characters on, the most so far today.\n\r",
				   count );
	else
		snprintf ( buf, sizeof ( buf ), "There are %d characters on, the most on today was %d.\n\r",
				   count, max_on );

	writeBuffer ( buf, ch );
}

DefineCommand ( cmd_inventory )
{
	writeBuffer ( "Your backpack holds the following items:\n\r", ch );
	show_list_to_char ( ch->carrying, ch, TRUE, TRUE );
	return;
}

DefineCommand ( cmd_equipment )
{
	Item *obj;
	int iWear;

	writeBuffer ( "Your currently equipped gear:\n\r", ch );
	for ( iWear = 0; iWear < MAX_WEAR; iWear++ ) {
		// -- crash protection we hope
		if ( where_name[iWear] == NULL ) { break; }

		if ( ( obj = get_eq_char ( ch, iWear ) ) == NULL ) {
			writeBuffer ( Format ( "\aW%s     \ac---\an\r\n", where_name[iWear] ), ch );
			continue;
		}

		if ( can_see_obj ( ch, obj ) ) {
			writeBuffer ( Format ( "%s\ac%s\r\n", where_name[iWear], format_obj_to_char ( obj, ch, TRUE ) ), ch );
		} else {
			writeBuffer ( Format ( "%s\acsomething.\r\n", where_name[iWear] ), ch );
		}
	}

	return;
}

DefineCommand ( cmd_compare )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Item *obj1;
	Item *obj2;
	int value1;
	int value2;
	const char *msg;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	if ( arg1[0] == '\0' ) {
		writeBuffer ( "Compare what to what?\n\r", ch );
		return;
	}

	if ( ( obj1 = get_obj_carry ( ch, arg1, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' ) {
		for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content ) {
			if ( obj2->wear_loc != WEAR_NONE
					&&  can_see_obj ( ch, obj2 )
					&&  obj1->item_type == obj2->item_type
					&&  ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) != 0 )
			{ break; }
		}

		if ( obj2 == NULL ) {
			writeBuffer ( "You aren't wearing anything comparable.\n\r", ch );
			return;
		}
	}

	else if ( ( obj2 = get_obj_carry ( ch, arg2, ch ) ) == NULL ) {
		writeBuffer ( "You do not have that item.\n\r", ch );
		return;
	}

	msg		= NULL;
	value1	= 0;
	value2	= 0;

	if ( obj1 == obj2 ) {
		msg = "You compare $p to itself.  It looks about the same.";
	} else if ( obj1->item_type != obj2->item_type ) {
		msg = "You can't compare $p and $P.";
	} else {
		switch ( obj1->item_type ) {
			default:
				msg = "You can't compare $p and $P.";
				break;

			case ITEM_ARMOR:
				value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
				value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
				break;

			case ITEM_WEAPON:
				if ( obj1->pIndexData->new_format )
				{ value1 = ( 1 + obj1->value[2] ) * obj1->value[1]; }
				else
				{ value1 = obj1->value[1] + obj1->value[2]; }

				if ( obj2->pIndexData->new_format )
				{ value2 = ( 1 + obj2->value[2] ) * obj2->value[1]; }
				else
				{ value2 = obj2->value[1] + obj2->value[2]; }
				break;
		}
	}

	if ( msg == NULL ) {
		if ( value1 == value2 ) { msg = "$p and $P look about the same."; }
		else if ( value1  > value2 ) { msg = "$p looks better than $P."; }
		else                         { msg = "$p looks worse than $P."; }
	}

	act ( msg, ch, obj1, obj2, TO_CHAR );
	return;
}

DefineCommand ( cmd_credits )
{
	cmd_function ( ch, &cmd_version, "" );
	cmd_function ( ch, &cmd_help, "diku" );
	return;
}

DefineCommand ( cmd_version )
{
	time_t lt = time ( 0 );
	struct tm *tmt_ptr = localtime ( &lt );

	writeBuffer ( Format ( "+------------------------------------------------+\n\r" ), ch );
	writeBuffer ( Format ( "| The Infected City: V%-25s  |\n\r", getVersion() ), ch );
	writeBuffer ( Format ( "| Written by: David Simmerson  (Omega)           |\n\r" ), ch );
	writeBuffer ( Format ( "| Infect Engine Copyright (c) 2013-%4d.         |\n\r", ( tmt_ptr->tm_year + 1900 ) ), ch );
	writeBuffer ( Format ( "+------------------------------------------------+\n\r" ), ch );
}

DefineCommand ( cmd_where )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	Socket *d;
	bool found;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Players near you:\n\r", ch );
		found = FALSE;
		for ( d = socket_list; d; d = d->next ) {
			if ( d->connected == STATE_PLAYING
					&& ( victim = d->character ) != NULL
					&&   !IS_NPC ( victim )
					&&   IN_ROOM ( victim ) != NULL
					&&   !IS_SET ( IN_ROOM ( victim )->room_flags, ROOM_NOWHERE )
					&&   ( is_room_owner ( ch, IN_ROOM ( victim ) )
						   ||    !room_is_private ( IN_ROOM ( victim ) ) )
					&&   IN_ROOM ( victim )->area == IN_ROOM ( ch )->area
					&&   can_see ( ch, victim ) ) {
				found = TRUE;
				sprintf ( buf, "%-28s %s\n\r",
						  victim->name, IN_ROOM ( victim )->name );
				writeBuffer ( buf, ch );
			}
		}
		if ( !found )
		{ writeBuffer ( "None\n\r", ch ); }
	} else {
		found = FALSE;
		for ( victim = char_list; victim != NULL; victim = victim->next ) {
			if ( IN_ROOM ( victim ) != NULL
					&&   IN_ROOM ( victim )->area == IN_ROOM ( ch )->area
					&&   !IS_AFFECTED ( victim, AFF_HIDE )
					&&   !IS_AFFECTED ( victim, AFF_SNEAK )
					&&   can_see ( ch, victim )
					&&   is_name ( arg, victim->name ) ) {
				found = TRUE;
				sprintf ( buf, "%-28s %s\n\r",
						  PERS ( victim, ch ), IN_ROOM ( victim )->name );
				writeBuffer ( buf, ch );
				break;
			}
		}
		if ( !found )
		{ act ( "You didn't find any $T.", ch, NULL, arg, TO_CHAR ); }
	}

	return;
}

DefineCommand ( cmd_consider )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;
	const char *msg;
	int diff;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Consider killing whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They're not here.\n\r", ch );
		return;
	}

	if ( is_safe ( ch, victim ) ) {
		writeBuffer ( "Don't even think about it.\n\r", ch );
		return;
	}

	diff = victim->level - ch->level;

	if ( diff <= -10 ) { msg = "You can kill $N naked and weaponless."; }
	else if ( diff <=  -5 ) { msg = "$N is no match for you."; }
	else if ( diff <=  -2 ) { msg = "$N looks like an easy kill."; }
	else if ( diff <=   1 ) { msg = "The perfect match!"; }
	else if ( diff <=   4 ) { msg = "$N says 'Do you feel lucky, punk?'."; }
	else if ( diff <=   9 ) { msg = "$N laughs at you mercilessly."; }
	else                    { msg = "Death will thank you for your gift."; }

	act ( msg, ch, NULL, victim, TO_CHAR );
	return;
}

void set_title ( Creature *ch, char *title )
{
	char buf[MAX_STRING_LENGTH];

	if ( IS_NPC ( ch ) ) {
		log_hd ( LOG_ERROR, "Set_title: NPC." );
		return;
	}

	if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' ) {
		buf[0] = ' ';
		strcpy ( buf + 1, title );
	} else {
		strcpy ( buf, title );
	}

	PURGE_DATA ( ch->pcdata->title );
	ch->pcdata->title = assign_string ( buf );
	return;
}

DefineCommand ( cmd_title )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Change your title to what?\n\r", ch );
		return;
	}

	char *a = ( char * ) argument;

	if ( strlen ( a ) > 45 )
	{ a[45] = '\0'; }

	smash_tilde ( a );
	set_title ( ch, a );
	writeBuffer ( "Ok.\n\r", ch );
}

DefineCommand ( cmd_description )
{
	char buf[MAX_STRING_LENGTH];

	if ( argument[0] != '\0' ) {
		buf[0] = '\0';
		smash_tilde ( argument );

		if ( argument[0] == '-' ) {
			int len;
			bool found = FALSE;

			if ( IS_NULLSTR ( ch->description ) ) {
				writeBuffer ( "No lines left to remove.\n\r", ch );
				return;
			}

			strcpy ( buf, ch->description );

			for ( len = strlen ( buf ); len > 0; len-- ) {
				if ( buf[len] == '\r' ) {
					if ( !found ) { /* back it up */
						if ( len > 0 )
						{ len--; }
						found = TRUE;
					} else { /* found the second one */
						buf[len + 1] = '\0';
						PURGE_DATA ( ch->description );
						ch->description = assign_string ( buf );
						writeBuffer ( "Your description is:\n\r", ch );
						writeBuffer ( ch->description ? ch->description :
									  "(None).\n\r", ch );
						return;
					}
				}
			}
			buf[0] = '\0';
			PURGE_DATA ( ch->description );
			ch->description = assign_string ( buf );
			writeBuffer ( "Description cleared.\n\r", ch );
			return;
		}
		if ( argument[0] == '+' ) {
			if ( ch->description != NULL )
			{ strcat ( buf, ch->description ); }
			argument++;
			while ( isspace ( *argument ) )
			{ argument++; }
		}

		if ( strlen ( buf ) >= 1024 ) {
			writeBuffer ( "Description too long.\n\r", ch );
			return;
		}

		strcat ( buf, argument );
		strcat ( buf, "\n\r" );
		PURGE_DATA ( ch->description );
		ch->description = assign_string ( buf );
	}

	writeBuffer ( "Your description is:\n\r", ch );
	writeBuffer ( ch->description ? ch->description : "(None).\n\r", ch );
	return;
}

DefineCommand ( cmd_report )
{
	char buf[MAX_INPUT_LENGTH];

	sprintf ( buf,
			  "You say 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'\n\r",
			  ch->hit,  ch->max_hit,
			  ch->mana, ch->max_mana,
			  ch->move, ch->max_move,
			  ch->exp   );

	writeBuffer ( buf, ch );

	sprintf ( buf, "$n says 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'",
			  ch->hit,  ch->max_hit,
			  ch->mana, ch->max_mana,
			  ch->move, ch->max_move,
			  ch->exp   );

	act ( buf, ch, NULL, NULL, TO_ROOM );

	return;
}

DefineCommand ( cmd_practice )
{
	char buf[MAX_STRING_LENGTH];
	int sn;

	if ( IS_NPC ( ch ) )
	{ return; }

	if ( argument[0] == '\0' ) {
		int col;

		col    = 0;
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name == NULL )
			{ break; }
			if ( ch->level < skill_table[sn].skill_level[ch->archetype]
					|| ch->pcdata->learned[sn] < 1 /* skill is not known */ )
			{ continue; }

			sprintf ( buf, "%-18s %3d%%  ",
					  skill_table[sn].name, ch->pcdata->learned[sn] );
			writeBuffer ( buf, ch );
			if ( ++col % 3 == 0 )
			{ writeBuffer ( "\n\r", ch ); }
		}

		if ( col % 3 != 0 )
		{ writeBuffer ( "\n\r", ch ); }

		sprintf ( buf, "You have %d practice sessions left.\n\r",
				  ch->practice );
		writeBuffer ( buf, ch );
	} else {
		Creature *mob;
		int adept;

		if ( !IS_AWAKE ( ch ) ) {
			writeBuffer ( "In your dreams, or what?\n\r", ch );
			return;
		}

		for ( mob = IN_ROOM ( ch )->people; mob != NULL; mob = mob->next_in_room ) {
			if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_PRACTICE ) )
			{ break; }
		}

		if ( mob == NULL ) {
			writeBuffer ( "You can't do that here.\n\r", ch );
			return;
		}

		if ( ch->practice <= 0 ) {
			writeBuffer ( "You have no practice sessions left.\n\r", ch );
			return;
		}

		if ( ( sn = find_spell ( ch, argument ) ) < 0
				|| ( !IS_NPC ( ch )
					 &&   ( ch->level < skill_table[sn].skill_level[ch->archetype]
							||    ch->pcdata->learned[sn] < 1 /* skill is not known */
							||    skill_table[sn].rating[ch->archetype] == 0 ) ) ) {
			writeBuffer ( "You can't practice that.\n\r", ch );
			return;
		}

		adept = IS_NPC ( ch ) ? 100 : archetype_table[ch->archetype].skill_adept;

		if ( ch->pcdata->learned[sn] >= adept ) {
			sprintf ( buf, "You are already learned at %s.\n\r",
					  skill_table[sn].name );
			writeBuffer ( buf, ch );
		} else {
			ch->practice--;

			// increment by ten!
			ch->pcdata->learned[sn] += 10;
			if ( ch->pcdata->learned[sn] < adept ) {
				act ( "You practice $T.",
					  ch, NULL, skill_table[sn].name, TO_CHAR );
				act ( "$n practices $T.",
					  ch, NULL, skill_table[sn].name, TO_ROOM );
			} else {
				ch->pcdata->learned[sn] = adept;
				act ( "You are now learned at $T.",
					  ch, NULL, skill_table[sn].name, TO_CHAR );
				act ( "$n is now learned at $T.",
					  ch, NULL, skill_table[sn].name, TO_ROOM );
			}
			if ( ch->pcdata->learned[sn] >= 100 ) {
				ch->pcdata->learned[sn] = 100;
				writeBuffer ( Format ( "You have mastered the skill: %s\r\n", skill_table[sn].name ), ch );
			}
		}
	}
	return;
}

/*
 * 'Wimpy' originally by Dionysos.
 */
DefineCommand ( cmd_wimpy )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int wimpy;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
	{ wimpy = ch->max_hit / 5; }
	else
	{ wimpy = atoi ( arg ); }

	if ( wimpy < 0 ) {
		writeBuffer ( "Your courage exceeds your wisdom.\n\r", ch );
		return;
	}

	if ( wimpy > ch->max_hit / 2 ) {
		writeBuffer ( "Such cowardice ill becomes you.\n\r", ch );
		return;
	}

	ch->wimpy	= wimpy;
	sprintf ( buf, "Wimpy set to %d hit points.\n\r", wimpy );
	writeBuffer ( buf, ch );
	return;
}

DefineCommand ( cmd_password )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char *pArg;
	char *pwdnew;
	char *p;
	char cEnd;

	if ( IS_NPC ( ch ) )
	{ return; }

	/*
	 * Can't use ChopC here because it smashes case.
	 * So we just steal all its code.  Bleagh.
	 */
	pArg = arg1;
	while ( isspace ( *argument ) )
	{ argument++; }

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
	{ cEnd = *argument++; }

	while ( *argument != '\0' ) {
		if ( *argument == cEnd ) {
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';

	pArg = arg2;
	while ( isspace ( *argument ) )
	{ argument++; }

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
	{ cEnd = *argument++; }

	while ( *argument != '\0' ) {
		if ( *argument == cEnd ) {
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';

	if ( arg1[0] == '\0' || arg2[0] == '\0' ) {
		writeBuffer ( "Syntax: password <old> <new>.\n\r", ch );
		return;
	}

	if ( strcmp ( crypt ( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) ) {
		WAIT_STATE ( ch, 40 );
		writeBuffer ( "Wrong password.  Wait 10 seconds.\n\r", ch );
		return;
	}

	if ( strlen ( arg2 ) < 7 ) {
		writeBuffer (
			"New password must be at least seven characters long.\n\r", ch );
		return;
	}

	if ( !strstr ( arg2, "1" ) &&
			!strstr ( arg2, "2" ) &&
			!strstr ( arg2, "3" ) &&
			!strstr ( arg2, "4" ) &&
			!strstr ( arg2, "5" ) &&
			!strstr ( arg2, "6" ) &&
			!strstr ( arg2, "7" ) &&
			!strstr ( arg2, "8" ) &&
			!strstr ( arg2, "9" ) &&
			!strstr ( arg2, "0" ) ) {
		writeBuffer ( "You must have a number in your password.\r\n", ch );
		return;
	}

	/*
	 * No tilde allowed because of player file format.
	 */
	pwdnew = crypt ( arg2, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ ) {
		if ( *p == '~' ) {
			writeBuffer (
				"New password not acceptable, try again.\n\r", ch );
			return;
		}
	}

	PURGE_DATA ( ch->pcdata->pwd );
	ch->pcdata->pwd = assign_string ( pwdnew );
	save_char_obj ( ch );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_levelup )
{
	// -- die out.
	assert ( ch );
	if ( IS_NPC ( ch ) ) { return; }
	if ( !IS_SET ( IN_ROOM ( ch )->room_flags, ROOM_SAFE ) ) {
		writeBuffer ( "You must be in a room that is safe to level up!\r\n", ch );
		return;
	}

	if ( cmd == 101 ) {
		switch ( argument[0] ) {
			default:
				ch->queries.querycommand = 0;
				break;
			case 'y':
			case 'Y':
				if ( ch->level < MAX_LEVEL && ch->exp >= ( ch->level * 200 ) ) {
					int old_level = ch->level;
					ch->level++;

					// -- announce our level gain!
					announce ( Format ( "%s has attained level %d", ch->name, ch->level ) );

					switch ( ch->level ) {
						default:
							if ( Math::instance().range ( 0, 3 ) == Math::instance().range ( 0, 7 ) ) {
								tweetStatement ( Format ( "%s has attained a new level, %d.", ch->name, ch->level ) );
							}
							break;
						case 5:
							tweetStatement ( Format ( "%s has survived the first 5 levels of %s.", ch->name, "The Infected City" ) );
							break;
						case 20:
							tweetStatement ( Format ( "%s has achieved level 20 in %s.", ch->name, "The Infected City" ) );
							break;
						case 40:
							tweetStatement ( Format ( "Level 40: %s has continued to survive in %s.", ch->name, "The Infected City" ) );
							break;
						case 50:
							tweetStatement ( Format ( "Level 50: %s is a true suvivor of %s.", ch->name, "The Infected City"  ) );
							break;
						case 51:
							tweetStatement ( Format ( "Level 51: %s is on the path to awesomeness in %s.", ch->name, "The Infected City" ) );
							break;
						case 52:
							tweetStatement ( Format ( "Level 52: %s has attained a truely amazing status in %s.", ch->name, "The Infected City" ) );
							break;
						case 53:
							tweetStatement ( Format ( "Level 53: %s has completed 53 levels of %s.", ch->name, "The Infected City" ) );
							break;
						case 54:
							tweetStatement ( Format ( "Level 54: %s has reached a powerful level of %s.", ch->name, "The Infected City" ) );
							break;
						case 55:
							tweetStatement ( Format ( "Level 55: %s has truly proven their worth in %s.", ch->name, "The Infected City" ) );
							break;
						case 56:
							tweetStatement ( Format ( "Level 56: %s survived 56 levels within %s.", ch->name, "The Infected City"  ) );
							break;
						case 57:
							tweetStatement ( Format ( "Level 57: %s endured 57 levels of %s.", ch->name, "The Infected City"  ) );
							break;
						case 58:
							tweetStatement ( Format ( "Level 58: %s has true suvivor instinct within %s.", ch->name, "The Infected City" ) );
							break;
						case 59:
							tweetStatement ( Format ( "Level 59: %s is one step away from MAX LEVEL!", ch->name ) );
							break;
						case 60:
							tweetStatement ( Format ( "*** MAX LEVEL ATTAINED! *** %s has become a master survivor in %s.", ch->name, "The Infected City"  ) );
							break;
					} // -- end switch

					wiznet ( Format ( "$N has attained level %d!", ch->level ), ch, NULL, WIZ_LEVELS, 0, 0 );
					advance_level ( ch, false );

					// -- take away the right portion of experience!
					ch->exp = ( ch->exp - ( old_level * 200 ) ) ;

					// -- for debugging purposes, we log it all!
					log_hd ( LOG_DEBUG, Format ( "%s has leveled up to level %d, %d experience remains, %d experience tnl.", ch->name, ch->level, ch->exp, ( ch->level * 200 ) ) );

					switch ( ch->level ) {
						default:
							break;

						case 15:
							// -- sub-class picking will go here.
							break;
						case 30:
							// -- sub-race picking will go here.
							break;
						case MAX_LEVEL:
							tweetStatement ( Format ( "%s has survived!", ch->name ) );
							// -- My little secret will go here!
							break;
					}
				} else {
					writeBuffer ( "Your not ready to level up yet!\r\n", ch );
				}
				break;
		} // -- end of the switch
	} // -- end if
	else {
		ch->queries.queryfunc = cmd_levelup;
		strcpy ( ch->queries.queryprompt, Format ( "\aRWarning, \acyou may be prompted for changes, make sure you are ready first.\an\r\nAre you sure you want to level up? (y/n)>" ) );
		ch->queries.querycommand = 101;
	}
	return;
}

int getApplyTotal ( Item * obj, int app_type )
{
	int to_return = 0;
	Affect * paf;

	ASSERT ( obj != NULL );

	if ( !obj->enchanted ) {
		if ( obj->pIndexData->affected != NULL ) {
			for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next ) {
				if ( paf->location != app_type )
				{ continue; }
				to_return += paf->modifier;
			}
		}
	}
	if ( obj->affected != NULL ) {
		for ( paf = obj->affected; paf != NULL; paf = paf->next ) {
			if ( paf->location != app_type )
			{ continue; }
			to_return += paf->modifier;
		}
	}

	/* possible future planning here
	if (obj->affected2 != NULL ) {
		for ( paf = obj->affected2; paf != NULL; paf = paf->next ) {
			if ( paf->location != app_type )
			{ continue; }
			to_return += paf->modifier;
		}
	} */
	return to_return;
}

DefineCommand ( cmd_about )
{
	Item *o, *on;

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: about <item name>\r\n", ch );
		return;
	}

	bool first = false;
	std::string search_for;
	ChopString ( argument, search_for );

	for ( o = CARRYING ( ch ); o; o = on ) {
		on = o->next_content;
		if ( is_name ( C_STR ( search_for ), o->name ) ) {
			writeBuffer ( Format ( "\ayAbout the Item:           \aW%s\an\r\n", o->short_descr ), ch );
			writeBuffer ( Format ( "\ayLevel:                    \aW%d\an\r\n", o->level ), ch );
			writeBuffer ( Format ( "\ayRequirements:             \acSTR\aB{\aW%d\aB} \acINT\aB{\aW%d\aB} \acCON\aB{\aW%d\aB} \acWIS\aB{\aW%d\aB} \acDEX\aB{\aW%d\aB} \acSIZE:\aB{\aW%s\aB}\an\r\n", o->requirements[STR_REQ],
								   o->requirements[INT_REQ], o->requirements[CON_REQ], o->requirements[WIS_REQ], o->requirements[DEX_REQ],
								   size_table[ ( int ) o->requirements[SIZ_REQ]].name ), ch );

			for ( int x = 1; apply_flags[x].name != NULL; x++ ) {
				// -- starts at 1 so it skips the apply none option.
				int ret = getApplyTotal ( o, apply_flags[x].bit );
				if ( ret != 0 ) {
					if ( !first ) {
						first = true;
						writeBuffer ( "\aGBenefits and Disadvantages:\an\r\n", ch );
					}
					writeBuffer ( Format ( "\t\aC%s \awwill be affected by \aO%d\an\r\n", capitalize ( apply_flags[x].name ), ret ), ch );
				}
			}
			return;
		}
	}
	writeBuffer ( "You do not seem to be carrying that item.\r\n", ch );
	return;
}

// -- EOF


