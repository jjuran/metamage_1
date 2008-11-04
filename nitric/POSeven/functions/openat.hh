// openat.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPEN_HH
#define POSEVEN_FUNCTIONS_OPEN_HH

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/functions/close.hh"
#include "POSeven/types/mode_t.hh"
#include "POSeven/types/open_flags_t.hh"


namespace poseven
{
	
	Nucleus::Owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode = mode_t( 0666 ) );
	
	inline Nucleus::Owned< fd_t > openat( fd_t dirfd, const std::string& path, open_flags_t flags, mode_t mode = mode_t( 644 ) )
	{
		return openat( dirfd, path.c_str(), flags, mode );
	}
	
}

namespace io
{
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( poseven::fd_t dirfd, const char* path, overload = overload() )
	{
		return poseven::openat( dirfd, path, poseven::o_rdonly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( poseven::fd_t dirfd, const std::string& path, overload = overload() )
	{
		return open_for_reading( dirfd, path.c_str(), overload() );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( poseven::fd_t dirfd, const char* path, overload = overload() )
	{
		return poseven::openat( dirfd, path, poseven::o_wronly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( poseven::fd_t dirfd, const std::string& path, overload = overload() )
	{
		return open_for_writing( dirfd, path.c_str(), overload() );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_io( poseven::fd_t dirfd, const char* path, overload = overload() )
	{
		return poseven::openat( dirfd, path, poseven::o_rdwr );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_io( poseven::fd_t dirfd, const std::string& path, overload = overload() )
	{
		return open_for_io( dirfd, path.c_str(), overload() );
	}
	
}

#endif

