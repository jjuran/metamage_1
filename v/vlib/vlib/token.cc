/*
	token.cc
	--------
*/

#include "vlib/token.hh"


namespace vlib
{
	
	token_type next_token( const char*& p, Token& result )
	{
		const char* q = p;
		
		result.type = next_token_type( q );
		
		result.text.assign( p, q );
		
		p = q;
		
		return result;
	}
	
}
