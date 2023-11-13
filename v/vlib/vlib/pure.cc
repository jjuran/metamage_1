/*
	pure.cc
	-------
*/

#include "vlib/pure.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/symbol.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/symdesc.hh"


namespace vlib
{
	
	bool is_pure( const Value& v )
	{
		if ( Symbol* sym = v.sym() )
		{
			const Value& v = sym->get();
			
			if ( const SymDesc* symdesc = v.is< SymDesc >() )
			{
				return symdesc->depth() == 0;  // Local variable reference?
			}
			
			return false;
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
					Writing to a file descriptor is obviously a side effect.
				*/
				
				case Op_unary_refer:
				case Op_send:
				case Op_recv:
					return false;
				
				case Op_block:
					if ( expr->right.is_evaluated() )
					{
						return is_pure( expr->right );
					}
					// fall through
					
				case Op_invocation:
					return is_pure( expr->right.expr()->right );
				
				default:
					return is_pure( expr->left )  &&  is_pure( expr->right );
			}
		}
		
		return true;
	}
	
	static
	Value v_is_pure( const Value& v )
	{
		return Boolean( is_pure( v ) );
	}
	
	#ifndef NULL
	#define NULL  0
	#endif
	
	const proc_info proc_is_pure = { "is_pure", &v_is_pure, NULL, Proc_pure };
	
}
