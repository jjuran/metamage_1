/*
	checkpoint_delta.cc
	-------------------
*/

#include "relix/time/checkpoint_delta.hh"

// Standard C
#include <time.h>


namespace relix
{
	
	static clock_t the_last_checkpoint_time;
	
	
	clock_t checkpoint_delta()
	{
		const clock_t now = ::clock();
		
		const clock_t delta = now - the_last_checkpoint_time;
		
		the_last_checkpoint_time = now;
		
		return delta;
	}
	
}
