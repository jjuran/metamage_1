/*	==============
 *	TargetNames.hh
 *	==============
 */

#ifndef ALINE_TARGETNAMES_HH
#define ALINE_TARGETNAMES_HH

// plus
#include "plus/string.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"

// A-line
#include "A-line/TargetTypes.hh"


namespace tool
{
	
	plus::string MakeTargetName( Platform      arch,
	                             Platform      runtime,
	                             Platform      api,
	                             BuildVariety  build );
	
}

#endif

