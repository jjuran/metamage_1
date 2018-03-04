/*
	iterator.hh
	-----------
*/

#ifndef VLIB_TYPES_ITERATOR_HH
#define VLIB_TYPES_ITERATOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch iterator_dispatch;
	
	class Iterator : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &iterator_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			Iterator( const Value& container )
			:
				Value( dummy_operand, Op_begin, container, &iterator_dispatch )
			{
			}
	};
	
	extern const type_info iterator_vtype;
	
}

#endif
