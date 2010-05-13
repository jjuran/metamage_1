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

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void symlinkat( const char* target, fd_t dirfd, const char* path )
	{
		throw_posix_result( ::symlinkat( target, dirfd, path ) );
	}
	
	template < class String1, class String2 >
	inline void symlinkat( const String1& from, fd_t dirfd, const String2& to )
	{
		symlinkat( iota::get_string_c_str( from ), dirfd, iota::get_string_c_str( to ) );
	}
	
}

#endif

