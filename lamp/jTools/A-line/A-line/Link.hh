/*	=======
 *	Link.hh
 *	=======
 */

#pragma once

// A-line
#include "A-line/Task.hh"


namespace ALine
{
	
	class Project;
	struct TargetInfo;
	
	void LinkProduct( const Project&     project,
	                  const TargetInfo&  targetInfo,
	                  const TaskPtr&     source_dependency );
	
}

