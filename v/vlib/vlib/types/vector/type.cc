/*
	type.cc
	-------
*/

#include "vlib/types/vector/type.hh"

// plus
#include "plus/string/concat.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/equal.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
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
		const Value* type = &expr()->left;
		
		if ( Expr* expr = type->listexpr() )
		{
			type = &expr->left;
		}
		
		return *type;
	}
	
	bool Vector_Type::has_fixed_length() const
	{
		const Value* type = &expr()->left;
		
		return type->listexpr() != NULL;
	}
	
	size_t Vector_Type::fixed_length() const
	{
		const Value* type = &expr()->left;
		
		if ( Expr* expr = type->listexpr() )
		{
			const Value& length = expr->right;
			
			return integer_cast< size_t >( length );
		}
		
		return 0;
	}
	
	static
	plus::string vectortype_str_make( const Value& v )
	{
		const Vector_Type& vt = static_cast< const Vector_Type& >( v );
		
		return "vector" + rep( make_array( vt.expr()->left ) );
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
	Value vectortype_member( const Vector_Type& vt, const Value& member )
	{
		const String* name_string = member.is< String >();
		
		if ( name_string == NULL )
		{
			THROW( "non-string vector type member" );
		}
		
		const plus::string& name = name_string->string();
		
		if ( ! vt.has_fixed_length() )
		{
			THROW( "no such (free-length) vector type member" );
		}
		
		if ( name == "size" )
		{
			return Integer( sizeof_vector( vt, vt.fixed_length() ) );
		}
		
		if ( name == "encode" )
		{
			return Type( packed_vtype );
		}
		
		if ( name == "decode" )
		{
			return vt;
		}
		
		return NIL;
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
					if ( vt.has_fixed_length() )
					{
						return Vector( a, packed->string(), vt.fixed_length() );
					}
					
					return Vector( a, packed->string() );
				}
				
				if ( b.type() == Value_empty_list )
				{
					return Vector( a, vt.fixed_length() );
				}
				
				return Vector( a, integer_cast< size_t >( b ) );
			
			case Op_member:
				return vectortype_member( vt, b );
			
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
		NULL,
		&transform,
		Type_pure,
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
