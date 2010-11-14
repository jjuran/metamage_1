/*	=============
 *	TimeKeeper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_TIMEKEEPER_HH
#define GENIE_PROCESS_TIMEKEEPER_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Genie
{
	
	struct Times
	{
		UInt64 user;
		UInt64 system;
		UInt64 child_user;
		UInt64 child_system;
		
		Times() : user(), system(), child_user(), child_system()  {}
	};
	
	class TimeKeeper
	{
		private:
			UInt64 itsLastTimerCheckpoint;
			UInt64 itsLastResume;
			UInt64 itsLastActivity;
			
			Times itsTimes;
		
		public:
			TimeKeeper();
			
			const UInt64& GetTimeOfLastResume() const  { return itsLastResume; }
			
			const UInt64& GetTimeOfLastActivity() const  { return itsLastActivity; }
			
			const Times& GetTimes() const  { return itsTimes; }
			
			void AccumulateChildTimes( const Times& times );
			
			void EnterSystemCall();
			
			void LeaveSystemCall();
			
			void SuspendTimer();
			void ResumeTimer();
			
	};
	
}

#endif

