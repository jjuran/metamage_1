// link.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_LINK_HH
#define POSEVEN_FUNCTIONS_LINK_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"


namespace poseven
{
	
	inline void link( const char* from, const char* to )
	{
		throw_posix_result( ::link( from, to ) );
	}
	
	inline void link( const std::string& from, const char* to )
	{
		link( from.c_str(), to );
	}
	
	inline void link( const char* from, const std::string& to )
	{
		link( from, to.c_str() );
	}
	
	inline void link( const std::string& from, const std::string& to )
	{
		link( from, to.c_str() );
	}
	
}

#endif

