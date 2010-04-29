/*
	concat.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/concat.hh"

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size )
	{
		var_string result;
		
		result.reserve( a_size + b_size );
		
		result.append( a, a_size );
		result.append( b, b_size );
		
		return result;
	}
	
}

