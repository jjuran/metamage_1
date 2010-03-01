/*
	plus/quad.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/quad.hh"

// iota
#include "iota/quad.hh"


namespace plus
{
	
	std::string encode_quad( unsigned long q )
	{
		std::string result;
		
		result.resize( sizeof 'quad' );
		
		iota::encode_quad( q, &result[0] );
		
		return result;
	}
	
}

