/*
	quantity.cc
	-----------
*/

#include "vlib/types/pattern/quantity.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	PatternQuantity::PatternQuantity( op_type quantity_op, const Value& base )
	:
		Value( base, quantity_op, dummy_operand, &patternquantity_dispatch )
	{
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( patternquantity_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch patternquantity_dispatch =
	{
		0,
		0,
		0,
		&ops,
	};
	
	const type_info patternquantity_vtype =
	{
		"pattern-quantity",
		&assign_to< PatternQuantity >,
		0,
		0,
		Type_pure,
	};
	
}
