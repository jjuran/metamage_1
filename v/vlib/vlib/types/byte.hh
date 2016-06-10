/*
	byte.hh
	-------
*/

#ifndef VLIB_TYPES_BYTE_HH
#define VLIB_TYPES_BYTE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	class Byte : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_byte;
			}
			
			static Value coerce( const Value& v );
			
			Byte( unsigned char c = 0 ) : Value( Value::byte( c ) )
			{
			}
	};
	
	extern const type_info byte_vtype;
	
}

#endif
