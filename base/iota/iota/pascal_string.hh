/*
	iota/pascal_string.hh
	---------------------
*/

#ifndef IOTA_PASCALSTRING_HH
#define IOTA_PASCALSTRING_HH

// more-libc
#include "more/size.h"


#if defined( __MACOS__ )  ||  defined( __APPLE__ )
#define IOTA_HAS_PASCAL_STRINGS  1
#endif


namespace iota
{
	
#if IOTA_HAS_PASCAL_STRINGS
	
	inline const char* get_pascal_string_data( const unsigned char* s )
	{
		return (const char*) s + 1;
	}
	
	inline std::size_t get_pascal_string_size( const unsigned char* s )
	{
		return s[0];
	}
	
#endif
	
}

#endif

