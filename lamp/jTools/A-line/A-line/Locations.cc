/*	============
 *	Locations.cc
 *	============
 */

#include "A-line/Locations.hh"

// Standard C
#include <stdlib.h>

// plus
#include "plus/string/concat.hh"

// poseven
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/stat.hh"

// pfiles
#include "pfiles/common.hh"

// one_path
#include "one_path/find_SDK_dir.hh"

// A-line
#include "A-line/Platform.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	const plus::string& mkdir_path( const plus::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path );
		}
		
		return path;
	}
	
	
	plus::string home_dir_pathname()
	{
		if ( const char* home = getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	plus::string UserSrcTreePath()
	{
		if ( const char* root = getenv( "ALINE_SRC_ROOT" ) )
		{
			return root;
		}
		
		if ( const char* tree = getenv( "ALINE_SRC_TREE" ) )
		{
			return tree;
		}
		
		return home_dir_pathname() / "src/tree";
	}
	
	plus::string get_user_cache_pathname()
	{
		plus::string home = home_dir_pathname();
		
		const char* cache = ALINE_MAC_DEVELOPMENT ? "Library/Caches/A-line"
		                                          : "var/cache/a-line";
		
		return mkdir_path( home / cache );
	}
	
	plus::string get_Interfaces_subdir( const char* name )
	{
		static plus::string interfaces = find_SDK_dir() + "/" "Interfaces" "/";
		
		return interfaces + name;
	}
	
	static plus::string BuildsDirPath()
	{
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return builds;
		}
		
		return mkdir_path( home_dir_pathname() / "var/build" );
	}
	
	plus::string InstallDirPath()
	{
		if ( const char* install_dir = getenv( "ALINE_INSTALL" ) )
		{
			return install_dir;
		}
		
		return mkdir_path( home_dir_pathname() / "var/bin" );
	}
	
	static plus::string ProjectConfigDirPath( const plus::string& projectPath )
	{
		plus::string confd = projectPath / "A-line.confd";
		
		if ( io::directory_exists( confd ) )
		{
			return confd;
		}
		
		return projectPath;
	}
	
	plus::string SourceDotListFile( const plus::string& projectPath )
	{
		return ProjectConfigDirPath( projectPath ) / "Source.list";
	}
	
	plus::string TargetDirPath( const plus::string& target )
	{
		return mkdir_path( BuildsDirPath() / target );
	}
	
	plus::string get_includes_union_pathname()
	{
		return mkdir_path( "include" );
	}
	
	plus::string LibrariesDirPath()
	{
		return mkdir_path( "lib" );
	}
	
	plus::string RezzedDirPath()
	{
		return mkdir_path( "rez" );
	}
	
	plus::string get_project_dependencies_pathname( const plus::string& project_name )
	{
		return "dep" / project_name;
	}
	
	plus::string ProjectDiagnosticsDirPath( const plus::string& proj )
	{
		return "log" / proj;
	}
	
	plus::string ProjectPrecompiledDirPath( const plus::string& proj )
	{
		return mkdir_path( "pch" / proj );
	}
	
	plus::string ProjectObjectsDirPath( const plus::string& proj )
	{
		return mkdir_path( "obj" / proj );
	}
	
	plus::string ProjectMetadataDirPath( const plus::string& proj )
	{
		return mkdir_path( "meta" / proj );
	}
	
	plus::string ProjectLinkedDirPath( const plus::string& proj )
	{
		return mkdir_path( "out" / proj );
	}
	
	plus::string ProjectOutputDirPath( const plus::string& proj )
	{
		return mkdir_path( "bin" / proj );
	}
	
}
