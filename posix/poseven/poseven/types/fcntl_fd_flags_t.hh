// fcntl_fd_flags_t.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_FCNTL_FD_FLAGS_T_HH
#define POSEVEN_TYPES_FCNTL_FD_FLAGS_T_HH

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
	
	enum fcntl_fd_flags_t
	{
		fd_cloexec = FD_CLOEXEC,
		
		fcntl_fd_flags_t_max = nucleus::enumeration_traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( fcntl_fd_flags_t )
	
}

#endif

