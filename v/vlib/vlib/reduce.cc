/*
	reduce.cc
	---------
*/

#include "vlib/reduce.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_iterator.hh"


#ifndef NULL
#define NULL  0
#endif


namespace vlib
{
	
	static
	Value call_handler( op_type op, const Value& a, const Value& b )
	{
		if ( const dispatch* methods = a.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( op, a, b );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		THROW( "binary operator not implemented in list reduction" );
		
		return NIL;
	}
	
	static
	Value reduce_list( const Value& v, op_type op )
	{
		if ( is_empty_list( v ) )
		{
			return v;
		}
		
		list_iterator args( v );
		
		Value result = args.use();
		
		while ( args )
		{
			const Value& next = args.use();
			
			result = call_handler( op, result, next );
		}
		
		return result;
	}
	
	static
	Value v_product( const Value& v )
	{
		return reduce_list( v, Op_multiply );
	}
	
	static
	Value v_sum( const Value& v )
	{
		return reduce_list( v, Op_add );
	}
	
	
	const proc_info proc_product = { "product", &v_product, NULL, Proc_pure };
	const proc_info proc_sum     = { "sum",     &v_sum,     NULL, Proc_pure };
	
}
