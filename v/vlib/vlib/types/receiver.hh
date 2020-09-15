/*
	receiver.hh
	-----------
*/

#ifndef VLIB_TYPES_RECEIVER_HH
#define VLIB_TYPES_RECEIVER_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch receiver_dispatch;
	
	class Receiver : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &receiver_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			Receiver( const Value& function )
			:
				Value( dummy_operand, Op_recv, function, &receiver_dispatch )
			{
			}
	};
	
	extern const type_info receiver_vtype;
	
}

#endif
