// times.hh
// --------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_TIMES_HH
#define POSEVEN_FUNCTIONS_TIMES_HH

// POSIX
#include <sys/times.h>


namespace poseven
{
	
	void times( struct tms& time_set );
	
	inline struct tms times()
	{
		struct tms time_set = { 0 };
		
		times( time_set );
		
		return time_set;
	}
	
}

#endif

