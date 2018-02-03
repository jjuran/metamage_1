/*
	is_function.cc
	--------------
*/

#include "vlib/is_function.hh"

// vlib
#include "vlib/types/proc.hh"


namespace vlib
{
	
	bool is_function( const Value& v )
	{
		if ( v.is< Proc >() )
		{
			return true;
		}
		
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_bind_args  ||  expr->op == Op_invocation )
			{
				return true;
			}
			
			if ( expr->op == Op_lambda )
			{
				return true;
			}
		}
		
		return false;
	}
	
}
