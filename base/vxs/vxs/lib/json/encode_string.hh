/*
	encode_string.hh
	----------------
*/

#ifndef VXS_LIB_JSON_ENCODESTRING_HH
#define VXS_LIB_JSON_ENCODESTRING_HH

// vxo
#include "vxo/box.hh"


namespace vxo  {
namespace json {
	
	Box encode_string( const char* p, unsigned n );
	
}
}

#endif
