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
	
	typedef std::string ConfigKey;
	typedef std::vector< std::string > ConfigValue;
	typedef std::map< ConfigKey, ConfigValue > ConfData;
	
	struct NoSuchProject
	{
		std::string name;
		
		NoSuchProject( const std::string& name ) : name( name )  {}
	};
	
	struct NoSuchPlatform
	{
		std::string name;
		
		NoSuchPlatform( const std::string& name ) : name( name )  {}
	};
	
	void AddPendingSubproject( const std::string& dir );
	
	ConfData MakeConfData( const DotConfData& data );
	
}

#endif

