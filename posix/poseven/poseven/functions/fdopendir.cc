/*
	fdopendir.cc
	------------
*/

#include "poseven/functions/fdopendir.hh"

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	nucleus::owned< dir_t > fdopendir( nucleus::owned< fd_t > fd )
	{
		DIR* handle = ::fdopendir( fd );
		
		if ( handle == NULL )
		{
			throw_errno( errno );
		}
		
		fd.release();
		
		return nucleus::owned< dir_t >::seize( handle );
	}
	
}

