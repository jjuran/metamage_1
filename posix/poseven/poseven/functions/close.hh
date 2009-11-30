// close.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#define POSEVEN_FUNCTIONS_CLOSE_HH

// POSIX
#include <unistd.h>

// Nucleus
#include "nucleus/owned.hh"

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace nucleus
{
	
	template <>
	struct disposer< poseven::fd_t > : std::unary_function< poseven::fd_t, void >
	{
		void operator()( poseven::fd_t fd ) const
		{
			poseven::handle_destruction_posix_result( ::close( fd ) );
		}
	};
	
}

namespace poseven
{
	
	inline void close( nucleus::owned< fd_t > fd )
	{
		throw_posix_result( ::close( fd.release() ) );
	}
	
}

#endif

