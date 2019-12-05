/*
	order.cc
	--------
*/

#include "vlib/order.hh"

// Standard C++
#include <algorithm>
#include <vector>

// vlib
#include "vlib/array-utils.hh"
#include "vlib/compare.hh"
#include "vlib/function-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/pure.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/any.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	void check_key_maker( const Value& key_maker )
	{
		if ( ! is_pure( key_maker ) )
		{
			THROW( "`via` right operand must be a pure function" );
		}
	}
	
	struct key_less
	{
		const Value& key;
		
		key_less( const Value& k ) : key( k )
		{
		}
		
		bool operator()( const Value& a, const Value& b ) const
		{
			return compare( call_function( key, a ),
			                call_function( key, b ) ) < 0;
		}
	};
	
	static
	Value minmax( const Value& v, bool min_vs_max )
	{
		const Expr* expr = v.expr();
		
		const bool is_via = expr  &&  expr->op == Op_via;
		
		const Value& container = is_via ? expr->left  : v;
		const Value& key_maker = is_via ? expr->right : Type( etc_vtype );
		
		if ( is_via )
		{
			check_key_maker( key_maker );
		}
		
		key_less less( key_maker );
		
		if ( ! is_array( container ) )
		{
			THROW( "Only arrays can be iterated for min/max" );
		}
		
		array_iterator args( container );
		
		if ( ! args )
		{
			return empty_list;
		}
		
		const Value* result = &args.use();
		
		while ( args )
		{
			const Value& next = args.use();
			
			if ( less( next, *result ) == min_vs_max )
			{
				result = &next;
			}
		}
		
		return *result;
	}
	
	static
	Value v_max( const Value& v )
	{
		return minmax( v, false );
	}
	
	static
	Value v_min( const Value& v )
	{
		return minmax( v, true );
	}
	
	static
	size_t length( const Value& v )
	{
		size_t n = 0;
		
		array_iterator it( v );
		
		while ( it )
		{
			++n;
			it.use();
		}
		
		return n;
	}
	
	struct value_less
	{
		bool operator()( const Value* a, const Value* b ) const
		{
			return compare( *a, *b ) < 0;
		}
	};
	
	static
	Value v_sorted( const Value& v )
	{
		const Value& container = v;
		
		if ( ! is_array( container ) )
		{
			THROW( "Only arrays can be sorted" );
		}
		
		size_t n = length( container );
		
		std::vector< const Value* > vector;
		
		vector.reserve( n );
		
		array_iterator it( container );
		
		while ( it )
		{
			vector.push_back( &it.use() );
		}
		
		std::stable_sort( vector.begin(), vector.end(), value_less() );
		
		list_builder result;
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			result.append( *vector[ i ] );
		}
		
		return make_array( result );
	}
	
	const proc_info proc_It_max = { "max",    &v_max,    NULL, Proc_pure };
	const proc_info proc_It_min = { "min",    &v_min,    NULL, Proc_pure };
	const proc_info proc_sorted = { "sorted", &v_sorted, NULL, Proc_pure };
	
}
