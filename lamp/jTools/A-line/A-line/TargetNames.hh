/*	==============
 *	TargetNames.hh
 *	==============
 */

#pragma once

// C++
#include <string>

// CompileDriver
#include "CompileDriver/Platform.hh"

// A-line
#include "A-line/TargetTypes.hh"


namespace tool
{
	
	typedef std::string TargetName;
	
	std::string ArchName( Platform arch );
	
	TargetName MakeTargetName( Platform      arch,
	                           Platform      runtime,
	                           Platform      api,
	                           BuildVariety  build );
	
}

