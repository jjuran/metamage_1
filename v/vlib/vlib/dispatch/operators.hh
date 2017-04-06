/*
	operators.hh
	------------
*/

#ifndef VLIB_DISPATCH_OPERATORS_HH
#define VLIB_DISPATCH_OPERATORS_HH

// vlib
#include "vlib/op_type.hh"


namespace vlib
{
	
	struct Target;
	class Value;
	
	typedef Value (*handler_1arg)( op_type op, const Value& v );
	typedef Value (*handler_2arg)( op_type op, const Value& a, const Value& b );
	
	typedef Value (*handler_step)( op_type op, const Target& target );
	
	typedef Value (*handler_into)( op_type        op,
	                               const Target&  target,
	                               const Value&   x,
	                               const Value&   b );
	
	struct operators
	{
		handler_1arg  unary;
		handler_2arg  binary;
		handler_step  advance;
		handler_into  mutating;
	};
	
}

#endif
