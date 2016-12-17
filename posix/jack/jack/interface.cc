/*
	interface.cc
	------------
*/

#include "jack/interface.hh"

// Standard C
#include <string.h>


#define STRLEN( s )  (sizeof "" s - 1)


namespace jack
{
	
	interface::interface( const char* subpath )
	:
		its_subpath_length( strlen( subpath ) )
	{
		its_path.reserve( its_subpath_length + STRLEN( ".socket" ) );
		
		its_path.assign( subpath, its_subpath_length );
	}
	
	const char* interface::full_path( const char* extension )
	{
		its_path.resize( its_subpath_length );
		
		its_path += extension;
		
		return its_path.c_str();
	}
	
}
