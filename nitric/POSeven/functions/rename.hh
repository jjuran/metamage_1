// rename.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_RENAME_HH
#define POSEVEN_FUNCTIONS_RENAME_HH

// Standard C++
#include <string>

// POSIX
#include <stdio.h>

// POSeven
#include "POSeven/Errno.hh"


namespace poseven
{
	
	inline void rename( const char* from, const char* to )
	{
		throw_posix_result( ::rename( from, to ) );
	}
	
	inline void rename( const std::string& from, const char* to )
	{
		rename( from.c_str(), to );
	}
	
	inline void rename( const char* from, const std::string& to )
	{
		rename( from, to.c_str() );
	}
	
	inline void rename( const std::string& from, const std::string& to )
	{
		rename( from, to.c_str() );
	}
	
}

#endif

