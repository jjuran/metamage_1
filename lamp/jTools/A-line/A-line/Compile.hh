/*	==========
 *	Compile.hh
 *	==========
 *	
 *	Implemented by Compile.cc
 */

#pragma once

// Standard C++
#include <vector>

// A-line
#include "A-line/Task.hh"


namespace ALine
{
	
	class Project;
	struct TargetInfo;
	
	void CompileSources( const Project&           project,
	                     const TargetInfo&        targetInfo,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies );
	
}

