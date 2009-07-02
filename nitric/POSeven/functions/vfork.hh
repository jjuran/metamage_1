// vfork.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_VFORK_HH
#define POSEVEN_FUNCTIONS_VFORK_HH

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/pid_t.hh"


namespace poseven
{

// gcc doesn't support inlining a call to setjmp()	
#ifdef __GNUC__
	
	#define POSEVEN_VFORK()  poseven::pid_t( poseven::throw_posix_result( vfork() ) )
	
#else
	
	inline pid_t vfork()
	{
		return pid_t( throw_posix_result( ::vfork() ) );
	}
	
	#define POSEVEN_VFORK()  poseven::vfork()
	
#endif
	
}

#endif

