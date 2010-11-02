/*
	symlinkat.hh
	------------
	
	Joshua Juran
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/


#ifndef POSEVEN_FUNCTIONS_SYMLINKAT_HH
#define POSEVEN_FUNCTIONS_SYMLINKAT_HH

// iota
#include "iota/string_traits.hh"

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	void symlinkat( const char* target, fd_t dirfd, const char* path );
	
	template < class String1, class String2 >
	inline void symlinkat( const String1& from, fd_t dirfd, const String2& to )
	{
		symlinkat( iota::get_string_c_str( from ), dirfd, iota::get_string_c_str( to ) );
	}
	
}

#endif

