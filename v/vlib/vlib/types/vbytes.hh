/*
	vbytes.hh
	---------
*/

#ifndef VLIB_TYPES_VBYTES_HH
#define VLIB_TYPES_VBYTES_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Byte;
	struct comparison;
	struct dispatch;
	struct stringify;
	
	extern const stringify vbytes_cpy;
	extern const comparison vbytes_comparison;
	
	class VBytes : public Value
	{
		public:
			VBytes( const plus::string& s, value_type type, const dispatch* d )
			:
				Value( (const vu_string&) s, type, d )
			{
			}
	};
	
	Value assign_byte_to_index( Value& s, const Value& x, const Byte& byte );
	
	Value division( const VBytes& bytes, const Value& divisor );
	
}

#endif
