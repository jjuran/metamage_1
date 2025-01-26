/*
	string_or_nothing.hh
	--------------------
*/

#ifndef VXOSTRING_LIB_STRINGORNOTHING_HH
#define VXOSTRING_LIB_STRINGORNOTHING_HH

// plus
#include "plus/string.hh"

// vxo-string
#include "vxo-string/string.hh"


namespace vxo
{
	
	static inline
	const plus::string& string_or_nothing( const vxo::Box& xstring )
	{
		if ( const String* string = xstring.is< String >() )
		{
			return string->get();
		}
		
		return plus::string::null;
	}
	
}

#endif
