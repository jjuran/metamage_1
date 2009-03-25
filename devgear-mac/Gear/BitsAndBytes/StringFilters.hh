/*	================
 *	StringFilters.hh
 *	================
 */

#ifndef BITSANDBYTES_STRINGFILTERS_HH
#define BITSANDBYTES_STRINGFILTERS_HH

// Standard C++
#include <string>


namespace BitsAndBytes
{
	
	inline std::string q( const std::string& str )
	{
		return std::string( "'" ) + str + "'";
	}
	
	inline std::string qq( const std::string& str )
	{
		return std::string( "\"" ) + str + '"';
	}
	
	inline std::string uq( const std::string& str )
	{
		return std::string( "`" ) + str + "'";
	}
	
}

#endif

