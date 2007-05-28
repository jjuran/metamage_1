// =======
// Open.hh
// =======

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
	
}

#endif

