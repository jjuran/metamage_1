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


namespace ALine
{
	
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
	
	std::string UserSrcTreePath()
	{
		if ( const char* tree = getenv( "ALINE_SRC_TREE" ) )
		{
			return tree;
		}
		
		return CurrentUserHomeDirPath() / "src/tree";
	}
	
	std::string get_user_cache_pathname()
	{
		std::string home = CurrentUserHomeDirPath();
		
		std::string subdir_name    = "var";
		std::string cache_dir_name = "cache";
		std::string aline_dir_name = "a-line";
		
		std::string subdir_pathname = CreateDirPath( home / subdir_name );
		
		std::string cache_pathname = CreateDirPath( subdir_pathname / cache_dir_name );
		
		return CreateDirPath( cache_pathname / aline_dir_name );
	}
	
	static std::string UserLabDirPath()
	{
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return builds;
		}
		
		return CreateDirPath( CurrentUserHomeDirPath() / "Developer/Builds" );
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
	
	std::string RezzedDirPath()
	{
		return CreateDirPath( "_Rezzed" );
	}
	
	std::string get_project_dependencies_pathname( const std::string& project_name )
	{
		return CreateDirPath( CreateDirPath( "_Dependencies" ) / project_name );
	}
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj )
	{
		return "Diagnostics" / proj;
	}
	
	std::string ProjectPrecompiledDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "PrecompiledHeaders" ) / proj );
	}
	
	std::string ProjectObjectsDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "Objects" ) / proj );
	}
	
	std::string ProjectMetadataDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "_Metadata" ) / proj );
	}
	
	std::string ProjectLinkedDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "_Linked" ) / proj );
	}
	
	std::string ProjectOutputDirPath( const std::string& proj )
	{
		return CreateDirPath( CreateDirPath( "Output" ) / proj );
	}
	
}

