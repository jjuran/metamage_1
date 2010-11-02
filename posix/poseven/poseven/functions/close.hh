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

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace nucleus
{
	
	template <>
	struct disposer< poseven::fd_t >
	{
		typedef poseven::fd_t  argument_type;
		typedef void           result_type;
		
		void operator()( poseven::fd_t fd ) const
		{
			poseven::handle_destruction_posix_result( ::close( fd ) );
		}
	};
	
}

namespace poseven
{
	
	void close( nucleus::owned< fd_t > fd );
	
}

#endif

