#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_shutdown )
{
	extern bool is_shutdown;
	Socket *d, *d_next;
	Creature *vch;

	if ( cmd == 307 ) {
		switch ( argument[0] ) {
			default:
				ch->queries.querycommand = 0;
				break;
			case 'y':
			case 'Y':
				// -- just make sure it is in the log properly.
				log_hd ( LOG_SECURITY, Format ( "%s has agree'd to shutdown InfectEngine", ch->name ) );

				append_file ( ch, SHUTDOWN_FILE, Format ( "Shutdown by %s.", ch->name ) );
				if ( ch->invis_level < LEVEL_HERO ) {
					cmd_function ( ch, &cmd_echo, Format ( "Shutdown by %s.", ch->name ) );
				}

				{
					const char *twit_table[] = {
						"<< Infect Engine is undergoing a #shutdown for #maintenance!",
						"<< Infect Engine will now #hotreboot, please check back shortly!!",
						"<< Infect Engine Core is shutting down for #maintenance",
						"<< The mud is shutting down for server #maintenance!",
						Format ( "<< %s is undergoing a #temporary #shutdown!", "The Infected City" ),
						Format ( "<< %s(%s) is undergoing a #shutdown!", "The Infected City", getVersion() ),
						Format ( "<< %s will shutdown!", "The Infected City" ),
						Format ( "<< %s: %s -- #shutdown!", "The Infected City", getVersion() ),
						NULL,
					};

					int cnt = 0;
					for ( int y = 0; twit_table[y] != NULL; y++ )
					{ cnt++; }

					std::string tweetStr = twit_table[Math::instance().range ( 0, ( cnt - 1 ) )];
					tweetStr = addTweetHashtags ( tweetStr );
					issueSystemCommandNoReturn ( Format ( "curl -u %s:%s -d \"status=%s\" http://localhost:8080/1.1/statuses/update.json",
														  "CombatMUD", "temppassword", C_STR ( tweetStr ) ) );

				}

				is_shutdown = TRUE;
				for ( d = socket_list; d != NULL; d = d_next ) {
					d_next = d->next;
					vch = d->original ? d->original : d->character;
					if ( vch != NULL )
					{ save_char_obj ( vch ); }
					close_socket ( d );
				}
				break;
		}
	} else {
		// -- set up the query prompt
		ch->queries.queryfunc = cmd_shutdown;
		strcpy ( ch->queries.queryprompt, "Are you sure you want to shutdown the MUD? (Y/n)" );
		ch->queries.querycommand = 307;
	}
	return;
}
