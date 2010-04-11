// copyfile.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_COPYFILE_HH
#define POSEVEN_FUNCTIONS_COPYFILE_HH

// compat
#include "lamp/copyfile.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void copyfile( const char* from, const char* to )
	{
		throw_posix_result( ::copyfile( from, to ) );
	}
	
}

#endif

