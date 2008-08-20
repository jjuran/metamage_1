/*	===========
 *	Includes.cc
 *	===========
 */

#include "A-line/Includes.hh"

// Standard C++
#include <map>

// A-line
#include "A-line/ExtractIncludes.hh"


namespace ALine
{
	
	std::map< std::string, IncludesCache > gIncludesCaches;
	
	
	const IncludesCache& GetIncludes( const std::string& pathname )
	{
		typedef std::map< std::string, IncludesCache >::const_iterator Iter;
		
		Iter it = gIncludesCaches.find( pathname );
		
		if ( it != gIncludesCaches.end() )
		{
			return it->second;
		}
		
		return gIncludesCaches[ pathname ] = ExtractIncludes( pathname );
	}
	
}

