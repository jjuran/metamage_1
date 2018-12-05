/*
	safe_list_builder.cc
	--------------------
*/

#include "vlib/iterators/safe_list_builder.hh"

// vlib
#include "vlib/list-utils.hh"


namespace vlib
{
	
	safe_list_builder::safe_list_builder() : its_list( Value_empty_list )
	{
	}
	
	void safe_list_builder::append( const Value& v )
	{
		if ( is_empty_list( v ) )
		{
			return;
		}
		
		if ( is_empty_list( its_list ) )
		{
			its_list = v;
			return;
		}
		
		its_list = Value( v, its_list );
	}
	
	Value safe_list_builder::get() const
	{
		return reverse_list( its_list );
	}
	
}
