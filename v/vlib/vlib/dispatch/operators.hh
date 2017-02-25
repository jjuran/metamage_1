/*
	operators.hh
	------------
*/

#ifndef VLIB_DISPATCH_OPERATORS_HH
#define VLIB_DISPATCH_OPERATORS_HH


namespace vlib
{
	
	class Value;
	
	typedef Value (*handler_1arg)( op_type op, const Value& v );
	typedef Value (*handler_2arg)( op_type op, const Value& a, const Value& b );
	
	struct operators
	{
		handler_1arg  unary;
		handler_2arg  binary;
	};
	
}

#endif
