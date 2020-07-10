/*
	invocation.hh
	-------------
*/

#ifndef VLIB_TYPES_INVOCATION_HH
#define VLIB_TYPES_INVOCATION_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch invocation_dispatch;
	
	class Invocation : public Value
	{
		public:
			Invocation( const Value& invoke, const Value& expr )
			:
				Value( invoke, Op_invocation, expr, &invocation_dispatch )
			{
			}
	};
	
	const Value& dereference_block( const Value& invocation );
	
}

#endif
