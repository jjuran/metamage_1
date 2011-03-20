/*
	replaced_string.cc
	------------------
*/

#include "plus/replaced_string.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>


namespace plus
{
	
	string replaced_string( const string&  s,
	                        char           old_char,
	                        char           new_char )
	{
		const char* data = s.c_str();
		
		if ( const char* first = strchr( data, old_char ) )
		{
			const size_t size = s.size();
			
			string result;
			
			char* p = result.reset( size );
			
			std::copy( data, first, p );
			
			std::replace_copy( first,
			                   data + size,
			                   p + (first - data),
			                   old_char,
			                   new_char );
			
			return result;
		}
		
		return s;
	}
	
}

