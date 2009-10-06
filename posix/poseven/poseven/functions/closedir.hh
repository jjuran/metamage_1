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
#include <dirent.h>

// Nucleus
#include "Nucleus/Owned.h"

// poseven
#include "poseven/Errno.hh"
#include "poseven/types/dir_t.hh"


namespace Nucleus
{
	
	template <>
	struct Disposer< poseven::dir_t > : public std::unary_function< poseven::dir_t, void >//,
	                                    //private poseven::DefaultDestructionPOSIXResultPolicy
	{
		void operator()( poseven::dir_t dir ) const
		{
			//(void) Nitrogen::FileManagerErrorsRegistrationDependency();
			//HandleDestructionPOSIXResult( ::closedir( dir ) );
			::closedir( dir );
		}
	};
	
}

namespace poseven
{
	
	inline void closedir( Nucleus::Owned< dir_t > dir )
	{
		throw_posix_result( ::closedir( dir.release() ) );
	}
	
}

#endif

