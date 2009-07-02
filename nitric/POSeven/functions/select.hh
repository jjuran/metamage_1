// select.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SELECT_HH
#define POSEVEN_FUNCTIONS_SELECT_HH

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/fd_t.hh"


namespace poseven
{
	
	inline unsigned select( unsigned   n_fds,
	                        fd_set    *r  = NULL,
	                        fd_set    *w  = NULL,
	                        fd_set    *x  = NULL,
	                        timeval   *tv = NULL )
	{
		return throw_posix_result( ::select( n_fds, r, w, x, tv ) );
	}
	
	inline void select( timeval& tv )
	{
		(void) select( 0, NULL, NULL, NULL, &tv );
	}
	
}

#endif

