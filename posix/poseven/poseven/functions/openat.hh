// openat.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPENAT_HH
#define POSEVEN_FUNCTIONS_OPENAT_HH

// POSIX
#include <fcntl.h>

// iota
#include "iota/string_traits.hh"

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
#endif
#ifndef POSEVEN_TYPES_MODE_T_HH
#include "poseven/types/mode_t.hh"
#endif
#ifndef POSEVEN_TYPES_OPENFLAGS_T_HH
#include "poseven/types/open_flags_t.hh"
#endif


namespace poseven
{
	
	nucleus::owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode = _666 );
	
	template < class String >
	inline nucleus::owned< fd_t >
	//
	openat( fd_t dirfd, const String& path, open_flags_t flags, mode_t mode = _666 )
	{
		return openat( dirfd, iota::get_string_c_str( path ), flags, mode );
	}
	
}

#endif

