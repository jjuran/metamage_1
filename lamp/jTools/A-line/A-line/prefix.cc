/*
	prefix.cc
	---------
*/

#include "A-line/prefix.hh"

// Standard C++
#include <algorithm>
#include <functional>

// plus
#include "plus/pointer_to_function.hh"

// A-line
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"


namespace tool
{
	
	static inline
	bool project_has_prefix( const plus::string& project_name, Platform platform )
	{
		return GetProject( project_name, platform ).HasPrecompiledHeader();
	}
	
	const Project* get_project_providing_prefix( const Project&  project,
	                                             Platform        platform )
	{
		const Project* result = NULL;
		
		if ( project.HasPrecompiledHeader() )
		{
			result = &project;
		}
		else
		{
			// This project doesn't have a precompiled header, but maybe a used one does
			typedef std::vector< plus::string >::const_iterator const_iterator;
			
			const_iterator it = std::find_if( project.AllUsedProjects().begin(),
			                                  project.AllUsedProjects().end(),
			                                  std::bind2nd( plus::ptr_fun( project_has_prefix ),
			                                                platform ) );
			
			if ( it != project.AllUsedProjects().end() )
			{
				result = &GetProject( *it, platform );
			}
		}
		
		return result;
	}
	
}
