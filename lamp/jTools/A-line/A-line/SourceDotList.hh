/*	================
 *	SourceDotList.hh
 *	================
 */

#ifndef ALINE_SOURCEDOTLIST_HH
#define ALINE_SOURCEDOTLIST_HH

// C++
#include <string>
#include <vector>


namespace tool
{
	
	void ReadSourceDotList( const char*                  pathname,
	                        std::vector< std::string >&  results );
	
}

#endif

