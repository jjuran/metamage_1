/*
	exception.cc
	------------
*/

#include "plus/exception.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/string.hh"


namespace plus
{
	
	static void assign( char* d, unsigned d_size, const char* s, unsigned s_len )
	{
		if ( s_len > d_size - 1 )
		{
			// Silently truncate
			s_len = d_size - 1;
		}
		
		mempcpy( d, s, s_len );
		
		d[ s_len ] = '\0';
	}
	
	exception::exception( const char* s, unsigned length )
	{
		assign( buffer, sizeof buffer, s, length );
	}
	
	exception::exception( const char* s )
	{
		assign( buffer, sizeof buffer, s, strlen( s ) );
	}
	
	exception::exception( const string& s )
	{
		assign( buffer, sizeof buffer, s.data(), s.size() );
	}
	
}
