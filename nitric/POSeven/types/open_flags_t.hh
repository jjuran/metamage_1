// open_flags_t.hh
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_OPEN_FLAGS_T_HH
#define POSEVEN_TYPES_OPEN_FLAGS_T_HH

// POSIX
#include <fcntl.h>

// Nucleus
#include "Nucleus/Flag.h"


namespace poseven
{
	
	enum open_flags_t
	{
		o_rdonly = O_RDONLY,
		o_wronly = O_WRONLY,
		o_rdwr   = O_RDWR,
		o_creat  = O_CREAT,
		o_trunc  = O_TRUNC,
		o_append = O_APPEND,
		o_excl   = O_EXCL,
		
	#ifdef O_TRUNC_LAZY
		
		o_trunc_lazy = O_TRUNC_LAZY,
		
	#endif
		
		open_flags_t_max = Nucleus::Enumeration_Traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( open_flags_t )
	
}

#endif

