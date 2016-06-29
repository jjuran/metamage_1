/*
	list-utils.cc
	-------------
*/

#include "vlib/list-utils.hh"


namespace vlib
{
	
	const Value& get_nth( const Value& list, unsigned i )
	{
		const Value* next = &list;
		
		while ( i-- > 0  &&  ! is_empty( *next ) )
		{
			next = &rest( *next );
		}
		
		return first( *next );
	}
	
	unsigned long count( const Value& list )
	{
		Expr* expr = list.listexpr();
		
		if ( expr == 0 )  // NULL
		{
			return ! is_empty( list );
		}
		
		unsigned long total = count( expr->left );
		
		while ( Expr* next = expr->right.expr() )
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
