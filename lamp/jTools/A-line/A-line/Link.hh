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
	
	extern bool avoid_long_names;
	
	void NameObjectFiles( const Project&  project,
	                      StringVector&   object_pathnames,
	                      bool            use_cpp );
	
	void LinkProduct( Project&                       project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 project_base_task,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies );
	
	inline
	bool can_run_Classic()
	{
	#if defined( __APPLE__ )  &&  defined( __POWERPC__ )
		
		/*
			This isn't a *guarantee* that Classic is available,
			but merely the possibility from the perspective of
			the code being compiled.  We might end up running
			in Mac OS X 10.5 (or even 10.6, with Rosetta), where
			Classic is no longer supported.  Or, perhaps Classic
			does exist, but Mac OS 9 simply hasn't been installed.
		*/
		
		return true;
		
	#endif
		
		return false;
	}
	
}

#endif
