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
	
	static inline
	Value get_byte( const char* begin, const char* end )
	{
		if ( begin < end )
		{
			return Byte( *begin );
		}
		
		return empty_list;
	}
	
	string_iterator::string_iterator( const Value& str ) : its_string( str )
	{
		if ( str.type() == V_str  ||  str.type() == V_pack )
		{
			const plus::string& s = its_string.string();
			
			begin = s.begin();
			end   = s.size() + begin;
			
			its_value = get_byte( begin, end );
			
			return;
		}
		
		THROW( "invalid range object" );
	}
	
	const Value& string_iterator::get() const
	{
		its_value = get_byte( begin, end );
		
		return its_value;
	}
	
}
