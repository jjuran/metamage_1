/*
	link.cc
	-------
*/

#include "poseven/functions/link.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void link( const char* from, const char* to )
	{
		throw_posix_result( ::link( from, to ) );
	}
	
}

