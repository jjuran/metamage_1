/*
	concat.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/concat.hh"

// Standard C
#include <string.h>


namespace plus
{
	
	static void concat( const char* a, string::size_type  a_size,
	                    const char* b, string::size_type  b_size,
	                    string& result )
	{
		const string::size_type total_size = a_size + b_size;
		
		char* buffer = (char*) ::operator new( total_size + 1 );
		
		memcpy( buffer,          a, a_size );
		memcpy( buffer + a_size, b, b_size );
		
		buffer[ total_size ] = '\0';
		
		result.assign( buffer, total_size, delete_basic );
	}
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size )
	{
		string result;
		
		concat( a, a_size, b, b_size, result );
		
		return result;
	}
	
}

