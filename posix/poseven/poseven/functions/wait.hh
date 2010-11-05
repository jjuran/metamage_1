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
#ifndef POSEVEN_TYPES_WAIT_T_HH
#include "poseven/types/wait_t.hh"
#endif


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

