/*	================
 *	ProjectConfig.hh
 *	================
 */

#ifndef COMPILEDRIVER_PROJECTCONFIG_HH
#define COMPILEDRIVER_PROJECTCONFIG_HH

// Standard C++
#include <map>
#include <vector>

// plus
#include "plus/string.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectDotConf.hh"


namespace tool
{
	
	typedef plus::string ConfigKey;
	typedef std::vector< plus::string > ConfigValue;
	typedef std::map< ConfigKey, ConfigValue > ConfData;
	
	struct NoSuchProject
	{
		plus::string name;
		
		NoSuchProject( const plus::string& name ) : name( name )  {}
	};
	
	struct NoSuchPlatform
	{
		plus::string name;
		
		NoSuchPlatform( const plus::string& name ) : name( name )  {}
	};
	
	void AddPendingSubproject( const plus::string& dir );
	
	bool AddPendingSubprojects();
	
	ConfData MakeConfData( const DotConfData& data );
	
}

#endif

