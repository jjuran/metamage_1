/*
	exceptions.hh
	-------------
*/

#ifndef VLIB_EXCEPTIONS_HH
#define VLIB_EXCEPTIONS_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/source.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct language_error
	{
		const plus::string  text;
		const source_spec   source;
		
		language_error( const plus::string& txt, const source_spec& src )
		:
			text  ( txt ),
			source( src )
		{
		}
	};
	
	struct user_exception
	{
		const Value        object;
		const source_spec  source;
		
		user_exception( const Value& obj, const source_spec& src )
		:
			object( obj ),
			source( src )
		{
		}
	};
	
	struct invalid_token_error
	{
		const plus::string  token;
		const source_spec   source;
		
		invalid_token_error( const plus::string& tok, const source_spec& src )
		:
			token ( tok ),
			source( src )
		{
		}
	};
	
	struct undeclared_symbol_error
	{
		const plus::string  name;
		const source_spec   source;
		
		undeclared_symbol_error( const plus::string& n, const source_spec& src )
		:
			name  ( n   ),
			source( src )
		{
		}
	};
	
}

#endif
