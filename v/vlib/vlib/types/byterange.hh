/*
	byterange.hh
	------------
*/

#ifndef VLIB_TYPES_BYTERANGE_HH
#define VLIB_TYPES_BYTERANGE_HH

// iota
#include "iota/byte_range.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch byterange_dispatch;
	
	class ByteRange : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &byterange_dispatch;
			}
			
			ByteRange( const Value& left, op_type op, const Value& right );
			
			iota::byte_range get() const
			{
				return pod_cast< iota::byte_range >();
			}
	};
	
	extern const type_info byterange_vtype;
	
}

#endif
