// closedir.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CLOSEDIR_HH
#define POSEVEN_FUNCTIONS_CLOSEDIR_HH

// POSIX
#ifdef __APPLE__
// Include <sys/types.h> before <dirent.h> to compile with Mac OS X 10.2's SDK.
#include <sys/types.h>
#endif
#include <dirent.h>

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_DIR_T_HH
#include "poseven/types/dir_t.hh"
#endif
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif


namespace nucleus
{
	
	template <>
	struct disposer< poseven::dir_t >
	{
		typedef poseven::dir_t  argument_type;
		typedef void            result_type;
		
		void operator()( poseven::dir_t dir ) const
		{
			poseven::handle_destruction_posix_result( ::closedir( dir ) );
		}
	};
	
}

namespace poseven
{
	
	void closedir( nucleus::owned< dir_t > dir );
	
}

#endif
