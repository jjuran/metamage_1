/*	=============
 *	TargetInfo.hh
 *	=============
 */

#ifndef ALINE_TARGETINFO_HH
#define ALINE_TARGETINFO_HH

// CompileDriver
#include "CompileDriver/Platform.hh"

// A-line
#include "A-line/TargetTypes.hh"


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	struct TargetInfo
	{
		CD::Platform  platform;
		Toolkit       toolkit;
		BuildVariety  build;
		
		TargetInfo() : platform( CD::Platform()     ),
		               toolkit ( toolkitUnspecified ),
		               build   ( buildDefault       )
		{
		}
		
		TargetInfo( CD::Platform  platform,
		            BuildVariety  build ) : platform( platform           ),
		                                    toolkit ( toolkitUnspecified ),
		                                    build   ( build              )
		{
		}
		
		void DeduceFromProduct( ProductType product );
		
		void ApplyOptions( TargetInfo options );
		
		void ApplyDefaults();
	};
	
}

#endif

