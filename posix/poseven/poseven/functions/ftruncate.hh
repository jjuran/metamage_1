// ftruncate.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FTRUNCATE_HH
#define POSEVEN_FUNCTIONS_FTRUNCATE_HH

// POSIX
#include <sys/types.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	void ftruncate( fd_t fd, off_t offset );
	
}

#endif

