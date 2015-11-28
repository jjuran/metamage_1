/*
	plus/string/mince.cc
	--------------------
*/

#include "plus/string/mince.hh"

// gear
#include "gear/mince.hh"

// debug
#include "debug/assert.hh"


namespace plus
{
	
	string mince( const string& s, const string::size_type stride )
	{
		ASSERT( stride > 0 );
		
		typedef string::size_type size_t;
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		const unsigned length = gear::minced_length( data, size, stride );
		
		string result;
		
		char* p = result.reset( length );
		
		gear::mince( p, data, data + size, stride );
		
		return result;
	}
	
}
