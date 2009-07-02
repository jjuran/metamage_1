// dirfd.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_DIRFD_HH
#define POSEVEN_FUNCTIONS_DIRFD_HH

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/types/dir_t.hh"
#include "POSeven/types/fd_t.hh"


#ifdef dirfd

inline int dirfd_function( DIR* dir )
{
	return dirfd( dir );
}

#undef dirfd

inline int dirfd( DIR* dir )
{
	return dirfd_function( dir );
}

#endif

namespace poseven
{
	
	inline fd_t dirfd( dir_t dir )
	{
		ASSERT( dir != NULL );
		
		return fd_t( ::dirfd( dir ) );
	}
	
}

#endif

