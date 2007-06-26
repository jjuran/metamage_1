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


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	typedef std::string TargetName;
	
	std::string ArchName( CD::Platform arch );
	
	TargetName MakeTargetName( CD::Platform  arch,
	                           CD::Platform  runtime,
	                           CD::Platform  api,
	                           BuildVariety  build );
	
}

