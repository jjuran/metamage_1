// fcntl_cmd_t.hh
// --------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_FCNTL_CMD_T_HH
#define POSEVEN_TYPES_FCNTL_CMD_T_HH

// POSIX
#include <fcntl.h>

// Nucleus
#include "nucleus/enumeration_traits.hh"


namespace poseven
{
	
	enum fcntl_cmd_t
	{
		f_dupfd  = F_DUPFD,
		f_getfd  = F_GETFD,
		f_setfd  = F_SETFD,
		f_getfl  = F_GETFL,
		f_setfl  = F_SETFL,
		f_getown = F_GETOWN,
		f_setown = F_SETOWN,
		f_getlk  = F_GETLK,
		f_setlk  = F_SETLK,
		f_setlkw = F_SETLKW,
		
	#ifdef F_DUPFD_CLOEXEC
		
		f_dupfd_cloexec = F_DUPFD_CLOEXEC,
		
	#endif
		
		fcntl_cmd_t_max = nucleus::enumeration_traits< int >::max
	};
	
}

#endif

