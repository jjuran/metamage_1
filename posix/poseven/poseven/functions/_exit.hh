// _exit.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SYSEXIT_HH
#define POSEVEN_FUNCTIONS_SYSEXIT_HH

// POSIX
#include <unistd.h>

// poseven
#ifndef POSEVEN_TYPES_EXIT_T_HH
#include "poseven/types/exit_t.hh"
#endif


namespace poseven
{
	
	inline void _exit( exit_t status )
	{
		::_exit( status );
	}
	
}

#endif

