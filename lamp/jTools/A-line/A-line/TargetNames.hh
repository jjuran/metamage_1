/*	==============
 *	TargetNames.hh
 *	==============
 */

#ifndef ALINE_TARGETNAMES_HH
#define ALINE_TARGETNAMES_HH

#if PRAGMA_ONCE
#pragma once
#endif

// C++
#include <string>

// CompileDriver
#include "CompileDriver/Platform.hh"

// A-line
#include "A-line/TargetTypes.hh"


namespace tool
{
	
	std::string ArchName( Platform arch );
	
	std::string MakeTargetName( Platform      arch,
	                            Platform      runtime,
	                            Platform      api,
	                            BuildVariety  build );
	
}

#endif

