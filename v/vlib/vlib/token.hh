/*
	token.hh
	--------
*/

#ifndef VLIB_TOKEN_HH
#define VLIB_TOKEN_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/token_type.hh"


namespace vlib
{
	
	struct Token
	{
		plus::string  text;
		token_type    type;
		bool          space_before;
		bool          space_after;
		
		Token() : type(), space_before(), space_after()
		{
		}
		
		operator token_type() const  { return type; }
	};
	
	token_type next_token( const char*& p, Token& result );
	
}

#endif
