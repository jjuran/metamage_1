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

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace POSeven
{
	
	Nucleus::Owned< FileDescriptor > Open( const char* name, int oflag, int mode = 0 );
	
}

namespace io
{
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_reading( const char* pathname )
	{
		return POSeven::Open( pathname, O_RDONLY );
	}
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_reading( const std::string& pathname )
	{
		return open_for_reading( pathname.c_str() );
	}
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_writing( const char* pathname )
	{
		return POSeven::Open( pathname, O_WRONLY );
	}
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_writing( const std::string& pathname )
	{
		return open_for_writing( pathname.c_str() );
	}
	
}

#endif

