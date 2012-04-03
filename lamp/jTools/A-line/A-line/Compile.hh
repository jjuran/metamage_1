/*	==========
 *	Compile.hh
 *	==========
 */

#ifndef ALINE_COMPILE_HH
#define ALINE_COMPILE_HH

// Standard C++
#include <vector>

// A-line
#include "A-line/Platform.hh"
#include "A-line/Task.hh"


namespace tool
{
	
	class Project;
	struct TargetInfo;
	
	void CompileSources( Project&                 project,
	                     const TargetInfo&        target_info,
	                     const TaskPtr&           project_base_task,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies );
	
}

#endif

