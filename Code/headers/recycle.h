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
