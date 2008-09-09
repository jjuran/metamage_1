/*	================
 *	ProjectCommon.cc
 *	================
 */

#include "A-line/ProjectCommon.hh"

// C++
#include <algorithm>
#include <map>
#include <vector>

// A-line
#include "A-line/BuildCommon.hh"
#include "A-line/Includes.hh"
#include "A-line/Project.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace ALine
{
	
	// A map from project name to project data
	typedef std::map< std::string, boost::shared_ptr< Project > > ProjectMap;
	
	// A map from platform to project map
	typedef std::map< Platform, ProjectMap > ProjectPlatformMap;
	
	
	static ProjectPlatformMap gProjectPlatformMap;
	
	
	Project& GetProject( const std::string& project_name, Platform platform )
	{
		boost::shared_ptr< Project >& project_ptr = gProjectPlatformMap[ platform ][ project_name ];
		
		if ( project_ptr == NULL )
		{
			const ProjectConfig& config = GetProjectConfig( project_name, platform );
			
			project_ptr.reset( new Project( project_name,
			                                platform,
			                                config.get_project_dir(),
			                                config.get_config_data() ) );
		}
		
		return *project_ptr;
	}
	
}

