/*
	quote.hh
	--------
*/

#ifndef VC_QUOTE_HH
#define VC_QUOTE_HH

// plus
#include "plus/string.hh"


namespace vc
{
	
	inline
	plus::string unquote_string( const plus::string& s )
	{
		return s.substr( 1, s.size() - 2 );
	}
	
	plus::string unquote_escaped_string( const plus::string& s );
	
}

#endif
