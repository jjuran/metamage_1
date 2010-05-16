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

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline void ioctl( fd_t fd, unsigned long command, void* argp )
	{
		throw_posix_result( ::ioctl( fd, command, argp ) );
	}
	
}

#endif

