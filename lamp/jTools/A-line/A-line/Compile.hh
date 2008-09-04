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
	
	void CompileSources( Project&                 project,
	                     const TargetInfo&        target_info,
	                     const TaskPtr&           project_base_task,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies );
	
}

