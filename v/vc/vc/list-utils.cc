/*
	list-utils.cc
	-------------
*/

#include "vc/list-utils.hh"


namespace vc
{
	
	unsigned long count( const Value& list )
	{
		if ( list.type != Value_pair )
		{
			return list.type != Value_empty_list;
		}
		
		Expr* expr = list.expr.get();
		
		unsigned long total = count( expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			total += count( next->left );
			
			expr = next;
		}
		
		return total + count( expr->right );
	}
	
	Value make_pair( const Value& left, const Value& right )
	{
		if ( left.type != Value_pair )
		{
			return Value( left, right );
		}
		
		Expr& expr = *left.expr.get();
		
		return Value( expr.left, make_pair( expr.right, right ) );
	}
	
}
