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

// Debug
#include "debug/assert.hh"

// poseven
#ifndef POSEVEN_TYPES_DIR_T_HH
#include "poseven/types/dir_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


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

