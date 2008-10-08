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

// Standard C++
#include <string>

// Convergence
#include "copyfile.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace poseven
{
	
	inline void copyfile( const char* from, const char* to )
	{
		throw_posix_result( ::copyfile( from, to ) );
	}
	
	inline void copyfile( const std::string& from, const char* to )
	{
		copyfile( from.c_str(), to );
	}
	
	inline void copyfile( const char* from, const std::string& to )
	{
		copyfile( from, to.c_str() );
	}
	
	inline void copyfile( const std::string& from, const std::string& to )
	{
		copyfile( from, to.c_str() );
	}
	
}

#endif

