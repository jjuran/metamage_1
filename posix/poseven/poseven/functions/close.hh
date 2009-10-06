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
#include "Nucleus/Owned.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/fd_t.hh"


namespace Nucleus
{
	
	template <>
	struct Disposer< poseven::fd_t > : std::unary_function< poseven::fd_t, void >,
	                                   poseven::DefaultDestructionErrnoPolicy
	{
		void operator()( poseven::fd_t fd ) const
		{
			// FIXME
			// HandleDestructionPOSIXError( ::close( fd ) );
			
			if ( ::close( fd ) <= -1 )
			{
				HandleDestructionErrno( errno );
			}
		}
	};
	
}

namespace poseven
{
	
	inline void close( Nucleus::Owned< fd_t > fd )
	{
		throw_posix_result( ::close( fd.Release() ) );
	}
	
}

#endif

