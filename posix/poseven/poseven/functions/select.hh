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
#include <sys/select.h>


namespace poseven
{
	
	unsigned select( unsigned   n_fds,
	                 fd_set    *r  = 0,
	                 fd_set    *w  = 0,
	                 fd_set    *x  = 0,
	                 timeval   *tv = 0 );  // NULL
	
	inline void select( timeval& tv )
	{
		(void) select( 0, 0, 0, 0, &tv );  // NULL
	}
	
}

#endif

