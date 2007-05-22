/*	==============
 *	BuildCommon.hh
 *	==============
 *	
 *	Implemented by BuildCommon.cc
 */

#pragma once

// Nitrogen
#include "Nitrogen/Files.h"

// A-line
#include "A-line/TargetInfo.hh"
#include "A-line/TargetNames.hh"


namespace ALine
{
	
	typedef UInt32 MacDate;
	
	struct BuildFailure  {};
	
	
	inline std::string MakeTargetName( TargetInfo info )
	{
		return MakeTargetName( info.platform.arch,
		                       info.platform.runtime,
		                       info.platform.api,
		                       info.build );
	}
	
	inline MacDate ModifiedDate( const FSSpec& item )
	{
		CInfoPBRec pb;
		
		Nitrogen::FSpGetCatInfo( item, pb );
		
		return MacDate( pb.hFileInfo.ioFlMdDat );
	}
	
	inline std::string ObjectFileName( const std::string& sourceName )
	{
		std::size_t dot = sourceName.find_last_of( '.' );
		// If the filename has no dot, then dot == npos, 
		// and substr() returns the whole string.  (And ".o" is appended.)
		return sourceName.substr( 0, dot ) + ".o";
	}
	
}

