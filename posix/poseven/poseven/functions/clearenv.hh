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

// POSIX
#include <stdlib.h>

// poseven
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif


namespace poseven
{
	
#if !defined( __APPLE__ )  &&  !defined( __CYGWIN__ )
	
	inline void clearenv()
	{
		(void) throw_posix_result( ::clearenv() );
	}
	
#endif
	
}

#endif

