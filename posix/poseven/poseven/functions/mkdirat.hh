// mkdirat.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_MKDIRAT_HH
#define POSEVEN_FUNCTIONS_MKDIRAT_HH

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/fd_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	void mkdirat( fd_t dirfd, const char* path, mode_t mode = _777 );
	
	template < class String >
	inline void mkdirat( const String& path, mode_t mode = _777 )
	{
		mkdirat( iota::get_string_c_str( path ), mode );
	}
	
}

#endif

