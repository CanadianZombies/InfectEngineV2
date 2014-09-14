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
		DEFINE_EVENT ( ExpEvent );
};

class TwitterEvent : public Event
{
		DEFINE_EVENT ( TwitterEvent );
};

class TimeEvent : public Event
{
		DEFINE_EVENT ( TimeEvent );
};

#endif

