/*
	string-utils.hh
	---------------
*/

#ifndef VLIB_STRINGUTILS_HH
#define VLIB_STRINGUTILS_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	plus::string make_string( const Value& value );
	
	plus::string repeat( const plus::string& s, plus::string::size_type n );
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n );
	
}

#endif
