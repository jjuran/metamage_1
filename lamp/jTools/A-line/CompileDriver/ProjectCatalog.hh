/*	=================
 *	ProjectCatalog.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTCATALOG_HH
#define COMPILEDRIVER_PROJECTCATALOG_HH

// Standard C++
#include <iterator>
#include <map>
#include <string>
#include <vector>

// A-line
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectConfig.hh"


namespace CompileDriver
{
	
	struct ProjectConfig
	{
		std::string  itsPathname;
		std::string  itsProjectFolder;
		ConfData     itsConfigData;
		
		ProjectConfig()  {}
		
		ProjectConfig( const std::string&  pathname,
		               const std::string&  folder,
		               const ConfData&     data ) : itsPathname     ( pathname ),
		                                            itsProjectFolder( folder   ),
		                                            itsConfigData   ( data     )
		{
		}
	};
	
	// A map from platform requirements to project config data
	typedef std::map< PlatformDemands, ProjectConfig > ProjectConfigCandidates;
	
	// A map from project name to config file set
	typedef std::map< std::string, ProjectConfigCandidates > ProjectCatalog;
	
	
	void AddProjectConfigFile( const std::string&      name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config );
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform platform );
	
	void ScanDirForProjects( const std::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< std::string > >  configs,
	                         std::back_insert_iterator< std::vector< std::string > >  folders );
	
}

#endif

