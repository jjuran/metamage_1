/*
	token.cc
	--------
*/

#include "vlib/token.hh"


namespace vlib
{
	
	/*
		For the purpose of tokenizing whitespace around operators,
		treat `(x?)` the same as `( x? )`.
	*/
	
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
	
	static inline
	bool whitespace_or_end( token_type token )
	{
		switch ( token )
		{
			case Token_rparen:
			case Token_rbrace:
			case Token_rbracket:
			
			case Token_whitespace:
			case Token_comment:
			case Token_newline:
			case Token_semicolon:
			case Token_comma:
			case Token_end:
				return true;
			
			default:
				return false;
		}
	}
	
	token_type next_token( const char*& p, Token& result )
	{
		result.space_before = whitespace( result );
		
		const char* q = p;
		
		do
		{
			result.type = next_token_type( q );
			
			result.text.assign( p, q );
			
			p = q;
		}
		while ( ignorable( result.type )  &&  (result.space_before = true) );
		
		result.space_after = whitespace_or_end( next_token_type( q ) );
		
		return result;
	}
	
}
