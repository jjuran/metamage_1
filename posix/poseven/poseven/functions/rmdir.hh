/*
	rmdir.hh
	--------
*/


#ifndef POSEVEN_FUNCTIONS_RMDIR_HH
#define POSEVEN_FUNCTIONS_RMDIR_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void rmdir( const char* path );
	
	template < class String >
	inline void rmdir( const String& path )
	{
		rmdir( iota::get_string_c_str( path ) );
	}
	
}

#endif

