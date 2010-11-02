/*
	rename.cc
	---------
*/

#include "poseven/functions/rename.hh"

// POSIX
#include <stdio.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void rename( const char* from, const char* to )
	{
		throw_posix_result( ::rename( from, to ) );
	}
	
}

