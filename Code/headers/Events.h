#ifndef _Events_Hpp
#define _Events_Hpp

class Event
{
	public:
		Event();
		virtual ~Event();
		Creature* cOwner;
		RoomData *rOwner;
		Item *iOwner; // -- Item *
		bool cExtractOnDeath;

	protected:
		time_t  mInitTime;  // the initiation time for the event
		double  mSecondsToExecute; // suspect time of ending
		void   *mDataTable[MAX_EVENT_DATA]; // max of MAX_DATA types of inputted data
		bool    mWillRestart;  // will it restart when it is done?
		int 	mEvType;
	public:
		virtual void Execute ( void ) = 0;
		void setType ( int t ) { mEvType = t; }
		int getType ( void ) { return mEvType; }

		void setInitTime ( time_t t )
		{
			mInitTime = t;
		}
		void setSeconds ( double t )
		{
			mSecondsToExecute = t;
		}

		double getSeconds ( void )
		{
			return mSecondsToExecute;
		}

		time_t getInitTime ( void )
		{
			return mInitTime;
		}

		void setOwner ( Creature *c ) { cOwner = c; rOwner = NULL; iOwner = NULL; }

		void setOwner ( Item *i ) { iOwner = i; cOwner = NULL; rOwner = NULL; }
		void setOwner ( RoomData *r ) { rOwner = r; cOwner = NULL; iOwner = NULL; }

		void extractOnDeath ( bool t ) { cExtractOnDeath = t; }
		void addData ( void *dataPtr, short pos );

		bool hasElapsed ( void )   // has the event elapsed its seconds requirements?
		{
			double elapsed = difftime ( time ( NULL ), mInitTime );
			if ( elapsed >= mSecondsToExecute )
			{ return true; }
			return false;
		}
		bool isRestart ( void )
		{
			return mWillRestart;
		}
		void setRestart ( bool t )
		{
			mWillRestart = t;
		}
};


class EventManager :  public Instance<EventManager>
{
	public:
		EventManager() { }
		~EventManager()
		{
			destroyEvents();
		}
	private:
		EventManager ( const EventManager& );
		EventManager&operator= ( const EventManager& );

		std::list<Event*>mEventList;
	public:
		std::list<Event *>::iterator getFirst() { return mEventList.begin(); }
		std::list<Event *>::iterator getLast() { return mEventList.end(); }
		void BootupEvents();
		int updateEvents ( void );
		void destroyEvents ( void );
		void destroyEvent ( Event *ev );
		Event *addEvent ( Event *ev, bool repeat, double seconds ); // return the event so we can store certain events easily
		// within structures for future usage.
		void reportEvents ( Creature *c );
		void stopEventType ( int ev_type );
		void newEventTime ( const std::string &name, int seconds );
		void purgeFromOwner ( Creature *c );
		void purgeFromOwner ( RoomData *r );
		void purgeFromOwner ( Item *i );
		void extractOnDeath ( Creature *c );

		/*
				void purgeFromOwner(Item *i);
				void purgeFromOwner(RoomData *r);
		*/
};

// -- this will speed up the process ever so slightly
// -- for the overall development of our event structures.
#define DEFINE_EVENT(EventClass) \
	public: \
	EventClass() { }; \
	~EventClass() { }; \
	void Execute(void) \
	 
class LuaEvent : public Event
{
                DEFINE_EVENT ( LuaEvent );
        private:
               	std::string mScriptName;
        public:
               	void setScriptName ( const char *mName ) { mScriptName = mName; }
                std::string getScriptName ( void ) { return mScriptName; }
};


class ExpEvent : public Event
{
	DEFINE_EVENT( ExpEvent );
};

class TwitterEvent : public Event
{
	DEFINE_EVENT ( TwitterEvent );
};

#endif

