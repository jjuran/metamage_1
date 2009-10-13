// pid_t.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_PID_T_HH
#define POSEVEN_TYPES_PID_T_HH

// POSIX
#include <unistd.h>

// Nucleus
#include "nucleus/enumeration_traits.hh"


namespace poseven
{
	
	enum pid_t
	{
		pid_t_min = -1,
		
		pid_t_max = nucleus::enumeration_traits< ::pid_t >::max
	};
	
}

#endif

