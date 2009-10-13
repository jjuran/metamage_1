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

// Standard C/C++
#include <cstdlib>

// Nucleus
#include "nucleus/enumeration_traits.hh"


namespace poseven
{
	
	enum exit_t
	{
		exit_success = EXIT_SUCCESS,
		exit_failure = EXIT_FAILURE,
		
		exit_t_max = nucleus::enumeration_traits< int >::max
	};
	
}

#endif

