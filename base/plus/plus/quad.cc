/*
	plus/quad.cc
	------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/quad.hh"

// gear
#include "gear/quad.hh"


namespace plus
{
	
	string encode_quad( unsigned long q )
	{
		string result;
		
		char* p = result.reset( 4 );  // sizeof 'quad'
		
		gear::encode_quad( q, p );
		
		return result;
	}
	
}

