/*	=========
 *	Filter.cc
 *	=========
 */

#include "recall/name_filter.hh"

// Standard C++
#include <algorithm>


#define STR_LEN( string )  "" string, (sizeof string - 1)

#define ARRAY_LEN( array )  ( sizeof array / sizeof array[0] )

namespace Backtrace
{
	
	struct Replacement
	{
		const char*  pattern;
		std::size_t  patternLength;
		const char*  newText;
		std::size_t  newTextLength;
	};
	
	#define REPLACE( newtext, pattern )  { STR_LEN( pattern ), STR_LEN( newtext ) }
	
	const Replacement gReplacements[] =
	{
		REPLACE( "std::string",  "std::basic_string< char, std::char_traits< char >, std::allocator< char > >" ),
		
		REPLACE( "std::vector< std::string >",  "std::vector< std::string, std::allocator< std::string > >" )
	};
	
	std::string FilterSymbol( const std::string& name )
	{
		const Replacement* end = gReplacements + ARRAY_LEN( gReplacements );
		
		std::string result = name;
		
		for ( const Replacement* it = gReplacements;  it < end;  ++it )
		{
			const char* pattern = it->pattern;
			const char* pattern_end = pattern + it->patternLength;
			
			while ( true )
			{
				std::string::iterator found = std::search( result.begin(),
				                                           result.end(),
				                                           pattern,
				                                           pattern_end );
				
				if ( found == result.end() )
				{
					break;
				}
				
				result.replace( found, found + it->patternLength, it->newText, it->newTextLength );
			}
		}
		
		return result;
	}
	
}

