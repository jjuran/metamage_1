/*
	utime.cc
	--------
*/

#include "poseven/functions/utime.hh"

// POSIX
#include <utime.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void utime( const char* path )
	{
		throw_posix_result( ::utime( path, 0 ) );  // NULL
	}
	
	void utime( const char* path, const utimbuf& time_buffer )
	{
		throw_posix_result( ::utime( path, &time_buffer ) );
	}
	
}

