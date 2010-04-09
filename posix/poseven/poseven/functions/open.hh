// open.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPEN_HH
#define POSEVEN_FUNCTIONS_OPEN_HH

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/mode_t.hh"
#include "poseven/types/open_flags_t.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > open( const char* name, open_flags_t oflag, mode_t mode = _666 );
	
	inline nucleus::owned< fd_t > open( const std::string& name, open_flags_t oflag, mode_t mode = _666 )
	{
		return open( name.c_str(), oflag, mode );
	}
	
}

#endif

