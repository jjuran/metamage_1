/*
	unhex.hh
	--------
*/

#ifndef VXS_LIB_UNHEX_HH
#define VXS_LIB_UNHEX_HH

// vxo
#include "vxo/error.hh"

// vxs
#include "vxs/string.hh"


namespace vxo
{
	
	typedef unsigned align_t;
	
	typedef Expected< String > Expected_String;
	
	Expected_String unhex( const char* data, size_t size, align_t align );
	
	template < class String >
	inline
	Expected_String unhex( const String& s, align_t align = 1 )
	{
		return unhex( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
