// wait.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_WAIT_HH
#define POSEVEN_FUNCTIONS_WAIT_HH

// POSIX
#include <sys/wait.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/wait_t.hh"


namespace poseven
{
	
	inline pid_t wait( wait_t& status )
	{
		return pid_t( throw_posix_result( ::wait( (int*) &status ) ) );
	}
	
	inline wait_result wait()
	{
		wait_result result;
		
		result.pid = wait( result.status );
		
		return result;
	}
	
}

#endif

