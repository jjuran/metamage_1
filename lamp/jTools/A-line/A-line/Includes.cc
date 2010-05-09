/*	===========
 *	Includes.cc
 *	===========
 */

#include "A-line/Includes.hh"

// Standard C++
#include <map>

// A-line
#include "A-line/ExtractIncludes.hh"


namespace tool
{
	
	std::map< plus::string, IncludesCache > gIncludesCaches;
	
	
	const IncludesCache& GetIncludes( const plus::string& pathname )
	{
		typedef std::map< plus::string, IncludesCache >::const_iterator Iter;
		
		Iter it = gIncludesCaches.find( pathname );
		
		if ( it != gIncludesCaches.end() )
		{
			return it->second;
		}
		
		IncludesCache& includes = gIncludesCaches[ pathname ];
		
		ExtractIncludes( includes, pathname.c_str() );
		
		return includes;
	}
	
}

