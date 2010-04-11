/*
	symlinkat.hh
	------------
	
	Joshua Juran
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/


#ifndef POSEVEN_FUNCTIONS_SYMLINKAT_HH
#define POSEVEN_FUNCTIONS_SYMLINKAT_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void symlinkat( const char* target, fd_t dirfd, const char* path )
	{
		throw_posix_result( ::symlinkat( target, dirfd, path ) );
	}
	
}

#endif

