/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/type_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
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
	Value assign_to_integer( const Value& v )
	{
		if ( v.type() == Value_number )
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
	Value assign_to_type( const Value& v )
	{
		if ( v.type() == Value_base_type )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type, 0 }
	
	DEFINE_TYPE_INFO( boolean  );
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( integer  );
	DEFINE_TYPE_INFO( string   );
	DEFINE_TYPE_INFO( type     );
	
	static
	Value reversed( const Value& list )
	{
		Expr* expr = list.expr();
		
		if ( expr == NULL )
		{
			return list;
		}
		
		Value result = expr->left;
		
		const Value* next = &expr->right;
		
		expr = next->expr();
		
		while ( expr  &&  expr->op == Op_list )
		{
			result = Value( expr->left, result );
			
			next = &expr->right;
			expr = next->expr();
		}
		
		return Value( *next, result );
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
			case Value_number:     return integer_vtype;
			case Value_string:     return string_vtype;
			
			case Value_pair:  break;
			
			default:  return "???";
		}
		
		Expr* expr = v.expr();
		
		Value reversed_result = v_typeof( expr->left );
		
		const Value* next = &expr->right;
		
		expr = next->expr();
		
		while ( expr  &&  expr->op == Op_list )
		{
			reversed_result = Value( v_typeof( expr->left ), reversed_result );
			
			next = &expr->right;
			expr = next->expr();
		}
		
		return reversed( Value( v_typeof( *next ), reversed_result ) );
	}
	
	const proc_info proc_typeof = { &v_typeof, "typeof" };
	
}
