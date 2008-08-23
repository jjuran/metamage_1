/*	==========
 *	Compile.hh
 *	==========
 *	
 *	Implemented by Compile.cc
 */

#pragma once

// A-line
#include "A-line/Task.hh"


namespace ALine
{
	
	class Project;
	struct TargetInfo;
	
	void CompileSources( const Project&     project,
	                     const TargetInfo&  targetInfo,
	                     const TaskPtr&     source_dependency );
	
}

