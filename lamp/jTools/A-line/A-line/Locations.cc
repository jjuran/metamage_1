/*	============
 *	Locations.cc
 *	============
 */

#include "A-line/Locations.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/stat.h>

// Nitrogen
#include "Nitrogen/Folders.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Divergence
#include "Divergence/Utilities.hh"

// CompileDriver
#include "CompileDriver/ProjectConfig.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Div = Divergence;
	namespace CD = CompileDriver;
	
	
	static N::FSDirSpec CreateFolder( const FSSpec& folder )
	{
		if ( io::item_exists( folder ) )
		{
			return NN::Convert< N::FSDirSpec >( folder );
		}
		
		return N::FSpDirCreate( folder );
	}
	
	static std::string CreateDirPath( const std::string& path )
	{
		int made_dir = mkdir( path.c_str(), 0700 );
		
		return path;
	}
	
	static N::FSDirSpec CurrentUserHomeDir()
	{
	#if TARGET_RT_MAC_MACHO
		
		return N::FindFolder( N::kCurrentUserFolderType, kDontCreateFolder );
		
	#else
		
		if ( const char* home = getenv( "HOME" ) )
		{
			return NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( home ) );
		}
		
		return N::RootDirectory( N::BootVolume() ) << "Users" << "jjuran";
		
	#endif
	}
	
	static std::string CurrentUserHomeDirPath()
	{
		if ( const char* home = getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	static N::FSDirSpec UserDeveloperFolder()
	{
		return CurrentUserHomeDir() << "Developer";
	}
	
	static std::string UserDeveloperPath()
	{
		return CurrentUserHomeDirPath() + "/Developer";
	}
	
	N::FSDirSpec UserProjectsFolder()
	{
	#if !TARGET_RT_MAC_MACHO
		
		if ( const char* projects = getenv( "ALINE_PROJECTS" ) )
		{
			return NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( projects ) );
		}
		
	#endif
		
		return UserDeveloperFolder() << "Projects";
	}
	
	std::string UserProjectsPath()
	{
		if ( const char* projects = getenv( "ALINE_PROJECTS" ) )
		{
			return projects;
		}
		
		return UserDeveloperPath() + "/Projects";
	}
	
	static N::FSDirSpec UserLabFolder()
	{
	#if !TARGET_RT_MAC_MACHO
		
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( builds ) );
		}
		
	#endif
		
		return CreateFolder( UserDeveloperFolder() & "Lab" );
	}
	
	static std::string UserLabDirPath()
	{
		if ( const char* builds = getenv( "ALINE_BUILDS" ) )
		{
			return builds;
		}
		
		return CreateDirPath( UserDeveloperPath() + "/Lab" );
	}
	
	bool ProjectHasSystemIncludes( const N::FSDirSpec& folder )
	{
		return io::item_exists( folder & "SystemIncludes" );
	}
	
	N::FSDirSpec ProjectSourcesFolder( const N::FSDirSpec& folder )
	{
		N::FSDirSpec source = folder;
		
		try
		{
			return source << "Sources";
		}
		catch ( N::FNFErr )
		{
		}
		
		return source;
	}
	
	std::string ProjectSourcesPath( const std::string& projectPath )
	{
		struct ::stat sb;
		
		std::string sources = projectPath + "/Sources";
		
		if ( stat( sources.c_str(), &sb ) == 0 )
		{
			return sources;
		}
		
		return projectPath;
	}
	
	N::FSDirSpec ProjectIncludesFolder( const N::FSDirSpec& folder )
	{
		N::FSDirSpec source = folder;
		
		try { return source << "SystemIncludes"; } catch ( N::FNFErr ) {}
		try { return source << "Includes";       } catch ( N::FNFErr ) {}
		try { return source << "include";        } catch ( N::FNFErr ) {}
		try { return source << "include";        } catch ( N::FNFErr ) {}
		
		return ProjectSourcesFolder( folder );
	}
	
	std::string ProjectIncludesPath( const std::string& projectPath )
	{
		struct ::stat sb;
		
		std::string includes = projectPath + "/Includes";
		
		if ( stat( includes.c_str(), &sb ) == 0 )
		{
			return includes;
		}
		
		includes = projectPath + "/include";
		
		if ( stat( includes.c_str(), &sb ) == 0 )
		{
			return includes;
		}
		
		return ProjectSourcesPath( projectPath );
	}
	
	static N::FSDirSpec ProjectControlFolder( const N::FSDirSpec& folder )
	{
		N::FSDirSpec source = folder;
		
		try
		{
			return source << "A-line.confd";
		}
		catch ( N::FNFErr )  {}
		
		return source;
	}
	
	static std::string ProjectConfigDirPath( const std::string& projectPath )
	{
		struct ::stat sb;
		
		std::string confd = projectPath + "/A-line.confd";
		
		if ( stat( confd.c_str(), &sb ) == 0 )
		{
			return confd;
		}
		
		return projectPath;
	}
	
	FSSpec SourceDotListFile( const N::FSDirSpec& folder )
	{
		return ProjectControlFolder( folder ) & "Source.list";
	}
	
	std::string SourceDotListFile( const std::string& projectPath )
	{
		return ProjectConfigDirPath( projectPath ) + "/Source.list";
	}
	
	static N::FSDirSpec TargetFolder( const std::string& target )
	{
		return CreateFolder( UserLabFolder() & target );
	}
	
	static N::FSDirSpec ProjectTargetFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( TargetFolder( target ) & proj );
	}
	
	N::FSDirSpec ProjectDiagnosticsFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( ProjectTargetFolder( proj, target ) & "Diagnostics" );
	}
	
	N::FSDirSpec ProjectPrecompiledFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( ProjectTargetFolder( proj, target ) & "Precompiled Header" );
	}
	
	N::FSDirSpec ProjectObjectsFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( ProjectTargetFolder( proj, target ) & "(Objects)" );
	}
	
	N::FSDirSpec ProjectLibrariesFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( ProjectTargetFolder( proj, target ) & "Output" );
	}
	
	static std::string TargetDirPath( const std::string& target )
	{
		return CreateDirPath( UserLabDirPath() + '/' + target );
	}
	
	static std::string ProjectTargetDirPath( const std::string& proj, const std::string& target )
	{
		return CreateDirPath( TargetDirPath( target ) + '/' + proj );
	}
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj, const std::string& target )
	{
		return CreateDirPath( ProjectTargetDirPath( proj, target ) + "/Diagnostics" );
	}
	
	std::string ProjectPrecompiledDirPath( const std::string& proj, const std::string& target )
	{
		return CreateDirPath( ProjectTargetDirPath( proj, target ) + "/Precompiled Header" );
	}
	
	std::string ProjectObjectsDirPath( const std::string& proj, const std::string& target )
	{
		return CreateDirPath( ProjectTargetDirPath( proj, target ) + "/(Objects)" );
	}
	
	std::string ProjectLibrariesDirPath( const std::string& proj, const std::string& target )
	{
		return CreateDirPath( ProjectTargetDirPath( proj, target ) + "/Output" );
	}
	
}

