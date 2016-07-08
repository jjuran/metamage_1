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
	
	static
	Value& last_mutable( Value& list )
	{
		Value* next = &list;
		
		while ( next->listexpr() != 0 )  // NULL
		{
			next = &rest_mutable( *next );
		}
		
		return *next;
	}
	
	void splice_lists( Value& left, const Value& right )
	{
		Value& last = last_mutable( left );
		
		last = Value( last, right );
	}
	
	Value make_pair( const Value& left, const Value& right )
	{
		if ( Expr* expr = left.listexpr() )
		{
			Value list = left;
			
			splice_lists( list, right );
			
			return list;
		}
		
		return Value( left, right );
	}
	
	Value reverse_list( const Value& list )
	{
		Expr* expr = list.listexpr();
		
		if ( expr == 0 )  // NULL
		{
			return list;  // empty list or single item
		}
		
		const Value* next = &expr->right;
		
		Value result = expr->left;
		
		while (( expr = next->listexpr() ))
		{
			result = Value( expr->left, result );
			
			next = &expr->right;
		}
		
		return Value( *next, result );
	}
	
}
