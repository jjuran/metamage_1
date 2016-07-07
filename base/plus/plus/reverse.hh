/*
	reverse.hh
	----------
*/

#ifndef PLUS_REVERSE_HH
#define PLUS_REVERSE_HH

// plus
#include "plus/string_fwd.hh"


namespace plus
{
	
	void reverse( char* begin, char* end );
	
	inline
	void reverse( char* data, unsigned long size )
	{
		reverse( data, data + size );
	}
	
	void reverse_in_place( string& s );
	
}

#endif
