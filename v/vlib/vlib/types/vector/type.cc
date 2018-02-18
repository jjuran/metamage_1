/*
	type.cc
	-------
*/

#include "vlib/types/vector/type.hh"

// plus
#include "plus/string/concat.hh"

// vlib
#include "vlib/equal.hh"
#include "vlib/string-utils.hh"
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
	plus::string vectortype_str_make( const Value& v )
	{
		const Vector_Type& vt = static_cast< const Vector_Type& >( v );
		
		return "vector" + rep( make_array( vt.element_type() ) );
	}
	
	static const stringify vectortype_str =
	{
		NULL,
		NULL,
		NULL,
		&vectortype_str_make,
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
					return Vector( a, packed->string() );
				}
				
				return Vector( a, integer_cast< size_t >( b ) );
			
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
		if ( const Vector* vector = v.is< Vector >() )
		{
			return equal( vector->get_type(), type );
		}
		
		return false;
	}
	
	static
	Value transform( const Value& type, const Value& v )
	{
		if ( typecheck( type, v ) )
		{
			return v;
		}
		
		if ( const Packed* packed = v.is< Packed >() )
		{
			return Vector( type, packed->string() );
		}
		
		return NIL;
	}
	
	static const typing type =
	{
		&typecheck,
		&transform,
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
