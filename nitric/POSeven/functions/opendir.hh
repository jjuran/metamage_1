// opendir.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPENDIR_HH
#define POSEVEN_FUNCTIONS_OPENDIR_HH

// Standard C++
#include <string>

// POSeven
#include "POSeven/functions/closedir.hh"


namespace poseven
{
	
	inline Nucleus::Owned< dir_t > opendir( const char* pathname )
	{
		DIR* handle = ::opendir( pathname );
		
		if ( handle == NULL )
		{
			throw_errno( errno );
		}
		
		return Nucleus::Owned< dir_t >::Seize( handle );
	}
	
	inline Nucleus::Owned< dir_t > opendir( const std::string& pathname )
	{
		return opendir( pathname.c_str() );
	}
	
}

#endif

