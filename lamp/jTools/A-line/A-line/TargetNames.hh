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
	
	std::string ArchName( CD::Architecture arch );
	
	TargetName MakeTargetName( CD::Architecture, CD::Runtime, CD::MacAPI, BuildVariety );
	
}

