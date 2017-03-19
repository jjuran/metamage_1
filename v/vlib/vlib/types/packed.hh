/*
	packed.hh
	---------
*/

#ifndef VLIB_TYPES_PACKED_HH
#define VLIB_TYPES_PACKED_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch packed_dispatch;
	
	class Packed : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_packed;
			}
			
			static Value coerce( const Value& v );
			
			Packed( const plus::string& s = plus::string::null )
			:
				Value( (const vu_string&) s, V_pack, &packed_dispatch )
			{
			}
	};
	
	extern const type_info packed_vtype;
	
}

#endif
