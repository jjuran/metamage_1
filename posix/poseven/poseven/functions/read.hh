// read.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_READ_HH
#define POSEVEN_FUNCTIONS_READ_HH

// POSIX
#include <sys/types.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	ssize_t read( fd_t fd, char* buffer, size_t bytes_requested );
	
}

#endif

