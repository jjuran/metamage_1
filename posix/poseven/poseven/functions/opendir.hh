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

// poseven
#include "poseven/functions/closedir.hh"


namespace poseven
{
	
	inline nucleus::owned< dir_t > opendir( const char* pathname )
	{
		DIR* handle = ::opendir( pathname );
		
		if ( handle == NULL )
		{
			throw_errno( errno );
		}
		
		return nucleus::owned< dir_t >::seize( handle );
	}
	
	inline nucleus::owned< dir_t > opendir( const std::string& pathname )
	{
		return opendir( pathname.c_str() );
	}
	
}

#endif

