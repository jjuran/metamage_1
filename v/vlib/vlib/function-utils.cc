/*
	function-utils.cc
	-----------------
*/

#include "vlib/function-utils.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/member.hh"
#include "vlib/os.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	Value call_function( const Value& f, const Value& arguments )
	{
		if ( is_empty_list( f ) )
		{
			return f;
		}
		
		if ( const dispatch* methods = f.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( Op_function, f, arguments );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		if ( Expr* expr = f.expr() )
		{
			if ( expr->op == Op_block  &&  expr->right.is_evaluated() )
			{
				return expr->right;
			}
			
			if ( expr->op == Op_multiply )
			{
				return call_function( expr->left,
				                      call_function( expr->right,
				                                     arguments ) );
			}
			
			if ( expr->op == Op_empower )
			{
				size_t n = integer_cast< size_t >( expr->right );
				
				Value result = arguments;
				
				while ( n-- )
				{
					periodic_yield();
					
					result = call_function( expr->left, result );
				}
				
				return result;
			}
			
			if ( expr->op == Op_list )
			{
				list_iterator it( f );
				
				list_builder results;
				
				while ( it )
				{
					const Value& f = it.use();
					
					results.append( call_function( f, arguments ) );
				}
				
				return results;
			}
			
			if ( expr->op == Op_getter )
			{
				const Value& member = expr->right;
				
				return calc_member( arguments, member );
			}
			
			const Value& method = expr->left;
			const Value& object = expr->right;
			
			return call_function( method, make_list( object, arguments ) );
		}
		
		THROW( "attempted call of non-function" );
		
		return Value();  // not reached
	}
	
}
