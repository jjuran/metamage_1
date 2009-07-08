// symlink.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SYMLINK_HH
#define POSEVEN_FUNCTIONS_SYMLINK_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline void symlink( const char* from, const char* to )
	{
		throw_posix_result( ::symlink( from, to ) );
	}
	
	inline void symlink( const std::string& from, const char* to )
	{
		symlink( from.c_str(), to );
	}
	
	inline void symlink( const char* from, const std::string& to )
	{
		symlink( from, to.c_str() );
	}
	
	inline void symlink( const std::string& from, const std::string& to )
	{
		symlink( from, to.c_str() );
	}
	
}

#endif

