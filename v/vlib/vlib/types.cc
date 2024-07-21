/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/is_function.hh"
#include "vlib/is_type.hh"
#include "vlib/proc_info.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/value.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	bool is_functional( const Value& v )
	{
		return is_function( v )  ||  is_type( v );
	}
	
	static
	Value assign_to_function( const Value& v )
	{
		if ( is_functional( v ) )
		{
			return v;
		}
		
		return Value_NIL;
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
		
		return Value_NIL;
	}
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type, 0, 0, 0, 0 }
	
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( c_str    );
	
	static const type_info no_typeinfo = { 0, 0, 0, 0, 0, 0 };
	
	static
	Type basic_typeof( const Value& v )
	{
		if ( is_function( v ) )
		{
			return function_vtype;
		}
		
		Expr* expr = v.expr();
		
		if ( expr == NULL )
		{
			return no_typeinfo;
		}
		
		if ( expr->op == Op_subscript )
		{
			return type_vtype;
		}
		
		if ( expr->op == Op_unary_deref )
		{
			return type_vtype;
		}
		
		return no_typeinfo;
	}
	
	static const Type etc = etc_vtype;
	
	static
	Value v_typeof( const Value& v )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_1arg handler = ops->unary )
				{
					const Value result = handler( Op_typeof, v );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		const Type basic_type = basic_typeof( v );
		
		if ( &basic_type.typeinfo() != &no_typeinfo )
		{
			return basic_type;
		}
		
		if ( is_empty_array( v ) )
		{
			return generic_array_type;
		}
		
		if ( v.type() == Value_empty_list )
		{
			return v;
		}
		
		Expr* expr = v.expr();
		
		if ( expr == NULL )
		{
			return String( "???" );
		}
		
		if ( expr->op == Op_array )
		{
			return generic_array_type;
		}
		
		if ( expr->op == Op_unary_refer )
		{
			try
			{
				Target target = make_target( expr->right );
				
				const Value& type = target.type->type() ? *target.type
				                                        : Value( etc );
				
				return Value( Op_unary_deref, type );
			}
			catch ( const exception& e )
			{
			}
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
				if ( is_type( expr->right ) )
				{
					return Type( type_vtype );
				}
			}
			
			THROW( "abuse of power" );
		}
		
		if ( expr->op == Op_module )
		{
			return Value( expr->op, etc );
		}
		
		if ( expr->op != Op_list )
		{
			THROW( "unexpected expression in typeof()" );
		}
		
		list_builder result;
		
		list_iterator it( v );
		
		while ( it )
		{
			result.append( v_typeof( it.use() ) );
		}
		
		return result;
	}
	
	const proc_info proc_typeof = { "typeof", &v_typeof, NULL, Proc_pure };
	
	Value generic_array_type( etc, Op_subscript, Value_empty_list );
	
}
