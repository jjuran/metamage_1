/*	=============
 *	TimeKeeper.cc
 *	=============
 */

#include "Genie/Process/TimeKeeper.hh"

// Standard C
#include <time.h>

// relix
#include "relix/task/schedule.hh"


namespace Genie
{
	
	TimeKeeper::TimeKeeper()
	:
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
	
	void TimeKeeper::EnterSystemCall()
	{
		itsTimes.tms_utime += relix::checkpoint_delta();
		
		itsLastActivity = ::clock();
	}
	
	void TimeKeeper::LeaveSystemCall()
	{
		itsTimes.tms_stime += relix::checkpoint_delta();
	}
	
	void TimeKeeper::SuspendTimer()
	{
		itsTimes.tms_stime += relix::checkpoint_delta();
	}
	
	void TimeKeeper::ResumeTimer()
	{
		(void) relix::checkpoint_delta();
	}
	
}

