/*
	quote.hh
	--------
*/

#ifndef VLIB_QUOTE_HH
#define VLIB_QUOTE_HH

// plus
#include "plus/string.hh"


namespace vlib
{
	
	char decode_escaped_byte( const char*& p );
	
	unsigned count_quoted_bytes( const char* p );
	
	unsigned char unquote_byte( const plus::string& s );
	
	inline
	plus::string unquote_string( const plus::string& s )
	{
		return s.substr( 1, s.size() - 2 );
	}
	
	plus::string unquote_escaped_string( const plus::string& s );
	
	plus::string::size_type quoted_length( unsigned char c );
	
	char* copy_quotable_byte( char* q, unsigned char c );
	
	char* quote_byte( char* p, unsigned char c );
	
	plus::string quote_string( const plus::string& s );
	
}

#endif
