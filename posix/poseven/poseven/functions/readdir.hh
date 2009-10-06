// readdir.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_READDIR_HH
#define POSEVEN_FUNCTIONS_READDIR_HH

// POSeven
#include "POSeven/types/dir_t.hh"
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline const dirent& readdir( dir_t dir )
	{
		const dirent* entry = ::readdir( dir );
		
		if ( entry == NULL )
		{
			throw_errno( errno );
		}
		
		return *entry;
	}
	
}

#endif

