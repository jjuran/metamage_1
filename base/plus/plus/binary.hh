/*
	plus/binary.hh
	--------------
*/

#ifndef PLUS_BINARY_HH
#define PLUS_BINARY_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	typedef unsigned align_t;
	
	string unbin( const char* data, unsigned long size, align_t align );
	
	template < class String >
	inline
	string unbin( const String& s, align_t align = 1 )
	{
		return unbin( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
