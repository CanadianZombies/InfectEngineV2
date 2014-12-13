#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_twitlist )
{
	extern std::list<std::string>tweetList;
	if ( IS_NULLSTR ( argument ) ) {
		std::string output ( "" );
		std::list<std::string>::iterator iter, itern;
		int cnt = 0;
		for ( iter = tweetList.begin(); iter != tweetList.end(); iter = itern ) {
			std::string s = ( *iter ).c_str();
			itern = ++iter;

			output.append ( Format ( "[%2d] - %s\n\r", cnt, C_STR ( s ) ) );
			cnt++;
		}

		output.append ( "Type /tweetlist [number] to remove a selected tweet.\n\r" );
		writePage ( C_STR ( output ), ch );
		return;
	}

	if ( !is_number ( argument ) ) {
		writeBuffer ( "Tweets are numbered, please select the tweet you want to remove.\n\r", ch );
		return;
	}

	std::list<std::string>::iterator iter, itern;
	int cnt = 0;
	for ( iter = tweetList.begin(); iter != tweetList.end(); iter = itern ) {
		std::string s = ( *iter ).c_str();
		itern = ++iter;

		if ( cnt == atoi ( argument ) ) {
			tweetList.remove ( s );
			writeBuffer ( "Tweet removed from list.\n\r", ch );
			return;
		}
		cnt++;
	}
	writeBuffer ( "Tweet not found.\n\r", ch );
	return;
}

