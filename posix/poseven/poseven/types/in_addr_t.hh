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
#include "nucleus/enumeration_traits.hh"


namespace poseven
{
	
	enum in_addr_t
	{
		inaddr_any       = 0x00000000,
		inaddr_broadcast = 0xffffffff,
		inaddr_none      = 0xffffffff,
		inaddr_loopback  = 0x7f000001,
		
		inaddr_unspec_group    = 0xe0000000,
		inaddr_allhosts_group  = 0xe0000001,
		inaddr_allrtrs_group   = 0xe0000002,
		inaddr_max_local_group = 0xe00000ff,
		
		in_addr_t_max = nucleus::enumeration_traits< ::in_addr_t >::max
	};
	
}

#endif

