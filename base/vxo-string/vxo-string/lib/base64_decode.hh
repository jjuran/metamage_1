/*
	base64_decode.hh
	----------------
*/

#ifndef VXOSTRING_LIB_BASE64DECODE_HH
#define VXOSTRING_LIB_BASE64DECODE_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	Box base64_decode( const char* data, size_t padded_size );
	
	template < class String >
	inline
	Box base64_decode( const String& s )
	{
		return base64_decode( get_string_data( s ), get_string_size( s ) );
	}
	
}

#endif
