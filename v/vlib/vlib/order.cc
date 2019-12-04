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
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"


namespace vlib
{
	
	static
	Value minmax( const Value& v, bool min_vs_max )
	{
		const Value& container = v;
		
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
			
			plus::cmp_t cmp = compare( *result, next );
			
			if ( (compare( *result, next ) > 0) == min_vs_max )
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
