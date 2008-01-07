/*	============
 *	Locations.cc
 *	============
 */

#include "A-line/Locations.hh"

// Standard C
#include <stdlib.h>

// POSeven
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// CompileDriver
#include "CompileDriver/ProjectConfig.hh"


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	
	using namespace io::path_descent_operators;
	
	
	static std::string CreateDirPath( const std::string& path )
	{
		int made_dir = mkdir( path.c_str(), 0700 );
		
		return path;
	}
	
	static std::string CurrentUserHomeDirPath()
	{
		if ( const char* home = getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	static std::string UserDeveloperPath()
	{
		return CurrentUserHomeDirPath() + "/Developer";
	}
	
	std::string UserProjectsPath()
	{
		if ( const char* projects = getenv( "ALINE_PROJECTS" ) )
		{
			return projects;
		}
		
		return UserDeveloperPath() / "Projects";
	}
	
	static std::string UserLabDirPath()
	{
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return builds;
		}
		
		return CreateDirPath( UserDeveloperPath() / "Builds" );
	}
	
	std::string ProjectSourcesPath( const std::string& projectPath )
	{
		std::string sources = projectPath / "Sources";
		
		if ( io::directory_exists( sources ) )
		{
			return sources;
		}
		
		return projectPath;
	}
	
	std::string ProjectIncludesPath( const std::string& projectPath )
	{
		std::string includes = projectPath / "Includes";
		
		if ( io::directory_exists( includes ) )
		{
			return includes;
		}
		
		includes = projectPath / "include";
		
		if ( io::directory_exists( includes ) )
		{
			return includes;
		}
		
		return ProjectSourcesPath( projectPath );
	}
	
	static std::string ProjectConfigDirPath( const std::string& projectPath )
	{
		std::string confd = projectPath / "A-line.confd";
		
		if ( io::directory_exists( confd ) )
		{
			return confd;
		}
		
		return projectPath;
	}
	
	std::string SourceDotListFile( const std::string& projectPath )
	{
		return ProjectConfigDirPath( projectPath ) / "Source.list";
	}
	
	std::string TargetDirPath( const std::string& target )
	{
		return CreateDirPath( UserLabDirPath() / target );
	}
	
	std::string LibrariesDirPath()
	{
		return CreateDirPath( "Libraries" );
	}
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "Diagnostics" ) / proj );
	}
	
	std::string ProjectPrecompiledDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "PrecompiledHeaders" ) / proj );
	}
	
	std::string ProjectObjectsDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "Objects" ) / proj );
	}
	
	std::string ProjectOutputDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "Output" ) / proj );
	}
	
}

