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
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF		16384
#define MAX_BUF_LIST 	10
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

/* note recycling */
#define ND NOTE_DATA
ND	*new_note args ( ( void ) );
void	recycle_note args ( ( NOTE_DATA *note ) );
#undef ND

/* ban data recycling */
#define BD Ban
BD	*new_ban args ( ( void ) );
void	recycle_ban args ( ( Ban *ban ) );
#undef BD

/* descriptor recycling */
#define DD Socket
DD	*new_descriptor args ( ( void ) );
void	recycle_descriptor args ( ( Socket *d ) );
#undef DD

/* char gen data recycling */
#define GD GEN_DATA
GD 	*new_gen_data args ( ( void ) );
void	recycle_gen_data args ( ( GEN_DATA * gen ) );
#undef GD

/* extra descr recycling */
#define ED DescriptionData
ED	*new_extra_descr args ( ( void ) );
void	recycle_extra_descr args ( ( DescriptionData *ed ) );
#undef ED

/* affect recycling */
#define AD Affect
AD	*new_affect args ( ( void ) );
void	recycle_affect args ( ( Affect *af ) );
#undef AD

/* object recycling */
#define OD Item
OD	*new_obj args ( ( void ) );
void	recycle_obj args ( ( Item *obj ) );
#undef OD

/* character recyling */
#define CD Creature
#define PD PlayerData
CD	*new_char args ( ( void ) );
void	recycle_char args ( ( Creature *ch ) );
PD	*new_pcdata args ( ( void ) );
void	recycle_pcdata args ( ( PlayerData *pcdata ) );
#undef PD
#undef CD


/* mob id and memory procedures */
#define MD MobMemory
long 	get_pc_id args ( ( void ) );
long	get_mob_id args ( ( void ) );
MD	*new_mem_data args ( ( void ) );
void	recycle_mem_data args ( ( MobMemory *memory ) );
MD	*find_memory args ( ( MobMemory *memory, long id ) );
#undef MD

/* buffer procedures */
BUFFER	*new_buf args ( ( void ) );
BUFFER  *new_buf_size args ( ( int size ) );
void	recycle_buf args ( ( BUFFER *buffer ) );
bool	add_buf args ( ( BUFFER *buffer, const char *string ) );
void	clear_buf args ( ( BUFFER *buffer ) );
char	*buf_string args ( ( BUFFER *buffer ) );

HELP_AREA *	new_had		args ( ( void ) );
HELP_DATA *	new_help	args ( ( void ) );
void		recycle_help	args ( ( HELP_DATA * ) );