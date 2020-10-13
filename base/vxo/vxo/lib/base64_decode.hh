/*
	base64_decode.hh
	----------------
*/

#ifndef VXO_LIB_BASE64DECODE_HH
#define VXO_LIB_BASE64DECODE_HH

// vxo
#include "vxo/error.hh"
#include "vxo/string.hh"


namespace vxo
{
	
	Expected< String > base64_decode( const char* data, size_t padded_size );
	
	template < class String >
	inline
	Expected< String > base64_decode( const String& s )
	{
		return base64_decode( get_string_data( s ), get_string_size( s ) );
	}
	
}

#endif
