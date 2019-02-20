/*
	token.cc
	--------
*/

#include "vlib/token.hh"


namespace vlib
{
	
	static inline
	bool ignorable( token_type token )
	{
		return token == Token_whitespace  ||  token == Token_comment;
	}
	
	token_type next_token( const char*& p, Token& result )
	{
		const char* q = p;
		
		do
		{
			result.type = next_token_type( q );
			
			result.text.assign( p, q );
			
			p = q;
		}
		while ( ignorable( result.type ) );
		
		return result;
	}
	
}
