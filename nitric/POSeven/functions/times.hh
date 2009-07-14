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

// POSeven
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline void times( struct tms& time_set )
	{
		throw_posix_result( ::times( &time_set ) );
	}
	
	inline struct tms times()
	{
		struct tms time_set = { 0 };
		
		times( time_set );
		
		return time_set;
	}
	
}

#endif

