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

std::list<std::string>tweetList;


void issueSystemCommandNoReturn ( const std::string &argument )
{
	if ( argument.empty() ) { return; }

	// -- possibly fix a glitch in popen
	fflush ( NULL );

	// -- causes infinite loop! (Drr)
	FILE *fp = popen ( C_STR ( argument ), "r" );
	if ( !fp ) {
		std::cerr << "issueSystemCommand failed  Unable to execute the command string supplied." << std::endl;
		return;
	}

	pclose ( fp );
	return;
}

std::string addTweetHashtags ( const std::string &tweetStr )
{
	std::string retStr = tweetStr;

	// -- add a date-stamp like #27MAY14, etc. Why?  Why not?
	// -- this is 100% happening unless the length will not support it.
	// -- all our tweets will have this otherwise.
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime ( buffer, 80, "%d%b%y", timeinfo );

	// -- make the date first, then the twitter string!  IF we are too long
	// -- already, we don't change the twitter-string here and we just simply
	// -- continue on with the rest of the hashtag process.
	if ( ( strlen ( buffer ) + retStr.length() + 2 ) <= 140 ) {
		retStr = Format ( "#%s ", all_capitalize ( buffer ) );
		retStr.append ( tweetStr );
	}

	// -- now we will attempt to add any other hashtags we can!
	struct twit_types {
		const char *hashtag;
		int chance;
	};

	// -- easy to add more to hashtag!
	const struct twit_types twit_table[] = {
		{ "#MUDS", 2 },					{ "#MUDDING", 30},
		{ "#TEXTGAMES", 5 },			{ "#PLAGUEDONES", 6 },
		{ "#ZOMBIES", 3},				{ "#WALKERS", 10},
		{ "#INFECTEDCITY", 18},			{ "#INFECTENGINE", 18},
		{ "#OLDSCHOOLGAMES", 30},		{ "#GAMES", 20},
		{ "#FREEGAMES", 30},			{ "#RPG", 30},
		{ "#WRITING", 30},				{ "#SCRAWL", 30},
		{ "#INFECT", 30 },				{ "#DEADWORLD", 30 },
		{ "#UNDEAD", 30 },				{ "#SURVIVAL", 75 },
		{ "#HOWDOISURVIVE", 75},		{ "#STALKED", 75 },
		{ "#RAGE", 75 },				{ "#RETROGAMING", 30 },
		{ "#LETTHEREBEZOMBIES", 100}, 	{ "#DEADWALKING", 100 },
		{ "#TEXTFORLIFE", 100},			{ "#CLASSICGAMING", 100 },
		{ "#CLASSICSURVIVAL", 100},		{ "#TBC", 100 },
		{ "#MTF", 100},					{ "#AWESOME", 100},
		{ "#MULTIUSERDUNGEON", 100},	{ "#GAMING", 100 },
		{ NULL, 0 },
	};

	// -- so we don't always add tags.
	if ( Math::instance().range ( 1, 2 ) == Math::instance().range ( 0, 3 ) ) {
		int max_tags = Math::instance().range ( 1, 3 );	// -- limit our tags.
		int current_tags = 0;

		for ( int y = 0; twit_table[y].hashtag != NULL; y++ ) {
			int length = strlen ( twit_table[y].hashtag );
			// -- we are not already part of it.
			if ( !IsSameList ( retStr, twit_table[y].hashtag ) && ( ( retStr.length() + length + 1 ) <= 140 ) ) {
				if ( Math::instance().range ( 0, twit_table[y].chance ) == Math::instance().range ( 0, twit_table[y].chance ) ) {
					retStr.append ( Format ( " %s", twit_table[y].hashtag ) );
					current_tags++;
					// -- so we don't create a million tags for shorter tweets.
					if ( current_tags == max_tags ) {
						break;
					}
				}
			}
		}
	}

	return ( C_STR ( retStr ) );
}

void tweetStatement ( const std::string &tweet )
{
#if !defined (_DEBUG_)
	try {
		// we will move this to a database for security purposes.
		std::string tweetStr = tweet;

		if ( tweet.empty() )
		{ return; }

		if ( tweet.length() > 140 ) {
			// logging/debugging purposes, so we don't exceed our max-length!
			log_hd ( LOG_ERROR, Format ( "tweetStatement: tweet longer then 140 characters in length '%s'", C_STR ( tweet ) ) );
			return;
		}

		tweetStr = addTweetHashtags ( tweetStr );

		// we don't work if we haven't got curl installed (required to tweet)
		tweetList.push_back ( tweetStr );
	} catch ( ... ) {
		CATCH ( false );
	}
#endif
	return;
}

// -- EOF

