/*
	truncate.cc
	-----------
*/

#include "poseven/functions/truncate.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void truncate( const char* path, off_t offset )
	{
		throw_posix_result( ::truncate( path, offset ) );
	}
	
}
