/*
	boolean.cc
	----------
*/

#include "vlib/types/boolean.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/null.hh"


namespace vlib
{
	
	const Boolean False( false );
	const Boolean True ( true  );
	
	
	static
	truth_test get_truth_test( const Value& v )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const veritization* t = methods->to_boolean )
			{
				return t->verity;
			}
		}
		
		return NULL;
	}
	
	Value Boolean::coerce( const Value& v )
	{
		if ( const truth_test is_true = get_truth_test( v ) )
		{
			return Boolean( is_true( v ) );
		}
		
		switch ( v.type() )
		{
			case Value_boolean:
				return v;
			
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_boolean()" );
			
			case Value_empty_list:
			case Value_empty_array:
				return Boolean();
			
			case Value_packed:
			case Value_string:
				return Boolean( ! v.string().empty() );
			
			case Value_base_type:
				return Boolean( &v.typeinfo() != &null_vtype );
			
			case Value_function:
			case Value_pair:
				if ( Expr* expr = v.expr() )
				{
					if ( expr->op == Op_empower )
					{
						// Check if the array is empty.
						return Boolean( expr->right.expr() != 0 );  // NULL
					}
				}
				return True;
		}
	}
	
	static
	const char* boolean_str_data( const Value& v )
	{
		return v.boolean() ? "true" : "false";
	}
	
	static const stringify boolean_str =
	{
		&boolean_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers boolean_stringifiers =
	{
		&boolean_str,
		// rep: ditto
		// bin: not defined
	};
	
	const dispatch boolean_dispatch =
	{
		&boolean_stringifiers,
	};
	
	const type_info boolean_vtype =
	{
		"boolean",
		&assign_to< Boolean >,
		&Boolean::coerce,
		0,
	};
	
}
