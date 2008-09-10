/*	==============
 *	BuildCommon.hh
 *	==============
 *	
 *	Implemented by BuildCommon.cc
 */

#pragma once

// Standard C
#include <time.h>

// POSeven
#include "POSeven/Stat.hh"


namespace ALine
{
	
	struct BuildFailure  {};
	
	
	inline time_t ModifiedDate( const std::string& pathname )
	{
		return poseven::stat( pathname ).st_mtime;
	}
	
	inline std::string ObjectFileName( const std::string& sourceName )
	{
		std::size_t dot = sourceName.find_last_of( '.' );
		// If the filename has no dot, then dot == npos, 
		// and substr() returns the whole string.  (And ".o" is appended.)
		return sourceName.substr( 0, dot ) + ".o";
	}
	
}

