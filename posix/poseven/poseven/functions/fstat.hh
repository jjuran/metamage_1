// fstat.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FSTAT_HH
#define POSEVEN_FUNCTIONS_FSTAT_HH

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline struct stat fstat( fd_t fd )
	{
		struct stat stat_buffer;
		
		throw_posix_result( ::fstat( fd, &stat_buffer ) );
		
		return stat_buffer;
	}
	
}

#endif

