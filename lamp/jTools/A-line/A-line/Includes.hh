/*	===========
 *	Includes.hh
 *	===========
 */

#ifndef ALINE_INCLUDES_HH
#define ALINE_INCLUDES_HH

// plus
#include "plus/string.hh"

// A-line
#include "A-line/StringVector.hh"


namespace tool
{
	
	struct IncludesCache
	{
		StringVector user;
		StringVector system;
	};
	
	const IncludesCache& GetIncludes( const plus::string& pathname );
	
}

#endif
