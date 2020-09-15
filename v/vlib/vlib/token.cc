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
	
	static inline
	bool whitespace( token_type token )
	{
		return ignorable( token )  ||  token == Token_newline;
	}
	
	token_type next_token( const char*& p, Token& result )
	{
		result.space_before = result.space_after;
		
		const char* q = p;
		
		do
		{
			result.type = next_token_type( q );
			
			result.text.assign( p, q );
			
			p = q;
		}
		while ( ignorable( result.type )  &&  (result.space_before = true) );
		
		result.space_after = whitespace( next_token_type( q ) );
		
		return result;
	}
	
}
