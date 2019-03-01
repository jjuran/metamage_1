/*
	request_timed_wakeup.cc
	-----------------------
*/

// Standard C
#include <stdint.h>

// relix-api
#include "relix/api/request_timed_wakeup.hh"

// Pedestal
#include "Pedestal/Application.hh"


namespace relix
{
	
	namespace Ped = Pedestal;
	
	/*
		Ticks are exactly 1/60 second in OS X, but not in OS 9.
		Here we use neither, but a powers-of-two approximation.
		
		In our approximation, there are 16912.5 microseconds per tick, which
		exceeds the actual ~16625.8 in classic Mac OS or ~16666.7 in OS X.
		
		The number of OS 9 ticks is slightly larger, since OS 9 ticks are
		slightly smaller and a few more of them are needed to fill a certain
		length of time.
		
		So our delay will be short-changed, but that's okay because callers
		are already expecting early wakeups and will keep recomputing it.
		As the delay approaches zero, the error becomes insignificant,
		and callers will keep looping until the delay becomes zero anyway.
	*/
	
	void request_timed_wakeup( unsigned long long microseconds )
	{
		const uint32_t max_sleep_ticks = 0x7FFFFFFF;
		const uint64_t max_microseconds = max_sleep_ticks * 1000000ull / 60;
		
		uint32_t sleep_ticks = max_sleep_ticks;
		
		if ( microseconds <= max_microseconds )
		{
			sleep_ticks = (microseconds - microseconds / 32) / 16384;
		}
		
		Ped::AdjustSleepForTimer( sleep_ticks );
	}
	
}
