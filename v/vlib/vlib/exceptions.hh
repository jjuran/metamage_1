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


namespace vlib
{
	
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
	
}

#endif
