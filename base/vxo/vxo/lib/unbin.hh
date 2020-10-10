/*
	unbin.hh
	--------
*/

#ifndef VXO_LIB_UNBIN_HH
#define VXO_LIB_UNBIN_HH

// vxo
#include "vxo/error.hh"
#include "vxo/string.hh"


namespace vxo
{
	
	typedef unsigned align_t;
	
	typedef Error_or< String > Error_or_String;
	
	Error_or_String unbin( const char* data, size_t size, align_t align );
	
	template < class String >
	inline
	Error_or_String unbin( const String& s, align_t align = 1 )
	{
		return unbin( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
