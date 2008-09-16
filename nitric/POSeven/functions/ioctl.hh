// ioctl.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_IOCTL_HH
#define POSEVEN_FUNCTIONS_IOCTL_HH

// POSIX
#include <sys/ioctl.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/fd_t.hh"


namespace poseven
{
	
	template < class FD, class Pointer >
	inline void ioctl( const FD& fd, unsigned long command, Pointer argp )
	{
		fd_t converted = fd;
		
		throw_posix_result( ::ioctl( converted, command, argp ) );
	}
	
}

#endif

