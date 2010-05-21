// exit_t.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_EXIT_T_HH
#define POSEVEN_TYPES_EXIT_T_HH

// Nucleus
#include "nucleus/enumeration_traits.hh"


namespace poseven
{
	
	enum exit_t
	{
		exit_success = 0,  // EXIT_SUCCESS
		exit_failure = 1,  // EXIT_FAILURE
		
		exit_t_max = nucleus::enumeration_traits< int >::max
	};
	
}

#endif

