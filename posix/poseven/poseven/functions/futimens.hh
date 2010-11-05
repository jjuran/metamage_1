// futimens.hh
// -----------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FUTIMENS_HH
#define POSEVEN_FUNCTIONS_FUTIMENS_HH

// POSIX
#include <sys/stat.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	void futimens( fd_t fd, const timespec times[2] );
	
#ifdef UTIME_OMIT
	
	inline void futimens( fd_t fd, const timespec& mod )
	{
		struct timespec times[2] = { { 0, UTIME_OMIT }, mod };
		
		futimens( fd, times );
	}
	
	inline void futimens( fd_t fd, const time_t& mod_time )
	{
		const timespec mod = { mod_time, 0 };
		
		futimens( fd, mod );
	}
	
#endif
	
}

#endif

