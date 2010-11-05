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
#ifndef POSEVEN_TYPES_ATFLAGS_T_HH
#include "poseven/types/at_flags_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


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

