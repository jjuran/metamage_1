/*
	plus/string/concat.cc
	---------------------
*/

#include "plus/string/concat.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"


namespace plus
{
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size )
	{
		string result;
		
		void* p = result.reset( a_size + b_size );
		
		p = mempcpy( p, a, a_size );
		p = mempcpy( p, b, b_size );
		
		return result;
	}
	
	string operator+( const string& a, const string& b )
	{
		return concat( a.data(), a.size(), b.data(), b.size() );
	}
	
	string operator+( const string& a, const char* b )
	{
		return concat( a.data(), a.size(), b, strlen( b ) );
	}
	
	string operator+( const char* a, const string& b )
	{
		return concat( a, strlen( a ), b.data(), b.size() );
	}
	
}

