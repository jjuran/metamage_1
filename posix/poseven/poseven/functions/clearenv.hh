// clearenv.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CLEARENV_HH
#define POSEVEN_FUNCTIONS_CLEARENV_HH

// Standard C
#include <stdlib.h>

// POSeven
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline void clearenv()
	{
		(void) throw_posix_result( ::clearenv() );
	}
	
}

#endif

