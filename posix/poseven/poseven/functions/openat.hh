// openat.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPENAT_HH
#define POSEVEN_FUNCTIONS_OPENAT_HH

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>

// Io
#include "io/io.hh"

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/mode_t.hh"
#include "poseven/types/open_flags_t.hh"


namespace poseven
{
	
	Nucleus::Owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode = _666 );
	
	inline Nucleus::Owned< fd_t > openat( fd_t dirfd, const std::string& path, open_flags_t flags, mode_t mode = _666 )
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

