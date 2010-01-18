/*	=============
 *	TimeKeeper.cc
 *	=============
 */

#include "Genie/Process/TimeKeeper.hh"

// Nitrogen
#include "Nitrogen/Timer.h"


namespace Genie
{
	
	static inline UInt64 GetTimer()
	{
		return Nitrogen::Microseconds();
	}
	
	TimeKeeper::TimeKeeper()
	:
		itsLastTimerCheckpoint( GetTimer() ),
		itsLastResume(),
		itsLastActivity()
	{
	}
	
	void TimeKeeper::AccumulateChildTimes( const Times& times )
	{
		itsTimes.child_user   += times.user   + times.child_user;
		itsTimes.child_system += times.system + times.child_system;
	}
	
	static void UpdateClock( UInt64& clock, UInt64& checkpoint )
	{
		UInt64 now = GetTimer();
		
		clock += now - checkpoint;
		
		checkpoint = now;
	}
	
	void TimeKeeper::EnterSystemCall( const char* name )
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
		UInt64 now = GetTimer();
		
		itsLastResume = now;
		
		itsLastTimerCheckpoint = now;
	}
	
}

