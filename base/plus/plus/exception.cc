/*
	exception.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/exception.hh"

// Standard C
#include <string.h>


namespace plus
{
	
	static void assign( char* d, unsigned d_size, const char* s, unsigned s_len )
	{
		if ( s_len > d_size - 1 )
		{
			// Silently truncate
			s_len = d_size - 1;
		}
		
		memcpy( d, s, s_len );
		
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
	
	
}

