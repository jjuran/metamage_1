/*
	truncate.hh
	-----------
*/

#ifndef POSEVEN_FUNCTIONS_TRUNCATE_HH
#define POSEVEN_FUNCTIONS_TRUNCATE_HH

// POSIX
#include <sys/types.h>

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void truncate( const char* path, off_t offset );
	
	template < class String >
	inline void truncate( const String& path, off_t offset )
	{
		using iota::get_string_c_str;
		
		truncate( get_string_c_str( path ), offset );
	}
	
}

#endif
