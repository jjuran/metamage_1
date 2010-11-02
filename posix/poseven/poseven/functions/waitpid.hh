// waitpid.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_WAITPID_HH
#define POSEVEN_FUNCTIONS_WAITPID_HH

// poseven
#ifndef POSEVEN_TYPES_WAIT_T_HH
#include "poseven/types/wait_t.hh"
#endif


namespace poseven
{
	
	pid_t waitpid( pid_t pid, wait_t& status, int options = 0 );
	
	inline wait_result waitpid( pid_t pid, int options = 0 )
	{
		wait_result result;
		
		result.pid = waitpid( pid, result.status, options );
		
		return result;
	}
	
}

#endif

