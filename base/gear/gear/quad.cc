/*
	gear/quad.hh
	------------
*/

#include "gear/quad.hh"


namespace gear
{
	
	unsigned long decode_quad( const unsigned char* s )
	{
		const unsigned long q = s[ 0 ] << 24
		                      | s[ 1 ] << 16
		                      | s[ 2 ] <<  8
		                      | s[ 3 ] <<  0;
		
		return q;
	}
	
	void encode_quad( unsigned long q, unsigned char* s )
	{
		s[ 0 ] = q >> 24;
		s[ 1 ] = q >> 16;
		s[ 2 ] = q >>  8;
		s[ 3 ] = q >>  0;
	}
	
}
