/*
	receiver.cc
	-----------
*/

#include "vlib/types/receiver.hh"

// vlib
#include "vlib/function-utils.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	Value Receiver::coerce( const Value& v )
	{
		return Receiver( v );
	}
	
	static
	const char* receiver_str_data( const Value& v )
	{
		return "<receiver>";
	}
	
	static const stringify receiver_str =
	{
		&receiver_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers receiver_stringifiers =
	{
		&receiver_str,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( receiver_vtype );
			
			case Op_recv:
				return call_function( v.expr()->right, empty_list );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch receiver_dispatch =
	{
		&receiver_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	const type_info receiver_vtype =
	{
		"receiver",
		&assign_to< Receiver >,
		&Receiver::coerce,
		NULL,
		NULL,
		0,
	};
	
}
