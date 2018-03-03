/*
	map-reduce.cc
	-------------
*/

#include "vlib/map-reduce.hh"

// vlib
#include "vlib/function-utils.hh"
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/iterators/list_builder.hh"


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
	
	Value reduce( const Value& container, const Value& f )
	{
		if ( ! is_functional( f ) )
		{
			THROW( "per (reduce) requires a function" );
		}
		
		generic_iterator it( container );
		
		if ( ! it )
		{
			return empty_list;
		}
		
		Value result = it.use();
		
		while ( it )
		{
			const Value& x = it.use();
			
			result = call_function( f, make_list( result, x ) );
		}
		
		return result;
	}
	
}
