/*	=======
 *	Link.hh
 *	=======
 */

#ifndef ALINE_LINK_HH
#define ALINE_LINK_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"

// A-line
#include "A-line/Task.hh"


namespace tool
{
	
	class Project;
	struct TargetInfo;
	
	void NameObjectFiles( const Project&                project,
	                      std::vector< plus::string >&  object_pathnames );
	
	void LinkProduct( Project&                       project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 project_base_task,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies );
	
}

#endif

