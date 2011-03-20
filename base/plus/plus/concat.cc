/*
	concat.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/concat.hh"

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
	
}

