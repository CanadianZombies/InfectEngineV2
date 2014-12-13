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

/*
 * Local functions.
 */
RoomData *	find_location	args ( ( Creature *ch, const char *arg ) );

void wiznet ( const char *str, Creature *ch, Item *obj,
			  long flag, long flag_skip, int min_level )
{
	Socket *d;

	for ( d = socket_list; d != NULL; d = d->next ) {
		if ( d->connected == STATE_PLAYING
				&&  IsStaff ( d->character )
				&&  IS_SET ( d->character->wiznet, WIZ_ON )
				&&  ( !flag || IS_SET ( d->character->wiznet, flag ) )
				&&  ( !flag_skip || !IS_SET ( d->character->wiznet, flag_skip ) )
				&&  get_trust ( d->character ) >= min_level
				&&  d->character != ch ) {
			if ( IS_SET ( d->character->wiznet, WIZ_PREFIX ) )
			{ writeBuffer ( "\t", d->character ); }
			act_new ( Format ( "\ac[\aYSTAFF\ac] \aW%s\an", str ), d->character, obj, ch, TO_CHAR, POS_DEAD );
		}
	}

	return;
}

RoomData *find_location ( Creature *ch, const char *arg )
{
	Creature *victim;
	Item *obj;

	if ( is_number ( arg ) )
	{ return get_room_index ( atoi ( arg ) ); }

	if ( ( victim = get_char_world ( ch, arg ) ) != NULL )
	{ return IN_ROOM ( victim ); }

	if ( ( obj = get_obj_world ( ch, arg ) ) != NULL )
	{ return obj->in_room; }

	return NULL;
}


DefineCommand ( cmd_owhere )
{
	char buf[MAX_INPUT_LENGTH];
	BUFFER *buffer;
	Item *obj;
	Item *in_obj;
	bool found;
	int number = 0, max_found;

	found = FALSE;
	number = 0;
	max_found = 200;

	buffer = new_buf();

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Find what?\n\r", ch );
		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj->next ) {
		if ( !can_see_obj ( ch, obj ) || !is_name ( argument, obj->name )
				||   ch->level < obj->level )
		{ continue; }

		found = TRUE;
		number++;

		for ( in_obj = obj; IN_OBJ ( in_obj ) != NULL; in_obj = IN_OBJ ( in_obj ) )
			;

		if ( CARRIED_BY ( in_obj ) != NULL && can_see ( ch, CARRIED_BY ( in_obj ) )
				&&   CARRIED_BY ( in_obj )->in_room != NULL )
			sprintf ( buf, "%3d) %s is carried by %s [Room %d]\n\r",
					  number, obj->short_descr, PERS ( CARRIED_BY ( in_obj ), ch ),
					  CARRIED_BY ( in_obj )->in_room->vnum );
		else if ( in_obj->in_room != NULL && can_see_room ( ch, in_obj->in_room ) )
			sprintf ( buf, "%3d) %s is in %s [Room %d]\n\r",
					  number, obj->short_descr, in_obj->in_room->name,
					  in_obj->in_room->vnum );
		else
		{ sprintf ( buf, "%3d) %s is somewhere\n\r", number, obj->short_descr ); }

		buf[0] = UPPER ( buf[0] );
		add_buf ( buffer, buf );

		if ( number >= max_found )
		{ break; }
	}

	if ( !found )
	{ writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch ); }
	else
	{ writePage ( buf_string ( buffer ), ch ); }

	recycle_buf ( buffer );
}


DefineCommand ( cmd_mwhere )
{
	char buf[MAX_STRING_LENGTH];
	BUFFER *buffer;
	Creature *victim;
	bool found;
	int count = 0;

	if ( argument[0] == '\0' ) {
		Socket *d;

		/* show characters logged */

		buffer = new_buf();
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->character != NULL && d->connected == STATE_PLAYING
					&&  d->character->in_room != NULL && can_see ( ch, d->character )
					&&  can_see_room ( ch, d->character->in_room ) ) {
				victim = d->character;
				count++;
				if ( d->original != NULL )
					snprintf ( buf, sizeof ( buf ), "%3d) %s (in the body of %s) is in %s [%d]\n\r",
							   count, d->original->name, victim->short_descr,
							   IN_ROOM ( victim )->name, IN_ROOM ( victim )->vnum );
				else
					snprintf ( buf, sizeof ( buf ), "%3d) %s is in %s [%d]\n\r",
							   count, victim->name, IN_ROOM ( victim )->name,
							   IN_ROOM ( victim )->vnum );
				add_buf ( buffer, buf );
			}
		}

		writePage ( buf_string ( buffer ), ch );
		recycle_buf ( buffer );
		return;
	}

	found = FALSE;
	buffer = new_buf();
	for ( victim = char_list; victim != NULL; victim = victim->next ) {
		if ( IN_ROOM ( victim ) != NULL
				&&   is_name ( argument, victim->name ) ) {
			found = TRUE;
			count++;
			sprintf ( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
					  IS_NPC ( victim ) ? victim->pIndexData->vnum : 0,
					  IS_NPC ( victim ) ? victim->short_descr : victim->name,
					  IN_ROOM ( victim )->vnum,
					  IN_ROOM ( victim )->name );
			add_buf ( buffer, buf );
		}
	}

	if ( !found )
	{ act ( "You didn't find any $T.", ch, NULL, argument, TO_CHAR ); }
	else
	{ writePage ( buf_string ( buffer ), ch ); }

	recycle_buf ( buffer );

	return;
}

DefineCommand ( cmd_protect )
{
	Creature *victim;

	if ( argument[0] == '\0' ) {
		writeBuffer ( "Protect whom from snooping?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, argument ) ) == NULL ) {
		writeBuffer ( "You can't find them.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_SNOOP_PROOF ) ) {
		act_new ( "$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		writeBuffer ( "Your snoop-proofing was just removed.\n\r", victim );
		REMOVE_BIT ( victim->comm, COMM_SNOOP_PROOF );
	} else {
		act_new ( "$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		writeBuffer ( "You are now immune to snooping.\n\r", victim );
		SET_BIT ( victim->comm, COMM_SNOOP_PROOF );
	}
}



DefineCommand ( cmd_snoop )
{
	char arg[MAX_INPUT_LENGTH];
	Socket *d;
	Creature *victim;
	char buf[MAX_STRING_LENGTH];

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Snoop whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL ) {
		writeBuffer ( "No descriptor to snoop.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Cancelling all snoops.\n\r", ch );
		wiznet ( "$N stops being such a snoop.",
				 ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust ( ch ) );
		for ( d = socket_list; d != NULL; d = d->next ) {
			if ( d->snoop_by == ch->desc )
			{ d->snoop_by = NULL; }
		}
		return;
	}

	if ( victim->desc->snoop_by != NULL ) {
		writeBuffer ( "Busy already.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, IN_ROOM ( victim ) ) && IN_ROOM ( ch ) != IN_ROOM ( victim )
			&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch )
			||   IS_SET ( victim->comm, COMM_SNOOP_PROOF ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( ch->desc != NULL ) {
		for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by ) {
			if ( d->character == victim || d->original == victim ) {
				writeBuffer ( "No snoop loops.\n\r", ch );
				return;
			}
		}
	}

	victim->desc->snoop_by = ch->desc;
	snprintf ( buf, sizeof ( buf ), "$N starts snooping on %s",
			   ( IS_NPC ( ch ) ? victim->short_descr : victim->name ) );
	wiznet ( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}



DefineCommand ( cmd_switch )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Switch into whom?\n\r", ch );
		return;
	}

	if ( ch->desc == NULL )
	{ return; }

	if ( ch->desc->original != NULL ) {
		writeBuffer ( "You are already switched.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch ) {
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) ) {
		writeBuffer ( "You can only switch into mobiles.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, IN_ROOM ( victim ) ) && IN_ROOM ( ch ) != IN_ROOM ( victim )
			&&  room_is_private ( IN_ROOM ( victim ) ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That character is in a private room.\n\r", ch );
		return;
	}

	if ( victim->desc != NULL ) {
		writeBuffer ( "Character in use.\n\r", ch );
		return;
	}

	snprintf ( buf, sizeof ( buf ), "$N switches into %s", victim->short_descr );
	wiznet ( buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust ( ch ) );

	ch->desc->character = victim;
	ch->desc->original  = ch;
	victim->desc        = ch->desc;
	ch->desc            = NULL;
	/* change communications to match */
	if ( ch->prompt != NULL )
	{ victim->prompt = assign_string ( ch->prompt ); }
	victim->comm = ch->comm;
	victim->lines = ch->lines;
	writeBuffer ( "Ok.\n\r", victim );
	return;
}



DefineCommand ( cmd_return )
{
	char buf[MAX_STRING_LENGTH];

	if ( ch->desc == NULL )
	{ return; }

	if ( ch->desc->original == NULL ) {
		writeBuffer ( "You aren't switched.\n\r", ch );
		return;
	}

	writeBuffer (
		"You return to your original body. Type replay to see any missed tells.\n\r",
		ch );
	if ( ch->prompt != NULL ) {
		PURGE_DATA ( ch->prompt );
		ch->prompt = NULL;
	}

	snprintf ( buf, sizeof ( buf ), "$N returns from %s.", ch->short_descr );
	wiznet ( buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust ( ch ) );
	ch->desc->character       = ch->desc->original;
	ch->desc->original        = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc                  = NULL;
	return;
}

/* trust levels for load and clone */
bool obj_check ( Creature *ch, Item *obj )
{
	if ( IS_TRUSTED ( ch, GOD )
			|| ( IS_TRUSTED ( ch, IMMORTAL ) && obj->level <= 20 && obj->cost <= 1000 )
			|| ( IS_TRUSTED ( ch, DEMI )	    && obj->level <= 10 && obj->cost <= 500 )
			|| ( IS_TRUSTED ( ch, ANGEL )    && obj->level <=  5 && obj->cost <= 250 )
			|| ( IS_TRUSTED ( ch, AVATAR )   && obj->level ==  0 && obj->cost <= 100 ) )
	{ return TRUE; }
	else
	{ return FALSE; }
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone ( Creature *ch, Item *obj, Item *clone )
{
	Item *c_obj, *t_obj;


	for ( c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content ) {
		if ( obj_check ( ch, c_obj ) ) {
			t_obj = create_object ( c_obj->pIndexData, 0 );
			clone_object ( c_obj, t_obj );
			obj_to_obj ( t_obj, clone );
			recursive_clone ( ch, c_obj, t_obj );
		}
	}
}

/* command that is similar to load */
DefineCommand ( cmd_clone )
{
	char arg[MAX_INPUT_LENGTH];
	char *rest;
	Creature *mob;
	Item  *obj;

	rest = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Clone what?\n\r", ch );
		return;
	}

	if ( !str_prefix ( arg, "object" ) ) {
		mob = NULL;
		obj = get_obj_here ( ch, rest );
		if ( obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else if ( !str_prefix ( arg, "mobile" ) || !str_prefix ( arg, "character" ) ) {
		obj = NULL;
		mob = get_char_room ( ch, rest );
		if ( mob == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	} else { /* find both */
		mob = get_char_room ( ch, argument );
		obj = get_obj_here ( ch, argument );
		if ( mob == NULL && obj == NULL ) {
			writeBuffer ( "You don't see that here.\n\r", ch );
			return;
		}
	}

	/* clone an object */
	if ( obj != NULL ) {
		Item *clone;

		if ( !obj_check ( ch, obj ) ) {
			writeBuffer (
				"Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_object ( obj->pIndexData, 0 );
		clone_object ( obj, clone );
		if ( CARRIED_BY ( obj ) != NULL )
		{ obj_to_char ( clone, ch ); }
		else
		{ obj_to_room ( clone, IN_ROOM ( ch ) ); }
		recursive_clone ( ch, obj, clone );

		act ( "$n has created $p.", ch, clone, NULL, TO_ROOM );
		act ( "You clone $p.", ch, clone, NULL, TO_CHAR );
		wiznet ( "$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
		return;
	} else if ( mob != NULL ) {
		Creature *clone;
		Item *new_obj;
		char buf[MAX_STRING_LENGTH];

		if ( !IS_NPC ( mob ) ) {
			writeBuffer ( "You can only clone mobiles.\n\r", ch );
			return;
		}

		if ( ( mob->level > 20 && !IS_TRUSTED ( ch, GOD ) )
				||  ( mob->level > 10 && !IS_TRUSTED ( ch, IMMORTAL ) )
				||  ( mob->level >  5 && !IS_TRUSTED ( ch, DEMI ) )
				||  ( mob->level >  0 && !IS_TRUSTED ( ch, ANGEL ) )
				||  !IS_TRUSTED ( ch, AVATAR ) ) {
			writeBuffer (
				"Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_mobile ( mob->pIndexData );
		clone_mobile ( mob, clone );

		for ( obj = mob->carrying; obj != NULL; obj = obj->next_content ) {
			if ( obj_check ( ch, obj ) ) {
				new_obj = create_object ( obj->pIndexData, 0 );
				clone_object ( obj, new_obj );
				recursive_clone ( ch, obj, new_obj );
				obj_to_char ( new_obj, clone );
				new_obj->wear_loc = obj->wear_loc;
			}
		}
		char_to_room ( clone, IN_ROOM ( ch ) );
		act ( "$n has created $N.", ch, NULL, clone, TO_ROOM );
		act ( "You clone $N.", ch, NULL, clone, TO_CHAR );
		snprintf ( buf, sizeof ( buf ), "$N clones %s.", clone->short_descr );
		wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
		return;
	}
}

/* RT to replace the two load commands */
DefineCommand ( cmd_load )
{
	char arg[MAX_INPUT_LENGTH];

	argument = ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  load mob <vnum>\n\r", ch );
		writeBuffer ( "  load obj <vnum> <level>\n\r", ch );
		return;
	}

	if ( SameString ( arg, "mob" ) || SameString ( arg, "char" ) ) {
		cmd_function ( ch, &cmd_mload, argument );
		return;
	}

	if ( SameString ( arg, "obj" ) ) {
		cmd_function ( ch, &cmd_oload, argument );
		return;
	}
	/* echo syntax */
	cmd_function ( ch, &cmd_load, "" );
}


DefineCommand ( cmd_mload )
{
	char arg[MAX_INPUT_LENGTH];
	NPCData *pMobIndex;
	Creature *victim;
	char buf[MAX_STRING_LENGTH];

	ChopC ( argument, arg );

	if ( arg[0] == '\0' || !is_number ( arg ) ) {
		writeBuffer ( "Syntax: load mob <vnum>.\n\r", ch );
		return;
	}

	if ( ( pMobIndex = get_mob_index ( atoi ( arg ) ) ) == NULL ) {
		writeBuffer ( "No mob has that vnum.\n\r", ch );
		return;
	}

	victim = create_mobile ( pMobIndex );
	char_to_room ( victim, IN_ROOM ( ch ) );
	act ( "$n has created $N!", ch, NULL, victim, TO_ROOM );
	snprintf ( buf, sizeof ( buf ), "$N loads %s.", victim->short_descr );
	wiznet ( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}



DefineCommand ( cmd_oload )
{
	char arg1[MAX_INPUT_LENGTH] , arg2[MAX_INPUT_LENGTH];
	ItemData *pObjIndex;
	Item *obj;
	int level;

	argument = ChopC ( argument, arg1 );
	ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number ( arg1 ) ) {
		writeBuffer ( "Syntax: load obj <vnum> <level>.\n\r", ch );
		return;
	}

	level = get_trust ( ch ); /* default */

	if ( arg2[0] != '\0' ) { /* load with a level */
		if ( !is_number ( arg2 ) ) {
			writeBuffer ( "Syntax: oload <vnum> <level>.\n\r", ch );
			return;
		}
		level = atoi ( arg2 );
		if ( level < 0 || level > get_trust ( ch ) ) {
			writeBuffer ( "Level must be be between 0 and your level.\n\r", ch );
			return;
		}
	}

	if ( ( pObjIndex = get_obj_index ( atoi ( arg1 ) ) ) == NULL ) {
		writeBuffer ( "No object has that vnum.\n\r", ch );
		return;
	}

	obj = create_object ( pObjIndex, level );
	if ( CAN_WEAR ( obj, ITEM_TAKE ) )
	{ obj_to_char ( obj, ch ); }
	else
	{ obj_to_room ( obj, IN_ROOM ( ch ) ); }
	act ( "$n has created $p!", ch, obj, NULL, TO_ROOM );
	wiznet ( "$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust ( ch ) );
	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_purge )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[100];
	Creature *victim;
	Item *obj;
	Socket *d;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		/* 'purge' */
		Creature *vnext;
		Item  *obj_next;

		for ( victim = IN_ROOM ( ch )->people; victim != NULL; victim = vnext ) {
			vnext = victim->next_in_room;
			if ( IS_NPC ( victim ) && !IS_SET ( victim->act, ACT_NOPURGE )
					&&   victim != ch /* safety precaution */ )
			{ extract_char ( victim, TRUE ); }
		}

		for ( obj = IN_ROOM ( ch )->contents; obj != NULL; obj = obj_next ) {
			obj_next = obj->next_content;
			if ( !IS_OBJ_STAT ( obj, ITEM_NOPURGE ) )
			{ extract_obj ( obj ); }
		}

		act ( "$n purges the room!", ch, NULL, NULL, TO_ROOM );
		writeBuffer ( "Ok.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( !IS_NPC ( victim ) ) {

		if ( ch == victim ) {
			writeBuffer ( "Ho ho ho.\n\r", ch );
			return;
		}

		if ( get_trust ( ch ) <= get_trust ( victim ) ) {
			writeBuffer ( "Maybe that wasn't a good idea...\n\r", ch );
			snprintf ( buf, sizeof ( buf ), "%s tried to purge you!\n\r", ch->name );
			writeBuffer ( buf, victim );
			return;
		}

		act ( "$n disintegrates $N.", ch, 0, victim, TO_NOTVICT );

		if ( victim->level > 1 )
		{ save_char_obj ( victim ); }
		d = victim->desc;
		extract_char ( victim, TRUE );
		if ( d != NULL )
		{ close_socket ( d ); }

		return;
	}

	act ( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
	extract_char ( victim, TRUE );
	return;
}



DefineCommand ( cmd_advance )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	Creature *victim;
	int level;
	int iLevel;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) ) {
		writeBuffer ( "Syntax: levelset <player> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "That player is not here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( ( level = atoi ( arg2 ) ) < 1 || level > MAX_LEVEL ) {
		writeBuffer ( Format ( "Level must be 1 to %d.\n\r", MAX_LEVEL ), ch );
		return;
	}

	if ( level > get_trust ( ch ) ) {
		writeBuffer ( "Limited to your trust level.\n\r", ch );
		return;
	}

	/*
	 * Lower level:
	 *   Reset to level 1.
	 *   Then raise again.
	 *   Currently, an imp can lower another imp.
	 *   -- Swiftest
	 */
	if ( level <= victim->level ) {
		int temp_prac;

		writeBuffer ( "Lowering a player's level!\n\r", ch );
		writeBuffer ( "Your level has been changed.\n\r", victim );
		temp_prac = victim->practice;
		victim->level    = 1;
		victim->exp      = exp_per_level ( victim, victim->pcdata->points );
		victim->max_hit  = 10;
		victim->max_mana = 100;
		victim->max_move = 100;
		victim->practice = 0;
		victim->hit      = victim->max_hit;
		victim->mana     = victim->max_mana;
		victim->move     = victim->max_move;
		advance_level ( victim, TRUE );
		victim->practice = temp_prac;
	} else {
		writeBuffer ( "Raising a player's level!\n\r", ch );
		writeBuffer ( "Your level has been changed\n\r", victim );
	}

	for ( iLevel = victim->level ; iLevel < level; iLevel++ ) {
		victim->level += 1;
		advance_level ( victim, TRUE );
	}
	writeBuffer ( Format ( "You are now level %d.\n\r", victim->level ), victim );
	victim->exp   = exp_per_level ( victim, victim->pcdata->points )
					* UMAX ( 1, victim->level );
	victim->trust = 0;
	save_char_obj ( victim );
	return;
}

DefineCommand ( cmd_trust )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	Creature *victim;
	int level;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number ( arg2 ) ) {
		writeBuffer ( "Syntax: trust <char> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "That player is not here.\n\r", ch );
		return;
	}

	if ( ( level = atoi ( arg2 ) ) < 0 || level > MAX_LEVEL ) {
		snprintf ( buf, sizeof ( buf ), "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL );
		writeBuffer ( buf, ch );
		return;
	}

	if ( level > get_trust ( ch ) ) {
		writeBuffer ( "Limited to your trust.\n\r", ch );
		return;
	}

	victim->trust = level;
	return;
}

DefineCommand ( cmd_freeze )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Freeze whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->act, PLR_FREEZE ) ) {
		REMOVE_BIT ( victim->act, PLR_FREEZE );
		writeBuffer ( "You can play again.\n\r", victim );
		writeBuffer ( "FREEZE removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N thaws %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->act, PLR_FREEZE );
		writeBuffer ( "You can't do ANYthing!\n\r", victim );
		writeBuffer ( "FREEZE set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N puts %s in the deep freeze.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	save_char_obj ( victim );

	return;
}



DefineCommand ( cmd_log )
{
	char arg[MAX_INPUT_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Log whom?\n\r", ch );
		return;
	}

	if ( SameString ( arg, "all" ) ) {
		if ( fLogAll ) {
			fLogAll = FALSE;
			writeBuffer ( "Log ALL off.\n\r", ch );
		} else {
			fLogAll = TRUE;
			writeBuffer ( "Log ALL on.\n\r", ch );
		}
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	/*
	 * No level check, gods can log anyone.
	 */
	if ( IS_SET ( victim->act, PLR_LOG ) ) {
		REMOVE_BIT ( victim->act, PLR_LOG );
		writeBuffer ( "LOG removed.\n\r", ch );
	} else {
		SET_BIT ( victim->act, PLR_LOG );
		writeBuffer ( "LOG set.\n\r", ch );
	}

	return;
}



DefineCommand ( cmd_noemote )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Noemote whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}


	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOEMOTE ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOEMOTE );
		writeBuffer ( "You can emote again.\n\r", victim );
		writeBuffer ( "NOEMOTE removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores emotes to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOEMOTE );
		writeBuffer ( "You can't emote!\n\r", victim );
		writeBuffer ( "NOEMOTE set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's emotes.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_noshout )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Noshout whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOSHOUT ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOSHOUT );
		writeBuffer ( "You can shout again.\n\r", victim );
		writeBuffer ( "NOSHOUT removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores shouts to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOSHOUT );
		writeBuffer ( "You can't shout!\n\r", victim );
		writeBuffer ( "NOSHOUT set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's shouts.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_notell )
{
	char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	Creature *victim;

	ChopC ( argument, arg );

	if ( arg[0] == '\0' ) {
		writeBuffer ( "Notell whom?", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( get_trust ( victim ) >= get_trust ( ch ) ) {
		writeBuffer ( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET ( victim->comm, COMM_NOTELL ) ) {
		REMOVE_BIT ( victim->comm, COMM_NOTELL );
		writeBuffer ( "You can tell again.\n\r", victim );
		writeBuffer ( "NOTELL removed.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N restores tells to %s.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	} else {
		SET_BIT ( victim->comm, COMM_NOTELL );
		writeBuffer ( "You can't tell!\n\r", victim );
		writeBuffer ( "NOTELL set.\n\r", ch );
		snprintf ( buf, sizeof ( buf ), "$N revokes %s's tells.", victim->name );
		wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}



DefineCommand ( cmd_peace )
{
	Creature *rch;

	for ( rch = IN_ROOM ( ch )->people; rch != NULL; rch = rch->next_in_room ) {
		if ( FIGHTING ( rch ) != NULL )
		{ stop_fighting ( rch, TRUE ); }
		if ( IS_NPC ( rch ) && IS_SET ( rch->act, ACT_AGGRESSIVE ) )
		{ REMOVE_BIT ( rch->act, ACT_AGGRESSIVE ); }
	}

	writeBuffer ( "Ok.\n\r", ch );
	return;
}

DefineCommand ( cmd_lockdown )
{
	extern bool lockdown;
	lockdown = !lockdown;

	if ( lockdown ) {
		wiznet ( "$N has lockdowned the game.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Game lockdowned.\n\r", ch );
	} else {
		wiznet ( "$N removes lockdown.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Game un-lockdowned.\n\r", ch );
	}

	return;
}

/* RT anti-newbie code */

DefineCommand ( cmd_newbielockdown )
{
	extern bool newbielockdown;
	newbielockdown = !newbielockdown;

	if ( newbielockdown ) {
		wiznet ( "$N locks out new characters.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "New characters have been locked out.\n\r", ch );
	} else {
		wiznet ( "$N allows new characters back in.", ch, NULL, 0, 0, 0 );
		writeBuffer ( "Newlock removed.\n\r", ch );
	}

	return;
}


DefineCommand ( cmd_slookup )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int sn;

	ChopC ( argument, arg );
	if ( arg[0] == '\0' ) {
		writeBuffer ( "Lookup which skill or spell?\n\r", ch );
		return;
	}

	if ( SameString ( arg, "all" ) ) {
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name == NULL )
			{ break; }
			sprintf ( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
					  sn, skill_table[sn].slot, skill_table[sn].name );
			writeBuffer ( buf, ch );
		}
	} else {
		if ( ( sn = skill_lookup ( arg ) ) < 0 ) {
			writeBuffer ( "No such skill or spell.\n\r", ch );
			return;
		}

		sprintf ( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
				  sn, skill_table[sn].slot, skill_table[sn].name );
		writeBuffer ( buf, ch );
	}

	return;
}

/* RT set replaces sset, mset, oset, and rset */
DefineCommand ( cmd_set )
{
	char arg[MAX_INPUT_LENGTH];

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set mob   <name> <field> <value>\n\r", ch );
		writeBuffer ( "  set obj   <name> <field> <value>\n\r", ch );
		writeBuffer ( "  set room  <room> <field> <value>\n\r", ch );
		writeBuffer ( "  set skill <name> <spell or skill> <value>\n\r", ch );
		return;
	}

	argument = ChopC ( argument, arg );

	if ( !str_prefix ( arg, "mobile" ) || !str_prefix ( arg, "character" ) ) {
		cmd_function ( ch, &cmd_mset, argument );
		return;
	}

	if ( !str_prefix ( arg, "skill" ) || !str_prefix ( arg, "spell" ) ) {
		cmd_function ( ch, &cmd_sset, argument );
		return;
	}

	if ( !str_prefix ( arg, "object" ) ) {
		cmd_function ( ch, &cmd_oset, argument );
		return;
	}

	if ( !str_prefix ( arg, "room" ) ) {
		cmd_function ( ch, &cmd_rset, argument );
		return;
	}
	/* echo syntax */
	cmd_function ( ch, &cmd_set, "" );
}


DefineCommand ( cmd_sset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Creature *victim;
	int value;
	int sn;
	bool fAll;

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	argument = ChopC ( argument, arg3 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set skill <name> <spell or skill> <value>\n\r", ch );
		writeBuffer ( "  set skill <name> all <value>\n\r", ch );
		writeBuffer ( "   (use the name of the skill, not the number)\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC ( victim ) ) {
		writeBuffer ( "Not on NPC's.\n\r", ch );
		return;
	}

	fAll = SameString ( arg2, "all" );
	sn   = 0;
	if ( !fAll && ( sn = skill_lookup ( arg2 ) ) < 0 ) {
		writeBuffer ( "No such skill or spell.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value.
	 */
	if ( !is_number ( arg3 ) ) {
		writeBuffer ( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi ( arg3 );
	if ( value < 0 || value > 100 ) {
		writeBuffer ( "Value range is 0 to 100.\n\r", ch );
		return;
	}

	if ( fAll ) {
		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name != NULL )
			{ victim->pcdata->learned[sn]	= value; }
		}
	} else {
		victim->pcdata->learned[sn] = value;
	}

	return;
}


DefineCommand ( cmd_mset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	char buf[100];
	Creature *victim;
	int value;

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Unknown Syntax!\r\nset char <name> <field> <value>\r\n", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    str int wis dex con sex archetype level\n\r",	ch );
		writeBuffer ( "    race group gold silver hp mana move prac\n\r", ch );
		writeBuffer ( "    align train thirst hunger drunk full\n\r",	ch );
		writeBuffer ( "    security\n\r",				ch );
		return;
	}

	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set char <name> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    str int wis dex con sex archetype level\n\r",	ch );
		writeBuffer ( "    race group gold silver hp mana move prac\n\r", ch );
		writeBuffer ( "    align train thirst hunger drunk full\n\r",	ch );
		writeBuffer ( "    security\n\r",				ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "They aren't here.\n\r", ch );
		return;
	}

	/* clear zones for mobs */
	victim->zone = NULL;

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = is_number ( arg3 ) ? atoi ( arg3 ) : -1;

	/*
	 * Set something.
	 */
	if ( SameString ( arg2, "str" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_STR ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Strength range is 3 to %d\n\r.",
					   get_max_train ( victim, STAT_STR ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_STR] = value;
		return;
	}

	if ( SameString ( arg2, "security" ) ) {	/* OLC */
		if ( IS_NPC ( ch ) ) {
			writeBuffer ( "Si, claro.\n\r", ch );
			return;
		}

		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value > ch->pcdata->security || value < 0 ) {
			if ( ch->pcdata->security != 0 ) {
				sprintf ( buf, "Valid security is 0-%d.\n\r",
						  ch->pcdata->security );
				writeBuffer ( buf, ch );
			} else {
				writeBuffer ( "Valid security is 0 only.\n\r", ch );
			}
			return;
		}
		victim->pcdata->security = value;
		return;
	}

	if ( SameString ( arg2, "int" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_INT ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Intelligence range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_INT ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_INT] = value;
		return;
	}

	if ( SameString ( arg2, "wis" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_WIS ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Wisdom range is 3 to %d.\n\r", get_max_train ( victim, STAT_WIS ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_WIS] = value;
		return;
	}

	if ( SameString ( arg2, "dex" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_DEX ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Dexterity range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_DEX ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_DEX] = value;
		return;
	}

	if ( SameString ( arg2, "con" ) ) {
		if ( value < 3 || value > get_max_train ( victim, STAT_CON ) ) {
			snprintf ( buf, sizeof ( buf ),
					   "Constitution range is 3 to %d.\n\r",
					   get_max_train ( victim, STAT_CON ) );
			writeBuffer ( buf, ch );
			return;
		}

		victim->perm_stat[STAT_CON] = value;
		return;
	}

	if ( !str_prefix ( arg2, "sex" ) ) {
		if ( value < 0 || value > 2 ) {
			writeBuffer ( "Sex range is 0 to 2.\n\r", ch );
			return;
		}
		victim->sex = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->true_sex = value; }
		return;
	}

	if ( !str_prefix ( arg2, "archetype" ) ) {
		int archetype;

		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Mobiles have no archetype.\n\r", ch );
			return;
		}

		archetype = archetype_lookup ( arg3 );
		if ( archetype == -1 ) {
			char buf[MAX_STRING_LENGTH];

			strcpy ( buf, "Possible archetypees are: " );
			for ( archetype = 0; archetype < MAX_CLASS; archetype++ ) {
				if ( archetype > 0 )
				{ strcat ( buf, " " ); }
				strcat ( buf, archetype_table[archetype].name );
			}
			strcat ( buf, ".\n\r" );

			writeBuffer ( buf, ch );
			return;
		}

		victim->archetype = archetype;
		return;
	}

	if ( !str_prefix ( arg2, "level" ) ) {
		if ( !IS_NPC ( victim ) ) {
			writeBuffer ( "Not on PC's.\n\r", ch );
			return;
		}

		if ( value < 0 || value > MAX_LEVEL ) {
			snprintf ( buf, sizeof ( buf ), "Level range is 0 to %d.\n\r", MAX_LEVEL );
			writeBuffer ( buf, ch );
			return;
		}
		victim->level = value;
		return;
	}

	if ( !str_prefix ( arg2, "gold" ) ) {
		victim->gold = value;
		return;
	}

	if ( !str_prefix ( arg2, "silver" ) ) {
		victim->silver = value;
		return;
	}

	if ( !str_prefix ( arg2, "hp" ) ) {
		if ( value < -10 || value > 30000 ) {
			writeBuffer ( "Hp range is -10 to 30,000 hit points.\n\r", ch );
			return;
		}
		victim->max_hit = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_hit = value; }
		return;
	}

	if ( !str_prefix ( arg2, "mana" ) ) {
		if ( value < 0 || value > 30000 ) {
			writeBuffer ( "Mana range is 0 to 30,000 mana points.\n\r", ch );
			return;
		}
		victim->max_mana = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_mana = value; }
		return;
	}

	if ( !str_prefix ( arg2, "move" ) ) {
		if ( value < 0 || value > 30000 ) {
			writeBuffer ( "Move range is 0 to 30,000 move points.\n\r", ch );
			return;
		}
		victim->max_move = value;
		if ( !IS_NPC ( victim ) )
		{ victim->pcdata->perm_move = value; }
		return;
	}

	if ( !str_prefix ( arg2, "practice" ) ) {
		if ( value < 0 || value > 250 ) {
			writeBuffer ( "Practice range is 0 to 250 sessions.\n\r", ch );
			return;
		}
		victim->practice = value;
		return;
	}

	if ( !str_prefix ( arg2, "train" ) ) {
		if ( value < 0 || value > 50 ) {
			writeBuffer ( "Training session range is 0 to 50 sessions.\n\r", ch );
			return;
		}
		victim->train = value;
		return;
	}

	if ( !str_prefix ( arg2, "align" ) ) {
		if ( value < -1000 || value > 1000 ) {
			writeBuffer ( "Alignment range is -1000 to 1000.\n\r", ch );
			return;
		}
		victim->alignment = value;
		return;
	}

	if ( !str_prefix ( arg2, "thirst" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Thirst range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_THIRST] = value;
		return;
	}

	if ( !str_prefix ( arg2, "drunk" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Drunk range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_DRUNK] = value;
		return;
	}

	if ( !str_prefix ( arg2, "full" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_FULL] = value;
		return;
	}

	if ( !str_prefix ( arg2, "hunger" ) ) {
		if ( IS_NPC ( victim ) ) {
			writeBuffer ( "Not on NPC's.\n\r", ch );
			return;
		}

		if ( value < -1 || value > 100 ) {
			writeBuffer ( "Full range is -1 to 100.\n\r", ch );
			return;
		}

		victim->pcdata->condition[COND_HUNGER] = value;
		return;
	}

	if ( !str_prefix ( arg2, "race" ) ) {
		int race;

		race = race_lookup ( arg3 );

		if ( race == 0 ) {
			writeBuffer ( "That is not a valid race.\n\r", ch );
			return;
		}

		if ( !IS_NPC ( victim ) && !race_table[race].pc_race ) {
			writeBuffer ( "That is not a valid player race.\n\r", ch );
			return;
		}

		victim->race = race;
		return;
	}

	if ( !str_prefix ( arg2, "group" ) ) {
		if ( !IS_NPC ( victim ) ) {
			writeBuffer ( "Only on NPCs.\n\r", ch );
			return;
		}
		victim->group = value;
		return;
	}


	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_mset, "" );
	return;
}

DefineCommand ( cmd_string )
{
	char type [MAX_INPUT_LENGTH];
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Creature *victim;
	Item *obj;

	smash_tilde ( argument );
	argument = ChopC ( argument, type );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  string char <name> <field> <string>\n\r", ch );
		writeBuffer ( "    fields: name short long desc title spec\n\r", ch );
		writeBuffer ( "  string obj  <name> <field> <string>\n\r", ch );
		writeBuffer ( "    fields: name short long extended\n\r", ch );
		return;
	}

	if ( !str_prefix ( type, "character" ) || !str_prefix ( type, "mobile" ) ) {
		if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ) {
			writeBuffer ( "They aren't here.\n\r", ch );
			return;
		}

		/* clear zone for mobs */
		victim->zone = NULL;

		/* string something */

		if ( !str_prefix ( arg2, "name" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Not on PC's.\n\r", ch );
				return;
			}
			PURGE_DATA ( victim->name );
			victim->name = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "description" ) ) {
			PURGE_DATA ( victim->description );
			victim->description = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "short" ) ) {
			PURGE_DATA ( victim->short_descr );
			victim->short_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "long" ) ) {
			PURGE_DATA ( victim->long_descr );
			strcat ( arg3, "\n\r" );
			victim->long_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "title" ) ) {
			if ( IS_NPC ( victim ) ) {
				writeBuffer ( "Not on NPC's.\n\r", ch );
				return;
			}

			set_title ( victim, arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "spec" ) ) {
			if ( !IS_NPC ( victim ) ) {
				writeBuffer ( "Not on PC's.\n\r", ch );
				return;
			}

			if ( ( victim->spec_fun = spec_lookup ( arg3 ) ) == 0 ) {
				writeBuffer ( "No such spec fun.\n\r", ch );
				return;
			}

			return;
		}
	}

	if ( !str_prefix ( type, "object" ) ) {
		/* string an obj */

		if ( ( obj = get_obj_world ( ch, arg1 ) ) == NULL ) {
			writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch );
			return;
		}

		if ( !str_prefix ( arg2, "name" ) ) {
			PURGE_DATA ( obj->name );
			obj->name = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "short" ) ) {
			PURGE_DATA ( obj->short_descr );
			obj->short_descr = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "long" ) ) {
			PURGE_DATA ( obj->description );
			obj->description = assign_string ( arg3 );
			return;
		}

		if ( !str_prefix ( arg2, "ed" ) || !str_prefix ( arg2, "extended" ) ) {
			DescriptionData *ed;

			argument = ChopC ( argument, arg3 );
			if ( argument == NULL ) {
				writeBuffer ( "Syntax: oset <object> ed <keyword> <string>\n\r",
							  ch );
				return;
			}

			char *dstr = ( char * ) argument;

			strcat ( dstr, "\n\r" );

			ed = new_extra_descr();

			ed->keyword		= assign_string ( arg3     );
			ed->description	= assign_string ( dstr );
			ed->next		= obj->extra_descr;
			obj->extra_descr	= ed;
			return;
		}
	}


	/* echo bad use message */
	cmd_function ( ch, &cmd_string, "" );
}



DefineCommand ( cmd_oset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	Item *obj;
	int value;

	smash_tilde ( argument );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set obj <object> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",				ch );
		writeBuffer ( "    value0 value1 value2 value3 value4 (v1-v4)\n\r",	ch );
		writeBuffer ( "    extra wear level weight cost timer\n\r",		ch );
		return;
	}

	if ( ( obj = get_obj_world ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "Nothing like that in heaven or earth.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = atoi ( arg3 );

	/*
	 * Set something.
	 */
	if ( SameString ( arg2, "value0" ) || SameString ( arg2, "v0" ) ) {
		obj->value[0] = UMIN ( 50, value );
		return;
	}

	if ( SameString ( arg2, "value1" ) || SameString ( arg2, "v1" ) ) {
		obj->value[1] = value;
		return;
	}

	if ( SameString ( arg2, "value2" ) || SameString ( arg2, "v2" ) ) {
		obj->value[2] = value;
		return;
	}

	if ( SameString ( arg2, "value3" ) || SameString ( arg2, "v3" ) ) {
		obj->value[3] = value;
		return;
	}

	if ( SameString ( arg2, "value4" ) || SameString ( arg2, "v4" ) ) {
		obj->value[4] = value;
		return;
	}

	if ( !str_prefix ( arg2, "extra" ) ) {
		obj->extra_flags = value;
		return;
	}

	if ( !str_prefix ( arg2, "wear" ) ) {
		obj->wear_flags = value;
		return;
	}

	if ( !str_prefix ( arg2, "level" ) ) {
		obj->level = value;
		return;
	}

	if ( !str_prefix ( arg2, "weight" ) ) {
		obj->weight = value;
		return;
	}

	if ( !str_prefix ( arg2, "cost" ) ) {
		obj->cost = value;
		return;
	}

	if ( !str_prefix ( arg2, "timer" ) ) {
		obj->timer = value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_oset, "" );
	return;
}



DefineCommand ( cmd_rset )
{
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	RoomData *location;
	int value;

	smash_tilde ( argument );
	argument = ChopC ( argument, arg1 );
	argument = ChopC ( argument, arg2 );
	strcpy ( arg3, argument );

	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' ) {
		writeBuffer ( "Syntax:\n\r", ch );
		writeBuffer ( "  set room <location> <field> <value>\n\r", ch );
		writeBuffer ( "  Field being one of:\n\r",			ch );
		writeBuffer ( "    flags sector\n\r",				ch );
		return;
	}

	if ( ( location = find_location ( ch, arg1 ) ) == NULL ) {
		writeBuffer ( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner ( ch, location ) && IN_ROOM ( ch ) != location
			&&  room_is_private ( location ) && !IS_TRUSTED ( ch, IMPLEMENTOR ) ) {
		writeBuffer ( "That room is private right now.\n\r", ch );
		return;
	}

	/*
	 * Snarf the value.
	 */
	if ( !is_number ( arg3 ) ) {
		writeBuffer ( "Value must be numeric.\n\r", ch );
		return;
	}
	value = atoi ( arg3 );

	/*
	 * Set something.
	 */
	if ( !str_prefix ( arg2, "flags" ) ) {
		location->room_flags	= value;
		return;
	}

	if ( !str_prefix ( arg2, "sector" ) ) {
		location->sector_type	= value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	cmd_function ( ch, &cmd_rset, "" );
	return;
}

/*
 * New routines by Dionysos.
 */
DefineCommand ( cmd_invis )
{
	int level;
	char arg[MAX_STRING_LENGTH];

	/* RT code for taking a level argument */
	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
		/* take the default path */

		if ( ch->invis_level ) {
			ch->invis_level = 0;
			act ( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly fade back into existence.\n\r", ch );
		} else {
			ch->invis_level = get_trust ( ch );
			act ( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly vanish into thin air.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi ( arg );
		if ( level < 2 || level > get_trust ( ch ) ) {
			writeBuffer ( "Invis level must be between 2 and your level.\n\r", ch );
			return;
		} else {
			ch->reply = NULL;
			ch->invis_level = level;
			act ( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You slowly vanish into thin air.\n\r", ch );
		}
	}

	return;
}


DefineCommand ( cmd_incognito )
{
	int level;
	char arg[MAX_STRING_LENGTH];

	/* RT code for taking a level argument */
	ChopC ( argument, arg );

	if ( arg[0] == '\0' )
		/* take the default path */

		if ( ch->incog_level ) {
			ch->incog_level = 0;
			act ( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You are no longer cloaked.\n\r", ch );
		} else {
			ch->incog_level = get_trust ( ch );
			act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You cloak your presence.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi ( arg );
		if ( level < 2 || level > get_trust ( ch ) ) {
			writeBuffer ( "Incog level must be between 2 and your level.\n\r", ch );
			return;
		} else {
			ch->reply = NULL;
			ch->incog_level = level;
			act ( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
			writeBuffer ( "You cloak your presence.\n\r", ch );
		}
	}

	return;
}



DefineCommand ( cmd_holylight )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( IS_SET ( ch->act, PLR_HOLYLIGHT ) ) {
		REMOVE_BIT ( ch->act, PLR_HOLYLIGHT );
		writeBuffer ( "God Sight has been disabled.\r\n", ch );
	} else {
		SET_BIT ( ch->act, PLR_HOLYLIGHT );
		writeBuffer ( "God Sight has been enabled.\r\n", ch );
	}

	return;
}

/* prefix command: it will put the string typed on each line typed */

DefineCommand ( cmd_prefi )
{
	writeBuffer ( "You cannot abbreviate the prefix command.\r\n", ch );
	return;
}

DefineCommand ( cmd_prefix )
{
	char buf[MAX_INPUT_LENGTH];

	if ( argument[0] == '\0' ) {
		if ( ch->prefix[0] == '\0' ) {
			writeBuffer ( "You have no prefix to clear.\r\n", ch );
			return;
		}

		writeBuffer ( "Prefix removed.\r\n", ch );
		PURGE_DATA ( ch->prefix );
		ch->prefix = assign_string ( "" );
		return;
	}

	if ( ch->prefix[0] != '\0' ) {
		snprintf ( buf, sizeof ( buf ), "Prefix changed to %s.\r\n", argument );
		PURGE_DATA ( ch->prefix );
	} else {
		snprintf ( buf, sizeof ( buf ), "Prefix set to %s.\r\n", argument );
	}
	writeBuffer ( buf, ch );
	ch->prefix = assign_string ( argument );
}



DefineCommand ( cmd_events )
{
	EventManager::instance().reportEvents ( ch );
	return;
}


DefineCommand ( cmd_noloot )
{
	if ( IS_NPC ( ch ) )
	{ return; }

	if ( IS_SET ( ch->act, PLR_CANLOOT ) ) {
		writeBuffer ( "Your corpse is now safe from thieves.\n\r", ch );
		REMOVE_BIT ( ch->act, PLR_CANLOOT );
	} else {
		writeBuffer ( "Your corpse may now be looted.\n\r", ch );
		SET_BIT ( ch->act, PLR_CANLOOT );
	}
}

DefineCommand ( cmd_nosummon )
{
	if ( IS_NPC ( ch ) ) {
		if ( IS_SET ( ch->imm_flags, IMM_SUMMON ) ) {
			writeBuffer ( "You are no longer immune to summon.\n\r", ch );
			REMOVE_BIT ( ch->imm_flags, IMM_SUMMON );
		} else {
			writeBuffer ( "You are now immune to summoning.\n\r", ch );
			SET_BIT ( ch->imm_flags, IMM_SUMMON );
		}
	} else {
		if ( IS_SET ( ch->act, PLR_NOSUMMON ) ) {
			writeBuffer ( "You are no longer immune to summon.\n\r", ch );
			REMOVE_BIT ( ch->act, PLR_NOSUMMON );
		} else {
			writeBuffer ( "You are now immune to summoning.\n\r", ch );
			SET_BIT ( ch->act, PLR_NOSUMMON );
		}
	}
}


