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
#ifndef POSEVEN_TYPES_ATFLAGS_T_HH
#include "poseven/types/at_flags_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	typedef struct stat struct_stat;
	
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
		using iota::get_string_c_str;
		
		return fstatat( dirfd, get_string_c_str( path ), sb, flags );
	}
	
	template < class String >
	inline
	struct_stat fstatat( fd_t           dirfd,
	                     const String&  path,
	                     at_flags_t     flags = at_flags_t() )
	{
		using iota::get_string_c_str;
		
		return fstatat( dirfd, get_string_c_str( path ), flags );
	}
	
}

#endif
