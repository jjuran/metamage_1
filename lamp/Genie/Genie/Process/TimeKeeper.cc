/*	=============
 *	TimeKeeper.cc
 *	=============
 */

#include "Genie/Process/TimeKeeper.hh"

// Standard C
#include <time.h>


namespace Genie
{
	
	TimeKeeper::TimeKeeper()
	:
		itsLastTimerCheckpoint( clock() ),
		itsLastActivity()
	{
		itsTimes.tms_utime  = 0;
		itsTimes.tms_stime  = 0;
		itsTimes.tms_cutime = 0;
		itsTimes.tms_cstime = 0;
	}
	
	void TimeKeeper::AccumulateChildTimes( const Times& times )
	{
		itsTimes.tms_cutime += times.tms_utime + times.tms_cutime;
		itsTimes.tms_cstime += times.tms_stime + times.tms_cstime;
	}
	
	static void UpdateClock( clock_t& clock, uint64_t& checkpoint )
	{
		const uint64_t now = ::clock();
		
		clock += now - checkpoint;
		
		checkpoint = now;
	}
	
	void TimeKeeper::EnterSystemCall()
	{
		UpdateClock( itsTimes.tms_utime, itsLastTimerCheckpoint );
		
		itsLastActivity = itsLastTimerCheckpoint;
	}
	
	void TimeKeeper::LeaveSystemCall()
	{
		UpdateClock( itsTimes.tms_stime, itsLastTimerCheckpoint );
	}
	
	void TimeKeeper::SuspendTimer()
	{
		UpdateClock( itsTimes.tms_stime, itsLastTimerCheckpoint );
	}
	
	void TimeKeeper::ResumeTimer()
	{
		const uint64_t now = clock();
		
		itsLastTimerCheckpoint = now;
	}
	
}

