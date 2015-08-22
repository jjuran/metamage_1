/*
	token.hh
	--------
*/

#ifndef VC_TOKEN_HH
#define VC_TOKEN_HH

// plus
#include "plus/string.hh"

// vc
#include "vc/token_type.hh"


namespace vc
{
	
	struct Token
	{
		plus::string  text;
		token_type    type;
		
		operator token_type() const  { return type; }
	};
	
	Token next_token( const char*& p );
	
}

#endif
