/*	=========
 *	Filter.cc
 *	=========
 */

#include "Backtrace/Filter.hh"

// Standard C++
#include <algorithm>


namespace Backtrace
{
	
	std::string FilterSymbol( const std::string& name )
	{
		std::string pattern = "std::basic_string< char, std::char_traits< char >, std::allocator< char > >";
		std::string newText = "std::string";
		
		std::string result = name;
		
		while ( true )
		{
			std::string::iterator found = std::search( result.begin(), result.end(), pattern.begin(), pattern.end() );
			
			if ( found == result.end() )
			{
				break;
			}
			
			result.replace( found, found + pattern.size(), newText );
		}
		
		return result;
	}
	
}

