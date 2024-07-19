/*
	type.cc
	-------
*/

#include "vlib/types/type.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/builtin.hh"
#include "vlib/types/null.hh"


namespace vlib
{
	
	static
	bool not_null( const Value& v )
	{
		return &v.typeinfo() != &null_vtype;
	}
	
	static const veritization type_veritization =
	{
		&not_null,
	};
	
	static
	Value call_type_constructor( const Value& type, const Value& arguments )
	{
		const type_info& typeinfo = type.typeinfo();
		
		if ( coerce_proc coerce = typeinfo.coerce )
		{
			return coerce( arguments );
		}
		
		const Value coerced = typeinfo.assign( arguments );
		
		if ( ! coerced )
		{
			THROW( "invalid type conversion arguments" );
		}
		
		return coerced;
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				if ( v.is< Null >() )
				{
					// null is its own type
					return v;
				}
				
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
				return call_type_constructor( a, b );
			
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
	
	const dispatch type_dispatch =
	{
		&builtin_stringifiers,
		&type_veritization,
		&builtin_comparison,
		&ops,
	};
	
	const type_info type_vtype =
	{
		"type",
		&assign_to< Type >,
		0,
		0,
		0,
		Type_pure,
	};
	
}
