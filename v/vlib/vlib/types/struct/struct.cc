/*
	struct.cc
	---------
*/

#include "vlib/types/struct/struct.hh"

// vlib
#include "vlib/target.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/bareword.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/struct/type.hh"


namespace vlib
{

static
const char* struct_str_data( const Value& v )
{
	return "<struct>";
}

static const stringify struct_str =
{
	&struct_str_data,
	NULL,
	NULL,
};

static
const char* struct_bin_data( const Value& v )
{
	const Struct& object = static_cast< const Struct& >( v );
	
	const plus::string& memory = object.get_data().string();
	
	return memory.data();
}

static
size_t struct_bin_size( const Value& v )
{
	const Struct& object = static_cast< const Struct& >( v );
	
	const plus::string& memory = object.get_data().string();
	
	return memory.size();
}

static const stringify struct_bin =
{
	&struct_bin_data,
	&struct_bin_size,
	NULL,
};

static const stringifiers struct_stringifiers =
{
	&struct_str,
	NULL,
	&struct_bin,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	const Struct& object = static_cast< const Struct& >( v );
	
	switch ( op )
	{
		case Op_typeof:
			return object.get_type();
		
		case Op_unary_deref:
			return object.get_type().unpack_fields( object.get_data() );
		
		default:
			break;
	}
	
	return NIL;
}

static inline
Value struct_member( const Struct& object, const Member& member )
{
	return object.get_type().get( object.get_data(), member );
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	const Struct& object = static_cast< const Struct& >( a );
	
	switch ( op )
	{
		case Op_member:
			return struct_member( object, b.as< Member >() );
		
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

const dispatch struct_dispatch =
{
	&struct_stringifiers,
	NULL,
	NULL,
	&ops,
};

Struct::Struct( const Struct_Type& type, const plus::string& buffer )
:
	Value( type,
	       Op_internal_vector,
	       Packed( buffer ),
	       &struct_dispatch )
{
	if ( buffer.size() != type.byte_size() )
	{
		THROW( "struct input buffer has wrong size" );
	}
}

Struct::Struct( const Struct_Type& type, const Value& init_list )
:
	Value( type,
	       Op_internal_vector,
	       type.pack_initializer_list( init_list ),
	       &struct_dispatch )
{
}

}
