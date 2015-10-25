/*
	token.cc
	--------
*/

#include "vlib/token.hh"


namespace vlib
{
	
	Token next_token( const char*& p )
	{
		const char* q = p;
		
		Token result;
		
		result.type = next_token_type( q );
		
		result.text.assign( p, q );
		
		p = q;
		
		return result;
	}
	
}
