// gettimeofday.hh
// ---------------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_GETTIMEOFDAY_HH
#define POSEVEN_FUNCTIONS_GETTIMEOFDAY_HH

// POSIX
#include <sys/time.h>


namespace poseven
{
	
	void gettimeofday( struct timeval& tv );
	
	inline struct timeval gettimeofday()
	{
		struct timeval tv = { 0 };
		
		gettimeofday( tv );
		
		return tv;
	}
	
}

#endif

