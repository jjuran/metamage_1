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
	}
	
	void TimeKeeper::AccumulateChildTimes( const Times& times )
	{
		itsTimes.child_user   += times.user   + times.child_user;
		itsTimes.child_system += times.system + times.child_system;
	}
	
	static void UpdateClock( uint64_t& clock, uint64_t& checkpoint )
	{
		const uint64_t now = ::clock();
		
		clock += now - checkpoint;
		
		checkpoint = now;
	}
	
	void TimeKeeper::EnterSystemCall()
	{
		UpdateClock( itsTimes.user, itsLastTimerCheckpoint );
		
		itsLastActivity = itsLastTimerCheckpoint;
	}
	
	void TimeKeeper::LeaveSystemCall()
	{
		UpdateClock( itsTimes.system, itsLastTimerCheckpoint );
	}
	
	void TimeKeeper::SuspendTimer()
	{
		UpdateClock( itsTimes.system, itsLastTimerCheckpoint );
	}
	
	void TimeKeeper::ResumeTimer()
	{
		const uint64_t now = clock();
		
		itsLastTimerCheckpoint = now;
	}
	
}

