/*	=============
 *	TimeKeeper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_TIMEKEEPER_HH
#define GENIE_PROCESS_TIMEKEEPER_HH

// Standard C
#include <stdint.h>


namespace Genie
{
	
	struct Times
	{
		uint64_t user;
		uint64_t system;
		uint64_t child_user;
		uint64_t child_system;
		
		Times() : user(), system(), child_user(), child_system()  {}
	};
	
	class TimeKeeper
	{
		private:
			uint64_t itsLastTimerCheckpoint;
			uint64_t itsLastActivity;
			
			Times itsTimes;
		
		public:
			TimeKeeper();
			
			const uint64_t& GetTimeOfLastActivity() const  { return itsLastActivity; }
			
			const Times& GetTimes() const  { return itsTimes; }
			
			void AccumulateChildTimes( const Times& times );
			
			void EnterSystemCall();
			
			void LeaveSystemCall();
			
			void SuspendTimer();
			void ResumeTimer();
			
	};
	
}

#endif

