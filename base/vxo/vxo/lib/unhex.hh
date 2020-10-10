/*
	unhex.hh
	--------
*/

#ifndef VXO_LIB_UNHEX_HH
#define VXO_LIB_UNHEX_HH

// vxo
#include "vxo/error.hh"
#include "vxo/string.hh"


namespace vxo
{
	
	typedef unsigned align_t;
	
	typedef Error_or< String > Error_or_String;
	
	Error_or_String unhex( const char* data, size_t size, align_t align );
	
	template < class String >
	inline
	Error_or_String unhex( const String& s, align_t align = 1 )
	{
		return unhex( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
