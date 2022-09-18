/*
	endian.cc
	---------
*/

#include "vlib/types/endian.hh"

// plus
#include "plus/reverse.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/member.hh"
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/types/endianness.hh"
#include "vlib/types/field.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"
#include "vlib/types/struct/type.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vector/type.hh"


namespace vlib
{

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( type_vtype );
		
		case Op_unary_deref:
			return v.expr()->right;
		
		default:
			break;
	}
	
	return Value();
}

static
Value v_reverse( const Value& v )
{
	Value bytes = v;
	
	plus::string& s = const_cast< plus::string& >( bytes.unshare().string() );
	
	plus::reverse_in_place( s );
	
	return bytes;
}

enum
{
	pure = Proc_pure,
};

static const proc_info proc_reverse = { "reverse", &v_reverse, NULL, pure };

static Proc reverse( proc_reverse );

static
Value endian_member( const Endian& endian, const Value& member )
{
	const String* name_string = member.is< String >();
	
	if ( name_string == NULL )
	{
		THROW( "non-string endianized field member" );
	}
	
	const Endianness& endianness = endian.endianness();
	const Value& underlying_type = endian.underlying_type();
	
	const plus::string& name = name_string->string();
	
	Value method = calc_member( underlying_type, member );
	
	if ( ! endianness.swapped()  ||  name == "size" )
	{
		return method;
	}
	
	if ( name == "encode" )
	{
		return Value( reverse, Op_multiply, method );
	}
	
	if ( name == "decode" )
	{
		return Value( method, Op_multiply, reverse );
	}
	
	return NIL;
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	const Endian& endian = static_cast< const Endian& >( a );
	
	switch ( op )
	{
		case Op_member:
			return endian_member( endian, b );
		
		default:
			break;
	}
	
	return NIL;
}

static const operators ops =
{
	&unary_op_handler,
	&binary_op_handler,
};

const dispatch endian_dispatch =
{
	0,  // NULL
	0,  // NULL
	0,  // NULL
	&ops,
};

Endian::Endian( const Endianness& e, const Value& type )
:
	Value( e, Op_named_unary, type, &endian_dispatch )
{
}

Value endianized( const Endianness& e, const Value& type )
{
	if ( type.is< Endian >() )
	{
		// The type is already endianized; pass it through.
		return type;
	}
	
	if ( const Field* field = type.is< Field >() )
	{
		if ( integer_cast< size_t >( field->size() ) == 1 )
		{
			return type;
		}
		
		return Endian( e, type );
	}
	
	if ( const Vector_Type* vt = type.is< Vector_Type >() )
	{
		const Vector_Type& type = *vt;
		
		const Value& element_type = type.element_type();
		
		Value ordered_type( endianized( e, element_type ) );
		
		if ( type.has_fixed_length() )
		{
			const size_t size = type.fixed_length();
			
			ordered_type = Value( ordered_type, Integer( size ) );
		}
		
		return Vector_Type( ordered_type );
	}
	
	if ( const Struct_Type* st = type.is< Struct_Type >() )
	{
		const Struct_Type& type = *st;
		
		const Value& name   = type.name();
		const Value& fields = type.fields();
		
		list_builder result;
		list_iterator it = fields;
		
		while ( it )
		{
			const Value& mapping = it.use();
			const Value& name = mapping.expr()->left;
			const Value& type = mapping.expr()->right;
			
			Value new_field( name, Op_mapping, endianized( e, type ) );
			
			result.append( new_field );
		}
		
		return Struct_Type( result, name );
	}
	
	return type;
}

}
