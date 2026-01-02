/*
	area.cc
	-------
*/

#include "vlib/area.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	unsigned long area( const Value& v )
	{
		unsigned long total = area( v.its_box ) + sizeof (Value);
		
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_1arg handler = ops->unary )
				{
					const Value result = handler( Op_areaof, v );
					
					if ( result )
					{
						total += integer_cast< unsigned long >( result );
					}
				}
			}
		}
		
		if ( v.type() != Value_pair )
		{
			return total;
		}
		
		Expr* expr = v.expr();
		
		while ( expr != NULL )
		{
			total -= sizeof (Value);
			
			total += area( expr->left );
			total += area( expr->right.its_box );
			
			if ( expr->right.type() != Value_pair )
			{
				break;
			}
			
			expr = expr->right.expr();
		}
		
		return total;
	}
	
}
