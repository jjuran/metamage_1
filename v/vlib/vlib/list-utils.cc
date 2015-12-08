/*
	list-utils.cc
	-------------
*/

#include "vlib/list-utils.hh"


namespace vlib
{
	
	const Value& first( const Value& list )
	{
		if ( Expr* expr = list.listexpr() )
		{
			return expr->left;
		}
		
		return list;  // not a (non-empty) list
	}
	
	const Value& rest( const Value& list )
	{
		if ( Expr* expr = list.listexpr() )
		{
			return expr->right;
		}
		
		return empty_list;
	}
	
	unsigned long count( const Value& list )
	{
		Expr* expr = list.listexpr();
		
		if ( expr == NULL )
		{
			return ! is_empty( list );
		}
		
		unsigned long total = count( expr->left );
		
		while ( Expr* next = get_expr( expr->right ) )
		{
			if ( next->op != Op_list )
			{
				return total + 1;
			}
			
			total += count( next->left );
			
			expr = next;
		}
		
		return total + count( expr->right );
	}
	
	Value make_pair( const Value& left, const Value& right )
	{
		if ( Expr* expr = left.listexpr() )
		{
			return Value( expr->left, make_pair( expr->right, right ) );
		}
		
		return Value( left, right );
	}
	
}
