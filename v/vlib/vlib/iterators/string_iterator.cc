/*
	string_iterator.cc
	------------------
*/

#include "vlib/iterators/string_iterator.hh"

// plus
#include "plus/string.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types/byte.hh"


namespace vlib
{
	
	string_iterator::string_iterator( const Value& str ) : its_string( str )
	{
		if ( str.type() == V_str  ||  str.type() == V_vec )
		{
			const plus::string& s = its_string.string();
			
			begin = s.begin();
			end   = s.size() + begin;
			
			its_value = begin < end ? Byte( *begin ) : Value_empty_list;
			
			return;
		}
		
		THROW( "invalid range object" );
	}
	
	const Value& string_iterator::get() const
	{
		its_value = begin < end ? Byte( *begin ) : Value_empty_list;
		
		return its_value;
	}
	
}
