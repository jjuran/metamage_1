/*
	order.cc
	--------
*/

#include "vlib/order.hh"

// vlib
#include "vlib/compare.hh"
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/array_iterator.hh"


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
	
	const proc_info proc_It_max = { "max",    &v_max,    NULL, Proc_pure };
	const proc_info proc_It_min = { "min",    &v_min,    NULL, Proc_pure };
	
}
