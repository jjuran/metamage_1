/*	================
 *	ProjectConfig.hh
 *	================
 */

#ifndef COMPILEDRIVER_PROJECTCONFIG_HH
#define COMPILEDRIVER_PROJECTCONFIG_HH

// Standard C++
#include <map>
#include <string>
#include <vector>

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectDotConf.hh"


namespace CompileDriver
{
	
	typedef std::string ProjName;
	
	typedef std::string ConfigKey;
	typedef std::vector< std::string > ConfigValue;
	typedef std::map< ConfigKey, ConfigValue > ConfData;
	
	struct ProjectData
	{
		std::string      folder;
		ConfData         confData;
		PlatformDemands  platformDemands;
		
		ProjectData()  {}
		
		ProjectData( const std::string& folder, const ConfData& conf );
	};
	
	struct NoSuchProject
	{
		ProjName name;
		
		NoSuchProject( const ProjName& name ) : name( name )  {}
	};
	
	void AddPendingSubproject( const std::string& dir );
	
	const ProjectData& GetProjectData( const ProjName& projName, const Platform& targetPlatform );
	
	const std::string&  GetProjectFolder( const ProjName& projName, const Platform& targetPlatform );
	const ConfData&     GetProjectConfig( const ProjName& projName, const Platform& targetPlatform );
	
	ConfData MakeConfData( const DotConfData& data );
	
}

#endif

