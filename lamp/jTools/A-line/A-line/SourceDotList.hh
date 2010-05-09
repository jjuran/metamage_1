/*	================
 *	SourceDotList.hh
 *	================
 */

#ifndef ALINE_SOURCEDOTLIST_HH
#define ALINE_SOURCEDOTLIST_HH

// C++
#include <vector>

// plus
#include "plus/string.hh"


namespace tool
{
	
	void ReadSourceDotList( const char*                   pathname,
	                        std::vector< plus::string >&  results );
	
}

#endif

