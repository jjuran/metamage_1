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
		EnvType       envType;
		Toolchain     toolchain;
		BuildVariety  build;
		
		TargetInfo() : platform ( CD::Platform()       ),
		               envType  ( envUnspecified       ),
		               toolchain( toolchainUnspecified ),
		               build    ( buildDefault         )
		{
		}
		
		TargetInfo( CD::Platform  platform,
		            BuildVariety  build ) : platform ( platform             ),
		                                    envType  ( envUnspecified       ),
		                                    toolchain( toolchainUnspecified ),
		                                    build    ( build                )
		{
		}
		
		void DeduceFromProduct( ProductType product );
		
		void ApplyOptions( TargetInfo options );
		
		void ApplyDefaults();
	};
	
}

#endif

