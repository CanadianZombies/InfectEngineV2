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

template<>Math *Instance<Math>::ms_Singleton = 0;

Math::Math()
{
	mTimeSeed = time ( NULL ) ^ getpid();
	mLastGen = 0;

	// -- generate our time
	srandom ( mTimeSeed );
}

Math::~Math() { }

int Math::fuzzy ( int number )
{
	switch ( bits ( 2 ) ) {
		case 0:
			number -= 1;
			break;
		case 3:
			number += 1;
			break;
	}

	return UMAX ( 1, number );
}

int Math::range ( int lower, int higher )
{
	int power;
	int number;

	if ( lower == 0 && higher == 0 )
	{ return 0; }

	if ( ( higher = higher - lower + 1 ) <= 1 )
	{ return lower; }

	for ( power = 2; power < higher; power <<= 1 )
		;

	while ( ( number = generate() & ( power - 1 ) ) >= higher )
		;

	return lower + number;
}

long Math::range ( long lower, long higher )
{
	long power;
	long number;

	if ( lower == 0 && higher == 0 )
	{ return 0; }

	if ( ( higher = higher - lower + 1 ) <= 1 )
	{ return lower; }

	for ( power = 2; power < higher; power <<= 1 )
		;

	while ( ( number = generate() & ( power - 1 ) ) >= higher )
		;

	return lower + number;
}

int Math::percent ( void )
{
	return range ( 1, 100 );
}

int Math::door ( void )
{
	int door;

	while ( ( door = generate() & ( 8 - 1 ) ) > 5 )
		;

	return door;
}

int Math::bits ( int number )
{
	return generate( ) & ( ( 1 << number ) - 1 );
}

long Math::generate ( void )
{
	long mLastGen = ( random() >> 6 );
	return mLastGen;
}

int Math::dice ( int number, int size )
{
	int idice;
	int sum;

	switch ( size ) {
		case 0:
			return 0;
		case 1:
			return number;
	}

	for ( idice = 0, sum = 0; idice < number; idice++ )
	{ sum += range ( 1, size ); }

	return sum;
}

int Math::interpolate ( int level, int value_00, int value_32 )
{
	return value_00 + level * ( value_32 - value_00 ) / 32;
}

int Math::interpolate64 ( int level, int value_00, int value_32 )
{
	return value_00 + level * ( value_32 - value_00 ) / 64;
}

bool Math::isBetween(int low, int high, int amount) {
	if(low >= amount && amount <= high)
	{ return true; }
	return false;
}

bool Math::isBetween(long low, long high, long amount) {
	if(low >= amount && amount <= high)
	{ return true; }
	return false;
}

bool Math::isBetween(double low, double high, double amount) {
	if(low >= amount && amount <= high)
	{ return true; }
	return false;
}
