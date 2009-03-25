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
	
	void ReadSourceDotList( const std::string&           pathname,
	                        std::vector< std::string >&  results );
	
	std::vector< std::string > ReadSourceDotList( const std::string& pathname );
	
}

#endif

