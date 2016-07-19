/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/value.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/data.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/null.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
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
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type, 0, 0 }
	
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( c_str    );
	DEFINE_TYPE_INFO( type     );
	
	bool is_null( const Value& v )
	{
		return v.type() == Value_base_type  &&  &v.typeinfo() == &null_vtype;
	}
	
	static
	Value v_typeof( const Value& v )
	{
		if ( is_function( v ) )
		{
			return function_vtype;
		}
		
		if ( is_null( v ) )
		{
			// null is its own type
			return null_vtype;
		}
		
		if ( is_empty_array( v ) )
		{
			return generic_array_type;
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
			return generic_array_type;
		}
		
		if ( expr->op == Op_mapping )
		{
			return Value( v_typeof( expr->left  ), expr->op,
			              v_typeof( expr->right ) );
		}
		
		if ( expr->op == Op_empower )
		{
			if ( is_type( expr->left ) )
			{
				if ( is_empty_array( expr->right ) )
				{
					return v;  // typeof type^[] is type^[]
				}
				
				if ( is_array( expr->right ) )
				{
					return Value( expr->left, expr->op, Value_empty_array );
				}
			}
			
			THROW( "abuse of power" );
		}
		
		list_builder result;
		
		list_iterator it( v );
		
		while ( it )
		{
			result.append( v_typeof( it.use() ) );
		}
		
		return result;
	}
	
	const proc_info proc_typeof = { "typeof", &v_typeof, NULL };
	
	Value generic_array_type( etc_vtype, Op_subscript, Value_empty_list );
	
}
