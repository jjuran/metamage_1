/*	================
 *	ProjectConfig.hh
 *	================
 */

#ifndef COMPILEDRIVER_PROJECTCONFIG_HH
#define COMPILEDRIVER_PROJECTCONFIG_HH

// Nitrogen
#include "Nitrogen/Files.h"

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectDotConf.hh"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	
	typedef std::string ProjName;
	
	typedef std::string ConfigKey;
	typedef std::vector< std::string > ConfigValue;
	typedef std::map< ConfigKey, ConfigValue > ConfData;
	
	struct ProjectData
	{
		std::string  folder;
		ConfData     confData;
		Platform     platform;
		
		ProjectData()  {}
		
		ProjectData( const std::string& folder, const ConfData& conf );
	};
	
	struct NoSuchProject
	{
		ProjName name;
		
		NoSuchProject( const ProjName& name ) : name( name )  {}
	};
	
	void AddPendingSubproject( const N::FSDirSpec& dir );
	
	const ProjectData& GetProjectData( const ProjName& projName, const Platform& targetPlatform );
	
	const std::string&  GetProjectFolder( const ProjName& projName, const Platform& targetPlatform );
	const ConfData&     GetProjectConfig( const ProjName& projName, const Platform& targetPlatform );
	
	ConfData MakeConfData( const DotConfData& data );
	
}

#endif

