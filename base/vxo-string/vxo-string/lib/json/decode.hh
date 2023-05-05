/*
	json/decode.hh
	--------------
*/

#ifndef VXOSTRING_LIB_JSON_DECODE_HH
#define VXOSTRING_LIB_JSON_DECODE_HH

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
