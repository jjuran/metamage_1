// Open.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_OPEN_HH
#define POSEVEN_OPEN_HH

// POSIX
#include <fcntl.h>

// Nucleus
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace poseven
{
	
	enum oflag_t
	{
		o_rdonly = O_RDONLY,
		o_wronly = O_WRONLY,
		o_rdwr   = O_RDWR,
		o_creat  = O_CREAT,
		o_trunc  = O_TRUNC,
		o_append = O_APPEND,
		o_excl   = O_EXCL,
		
		oflag_t_max = Nucleus::Enumeration_Traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( oflag_t )
	
	Nucleus::Owned< fd_t > open( const char* name, oflag_t oflag, int mode = 0 );
	
}

namespace io
{
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdonly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( const std::string& pathname, overload = overload() )
	{
		return open_for_reading( pathname.c_str(), overload() );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( const std::string& pathname, overload = overload() )
	{
		return open_for_writing( pathname.c_str(), overload() );
	}
	
}

#endif

