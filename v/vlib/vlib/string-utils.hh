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
	
	enum stringification
	{
		Stringified_to_pack = -1,
		Stringified_to_print,
		Stringified_to_reproduce,
	};
	
	plus::string make_string( const Value& value, stringification mode );
	
	inline
	plus::string rep( const Value& v )
	{
		return make_string( v, Stringified_to_reproduce );
	}
	
	inline
	Value pack( const Value& v )
	{
		return make_data( make_string( v, Stringified_to_pack ) );
	}
	
	plus::string repeat( const plus::string& s, plus::string::size_type n );
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n );
	
	Value lines( const plus::string& s );
	
	plus::string format( const plus::string& form, const Value& params );
	
}

#endif
