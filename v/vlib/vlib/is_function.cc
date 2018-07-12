/*
	is_function.cc
	--------------
*/

#include "vlib/is_function.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/typing.hh"
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
	
	bool is_functionally_impure( const Value& v )
	{
		if ( v.type() == Value_base_type )
		{
			return ! (v.typeinfo().flags & Type_pure);
		}
		
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const typing* typ = methods->type )
			{
				return ! (typ->flags & Type_pure);
			}
		}
		
		if ( const Proc* proc = v.is< Proc >() )
		{
			return ! proc->is_pure();
		}
		
		if ( Expr* expr = v.expr() )
		{
			switch ( expr->op )
			{
				case Op_bind_args:
					return is_functionally_impure( expr->left );
				
				default:
					break;
			}
		}
		
		return is_function( v );
	}
	
}
