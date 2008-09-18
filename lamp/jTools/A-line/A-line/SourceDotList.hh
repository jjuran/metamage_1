/*	================
 *	SourceDotList.hh
 *	================
 */

#ifndef ALINE_SOURCEDOTLIST_HH
#define ALINE_SOURCEDOTLIST_HH

#if PRAGMA_ONCE
#pragma once
#endif

// C++
#include <string>
#include <vector>


namespace tool
{
	
	std::vector< std::string > ReadSourceDotList( const std::string& pathname );
	
}

#endif

