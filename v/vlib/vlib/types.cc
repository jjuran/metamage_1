/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/value.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/data.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	extern const proc_info proc_lines;
	
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
	Value coerce_to_string( const Value& v )
	{
		return make_string( v, Stringified_to_print );
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
		
		THROW( "nonexistent string member" );
		
		return Value_nothing;
	}
	
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type, 0, 0 }
	
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( c_str    );
	DEFINE_TYPE_INFO( type     );
	
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
