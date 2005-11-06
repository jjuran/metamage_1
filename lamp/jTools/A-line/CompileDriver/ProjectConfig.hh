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
		N::FSDirSpec  folder;
		ConfData      confData;
		Platform      platform;
		
		ProjectData()  {}
		
		ProjectData( const N::FSDirSpec& folder, const ConfData& conf );
	};
	
	struct NoSuchProject
	{
		ProjName name;
		
		NoSuchProject( const ProjName& name ) : name( name )  {}
	};
	
	void AddProjectConfig( const ProjName& projName,
	                       const N::FSDirSpec& folder,
	                       const ConfData& conf );
	
	void AddPendingSubproject( const N::FSDirSpec& dir );
	
	const ProjectData& GetProjectData( const ProjName& projName, const Platform& targetPlatform );
	
	const N::FSDirSpec& GetProjectFolder( const ProjName& projName, const Platform& targetPlatform );
	const ConfData&     GetProjectConfig( const ProjName& projName, const Platform& targetPlatform );
	
	ConfData MakeConfData( const DotConfData& data );
	
}

#endif

