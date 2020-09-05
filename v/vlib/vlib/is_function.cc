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
			switch ( expr->op )
			{
				case Op_bind_args:
				case Op_invocation:
				case Op_lambda:
				case Op_getter:
				case Op_multiply:
					return true;
				
				case Op_empower:
					return expr->right.type() == V_int;
				
				default:
					break;
			}
		}
		
		return false;
	}
	
}
