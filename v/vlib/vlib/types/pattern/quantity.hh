/*
	quantity.hh
	-----------
*/

#ifndef VLIB_TYPES_PATTERN_QUANTITY_HH
#define VLIB_TYPES_PATTERN_QUANTITY_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch patternquantity_dispatch;
	
	class PatternQuantity : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &patternquantity_dispatch;
			}
			
			PatternQuantity( op_type quantity_op, const Value& base );
	};
	
	extern const type_info patternquantity_vtype;
	
}

#endif
