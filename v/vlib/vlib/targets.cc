/*
	targets.cc
	----------
*/

#include "vlib/targets.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/table-utils.hh"
#include "vlib/throw.hh"


namespace vlib
{
	
	static
	Target subscript_target( const Target& target, const Value& subscript )
	{
		Target result;
		
		result.type = &NIL;
		
		Expr* expr = target.addr->unshare().expr();
		
		if ( expr == 0 )  // NULL
		{
			// string or pack
			
			THROW( "can't target elements of packed structures" );
		}
		
		if ( expr->op == Op_array )
		{
			get_array_index_type( *target.type, result.type );
			
			result.addr = get_array_subscript_addr( expr, subscript );
		}
		
		if ( expr->op == Op_empower )
		{
			/*
				Specifying the value type for tables isn't implemented yet.
			*/
			
			result.addr = get_table_subscript_addr( expr, subscript );
		}
		
		return result;
	}
	
	Target make_target( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				Value& container = expr->left;
				Value& subscript = expr->right;
				
				Target target = make_target( container );
				
				try
				{
					return subscript_target( target, subscript );
				}
				catch ( const mutable_list_overrun& )
				{
					THROW( "lvalue array subscript out of range" );
				}
			}
		}
		
		if ( ! is_symbol( v ) )
		{
			THROW( "target value isn't a symbol or component thereof" );
		}
		
		return v.sym()->target();
	}
	
}
