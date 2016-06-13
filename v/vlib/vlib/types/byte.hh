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
			
		#define IBOX( i )  plus::ibox( (unsigned long) i ).move()
			
			Byte( unsigned char c = 0 ) : Value( IBOX( c ), Value_byte )
			{
			}
			
		#undef IBOX
	};
	
	extern const type_info byte_vtype;
	
}

#endif
