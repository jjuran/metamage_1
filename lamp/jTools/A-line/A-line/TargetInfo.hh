/*	=============
 *	TargetInfo.hh
 *	=============
 */

#ifndef ALINE_TARGETINFO_HH
#define ALINE_TARGETINFO_HH

// A-line
#include "A-line/Platform.hh"
#include "A-line/TargetTypes.hh"


namespace tool
{
	
	struct TargetInfo
	{
		Platform      platform;
		EnvType       envType;
		Toolchain     toolchain;
		BuildVariety  build;
		
		TargetInfo() : platform ( Platform()           ),
		               envType  ( envUnspecified       ),
		               toolchain( toolchainUnspecified ),
		               build    ( buildDefault         )
		{
		}
		
		TargetInfo( Platform      platform,
		            BuildVariety  build ) : platform ( platform             ),
		                                    envType  ( envUnspecified       ),
		                                    toolchain( toolchainUnspecified ),
		                                    build    ( build                )
		{
		}
	};
	
}

#endif
