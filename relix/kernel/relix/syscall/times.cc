/*
	times.cc
	--------
*/

#include "relix/syscall/times.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"


namespace relix
{
	
	static clock_t the_start_time = clock();
	
	
	clock_t times( struct tms* tp )
	{
		if ( tp != NULL )
		{
			*tp = current_process().get_times();
		}
		
		return clock() - the_start_time;
	}
	
}

