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

// A-line
#include "A-line/TargetInfo.hh"
#include "A-line/TargetNames.hh"


namespace ALine
{
	
	struct BuildFailure  {};
	
	
	inline std::string MakeTargetName( TargetInfo info )
	{
		return MakeTargetName( info.platform & CD::archMask,
		                       info.platform & CD::runtimeMask,
		                       info.platform & CD::apiMask,
		                       info.build );
	}
	
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

