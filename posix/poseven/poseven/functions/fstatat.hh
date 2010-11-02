// fstatat.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FSTATAT_HH
#define POSEVEN_FUNCTIONS_FSTATAT_HH

// POSIX
#include <sys/stat.h>

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/at_flags_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	bool fstatat( fd_t           dirfd,
	              const char    *path,
	              struct stat&   sb,
	              at_flags_t     flags = at_flags_t() );
	
	struct stat fstatat( fd_t         dirfd,
	                     const char  *path,
	                     at_flags_t   flags = at_flags_t() );
	
	template < class String >
	inline bool fstatat( fd_t           dirfd,
	                     const String&  path,
	                     struct stat&   sb,
	                     at_flags_t     flags = at_flags_t() )
	{
		return fstatat( dirfd, iota::get_string_c_str( path ), sb, flags );
	}
	
	template < class String >
	inline struct ::stat fstatat( fd_t           dirfd,
	                              const String&  path,
	                              at_flags_t     flags = at_flags_t() )
	{
		return fstatat( dirfd, iota::get_string_c_str( path ), flags );
	}
	
}

#endif

