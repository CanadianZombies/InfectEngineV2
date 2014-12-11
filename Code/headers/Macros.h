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

#ifndef _Macros_H
#define _Macros_H

/*
 * Utility macros.
 */
#define C_STR(x) 	      (x.c_str())
#define STR(x)                #x

// -- Make two strings best of friends and join them like blood brothers.
#define CONCAT_STR(A, B) (#A " " #B)

//-- ASSERT
#define ASSERT(exp) do { \
		if(exp) { \
			; \
		} \
		else \
		{ SUICIDE; } \
	} while(0)

// ------------------------------------------------------------
// -- forces the mud to CRASH at specified part.
// -- LOG_SUICIDE forces the mud to log where the suicide was called from, then crashes the mud
#define SUICIDE      (log_hd(LOG_DEBUG|LOG_SUICIDE, "SUICIDE: "  __FILE__ ) )

// -- SUICIDE_REAL skips the logging part and just crashes the mud with a sigsegv in an attempt to generate a CORE file.
#define SUICIDE_REAL (kill(getpid(), SIGSEGV))

// -- THROW_ERROR allows us to throw out an error easily, with all the appropriate information attached to it
// -- for file, function, and line it came from.
#define THROW_ERROR(...) (throw std::runtime_error( Format("InfectEngine encountered a runtime error: %s, %s, %d: %s", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)) )

// -- use assert to throw a message
#if defined (_USE_THROW_ERROR_)
#define ASSERT_THROW(exp, message) do { \
		if(exp) \
			; \
		else { \
			THROW_ERROR(CONCAT_STR(exp, message)); \
		} \
	} while(0)
#else
#define ASSERT_THROW(exp, message) do { if(exp) { ; } else { ; } } while(0)
#endif

// -- lets log our errno string properly.
#define ReportErrno(ErrorString) _Error( (ErrorString), __FILE__, __FUNCTION__, __LINE__)
#define CATCH(willAbort) catchException(willAbort, __FILE__, __FUNCTION__, __LINE__)

// -- creates an internal exception trace.  This prints out the stack of the last few system calls.
#define ExceptionTrace() do \
	{ \
		Dl_info dli; \
		int backx = 0; \
		void *returnaddr; \
		for ( backx = 0; backx < 8; backx++ ) \
		{ \
			returnaddr = (void *) 1; \
			switch( backx ) \
			{ \
				case 0: returnaddr = __builtin_return_address(0); break; \
				case 1: returnaddr = __builtin_return_address(1); break; \
				case 2: returnaddr = __builtin_return_address(2); break; \
				case 3: returnaddr = __builtin_return_address(3); break; \
				case 4: returnaddr = __builtin_return_address(4); break; \
				case 5: returnaddr = __builtin_return_address(5); break; \
				case 6: returnaddr = __builtin_return_address(6); break; \
				case 7: returnaddr = __builtin_return_address(7); break; \
				case 8: returnaddr = __builtin_return_address(8); break; \
				default: returnaddr = 0; break; \
			} \
			if ( !returnaddr ) \
				break; \
			dladdr( returnaddr, &dli); \
			log_hd(LOG_ERROR, Format("\t%*s called by %p #%d [ %s(%p) %s(%p) ].", \
									 (int)strlen(__func__ ), backx == 0 ? __func__ : "...", \
									 returnaddr, backx, \
									 strrchr(dli.dli_fname, '/') ? strrchr( dli.dli_fname, '/')+1 : dli.dli_fname, \
									 dli.dli_fbase, dli.dli_sname, dli.dli_saddr)); \
		}	\
	} \
	while(0)

// -- Give the chance to save, so, while not everything will cause a flatfile save
// -- this give sthe chance to cause a save to occur.  Typically, saving only occurs
// -- every few minutes with the updater, and on death.  But since death deletes the pfile
// -- this needs to be a lot more frequent.
#define SAVE_CHANCE(ch) \
	do { \
		if(!IS_NPC(ch)) { \
			if(Math::instance().percent() == Math::instance().range(0,125)) \
			{ save_char_obj(ch); } \
		} \
	} while(0)


#define IS_VALID(data)		((data) != NULL && (data)->valid)			// -- Relic
#define VALIDATE(data)		((data)->valid = TRUE)								// -- Relic
#define INVALIDATE(data)	((data)->valid = FALSE)								// -- Relic

#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))

#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))

#define IN_ROOM(ch) ((ch)->in_room)
#define FIGHTING(ch) ((ch)->fighting)

#define CARRYING(ch) ((ch)->carrying)
#define CARRIED_BY(obj) ((obj)->carried_by)
#define IN_OBJ(obj) ((obj)->in_obj)

#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define GET_UNSET(flag1,flag2)	(~(flag1)&((flag1)|(flag2)))    /* macro for flag swapping */
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (NPCData *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (ItemData *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = IN_ROOM(ch) )
#define EDIT_AREA(Ch, Area)	( Area = (Zone *)Ch->desc->pEdit )
#define EDIT_MPCODE(Ch, Code)   ( Code = (MPROG_CODE*)Ch->desc->pEdit )

#define IS_NULLSTR(str)		((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)	( ((min) < (num)) && ((num) < (max)) )
#define CHECK_POS(a, b, c)	{							\
		(a) = (b);					\
		if ( (a) < 0 )					\
			log_hd(LOG_ERROR, Format("CHECK_POS : " c " == %d < 0", a) );	\
	}							\
	 
#define nelems(x)  (sizeof(x) / sizeof(x[0]))

#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))

// -- staff is now controlled by flags, NOT by level!
#define IsStaff(ch)		(IS_SET((ch)->sflag, CR_STAFF) )  // -- (get_trust(ch) >= LEVEL_IMMORTAL)

#define IS_LEGEND(ch) (get_trust(ch) == MAX_LEVEL)					// -- Legends are just that, legends. Almsot impossible to reach this level!
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)					// -- Once a hero, always a hero!
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))

#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define GET_AGE(ch)		((int) (17 + ((ch)->played + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)

#define GET_AC(ch,type)		((ch)->armor[type] + ( IS_AWAKE(ch) ? get_defense(ch) : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) 	((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
							 (ch)->in_room->room_flags,		    \
							 ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))

#define get_carry_weight(ch)	((ch)->carry_weight + (ch)->silver/10 + (ch)->gold * 2 / 5)

#define act(format,ch,arg1,arg2,type) act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)

#define HAS_TRIGGER(ch,trig)	(IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)	( !IS_NPC(ch) && !IS_SWITCHED( ch ) &&	  \
								  ( ch->pcdata->security >= Area->security  \
									|| strstr( Area->builders, ch->name )	  \
									|| strstr( Area->builders, "All" ) ) )

#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? (obj)->value[4] : 100)


#define ALLOC_DATA(pointer, datatype, elements)   \
	do { \
		if (!((pointer) = (datatype *) calloc ((elements), sizeof(datatype)))) \
		{ \
			log_hd(LOG_ERROR, Format("Unable to calloc new data for pointer (" #pointer ") from file: %s function: %s, line: %d *** ABORTING ***", __FILE__, __FUNCTION__, __LINE__ ) ); \
			kill(getpid(),SIGSEGV); \
		} \
	} \
	while(0)

#define ACTUAL_PURGE(data) \
	do { \
		if(data) \
		{ \
			if(typeid(data) == typeid(char *) || typeid(data) == typeid(const char *)) \
			{ \
				delete [] data; \
			} \
			else { \
				free ((void *) data); \
				data = NULL; \
			} \
		} \
	} \
	while(0);

#define PURGE_DATA(data) \
	do { \
		if(data) { \
			ACTUAL_PURGE(data);  \
		} else { \
			if(true == false) { \
				log_hd(LOG_DEBUG, Format("PURGE_DATA called on NULL data from: %s @ %s : %d", __FILE__, __FUNCTION__, __LINE__)); \
			} \
		} \
	} while(0)


#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
							  ( IS_NPC(ch) ? (ch)->short_descr	\
								: (ch)->name ) : "an unknown figure" )


#endif
