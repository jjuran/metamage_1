/*
	unbin.hh
	--------
*/

#ifndef VXOSTRING_LIB_UNBIN_HH
#define VXOSTRING_LIB_UNBIN_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	typedef unsigned align_t;
	
	Box unbin( const char* data, size_t size, align_t align );
	
	template < class String >
	inline
	Box unbin( const String& s, align_t align = 1 )
	{
		return unbin( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
