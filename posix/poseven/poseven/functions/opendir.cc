/*
	opendir.cc
	----------
*/

#include "poseven/functions/opendir.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	nucleus::owned< dir_t > opendir( const char* pathname )
	{
		DIR* handle = ::opendir( pathname );
		
		if ( handle == NULL )
		{
			throw_errno( errno );
		}
		
		return nucleus::owned< dir_t >::seize( handle );
	}
	
}

