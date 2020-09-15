/*
	array_iterator.cc
	-----------------
*/

#include "vlib/iterators/array_iterator.hh"

// debug
#include "debug/assert.hh"


namespace vlib
{
	
	static
	const Value& array_list( const Value& array )
	{
		ASSERT( is_array( array ) );
		
		if ( is_empty_array( array ) )
		{
			return empty_list;
		}
		
		return array.expr()->right;
	}
	
	array_iterator::array_iterator( const Value& array )
	: list_iterator( array_list( array ) )
	{
	}
	
}
