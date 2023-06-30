/*
	prefix.cc
	---------
*/

#include "A-line/prefix.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"


namespace tool
{
	
	const Project* get_project_providing_prefix( const Project&  project,
	                                             Platform        platform )
	{
	#ifdef __MC68K__
		
		/*
			CodeWarrior Pro 4 doesn't have the -convertpaths option, so we
			need to preprocess with mxcpp to deal with POSIX include paths.
		*/
		return NULL;
		
	#endif
		
	#ifndef __RELIX__
		
		if ( Options().preprocess )
		{
			// On non-MacRelix, precompiling does *not* override preprocessing
			return NULL;
		}
		
	#endif
		
		if ( project.HasPrecompiledHeader() )
		{
			return &project;
		}
		
		// This project doesn't have a precompiled header, but maybe a used one does
		typedef StringVector::const_iterator const_iterator;
		
		const_iterator const end = project.AllUsedProjects().end();
		const_iterator it        = project.AllUsedProjects().begin();
		
		while ( it != end )
		{
			const Project& project = GetProject( *it++, platform );
			
			if ( project.HasPrecompiledHeader() )
			{
				return &project;
			}
		}
		
		return NULL;
	}
	
}
