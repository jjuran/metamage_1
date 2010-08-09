/*
	plus/quad.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/quad.hh"

// iota
#include "iota/quad.hh"

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	string encode_quad( unsigned long q )
	{
		var_string result;
		
		char* p = result.reset( sizeof 'quad' );
		
		iota::encode_quad( q, p );
		
		return result;
	}
	
}

