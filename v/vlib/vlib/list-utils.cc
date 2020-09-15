/*
	list-utils.cc
	-------------
*/

#include "vlib/list-utils.hh"

// vlib
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	Value& first_mutable( Value& list )
	{
		list.unshare();
		
		if ( Expr* expr = list.listexpr() )
		{
			return expr->left;
		}
		
		return list;  // not a (non-empty) list
	}
	
	Value& rest_mutable( Value& list )
	{
		list.unshare();
		
		if ( Expr* expr = list.listexpr() )
		{
			return expr->right;
		}
		
		throw mutable_list_overrun();
	}
	
	const Value& get_nth( const Value& list, unsigned i )
	{
		const Value* next = &list;
		
		while ( i-- > 0  &&  ! is_empty_list( *next ) )
		{
			next = &rest( *next );
		}
		
		return first( *next );
	}
	
	Value& get_nth_mutable( Value& list, unsigned i )
	{
		Value* next = &list;
		
		while ( i-- > 0  &&  ! is_empty_list( *next ) )
		{
			next = &rest_mutable( *next );
		}
		
		return first_mutable( *next );
	}
	
	unsigned long count( const Value& list )
	{
		list_iterator it( list );
		
		unsigned long total = 0;
		
		while ( it.next() )
		{
			++total;
		}
		
		return total;
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
		if ( is_empty_list( right ) )
		{
			return;
		}
		
		if ( is_empty_list( left ) )
		{
			left = right;
			return;
		}
		
		Value& last = last_mutable( left );
		
		last = Value( last, right );
	}
	
	Value make_list( const Value& left, const Value& right )
	{
		Value list = left;
		
		splice_lists( list, right );
		
		return list;
	}
	
	Value reverse_list( const Value& list )
	{
		if ( list.listexpr() == 0 )  // NULL
		{
			return list;  // empty list or single item
		}
		
		list_iterator it( list );
		
		Value result = it.use();
		
		while ( it )
		{
			result = Value( it.use(), result );
		}
		
		return result;
	}
	
	Value bind_args( const Value& f, const Value& arguments )
	{
		if ( is_empty_list( arguments ) )
		{
			return f;
		}
		
		Expr* expr = f.expr();
		
		if ( expr != 0  &&  expr->op == Op_bind_args )  // NULL
		{
			return bind_args( expr->left, make_list( expr->right, arguments ) );
		}
		
		return Value( f, Op_bind_args, arguments );
	}
	
}
