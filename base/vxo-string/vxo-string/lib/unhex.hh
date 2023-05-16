/*
	unhex.hh
	--------
*/

#ifndef VXOSTRING_LIB_UNHEX_HH
#define VXOSTRING_LIB_UNHEX_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	typedef unsigned align_t;
	
	Box unhex( const char* data, size_t size, align_t align );
	
	template < class String >
	inline
	Box unhex( const String& s, align_t align = 1 )
	{
		return unhex( get_string_data( s ), get_string_size( s ), align );
	}
	
}

#endif
