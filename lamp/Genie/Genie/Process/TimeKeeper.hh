/*	=============
 *	TimeKeeper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_TIMEKEEPER_HH
#define GENIE_PROCESS_TIMEKEEPER_HH

// POSIX
#include <sys/times.h>

// Standard C
#include <stdint.h>


namespace Genie
{
	
	typedef tms Times;
	
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

