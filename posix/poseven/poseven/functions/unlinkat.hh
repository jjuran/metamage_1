/*
	unlinkat.hh
	-----------
	
	Joshua Juran
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/

#ifndef POSEVEN_FUNCTIONS_UNLINKAT_HH
#define POSEVEN_FUNCTIONS_UNLINKAT_HH

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/at_flags_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	void unlinkat( fd_t         dirfd,
	               const char*  path,
	               at_flags_t   flags = at_flags_t() );
	
	template < class String >
	inline void unlinkat( const String& path )
	{
		unlinkat( iota::get_string_c_str( path ) );
	}
	
}

#endif

