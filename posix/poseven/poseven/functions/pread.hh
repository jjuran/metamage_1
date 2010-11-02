// pread.hh
// --------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_PREAD_HH
#define POSEVEN_FUNCTIONS_PREAD_HH

// POSIX
#include <sys/types.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	ssize_t pread( fd_t fd, char* buffer, size_t n_bytes, off_t offset );
	
}

#endif

