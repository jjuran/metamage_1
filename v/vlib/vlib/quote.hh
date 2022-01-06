/*
	quote.hh
	--------
*/

#ifndef VLIB_QUOTE_HH
#define VLIB_QUOTE_HH

// Standard C
#include <stdint.h>

// plus
#include "plus/string.hh"


namespace vlib
{
	
	char decode_escaped_byte( const char*& p );
	
	unsigned count_quoted_bytes( const char* p );
	
	uint32_t unquote_mb32( const char* p );
	
	inline
	plus::string unquote_string( const plus::string& s )
	{
		return s.substr( 1, s.size() - 2 );
	}
	
	plus::string unquote_escaped_string( const plus::string& s );
	
	plus::string quote_string( const plus::string& s );
	
}

#endif
