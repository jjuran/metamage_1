/*
	alarm_clock.cc
	--------------
*/

#include "relix/task/alarm_clock.hh"

// Standard C
#include <time.h>


namespace relix
{
	
	typedef unsigned long long uint64_t;
	
	
	unsigned alarm_clock::set( unsigned seconds )
	{
		const uint64_t million = 1000000;
		
		const uint64_t now = clock();
		
		unsigned int remainder = 0;
		
		if ( its_time )
		{
			remainder = (its_time - now) / million + 1;
		}
		
		its_time = seconds ? now + seconds * million : 0;
		
		return remainder;
	}
	
	bool alarm_clock::check()
	{
		if ( its_time )
		{
			const uint64_t now = clock();
			
			if ( now > its_time )
			{
				its_time = 0;
				
				return true;
			}
		}
		
		return false;
	}
	
}

