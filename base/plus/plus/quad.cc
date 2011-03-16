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
	
	string encode_quad( unsigned long q )
	{
		string result;
		
		char* p = result.reset( 4 );  // sizeof 'quad'
		
		iota::encode_quad( q, p );
		
		return result;
	}
	
}

