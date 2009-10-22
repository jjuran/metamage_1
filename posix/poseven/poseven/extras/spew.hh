// spew.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_EXTRAS_SPEW_HH
#define POSEVEN_EXTRAS_SPEW_HH

// Standard C++
#include <string>

// Io
#include "io/spew.hh"

// Nucleus
#include "Nucleus/Flattener.h"

// poseven
#include "poseven/FileDescriptor.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	inline void spew( fd_t fd, const char* buffer, std::size_t length )
	{
		io::spew_output( fd, buffer, length );
	}
	
	
	inline void spew( const char* path, const char* buffer, std::size_t length )
	{
		spew( open( path, o_wronly | o_trunc ), buffer, length );
	}
	
	inline void spew( const char* path, const std::string& stuff )
	{
		spew( path, stuff.data(), stuff.length() );
	}
	
	inline void spew( const std::string& path, const char* buffer, std::size_t length )
	{
		spew( path.c_str(), buffer, length );
	}
	
	inline void spew( const std::string& path, const std::string& stuff )
	{
		spew( path.c_str(), stuff );
	}
	
}

#endif

