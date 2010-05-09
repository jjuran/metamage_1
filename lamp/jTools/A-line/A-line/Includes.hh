/*	===========
 *	Includes.hh
 *	===========
 */

#ifndef ALINE_INCLUDES_HH
#define ALINE_INCLUDES_HH

// C++
#include <vector>

// plus
#include "plus/string.hh"


namespace tool
{
	
	struct IncludesCache
	{
		std::vector< plus::string > user;
		std::vector< plus::string > system;
	};
	
	const IncludesCache& GetIncludes( const plus::string& pathname );
	
}

#endif

