/*
	replaced_string.cc
	------------------
*/

#include "plus/replaced_string.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	string replaced_string( const string&  s,
	                        char           old_char,
	                        char           new_char )
	{
		const char* data = s.c_str();
		
		if ( const char* slash = strchr( data, old_char ) )
		{
			var_string new_string = s;
			
			std::replace( new_string.begin() + (slash - data),
			              new_string.end(),
			              old_char,
			              new_char );
			
			return new_string;
		}
		
		return s;
	}
	
}

