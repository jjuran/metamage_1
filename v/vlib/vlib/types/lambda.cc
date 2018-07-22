/*
	lambda.cc
	---------
*/

#include "vlib/types/lambda.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/peephole.hh"
#include "vlib/return.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"


namespace vlib
{
	
	static
	Value call_function_body( const Value& f, const Value& arguments )
	{
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
		
		THROW( "attempted call of lambda with invalid body" );
		
		return Value();  // not reached
	}
	
	static
	Value call_lambda( const Value& lambda, const Value& arguments )
	{
		try
		{
			return call_function_body( lambda.expr()->right, arguments );
		}
		catch ( const transfer_via_return& e )
		{
			return e.object;
		}
		catch ( const transfer_via_break& e )
		{
			THROW( "`break` used outside of loop" );
		}
		catch ( const transfer_via_continue& e )
		{
			THROW( "`continue` used outside of loop" );
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return call_lambda( a, b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		0,  // NULL
		&binary_op_handler,
	};
	
	static const dispatch lambda_dispatch =
	{
		0,  // NULL
		0,  // NULL
		0,  // NULL
		&ops,
	};
	
	Lambda::Lambda( const Value& body )
	:
		Value( dummy_operand, Op_lambda, body, &lambda_dispatch )
	{
		if ( ! is_block( body ) )
		{
			THROW( "`lambda` requires a block" );
		}
		
		optimize_lambda_body( expr()->right );
	}
	
}
