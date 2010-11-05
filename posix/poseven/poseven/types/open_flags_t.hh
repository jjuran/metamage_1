// open_flags_t.hh
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_OPEN_FLAGS_T_HH
#define POSEVEN_TYPES_OPEN_FLAGS_T_HH

// POSIX
#include <fcntl.h>

// Nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace poseven
{
	
	enum open_flags_t
	{
		o_rdonly   = O_RDONLY,
		o_wronly   = O_WRONLY,
		o_rdwr     = O_RDWR,
		o_creat    = O_CREAT,
		o_trunc    = O_TRUNC,
		o_append   = O_APPEND,
		o_excl     = O_EXCL,
		o_nonblock = O_NONBLOCK,
		o_noctty   = O_NOCTTY,
		
	#ifdef O_NOATIME
		
		o_noatime  = O_NOATIME,
		
	#endif
		
	#ifdef O_NONE
		
		o_none = O_NONE,
		
	#endif
		
	#ifdef O_NOFOLLOW
		
		o_nofollow = O_NOFOLLOW,
		
	#endif
		
	#ifdef O_EXEC
		
		o_exec = O_EXEC,
		
	#endif
		
	#ifdef O_SEARCH
		
		o_search = O_SEARCH,
		
	#endif
		
	#ifdef O_DIRECTORY
		
		o_directory = O_DIRECTORY,
		
	#endif
		
	#ifdef O_BINARY
		
		o_binary = O_BINARY,
		
	#endif
		
	#ifdef O_CLOEXEC
		
		o_cloexec = O_CLOEXEC,
		
	#endif
		
	#ifdef O_XATTR
		
		o_xattr = O_XATTR,
		
	#endif
		
		open_flags_t_max = nucleus::enumeration_traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( open_flags_t )
	
}

#endif

