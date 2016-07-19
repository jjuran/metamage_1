/*
	list_builder.cc
	---------------
*/

#include "vlib/iterators/list_builder.hh"


namespace vlib
{
	
	list_builder::list_builder() : its_list( Value_empty_list )
	{
		its_mark = 0;  // NULL
	}
	
	void list_builder::append( const Value& v )
	{
		if ( is_empty_list( v ) )
		{
			return;
		}
		
		if ( its_mark == 0 )  // NULL
		{
			its_list = v;
			its_mark = &its_list;
		}
		else
		{
			*its_mark = Value( *its_mark, v );
			
			its_mark = &its_mark->expr()->right;
		}
		
		while ( its_mark->listexpr() )
		{
			its_mark = &its_mark->unshare().expr()->right;
		}
	}
	
	Value list_builder::move()
	{
		const Value result = its_list;
		
		its_list = empty_list;
		its_mark = 0;  // NULL
		
		return result;
	}
	
}
