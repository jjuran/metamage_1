/*
	string-utils.hh
	---------------
*/

#ifndef VC_STRINGUTILS_HH
#define VC_STRINGUTILS_HH

// plus
#include "plus/string.hh"

// vc
#include "vc/value.hh"


namespace vc
{
	
	plus::string make_string( const Value& value );
	
	plus::string repeat( const plus::string& s, plus::string::size_type n );
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n );
	
}

#endif
