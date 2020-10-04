/*
	json/decode.hh
	--------------
*/

#ifndef VXS_LIB_JSON_DECODE_HH
#define VXS_LIB_JSON_DECODE_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	namespace json
	{
		
		Box json_decode( const char* json );
		
	}
	
	using json::json_decode;
}

#endif
