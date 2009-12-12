/*	============
 *	Locations.cc
 *	============
 */

#include "A-line/Locations.hh"

// Standard C
#include <stdlib.h>

// poseven
#include "poseven/Pathnames.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/stat.hh"

// A-line
#include "CompileDriver/Platform.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	const std::string& mkdir_path( const std::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path );
		}
		
		return path;
	}
	
	
	std::string home_dir_pathname()
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
		
		return home_dir_pathname() / "src/tree";
	}
	
	std::string get_user_cache_pathname()
	{
		std::string home = home_dir_pathname();
		
		const char* cache = ALINE_MAC_DEVELOPMENT ? "Library/Caches/A-line"
		                                          : "var/cache/a-line";
		
		return mkdir_path( home / cache );
	}
	
	static std::string UserLabDirPath()
	{
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return builds;
		}
		
		return mkdir_path( home_dir_pathname() / "Developer/Builds" );
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
		return mkdir_path( UserLabDirPath() / target );
	}
	
	std::string get_includes_union_pathname()
	{
		return mkdir_path( "Includes" );
	}
	
	std::string LibrariesDirPath()
	{
		return mkdir_path( "Libraries" );
	}
	
	std::string RezzedDirPath()
	{
		return mkdir_path( "_Rezzed" );
	}
	
	std::string get_project_dependencies_pathname( const std::string& project_name )
	{
		return "_Dependencies" / project_name;
	}
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj )
	{
		return "Diagnostics" / proj;
	}
	
	std::string ProjectPrecompiledDirPath( const std::string& proj )
	{
		return mkdir_path( "PrecompiledHeaders" / proj );
	}
	
	std::string ProjectObjectsDirPath( const std::string& proj )
	{
		return mkdir_path( "Objects" / proj );
	}
	
	std::string ProjectMetadataDirPath( const std::string& proj )
	{
		return mkdir_path( "_Metadata" / proj );
	}
	
	std::string ProjectLinkedDirPath( const std::string& proj )
	{
		return mkdir_path( "_Linked" / proj );
	}
	
	std::string ProjectOutputDirPath( const std::string& proj )
	{
		return mkdir_path( "Output" / proj );
	}
	
}

