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

// poseven
#include "poseven/types/wait_t.hh"


namespace poseven
{
	
	pid_t wait( wait_t& status );
	
	inline wait_result wait()
	{
		wait_result result;
		
		result.pid = wait( result.status );
		
		return result;
	}
	
}

#endif

