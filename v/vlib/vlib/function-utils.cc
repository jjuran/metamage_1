/*
	function-utils.cc
	-----------------
*/

#include "vlib/function-utils.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"


namespace vlib
{
	
	Value call_function( const Value& f, const Value& arguments )
	{
		if ( const dispatch* methods = f.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( Op_function, f, arguments );
					
					if ( result.type() )
					{
						return result;
					}
				}
			}
		}
		
		if ( Expr* expr = f.expr() )
		{
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, make_list( object, arguments ) );
		}
		
		THROW( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
}
