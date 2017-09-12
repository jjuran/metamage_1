/*
	type.cc
	-------
*/

#include "vlib/types/struct/type.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/string/concat.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/equal.hh"
#include "vlib/function-utils.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/bareword.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// vx
#include "vlib/types/struct/struct.hh"


#define ANONYMOUS_STRUCT  "<anonymous struct>"

#define STRLEN(s)  (sizeof "" s - 1)


namespace vlib
{

static
const Value& unembrace( const Value& field_defs )
{
	if ( Expr* expr = field_defs.expr() )
	{
		if ( (op_type) (uint8_t) expr->op == Op_block )
		{
			expr = expr->right.expr();
			
			if ( expr  &&  (op_type) (uint8_t) expr->op == Op_scope )
			{
				return expr->right;
			}
		}
	}
	
	return field_defs;
}

Struct_Type::Struct_Type( const Value& fields, const Value& name )
:
	Value( name, Op_empower, unembrace( fields ), &structtype_dispatch )
{
}

const Value& Struct_Type::name() const
{
	return expr()->left;
}

const Value& Struct_Type::fields() const
{
	return expr()->right;
}

static const String size_name( "size" );

static const String encode_name( "encode" );
static const String decode_name( "decode" );

static
handler_2arg get_binary_ops_handler( const Value& object )
{
	if ( const dispatch* methods = object.dispatch_methods() )
	{
		if ( const operators* ops = methods->ops )
		{
			if ( handler_2arg handler = ops->binary )
			{
				return handler;
			}
		}
	}
	
	THROW( "invalid encoder/decoder object in struct type" );
	
	return NULL;
}

static inline
handler_2arg get_binary_ops_handler_unchecked( const Value& object )
{
	return object.dispatch_methods()->ops->binary;
}

static
Value member_op( handler_2arg handler, const Value& obj, const Value& name )
{
	if ( const Value result = handler( Op_member, obj, name ) )
	{
		return result;
	}
	
	THROW( "required endec member missing" );
	
	return NIL;
}

static inline
Value endec_encode( handler_2arg handler, const Value& endec )
{
	return member_op( handler, endec, encode_name );
}

static inline
Value endec_decode( handler_2arg handler, const Value& endec )
{
	return member_op( handler, endec, decode_name );
}

static
size_t field_size( const Value& field )
{
	if ( Expr* expr = field.expr() )
	{
		if ( expr->op == Op_mapping )
		{
			const Value& type = expr->right;
			
			handler_2arg handler = get_binary_ops_handler( type );
			
			const Value size = member_op( handler, type, size_name );
			
			return integer_cast< size_t >( size );
		}
	}
	
	THROW( "struct field type has no defined size" );
	
	return 0;
}

size_t Struct_Type::byte_size() const
{
	size_t size = 0;
	
	list_iterator defs = fields();
	
	while ( defs )
	{
		size += field_size( defs.use() );
	}
	
	return size;
}

static
Value decode_field( const Value& endec, const plus::string& bytes )
{
	handler_2arg handler = get_binary_ops_handler_unchecked( endec );
	
	const Value decode = endec_decode( handler, endec );
	
	const Value decoded = call_function( decode, Packed( bytes ) );
	
	return decoded;
}

static
plus::string encode_field( const Value& endec, const Value& v )
{
	handler_2arg handler = get_binary_ops_handler_unchecked( endec );
	
	const Value encode = endec_encode( handler, endec );
	
	const Value encoded = call_function( encode, v );
	
	return encoded.string();
}

Value Struct_Type::get( const Value& data, const Member& name ) const
{
	size_t offset = 0;
	size_t size   = 0;
	
	list_iterator defs = fields();
	
	while ( defs )
	{
		const Value& field = defs.use();
		const Value& fname = field.expr()->left;
		
		size = field_size( field );
		
		if ( fname.string() == name.get() )
		{
			const Value& endec = field.expr()->right;
			plus::string bytes = data.string().substr( offset, size );
			
			return decode_field( endec, bytes );
		}
		
		offset += size;
	}
	
	THROW( "No such struct member" );
	
	return NIL;
}

void Struct_Type::set( Value& data, const Member& name, const Value& v ) const
{
	size_t offset = 0;
	size_t size   = 0;
	
	list_iterator defs = fields();
	
	while ( defs )
	{
		const Value& field = defs.use();
		const Value& fname = field.expr()->left;
		
		size = field_size( field );
		
		if ( fname.string() == name.get() )
		{
			const Value& endec = field.expr()->right;
			plus::string bytes = encode_field( endec, v );
			
			if ( bytes.size() != size )
			{
				THROW( "struct field size mismatch" );
			}
			
			char* start = (char*) &data.unshare().string()[ offset ];
			
			memcpy( start, bytes.data(), size );
			return;
		}
		
		offset += size;
	}
	
	THROW( "No such struct member" );
}

Value Struct_Type::pack_initializer_list( const Value& init_list ) const
{
	const size_t size = byte_size();
	
	plus::string buffer;
	
	char* p = buffer.reset( size );
	
	char* end = p + size;
	
	list_iterator defs = fields();
	list_iterator data = init_list;
	
	while ( defs  &&  data )
	{
		const Value& endec = defs.use().expr()->right;
		
		handler_2arg handler = get_binary_ops_handler_unchecked( endec );
		
		const Value encode = endec_encode( handler, endec );
		
		const Value encoded = call_function( encode, data.use() );
		
		const plus::string& s = encoded.string();
		
		p = (char*) mempcpy( p, s.data(), s.size() );
	}
	
	if ( data )
	{
		THROW( "Too many struct field initializers" );
	}
	
	memset( p, '\0', end - p );
	
	return Packed( buffer );
}

Value Struct_Type::unpack_fields( const Value& data ) const
{
	size_t offset = 0;
	size_t size   = 0;
	
	list_iterator defs = fields();
	list_builder  list;
	
	while ( defs )
	{
		const Value& field = defs.use();
		const Value& fname = field.expr()->left;
		
		size = field_size( field );
		
		const Value& endec = field.expr()->right;
		plus::string bytes = data.string().substr( offset, size );
		
		Value mapping( fname, Op_mapping, decode_field( endec, bytes ) );
		
		list.append( mapping );
		
		offset += size;
	}
	
	return list;
}

static
const char* structtype_str_data( const Value& v )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( v );
	
	const plus::string& name = st.name().string();
	
	return ! name.empty() ? name.c_str() : ANONYMOUS_STRUCT;
}

static
size_t structtype_str_size( const Value& v )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( v );
	
	const size_t size = st.name().string().size();
	
	return size ? size : STRLEN( ANONYMOUS_STRUCT );
}

static const stringify structtype_str =
{
	&structtype_str_data,
	&structtype_str_size,
	NULL,
};

static
plus::string structtype_rep_make( const Value& v )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( v );
	
	const plus::string& name = st.name().string();
	
	return "struct " +
	       name +
	       &" "[ name.empty() ] +
	       rep( Value( Op_block, st.fields() ) );
}

static const stringify structtype_rep =
{
	NULL,
	NULL,
	NULL,
	&structtype_rep_make,
};

const stringifiers structtype_stringifiers =
{
	&structtype_str,
	&structtype_rep,
	// bin: not defined
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( v );
	
	switch ( op )
	{
		case Op_typeof:
			return Type( type_vtype );
		
		case Op_unary_deref:
			return st.fields();
		
		default:
			break;
	}
	
	return NIL;
}

static
Value structtype_member( const Struct_Type& st, const Value& member )
{
	const String* name_string = member.is< String >();
	
	if ( name_string == NULL )
	{
		THROW( "non-string struct type member" );
	}
	
	const plus::string& name = name_string->string();
	
	if ( name == "name" )
	{
		return st.name();
	}
	
	if ( name == "size" )
	{
		return Integer( st.byte_size() );
	}
	
	if ( name == "encode" )
	{
		return Type( packed_vtype );
	}
	
	if ( name == "decode" )
	{
		return st;
	}
	
	return NIL;
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( a );
	
	switch ( op )
	{
		case Op_function:
		case Op_named_unary:
			if ( const Packed* packed = b.is< Packed >() )
			{
				return Struct( st, packed->string() );
			}
			
			if ( is_empty_list( b )  ||  is_empty_array( b ) )
			{
				return Struct( st );
			}
			
			if ( is_array( b ) )
			{
				return Struct( st, b.expr()->right );
			}
			
			THROW( "invalid struct construction" );
			break;
		
		case Op_member:
			return structtype_member( st, b );
		
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

static
bool typecheck( const Value& type, const Value& v )
{
	if ( const Struct* object = v.is< Struct >() )
	{
		return equal( object->get_type(), type );
	}
	
	return false;
}

static
Value transform( const Value& type, const Value& v )
{
	const Struct_Type& st = static_cast< const Struct_Type& >( type );
	
	if ( typecheck( type, v ) )
	{
		return v;
	}
	
	if ( const Packed* packed = v.is< Packed >() )
	{
		return Struct( st, packed->string() );
	}
	
	return NIL;
}

static const typing type =
{
	&typecheck,
	&transform,
	Type_pure,
};

const dispatch structtype_dispatch =
{
	&structtype_stringifiers,
	NULL,
	NULL,
	&ops,
	&type,
};

}
