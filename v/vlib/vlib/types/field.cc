/*
	field.cc
	--------
*/

#include "vlib/types/field.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"


#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace vlib
{
	
	static
	const char* field_str_data( const Value& v )
	{
		const Field& type = (const Field&) v;
		
		return type.name();
	}
	
	static const stringify field_str =
	{
		&field_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers field_stringifiers =
	{
		&field_str,
		// For rep, fall back to str
		// Type bin is unimplemented
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
	const Value* encode_decode( const Field& type, const plus::string& member )
	{
		const char*  member_data = member.data();
		const size_t member_size = member.size();
		
		if ( member_size == STRLEN( "xxcode" ) )
		{
			if ( memcmp( member_data + 2, STR_LEN( "code" ) ) != 0 )
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
		
		if ( memcmp( member_data, STR_LEN( "en" ) ) == 0 )
		{
			return &type.encoder();
		}
		
		if ( memcmp( member_data, STR_LEN( "de" ) ) == 0 )
		{
			return &type.decoder();
		}
		
		return NULL;
	}
	
	static
	Value field_member( const Field& type, const plus::string& name )
	{
		if ( name == "size" )
		{
			return type.size();
		}
		
		if ( const Value* endec = encode_decode( type, name ) )
		{
			return *endec;
		}
		
		THROW( "no such field member" );
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const Field& type = (const Field&) a;
		
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				if ( extend_proc extend = type.base_type().extend )
				{
					if ( Value v = extend( b ) )
					{
						return v;
					}
				}
				
				return type.base_type().coerce( b );
			
			case Op_subscript:
				if ( is_empty_list( b ) )
				{
					return Value( a, op, b );
				}
				
				break;
			
			case Op_member:
				if ( b.type() != V_str )
				{
					THROW( "non-string member name" );
				}
				
				return field_member( type, b.string() );
			
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
	bool typecheck( const Value& t, const Value& v )
	{
		const Field& type = (const Field&) t;
		
		return type.base_type().assign( v ).type();
	}
	
	static
	Value construct( const Value& t, const Value& v )
	{
		const Field& type = (const Field&) t;
		
		return type.base_type().assign( v );
	}
	
	static
	Value transform( const Value& t, const Value& v )
	{
		const Field& type = (const Field&) t;
		
		return type.base_type().coerce( v );
	}
	
	static const typing type =
	{
		&typecheck,
		&construct,
		&transform,
		Type_pure,
	};
	
	const dispatch field_dispatch =
	{
		&field_stringifiers,
		NULL,
		NULL,
		&ops,
		&type,
	};
	
	Field::Field( unsigned          size,
	              const type_info&  type,
	              const proc_info&  encode,
	              const proc_info&  decode )
	:
		Value( Value( Type( type ), Op_mapping, Integer( size ) ),
		       Op_typeof,
		       Value( Proc( encode ), Proc( decode ) ),
		       &field_dispatch )
	{
	}
	
}
