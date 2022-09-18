/*
	endianness.hh
	-------------
*/

#ifndef VLIB_TYPES_ENDIANNESS_HH
#define VLIB_TYPES_ENDIANNESS_HH

// iota
#include "iota/endian.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch endianness_dispatch;
	
	enum endianness
	{
		big_endian    = -1,
		native_endian =  0,
		little_endian =  1,
	};
	
	class Endianness : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &endianness_dispatch;
			}
			
			explicit Endianness( endianness e );
			
			bool swapped() const
			{
				endianness e = pod_cast< endianness >();
				
				return e  &&  e < 0 == iota::is_little_endian();
			}
	};
	
}

#endif
