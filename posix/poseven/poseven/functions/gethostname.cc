/*
	gethostname.cc
	--------------
	
	Copyright 2009, Joshua Juran.
*/

#include "poseven/functions/gethostname.hh"

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif


namespace poseven
{
	
	void gethostname( std::string& name )
	{
		name.resize( HOST_NAME_MAX + 1 );
		
		throw_posix_result( ::gethostname( &name[0], name.size() ) );
		
		name.resize( strlen( name.c_str() ) );
	}
	
}

