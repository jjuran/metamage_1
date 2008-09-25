// in_addr_t.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_IN_ADDR_T_HH
#define POSEVEN_TYPES_IN_ADDR_T_HH

// POSIX
#include <netinet/in.h>

// Nucleus
#include "Nucleus/Enumeration.h"


namespace poseven
{
	
	enum in_addr_t
	{
		inaddr_any       = INADDR_ANY,
		inaddr_broadcast = INADDR_BROADCAST,
		inaddr_none      = INADDR_NONE,
		inaddr_loopback  = INADDR_LOOPBACK,
		
		inaddr_unspec_group    = INADDR_UNSPEC_GROUP,
		inaddr_allhosts_group  = INADDR_ALLHOSTS_GROUP,
		inaddr_allrtrs_group   = INADDR_ALLRTRS_GROUP,
		inaddr_max_local_group = INADDR_MAX_LOCAL_GROUP,
		
		in_addr_t_max = Nucleus::Enumeration_Traits< ::in_addr_t >::max
	};
	
}

#endif

