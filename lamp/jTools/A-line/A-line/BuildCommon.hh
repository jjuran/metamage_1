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
	
	namespace N = Nitrogen;
	
	using std::string;
	
	
	typedef UInt32 MacDate;
	
	struct BuildFailure  {};
	
	
	inline std::string MakeTargetName( TargetInfo info )
	{
		return MakeTargetName( info.platform.arch,
		                       info.platform.runtime,
		                       info.platform.api,
		                       info.build );
	}
	
	void GetCatInfo( const FSSpec& item, CInfoPBRec& pb );
	
	inline MacDate ModifiedDate( const FSSpec& item )
	{
		CInfoPBRec pb;
		
		GetCatInfo( item, pb );
		
		return MacDate( pb.hFileInfo.ioFlMdDat );
	}
	
	inline std::string ObjectFileName( const std::string& sourceName )
	{
		string::size_type dot = sourceName.find_last_of( '.' );
		// If the filename has no dot, then dot == npos, 
		// and substr() returns the whole string.  (And ".o" is appended.)
		return sourceName.substr( 0, dot ) + ".o";
	}
	
}

