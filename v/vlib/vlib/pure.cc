/*
	pure.cc
	-------
*/

#include "vlib/pure.hh"

// vlib
#include "vlib/symbol.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/proc.hh"


namespace vlib
{
	
	bool is_pure( const Value& v )
	{
		if ( Symbol* sym = v.sym() )
		{
			return sym->get().desc() <= 0xFFFF;  // Local variable reference?
		}
		
		if ( const Proc* proc = v.is< Proc >() )
		{
			return proc->is_pure();
		}
		
		if ( v.type() == Value_base_type )
		{
			return v.typeinfo().flags & Type_pure;
		}
		
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const typing* typ = methods->type )
			{
				return typ->flags & Type_pure;
			}
		}
		
		if ( Expr* expr = v.expr() )
		{
			switch ( expr->op )
			{
				/*
					Reference construction returns non-equal results.
				*/
				
				case Op_unary_refer:
					return false;
				
				case Op_block:
				case Op_invocation:
					return is_pure( expr->right.expr()->right );
				
				default:
					return is_pure( expr->left )  &&  is_pure( expr->right );
			}
		}
		
		return true;
	}
	
}
