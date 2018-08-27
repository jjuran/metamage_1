/*
	map-reduce.cc
	-------------
*/

#include "vlib/map-reduce.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/function-utils.hh"
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/boolean.hh"


#ifndef NULL
#define NULL  0
#endif


namespace vlib
{
	
	Value map( const Value& container, const Value& f )
	{
		if ( ! is_functional( f ) )
		{
			THROW( "map requires a function" );
		}
		
		generic_iterator it( container );
		
		list_builder result;
		
		while ( it )
		{
			const Value& x = it.use();
			
			Value f_x = call_function( f, x );
			
			result.append( f_x );
		}
		
		return make_array( result );
	}
	
	Value filter( const Value& container, const Value& f )
	{
		if ( ! is_functional( f ) )
		{
			THROW( "filter requires a function" );
		}
		
		generic_iterator it( container );
		
		list_builder result;
		
		while ( it )
		{
			const Value& x = it.use();
			
			Value passed = call_function( f, x );
			
			if ( passed.to< Boolean >().boolean() )
			{
				result.append( x );
			}
		}
		
		return make_array( result );
	}
	
	Value reduce( const Value& container, const Value& reducer )
	{
		const Value* routine = &reducer;
		const Value* initial = NULL;
		
		if ( Expr* expr = reducer.expr() )
		{
			if ( expr->op == Op_forward_init )
			{
				initial = &expr->left;
				routine = &expr->right;
			}
			else if ( expr->op == Op_reverse_init )
			{
				THROW( "per (reduce) doesn't support reverse folding yet" );
			}
		}
		
		const Value& f = *routine;
		
		if ( ! is_functional( f ) )
		{
			THROW( "per (reduce) requires a function" );
		}
		
		generic_iterator it( container );
		
		if ( ! initial  &&  ! it )
		{
			return empty_list;
		}
		
		Value result = initial ? *initial : it.use();
		
		while ( it )
		{
			const Value& x = it.use();
			
			result = call_function( f, make_list( result, x ) );
		}
		
		return result;
	}
	
}
