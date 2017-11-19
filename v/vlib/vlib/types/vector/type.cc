/*
	type.cc
	-------
*/

#include "vlib/types/vector/type.hh"

// vlib
#include "vlib/equal.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"

// vx
#include "vlib/types/vector/vector.hh"


namespace vlib
{
	
	Vector_Type::Vector_Type( const Value& element_type )
	:
		Value( element_type, Op_empower, nothing, &vectortype_dispatch )
	{
	}
	
	const Value& Vector_Type::element_type() const
	{
		return expr()->left;
	}
	
	static
	const char* vectortype_str_data( const Value& v )
	{
		return "<vector type>";
	}
	
	static const stringify vectortype_str =
	{
		&vectortype_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers vectortype_stringifiers =
	{
		&vectortype_str,
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
		const Vector_Type& vt = static_cast< const Vector_Type& >( a );
		
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				if ( const Packed* packed = b.is< Packed >() )
				{
					return Vector( vt.element_type(), packed->string() );
				}
				
				return Vector( vt.element_type(), integer_cast< size_t >( b ) );
			
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
		const Vector_Type& vt = static_cast< const Vector_Type& >( type );
		
		if ( const Vector* vector = v.is< Vector >() )
		{
			return equal( vector->get_endec(), vt.element_type() );
		}
		
		return false;
	}
	
	static const typing type =
	{
		&typecheck,
	};
	
	const dispatch vectortype_dispatch =
	{
		&vectortype_stringifiers,
		NULL,
		NULL,
		&ops,
		&type,
	};
	
}
