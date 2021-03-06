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

/* stuff for recyling notes */
NOTE_DATA *note_free;

NOTE_DATA *new_note()
{
	NOTE_DATA *note;

	if ( note_free == NULL ) {
		ALLOC_DATA ( note, NOTE_DATA, 1 );
	} else {
		note = note_free;
		note_free = note_free->next;
	}
	VALIDATE ( note );
	return note;
}

void recycle_note ( NOTE_DATA *note )
{
	if ( !IS_VALID ( note ) )
	{ return; }

	PURGE_DATA ( note->text    );
	PURGE_DATA ( note->subject );
	PURGE_DATA ( note->to_list );
	PURGE_DATA ( note->date    );
	PURGE_DATA ( note->sender  );
	INVALIDATE ( note );

	note->next = note_free;
	note_free   = note;
}


/* stuff for recycling ban structures */
Ban *ban_free;

Ban *new_ban ( void )
{
	static Ban ban_zero;
	Ban *ban;

	if ( ban_free == NULL ) {
		ALLOC_DATA ( ban, Ban, 1 );
	} else {
		ban = ban_free;
		ban_free = ban_free->next;
	}

	*ban = ban_zero;
	VALIDATE ( ban );
	ban->name = NULL;
	return ban;
}

void recycle_ban ( Ban *ban )
{
	if ( !IS_VALID ( ban ) )
	{ return; }

	PURGE_DATA ( ban->name );
	INVALIDATE ( ban );

	ban->next = ban_free;
	ban_free = ban;
}

/* stuff for recycling descriptors */
Socket *descriptor_free;

Socket *new_descriptor ( void )
{
	static Socket d_zero;
	Socket *d;

	if ( descriptor_free == NULL ) {
		ALLOC_DATA ( d, Socket, 1 );
	} else {
		d = descriptor_free;
		descriptor_free = descriptor_free->next;
	}

	*d = d_zero;
	VALIDATE ( d );
	return d;
}

void recycle_descriptor ( Socket *d )
{
	if ( !IS_VALID ( d ) )
	{ return; }

	PURGE_DATA ( d->host );
	PURGE_DATA ( d->outbuf );
	INVALIDATE ( d );

	d->next = descriptor_free;
	descriptor_free = d;
}

/* stuff for recycling gen_data */
GEN_DATA *gen_data_free;

GEN_DATA *new_gen_data ( void )
{
	static GEN_DATA gen_zero;
	GEN_DATA *gen;

	if ( gen_data_free == NULL ) {
		ALLOC_DATA ( gen, GEN_DATA, 1 );
	} else {
		gen = gen_data_free;
		gen_data_free = gen_data_free->next;
	}
	*gen = gen_zero;
	VALIDATE ( gen );
	return gen;
}

void recycle_gen_data ( GEN_DATA *gen )
{
	if ( !IS_VALID ( gen ) )
	{ return; }

	INVALIDATE ( gen );

	gen->next = gen_data_free;
	gen_data_free = gen;
}

/* stuff for recycling extended descs */
DescriptionData *extra_descr_free;

DescriptionData *new_extra_descr ( void )
{
	DescriptionData *ed;

	if ( extra_descr_free == NULL ) {
		ALLOC_DATA ( ed, DescriptionData, 1 );
	} else {
		ed = extra_descr_free;
		extra_descr_free = extra_descr_free->next;
	}

	ed->keyword = NULL;
	ed->description = NULL;
	VALIDATE ( ed );
	return ed;
}

void recycle_extra_descr ( DescriptionData *ed )
{
	if ( !IS_VALID ( ed ) )
	{ return; }

	PURGE_DATA ( ed->keyword );
	PURGE_DATA ( ed->description );
	INVALIDATE ( ed );

	ed->next = extra_descr_free;
	extra_descr_free = ed;
}


/* stuff for recycling affects */
Affect *affect_free;

Affect *new_affect ( void )
{
	static Affect af_zero;
	Affect *af;

	if ( affect_free == NULL ) {
		ALLOC_DATA ( af, Affect, 1 );
	} else {
		af = affect_free;
		affect_free = affect_free->next;
	}

	*af = af_zero;


	VALIDATE ( af );
	return af;
}

void recycle_affect ( Affect *af )
{
	if ( !IS_VALID ( af ) )
	{ return; }

	INVALIDATE ( af );
	af->next = affect_free;
	affect_free = af;
}

/* stuff for recycling objects */
Item *obj_free;

Item *new_obj ( void )
{
	static Item obj_zero;
	Item *obj;

	if ( obj_free == NULL ) {
		ALLOC_DATA ( obj, Item, 1 );
	} else {
		obj = obj_free;
		obj_free = obj_free->next;
	}
	*obj = obj_zero;
	VALIDATE ( obj );

	return obj;
}

void recycle_obj ( Item *obj )
{
	Affect *paf, *paf_next;
	DescriptionData *ed, *ed_next;

	if ( !IS_VALID ( obj ) )
	{ return; }

	for ( paf = obj->affected; paf != NULL; paf = paf_next ) {
		paf_next = paf->next;
		recycle_affect ( paf );
	}
	obj->affected = NULL;

	for ( ed = obj->extra_descr; ed != NULL; ed = ed_next ) {
		ed_next = ed->next;
		recycle_extra_descr ( ed );
	}
	obj->extra_descr = NULL;

	EventManager::instance().purgeFromOwner ( obj );

	PURGE_DATA ( obj->name        );
	PURGE_DATA ( obj->description );
	PURGE_DATA ( obj->short_descr );
	PURGE_DATA ( obj->owner     );
	INVALIDATE ( obj );

	obj->next   = obj_free;
	obj_free    = obj;
}


/* stuff for recyling characters */
Creature *char_free;

Creature *new_char ( void )
{
	static Creature ch_zero;
	Creature *ch;
	int i;

	if ( char_free == NULL ) {
		ALLOC_DATA ( ch, Creature, 1 );
	} else {
		ch = char_free;
		char_free = char_free->next;
	}

	*ch				= ch_zero;
	VALIDATE ( ch );
	ch->name                    = NULL;
	ch->short_descr             = NULL;
	ch->long_descr              = NULL;
	ch->description             = NULL;
	ch->prompt                  = NULL;
	ch->prefix			= NULL;
	ch->logon                   = current_time;
	ch->lines                   = PAGELEN;
	for ( i = 0; i < 4; i++ )
	{ ch->armor[i]            = 100; }
	ch->position                = POS_STANDING;
	ch->hit                     = 20;
	ch->max_hit                 = 20;
	ch->mana                    = 100;
	ch->max_mana                = 100;
	ch->move                    = 100;
	ch->max_move                = 100;
	ch->sitrep                  = 0;
	ch->exp_pool		    = 0;

	// -- zeroize the structures.
	ch->queries.querydata = NULL;
	ch->queries.queryprompt[0] = '\0';
	ch->queries.queryprompt2[0] = '\0';
	ch->queries.querycommand = 0;
	ch->queries.queryintcommand = 0;
	ch->queries.query_string = NULL;

	for ( i = 0; i < MAX_STATS; i ++ ) {
		ch->perm_stat[i] = 13;
		ch->mod_stat[i] = 0;
	}

	return ch;
}


void recycle_char ( Creature *ch )
{
	Item *obj;
	Item *obj_next;
	Affect *paf;
	Affect *paf_next;

	if ( !IS_VALID ( ch ) )
	{ return; }

	if ( IS_NPC ( ch ) )
	{ mobile_count--; }

	for ( obj = ch->carrying; obj != NULL; obj = obj_next ) {
		obj_next = obj->next_content;
		extract_obj ( obj );
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next ) {
		paf_next = paf->next;
		affect_remove ( ch, paf );
	}

	PURGE_DATA ( ch->name );
	PURGE_DATA ( ch->short_descr );
	PURGE_DATA ( ch->long_descr );
	PURGE_DATA ( ch->description );
	PURGE_DATA ( ch->prompt );
	PURGE_DATA ( ch->prefix );

	EventManager::instance().purgeFromOwner ( ch );

	// -- zeroize the structures.
	ch->queries.querydata = NULL;
	ch->queries.queryprompt[0] = '\0';
	ch->queries.queryprompt2[0] = '\0';
	ch->queries.querycommand = 0;
	ch->queries.queryintcommand = 0;

	PURGE_DATA ( ch->queries.query_string );

	recycle_note  ( ch->pnote );
	recycle_pcdata ( ch->pcdata );

	ch->next = char_free;
	char_free  = ch;

	INVALIDATE ( ch );
	return;
}

PlayerData *pcdata_free;

PlayerData *new_pcdata ( void )
{
	int alias;

	static PlayerData pcdata_zero;
	PlayerData *pcdata;

	if ( pcdata_free == NULL ) {
		ALLOC_DATA ( pcdata, PlayerData, 1 );
	} else {
		pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}

	*pcdata = pcdata_zero;

	for ( alias = 0; alias < MAX_ALIAS; alias++ ) {
		pcdata->alias[alias] = NULL;
		pcdata->alias_sub[alias] = NULL;
	}

	pcdata->buffer = new_buf();

	VALIDATE ( pcdata );
	return pcdata;
}


void recycle_pcdata ( PlayerData *pcdata )
{
	int alias;

	if ( !IS_VALID ( pcdata ) )
	{ return; }

	PURGE_DATA ( pcdata->pwd );
	PURGE_DATA ( pcdata->bamfin );
	PURGE_DATA ( pcdata->bamfout );
	PURGE_DATA ( pcdata->title );
	recycle_buf ( pcdata->buffer );

	for ( alias = 0; alias < MAX_ALIAS; alias++ ) {
		PURGE_DATA ( pcdata->alias[alias] );
		PURGE_DATA ( pcdata->alias_sub[alias] );
	}
	INVALIDATE ( pcdata );
	pcdata->next = pcdata_free;
	pcdata_free = pcdata;

	return;
}




/* stuff for setting ids */
long	last_pc_id;
long	last_mob_id;

long get_pc_id ( void )
{
	int val;

	val = ( current_time <= last_pc_id ) ? last_pc_id + 1 : current_time;
	last_pc_id = val;
	return val;
}

long get_mob_id ( void )
{
	last_mob_id++;
	return last_mob_id;
}

MobMemory *mem_data_free;

/* procedures and constants needed for buffering */

BUFFER *buf_free;

MobMemory *new_mem_data ( void )
{
	MobMemory *memory;

	if ( mem_data_free == NULL ) {
		ALLOC_DATA ( memory, MobMemory, 1 );
	} else {
		memory = mem_data_free;
		mem_data_free = mem_data_free->next;
	}

	memory->next = NULL;
	memory->id = 0;
	memory->reaction = 0;
	memory->when = 0;
	VALIDATE ( memory );

	return memory;
}

void recycle_mem_data ( MobMemory *memory )
{
	if ( !IS_VALID ( memory ) )
	{ return; }

	memory->next = mem_data_free;
	mem_data_free = memory;
	INVALIDATE ( memory );
}



/* buffer sizes */
const int buf_size[MAX_BUF_LIST] = {
	16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size ( int val )
{
	int i;

	for ( i = 0; i < MAX_BUF_LIST; i++ ) {
		if ( buf_size[i] >= val ) {
			return buf_size[i];
		}
	}
	return -1;
}

BUFFER *new_buf()
{
	BUFFER *buffer;

	if ( buf_free == NULL ) {
		ALLOC_DATA ( buffer, BUFFER, 1 );
	} else {
		buffer = buf_free;
		buf_free = buf_free->next;
	}

	buffer->next	= NULL;
	buffer->state	= BUFFER_SAFE;
	buffer->size	= get_size ( BASE_BUF );

	ALLOC_DATA ( buffer->string, char, buffer->size );
	buffer->string[0]	= '\0';
	VALIDATE ( buffer );

	return buffer;
}

BUFFER *new_buf_size ( int size )
{
	BUFFER *buffer;

	if ( buf_free == NULL ) {
		ALLOC_DATA ( buffer, BUFFER, 1 ); // -- buffer = (BUFFER*)alloc_perm(sizeof(*buffer));
	} else {
		buffer = buf_free;
		buf_free = buf_free->next;
	}

	buffer->next        = NULL;
	buffer->state       = BUFFER_SAFE;
	buffer->size        = get_size ( size );
	if ( buffer->size == -1 ) {
		log_hd ( LOG_ERROR | LOG_DEBUG, Format ( "new_buf: buffer size %d too large. SUICIDE called.", size ) );
		SUICIDE;
	}
	ALLOC_DATA ( buffer->string, char, buffer->size );
	buffer->string[0]   = '\0';
	VALIDATE ( buffer );

	return buffer;
}


void recycle_buf ( BUFFER *buffer )
{
	if ( !IS_VALID ( buffer ) )
	{ return; }

	PURGE_DATA ( buffer->string );

	buffer->string = NULL;
	buffer->size   = 0;
	buffer->state  = BUFFER_FREED;
	INVALIDATE ( buffer );

	buffer->next  = buf_free;
	buf_free      = buffer;
}


bool add_buf ( BUFFER *buffer, const char *string )
{
	int len;
	char *oldstr;
	int oldsize;

	oldstr = buffer->string;
	oldsize = buffer->size;

	if ( buffer->state == BUFFER_OVERFLOW ) /* don't waste time on bad strings! */
	{ return FALSE; }

	len = strlen ( buffer->string ) + strlen ( string ) + 1;

	while ( len >= buffer->size ) { /* increase the buffer size */
		buffer->size 	= get_size ( buffer->size + 1 );
		{
			if ( buffer->size == -1 ) { /* overflow */
				buffer->size = oldsize;
				buffer->state = BUFFER_OVERFLOW;
				log_hd ( LOG_ERROR, Format ( "buffer overflow past size %d", buffer->size ) );
				return FALSE;
			}
		}
	}

	if ( buffer->size != oldsize ) {
		ALLOC_DATA ( buffer->string, char, buffer->size );

		strcpy ( buffer->string, oldstr );
		PURGE_DATA ( oldstr );
	}

	strcat ( buffer->string, string );
	return TRUE;
}


void clear_buf ( BUFFER *buffer )
{
	buffer->string[0] = '\0';
	buffer->state     = BUFFER_SAFE;
}


char *buf_string ( BUFFER *buffer )
{
	return buffer->string;
}

/* stuff for recycling mobprograms */
MPROG_LIST *mprog_free;

MPROG_LIST *new_mprog ( void )
{
	static MPROG_LIST mp_zero;
	MPROG_LIST *mp;

	if ( mprog_free == NULL ) {
		ALLOC_DATA ( mp, MPROG_LIST, 1 );
	} else {
		mp = mprog_free;
		mprog_free = mprog_free->next;
	}

	*mp = mp_zero;
	mp->vnum             = 0;
	mp->trig_type        = 0;
	mp->code             = assign_string ( "" );
	VALIDATE ( mp );
	return mp;
}

void recycle_mprog ( MPROG_LIST *mp )
{
	if ( !IS_VALID ( mp ) )
	{ return; }

	INVALIDATE ( mp );
	mp->next = mprog_free;
	mprog_free = mp;
}

HELP_AREA * had_free;

HELP_AREA * new_had ( void )
{
	HELP_AREA * had;
	static	HELP_AREA   zHad;

	if ( had_free ) {
		had		= had_free;
		had_free	= had_free->next;
	} else {
		ALLOC_DATA ( had, HELP_AREA, 1 );
	}
	*had = zHad;

	return had;
}

HELP_DATA * help_free;

HELP_DATA * new_help ( void )
{
	HELP_DATA * help;

	if ( help_free ) {
		help		= help_free;
		help_free	= help_free->next;
	} else {
		ALLOC_DATA ( help, HELP_DATA, 1 );
	}
	return help;
}

void recycle_help ( HELP_DATA *help )
{
	PURGE_DATA ( help->keyword );
	PURGE_DATA ( help->text );

	help->next = help_free;
	help_free = help;
}
