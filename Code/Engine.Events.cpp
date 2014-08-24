#include "merc.h"


template<>EventManager *Instance<EventManager>::ms_Singleton = 0;

void EventManager::BootupEvents()
{
	// -- Event manager is now being tested with a repeating event every 15 minutes (60*15)
	// -- if all works out, this will announce the time every 15 minutes to all connected sockets.
	addEvent ( new TwitterEvent(), true, EV_MINUTE + ( EV_SECOND * 30 ) );		// Every 1 and a half minutes
	addEvent ( new ExpEvent(), true, EV_MINUTE );					// Every minute
	return;
}

int EventManager::updateEvents ( void )
{
	std::list<Event *>::iterator iter, iter_next;
	try {
		// -- event list glitch protection.
#ifdef _EVENT_COUNTER_
		static  int 	emptyUpdateList;
		if ( mEventList.empty() ) {
			// -- if the list is empty, we give it 10 loops to see if it comes back, then we reboot the mud hard!
			emptyUpdateList++;
			if ( emptyUpdateList > 10 ) {
				log_hd ( LOG_ERROR | LOG_DEBUG, "EventManager::updateEvents: the event list is empty!  Major glitch suspected, shutting down." );
				
				return POLL_ERROR;
			}
			
			return POLL_SUCCESS;
		}
#endif

		// -- Execute our C++ Events
		for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
			Event *ev = ( *iter );
			iter_next = ++iter;
			// no fancy --/++ counters like in most systems; here we use
			// just a simple raw check to see how many seconds have passed
			// min of 0 seconds, max of, well, allot.  Either way, we can be
			// quite creative now with our events.
			if ( ev->hasElapsed() ) {

				// -- February 25, 2014
				// -- Written by: David Simmerson
				// -- if we crash during an event, the core-file will have
				// -- access to the System.pCrash.last_event crash pointer
				// -- so even if we do not receive the most accurate
				// -- core-file, we can at-least find out if we died in an
				// -- event.
				const char* mangled_name = typeid ( *ev ).name();

				int status;
				char* demangled_name = __cxxabiv1::__cxa_demangle ( mangled_name, 0, 0, &status );

				log_hd ( LOG_DEBUG, Format ( "Running Event: (%p)%s", ev, demangled_name ) );
				if ( demangled_name )
				{
					free ( demangled_name );
					demangled_name = NULL;
				}

				// -- add the stack here instead of in all the individual Execute calls..
				// -- of-course, this is a lazy way of doing it and it should probably be
				// -- updated at a later time to fit more appropriately.
				ev->Execute();  // run the event!
				// is the event scheduled to restart?
				if ( ev->isRestart() ) {
					ev->setInitTime ( time ( NULL ) );
					continue;
				}
				mEventList.remove ( ev );
				delete ev;
				ev = NULL;
			}
		}
	} catch ( ... ) {
		CATCH ( false );
	}

	// -- chain so that we don't break the lldb calls.
	tail_chain();
	
	// -- a typical action to ensure the debugger doesn't spaz out like a little hooker.
	return 1;
}

// destroy all events within the system!
void EventManager::destroyEvents ( void )
{
	try {
		std::list<Event *>::iterator iter, iter_next;
		int destroyCounter = 0;

		for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
			Event *ev = ( *iter );
			iter_next = ++iter;
			mEventList.remove ( ev );

			const char* mangled_name = typeid ( *ev ).name();

			int status;
			char* demangled_name = __cxxabiv1::__cxa_demangle ( mangled_name, 0, 0, &status );

			log_hd ( LOG_DEBUG, Format ( "\tEvent: %s(%p) destroyed.", demangled_name, ev ) );

			if ( demangled_name ) { ; }
			{
				free ( demangled_name );
				demangled_name = NULL;
			}
			destroyCounter++;

			delete ev;
			ev = NULL;
		}

		log_hd ( LOG_DEBUG, Format ( "\t%d Events were destroyed.", destroyCounter ) );
		mEventList.clear(); // finally clear the event list
	} catch ( ... ) {
		CATCH ( false );
	}
	return;
}

void EventManager::destroyEvent ( Event *ev )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;
		if ( e == ev ) {
			mEventList.remove ( ev );
			delete ev;
			ev = NULL;
		}
	}
	return;
}

// report our Events to a selected character
// this will allow us to see all the important data!
void EventManager::reportEvents ( Creature *c )
{
	std::list<Event *>::iterator iter, iter_next;
	int cnt = 0;
	double totSeconds = 0;

	if ( !c ) {
		log_hd ( LOG_ERROR, Format ( "Called %s without any Creature data.", __FUNCTION__ ) );
		return;
	}

	if ( !c->desc ) {
		log_hd ( LOG_ERROR, Format ( "Called %s with a Creature with no socket data", __FUNCTION__ ) );
		return;
	}

	writeBuffer ( Format ( "\n\r\a[F355]Events of %s\an\n\r", "The Infected City" ), c );
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *ev = ( *iter );
		iter_next = ++iter;
		double elapsed = difftime ( time ( NULL ), ev->getInitTime() );
		double ExecutesIn = ev->getSeconds() - elapsed;
		totSeconds += ExecutesIn;

		const char* mangled_name = typeid ( *ev ).name();

		int status;
		char* demangled_name = __cxxabiv1::__cxa_demangle ( mangled_name, 0, 0, &status );

		if ( ev->getType() == EV_LUA ) {
			LuaEvent *le = ( LuaEvent * ) ev;
			writeBuffer ( Format ( "\a[F504]Ev: \a[F152]%25s \a[F504] (\a[F531]%p\a[F504]) \a[F504]Repeats: \a[F152]%s - \a[F504]Start: \a[F152]%s - \a[F504]Executes in \a[F152]%f \a[F504]seconds \ay(\a[F500]%s\ay)\a[F504].\an\n\r",
											   status ? mangled_name : demangled_name, ev,
											   ev->isRestart() ? "Yes" : "No",
											   getDateTime ( ev->getInitTime() ), ExecutesIn, C_STR ( le->getScriptName() ) ),c );
		} else {
			writeBuffer ( Format ( "\a[F504]Ev: \a[F152]%25s \a[F504] (\a[F531]%p\a[F504]) \a[F504]Repeats: \a[F152]%s - \a[F504]Start: \a[F152]%s - \a[F504]Executes in \a[F152]%f \a[F504]seconds.\an\n\r",
											   status ? mangled_name : demangled_name, ev,
											   ev->isRestart() ? "Yes" : "No",
											   getDateTime ( ev->getInitTime() ), ExecutesIn ),c );
		}
		cnt++;

		if ( demangled_name ) { ; }
		{
			free ( demangled_name );
			demangled_name = NULL;
		}
	}
	writeBuffer ( Format ( "There are %d events in queue with a total of %f seconds.\an\n\r", cnt, totSeconds ),c );
	return;
}

Event *EventManager::addEvent ( Event *ev, bool repeat, double seconds )
{
	std::list<Event *>::iterator iter, iter_next;

	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;
		// already in the event list!
		if ( e == ev ) {
			return ev;
		}
	}
	// should we reach this point in time.
	mEventList.push_back ( ev );
	ev->setRestart ( repeat );
	ev->setSeconds ( seconds );
	ev->setInitTime ( time ( NULL ) );
	ev->setType ( EV_CPP );
	
	return ev;
}

void EventManager::stopEventType ( int ev_type )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;
		// -- already in the event list!
		if ( e->getType() == ev_type ) {
			// -- attempt to run the script name
			if ( e->getType() == EV_LUA ) {
				// -- brace for impact, this has the potential to crash the mud if something
				// -- is not flagged properly and the system assumes it is a LUA event when it
				// -- really isn't.
//				LuaEvent *ev = ( LuaEvent * ) e;
				// -- force the event to run, that way we can kill out all the co-routines
				// -- attached to it following execution.
				// -- this *SHOULD* allow for missions to not be corrupted.
//				if ( ScriptEngine::instancePtr() )
//				{ ScriptEngine::instance().runEvent ( ev->getScriptName() ); }
			}
			mEventList.remove ( e );
			delete e;
		}
	}
	return;
}

void EventManager::newEventTime ( const std::string &name, int seconds )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;
		// -- attempt to run the script name

		if ( e->getType() == EV_LUA ) {
			// -- brace for impact, this has the potential to crash the mud if something
			// -- is not flagged properly and the system assumes it is a LUA event when it
			// -- really isn't.
			LuaEvent *ev = ( LuaEvent * ) e;
			if ( SameString ( ev->getScriptName(), name ) ) {
				ev->setSeconds ( seconds );
			}
		}
	}
	return;
}


void EventManager::purgeFromOwner ( Creature *c )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;

		if ( e->cOwner == c ) {
			mEventList.remove ( e );
			delete e;
			continue;
		}
	}
	return;
}

void EventManager::extractOnDeath ( Creature *c )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;

		if ( e->cOwner == c && e->cExtractOnDeath ) {
			mEventList.remove ( e );
			delete e;
			continue;
		}
	}
	return;
}



void EventManager::purgeFromOwner ( Item *i )
{
	std::list<Event *>::iterator iter, iter_next;
	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;

		if ( e->iOwner == i ) {
			mEventList.remove ( e );
			delete e;
			continue;
		}
	}
}

void EventManager::purgeFromOwner ( RoomData *r )
{
	std::list<Event *>::iterator iter, iter_next;

	if ( r == NULL )
	{  return; }

	if ( mEventList.empty() ) {
		return;
	}

	for ( iter = mEventList.begin(); iter != mEventList.end(); iter = iter_next ) {
		Event *e = ( *iter );
		iter_next = ++iter;

		if ( e->rOwner == r ) {
			mEventList.remove ( e );
			delete e;
			continue;
		}
	}
	return;
}

Event::Event() : mInitTime ( 0 ), mSecondsToExecute ( 0 ), mWillRestart ( 0 )
{

	cOwner = NULL;
	cExtractOnDeath = false;
	iOwner = NULL;
	rOwner = NULL;

}
Event::~Event() { }

void Event::addData ( void *dataPtr, short pos )
{
	if ( pos > MAX_EVENT_DATA ) {
		log_hd ( LOG_ERROR, Format ( "Event data position would exceed max of %d.", MAX_EVENT_DATA ) );
		throw ( "Events Broken: Exceeded MAX data position!" );
		return;
	}
	mDataTable[pos] = dataPtr;
	return;
}

void TwitterEvent::Execute ( void )
{
#if !defined (_DEBUG_)	// only use our twitter event on our REAL mud, not the DEV mud!
	try {
		extern std::list<std::string>tweetList;
		std::string tweetStr;

		std::list<std::string>::iterator iter, itern;

		if ( tweetList.empty() )
		{ return; }

		for ( iter = tweetList.begin(); iter != tweetList.end(); iter = itern ) {
			tweetStr = ( *iter ).c_str();


			// -- no longer using the standard IssueSystemCommand to prevent the fgetf crash bug.
			issueSystemCommandNoReturn ( Format ( "curl -u %s:%s -d \"status=%s\" http://localhost:8080/1.1/statuses/update.json",
												  "CombatMUD", "temppassword",
												  C_STR ( tweetStr ) ) );
			// -- change scope.
			{
				tweetList.erase ( iter );
			}
			// -- again change scope.
			{
				// saveTweets();
			}
			itern = ++iter;
			break;
		}
	} catch ( ... ) {
		CATCH ( false );
	}
#endif
	return;
}

// -- -------------------------------------------------------------
// -- EXP Bucket system, so experience gains happen periodically
// -- and not all at once.  Ie, it can take some time to level up!
// -- -------------------------------------------------------------
void ExpEvent::Execute ( void )
{
	Creature *ch, *ch_n;
	for ( ch = char_list; ch; ch = ch_n) {
		ch_n = ch->next;

		if ( !ch )	{ SUICIDE; }				// -- the impossible has happened
		if ( IS_NPC(ch)) { continue; }				// -- NPC's do not gain
		if ( ch->level >= MAX_LEVEL ) { continue; }		// -- Max Level does not gain
		if ( ch->fighting ) { continue; }			// -- do not gain while in combat.

		int totalGain = 0;
		if ( number_range ( 0, 3 ) == number_range ( 0, 7 ) ) {
			if ( ch->exp_pool != 0 && ch->exp < ( ch->level * 200 ) ) {
				ch->exp_pool--;
				totalGain++;

				// -- chance for the bonus experience
				if ( ch->exp_pool > 30 && number_range ( 0, 3 ) == number_range ( 0, 4 ) ) {
					int jump = number_range ( 1, 30 );
					ch->exp_pool = ( ch->exp_pool - jump );

					totalGain += jump;

					// -- bonus experience to help levelling along
					if ( number_range ( 0, 3 ) == number_range ( 0, 15 ) ) {
						ch->exp += ch->level;
						totalGain += ch->level;
					}
				}

				// -- lets try and give out some extra exp, leveling is crazy hard
				// -- when all you get is 1xp every now and then.
				if ( ch->exp_pool > 10 ) {
					int jm = number_range ( 1, 10 );
					ch->exp_pool = ( ch->exp_pool - jm );
					totalGain += jm;
				}

				// -- assign the experience properly.
				ch->exp += totalGain;
	//			update_board ( ch, totalGain, BOARD_EXP );

				writeBuffer ( Format ( "\n\r\ac*** \aYYour survival in \aG%s \aYhas yielded the reward of \aR%d\aY experience gained \ac***\an\n\r", "The Infected City", totalGain ), ch );

				if ( ch->level < MAX_LEVEL && ch->exp >= ( ch->level * 200 ) ) {
					ch->level++;

					// -- announce our level gain!
					announce ( Format ( "%s has attained level %d", ch->name, ch->level ) );

					switch ( ch->level ) {
						default:
							break;
						case 5:
							tweetStatement ( Format ( "%s has survived the first 5 levels of %s.", 
								ch->name, "The Infected City" ) );
							break;
						case 20:
							tweetStatement ( Format ( "%s has achieved level 20 in %s.", ch->name, "The Infected City" ) );
							break;
						case 40:
							tweetStatement ( Format ( "Level 40: %s has continued to survive in %s.",ch->name, "The Infected City" ) );
							break;
						case 50:
							tweetStatement ( Format ( "Level 50: %s is a true suvivor of %s.", ch->name, "The Infected City"  ) );
							break;
						case 51:
							tweetStatement ( Format ( "Level 51: %s is on the path to awesomeness in %s.", ch->name, "The Infected City"  ) );
							break;
						case 52:
							tweetStatement ( Format ( "Level 52: %s has attained a truely amazing status in %s.", ch->name, "The Infected City"  ) );
							break;
						case 53:
							tweetStatement ( Format ( "Level 53: %s has completed 93 levels of %s.", ch->name, "The Infected City"  ) );
							break;
						case 54:
							tweetStatement ( Format ( "Level 54: %s has reached a powerful level of %s.", ch->name, "The Infected City"  ) );
							break;
						case 55:
							tweetStatement ( Format ( "Level 55: %s has truly proven their worth in %s.", ch->name, "The Infected City"  ) );
							break;
						case 56:
							tweetStatement ( Format ( "Level 56: %s survived 96 levels within %s.", ch->name, "The Infected City"  ) );
							break;
						case 57:
							tweetStatement ( Format ( "Level 57: %s endured 97 levels of %s.", ch->name, "The Infected City"  ) );
							break;
						case 58:
							tweetStatement ( Format ( "Level 58: %s has true suvivor instinct within %s.", ch->name, "The Infected City"  ) );
							break;
						case 59:
							tweetStatement ( Format ( "Level 59: %s is one step away from MAX LEVEL!", ch->name ) );
							break;
						case 60:
							tweetStatement ( Format ( "*** MAX LEVEL ATTAINED! *** %s has become a master survivor in %s. ", ch->name, "The Infected City"  ) );
							break;
					} // -- end switch

					advance_level(ch, false);
					ch->exp = 0;
				} // -- end level gain
			} // -- end can we gain exp
		} // -- end random chance

		// -- random standing experience gain (It just happens)
		if ( number_range ( 0, 115 ) == number_percent() ) {
			ch->exp_pool += number_range ( number_range ( 0, 3 ), number_range ( 3, 9 ) );
		}

		// -- save our character
		SAVE_CHANCE ( ch );
	} // -- end for-loop
	return;
}

