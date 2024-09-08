/*
	metatype.cc
	-----------
*/

#include "vlib/types/vector/metatype.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/type.hh"

// vx
#include "vlib/types/vector/vector.hh"
#include "vlib/types/vector/type.hh"


namespace vlib
{
	
	static
	const char* vectormetatype_str_data( const Value& v )
	{
		return "vector";
	}
	
	static const stringify vectormetatype_str =
	{
		&vectormetatype_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers vectormetatype_stringifiers =
	{
		&vectormetatype_str,
		// rep: ditto
		// bin: not defined
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( type_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
			case Op_subscript:
				return Vector_Type( b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	static
	bool typecheck( const Value& type, const Value& v )
	{
		return Vector::test( v );
	}
	
	static const typing type =
	{
		&typecheck,
		NULL,
		NULL,
		Type_pure,
	};
	
	const dispatch vectormetatype_dispatch =
	{
		&vectormetatype_stringifiers,
		NULL,
		NULL,
		&ops,
		&type,
	};
	
}
