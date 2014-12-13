#include "Copyright.h"
#include "Engine.h"

DefineCommand ( cmd_reboot )
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
				if ( ch->invis_level < LEVEL_HERO ) {
					cmd_function ( ch, &cmd_echo, Format ( "Reboot by %s.", ch->name ) );
				}

				{
					const char *twit_table[] = {
						"<< Infect Engine is undergoing a #reboot for #maintenance!",
						"<< Infect Engine will now #reboot, please check back shortly!!",
						"<< Infect Engine Core is rebooting for #maintenance",
						"<< The mud is rebooting for server #maintenance!",
						Format ( "<< %s is undergoing a #temporary #reboot!", "The Infected City" ),
						Format ( "<< %s(%s) is undergoing a #reboot!", "The Infected City", getVersion() ),
						Format ( "<< %s will reboot!", "The Infected City" ),
						Format ( "<< %s: %s -- #reboot!", "The Infected City", getVersion() ),
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
		}
	} else {
		// -- set up the query prompt
		ch->queries.queryfunc = cmd_reboot;
		strcpy ( ch->queries.queryprompt, "Are you sure you want to reboot the MUD? (Y/n)" );
		ch->queries.querycommand = 307;
	}
	return;
}
