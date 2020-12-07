/*	=======
 *	Link.hh
 *	=======
 */

#ifndef ALINE_LINK_HH
#define ALINE_LINK_HH

// Standard C++
#include <vector>

// A-line
#include "A-line/StringVector.hh"
#include "A-line/Task.hh"


namespace tool
{
	
	class Project;
	struct TargetInfo;
	
	void NameObjectFiles( const Project&  project,
	                      StringVector&   object_pathnames,
	                      bool            use_cpp );
	
	void LinkProduct( Project&                       project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 project_base_task,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies );
	
}

#endif
