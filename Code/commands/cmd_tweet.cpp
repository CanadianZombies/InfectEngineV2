#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_tweet )
{
	if ( cmd == 1000 ) {
		if ( LOWER ( argument[0] ) == 'y' ) {
			log_hd ( LOG_DEBUG, Format ( "%s has added '%s' to the twitlist query!", ch->name, ch->queries.query_string ) );
			// -- tweet on behalf of The Infected City
			tweetStatement ( ch->queries.query_string );
			writeBuffer ( "\r\nTweet has been added to the queue.\r\n", ch );

			// -- remove our old data
			delete ch->queries.query_string;
			ch->queries.query_string = NULL;

			ch->queries.querycommand = 0;
			return;
		}
		// -- we selected anything BUT the Y option, that means
		// -- we are NOT doing it, so we clear out the memory applicable
		// -- and attempt to avoid memory issues later on.
		ch->queries.querycommand = 0;
		delete ch->queries.query_string;
		ch->queries.query_string = NULL;
		return;
	}

	if ( IS_NULLSTR ( argument ) ) {
		writeBuffer ( "Syntax: tweet <message under 140 characters>\n\r", ch );
		return;
	}

	if ( strlen ( argument ) > 140 ) {
		writeBuffer ( "Tweets must be under 140 characters in length.\n\r", ch );
		return;
	}


	ch->queries.queryfunc = cmd_tweet;
	strcpy ( ch->queries.queryprompt, Format ( "Are you sure you want to tweet '%s' behalf of %s? (y/n)>", argument, "The Infected City" ) );

	// -- Assign the new new string to push
	delete ch->queries.query_string;
	ch->queries.query_string = NULL;

	// -- push the new query data
	ch->queries.query_string = assign_string ( argument );
	ch->queries.querycommand = 1000;
	return;
}

