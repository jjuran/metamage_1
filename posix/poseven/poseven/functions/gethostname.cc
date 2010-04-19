/*
	gethostname.cc
	--------------
	
	Copyright 2009, Joshua Juran.
*/

#include "poseven/functions/gethostname.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif


namespace poseven
{
	
	const char* gethostname()
	{
		static char buffer[ HOST_NAME_MAX + 1 ];
		
		throw_posix_result( ::gethostname( buffer, sizeof buffer ) );
		
		return buffer;
		
	}
	
}

