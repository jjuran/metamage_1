/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// Standard C
#include <stdint.h>
#include <string.h>

// vlib
#include "vlib/error.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/type_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	extern const proc_info proc_lines;
	
	static
	Value assign_to_boolean( const Value& v )
	{
		if ( v.type() == Value_boolean )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_function( const Value& v )
	{
		if ( is_functional( v ) )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_byte( const Value& v )
	{
		if ( v.type() == Value_byte )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_integer( const Value& v )
	{
		if ( v.type() == Value_number )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_data( const Value& v )
	{
		if ( v.type() == Value_data )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_string( const Value& v )
	{
		if ( v.type() == Value_string )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_c_str( const Value& v )
	{
		if ( v.type() == Value_string )
		{
			const plus::string& s = v.string();
			
			if ( strlen( s.c_str() ) == s.size() )
			{
				return v;
			}
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_type( const Value& v )
	{
		if ( v.type() == Value_base_type )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value coerce_to_boolean( const Value& v )
	{
		switch ( v.type() )
		{
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_boolean()" );
			
			case Value_empty_list:
				return false;
			
			case Value_boolean:
				return v;
			
			case Value_byte:
			case Value_number:
				return ! v.number().is_zero();
			
			case Value_string:
				return ! v.string().empty();
			
			case Value_base_type:
			case Value_function:
			case Value_pair:
				if ( Expr* expr = v.expr() )
				{
					if ( expr->op == Op_array )
					{
						// Empty array is false.
						return ! is_empty( expr->right );
					}
				}
				
				return true;
		}
	}
	
	static
	Value coerce_to_byte( const Value& v )
	{
		switch ( v.type() )
		{
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_byte()" );
			
			case Value_empty_list:
				return Value::byte();
			
			case Value_byte:
			case Value_number:
				return Value::byte( uint8_t( v.number().clipped() * v.number().sign() ) );
		}
	}
	
	static
	Value coerce_to_integer( const Value& v )
	{
		switch ( v.type() )
		{
			default:
				TYPE_ERROR( "integer conversion not defined for type" );
			
			case Value_empty_list:
				return 0;
			
			case Value_boolean:
				return v.boolean() + 0;
			
			case Value_byte:
				return v.number();
			
			case Value_number:
				return v;
		}
	}
	
	static
	Value coerce_to_data( const Value& v )
	{
		switch ( v.type() )
		{
			default:
				TYPE_ERROR( "type not convertible to data" );
			
			case Value_empty_list:
				return make_data( plus::string::null );
			
			case Value_string:
				return make_data( v.string() );
			
			case Value_byte:
			case Value_pair:
				return pack( v );
			
			case Value_data:
				return v;
		}
	}
	
	static
	Value coerce_to_string( const Value& v )
	{
		return make_string( v, Stringified_to_print );
	}
	
	static
	Value data_member( const Value&         obj,
	                   const plus::string&  member )
	{
		if ( member == "string" )
		{
			return obj.string();
		}
		
		SYNTAX_ERROR( "nonexistent data member" );
		
		return Value_nothing;
	}
	
	static
	Value string_member( const Value& obj,
	                     const plus::string& member )
	{
		if ( member == "length" )
		{
			return obj.string().size();
		}
		
		if ( member == "join" )
		{
			return bind_args( proc_join, obj );
		}
		
		if ( member == "lines" )
		{
			return bind_args( proc_lines, obj );
		}
		
		SYNTAX_ERROR( "nonexistent string member" );
		
		return Value_nothing;
	}
	
	enum signedness
	{
		u,
		i,
	};
	
	template < bool S, int N >  struct stdint;
	
	template <>  struct stdint< i,  8 > { typedef int8_t   type; };
	template <>  struct stdint< u,  8 > { typedef uint8_t  type; };
	template <>  struct stdint< i, 16 > { typedef int16_t  type; };
	template <>  struct stdint< u, 16 > { typedef uint16_t type; };
	template <>  struct stdint< i, 32 > { typedef int32_t  type; };
	template <>  struct stdint< u, 32 > { typedef uint32_t type; };
	template <>  struct stdint< i, 64 > { typedef int64_t  type; };
	template <>  struct stdint< u, 64 > { typedef uint64_t type; };
	
	static
	uint64_t coerced_int( const Value& v )
	{
		return coerce_to_integer( v ).number().clipped_to< uint64_t >();
	}
	
	#define DEFINE_ADAPT_TO_INT( s, n )       \
	static                                    \
	Value assign_to_##s##n( const Value& v )  \
	{                                         \
		if ( v.type() == Value_number )  \
		{                                \
			if ( v.number().demotes_to< stdint< s, n >::type >() )  return v;  \
		}                                \
		return Value_nothing;            \
	}                                         \
	static                                    \
	Value coerce_to_##s##n( const Value& v )  \
	{ return (stdint< s, n >::type) coerced_int( v ); }
	
	DEFINE_ADAPT_TO_INT( i, 64 )
	DEFINE_ADAPT_TO_INT( u, 64 )
	DEFINE_ADAPT_TO_INT( i, 32 )
	DEFINE_ADAPT_TO_INT( u, 32 )
	DEFINE_ADAPT_TO_INT( i, 16 )
	DEFINE_ADAPT_TO_INT( u, 16 )
	DEFINE_ADAPT_TO_INT( i, 8  )
	DEFINE_ADAPT_TO_INT( u, 8  )
	
	static
	Value identity( const Value& v )
	{
		return v;
	}
	
	const type_info etc_vtype = { "...", &identity, 0, 0 };
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type, 0, 0 }
	
	#define DEFINE_TYPE_INFO_A_C( T )  \
	const type_info T##_vtype = { #T, &assign_to_##T, &coerce_to_##T, 0 }
	
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( c_str    );
	DEFINE_TYPE_INFO( type     );
	
	DEFINE_TYPE_INFO_A_C( i64 );
	DEFINE_TYPE_INFO_A_C( u64 );
	DEFINE_TYPE_INFO_A_C( i32 );
	DEFINE_TYPE_INFO_A_C( u32 );
	DEFINE_TYPE_INFO_A_C( i16 );
	DEFINE_TYPE_INFO_A_C( u16 );
	DEFINE_TYPE_INFO_A_C( i8  );
	DEFINE_TYPE_INFO_A_C( u8  );
	
	const type_info boolean_vtype =
	{
		"boolean",
		&assign_to_boolean,
		&coerce_to_boolean,
		0,
	};
	
	const type_info byte_vtype =
	{
		"byte",
		&assign_to_byte,
		&coerce_to_byte,
		0,
	};
	
	const type_info integer_vtype =
	{
		"integer",
		&assign_to_integer,
		&coerce_to_integer,
		0,
	};
	
	const type_info data_vtype =
	{
		"data",
		&assign_to_data,
		&coerce_to_data,
		&data_member,
	};
	
	const type_info string_vtype =
	{
		"string",
		&assign_to_string,
		&coerce_to_string,
		&string_member,
	};
	
	static
	Value v_join( const Value& args )
	{
		const Value& glue = first( args );
		
		const Value pieces = rest( args );
		
		return join( glue.string(), pieces, count( pieces ) );
	}
	
	static
	Value v_lines( const Value& v )
	{
		return lines( v.string() );
	}
	
	static
	Value v_typeof( const Value& v )
	{
		if ( is_function( v ) )
		{
			return function_vtype;
		}
		
		switch ( v.type() )
		{
			case Value_empty_list:  return v;
			
			case Value_base_type:  return type_vtype;
			case Value_boolean:    return boolean_vtype;
			case Value_byte:       return byte_vtype;
			case Value_data:       return data_vtype;
			case Value_number:     return integer_vtype;
			case Value_string:     return string_vtype;
			
			case Value_pair:  break;
			
			default:  return "???";
		}
		
		Expr* expr = v.expr();
		
		if ( expr->op == Op_array )
		{
			return Value( etc_vtype, Op_subscript, empty_list );
		}
		
		if ( expr->op == Op_mapping )
		{
			return Value( v_typeof( expr->left  ), expr->op,
			              v_typeof( expr->right ) );
		}
		
		Value result = Value( empty_list, v_typeof( expr->left ) );
		
		Expr* r = result.expr();
		
		const Value* next = &expr->right;
		
		expr = next->expr();
		
		while ( expr  &&  expr->op == Op_list )
		{
			r->right = Value( r->right, v_typeof( expr->left ) );
			
			next = &expr->right;
			expr = next->expr();
			r = r->right.expr();
		}
		
		r->right = Value( r->right, v_typeof( *next ) );
		
		return result.expr()->right;
	}
	
	static const Value string     = string_vtype;
	static const Value string_etc = Value( string_vtype, etc_vtype );
	
	const proc_info proc_join   = { &v_join,   "join",   &string_etc };
	const proc_info proc_lines  = { &v_lines,  "lines",  &string     };
	const proc_info proc_typeof = { &v_typeof, "typeof", NULL        };
	
}
