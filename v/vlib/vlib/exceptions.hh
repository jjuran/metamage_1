/*
	exceptions.hh
	-------------
*/

#ifndef VLIB_EXCEPTIONS_HH
#define VLIB_EXCEPTIONS_HH

// iota
#include "iota/class.hh"

// plus
#include "plus/string.hh"

// vlib
#include "vlib/source.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct transfer_via_break
	{
		const source_spec  source;
		
		transfer_via_break( const source_spec& src ) : source( src )
		{
		}
		
		NO_NEW_DELETE
	};
	
	struct transfer_via_continue
	{
		const source_spec  source;
		
		transfer_via_continue( const source_spec& src ) : source( src )
		{
		}
		
		NO_NEW_DELETE
	};
	
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
		
		NO_NEW_DELETE
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
		
		NO_NEW_DELETE
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
		
		NO_NEW_DELETE
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
		
		NO_NEW_DELETE
	};
	
}

#endif
