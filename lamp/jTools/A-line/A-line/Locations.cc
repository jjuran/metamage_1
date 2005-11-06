/*	============
 *	Locations.cc
 *	============
 */

#include "A-line/Locations.hh"

// Standard C
#include <stdlib.h>

// Nitrogen
#include "Nitrogen/Folders.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Kerosene
#include "SystemCalls.hh"

// CompileDriver
#include "CompileDriver/ProjectConfig.hh"


namespace ALine {
	
	namespace N = Nitrogen;
	namespace CD = CompileDriver;
	
	
	static N::FSDirSpec CreateFolder( const FSSpec& folder )
	{
		if ( N::FSpTestItemExists( folder ) )
		{
			return N::Convert< N::FSDirSpec >( folder );
		}
		
		return N::FSpDirCreate( folder );
	}
	
	static N::FSDirSpec CurrentUserHomeDir()
	{
	#if TARGET_RT_MAC_MACHO
		
		return N::FindFolder( N::FolderType::Make( kCurrentUserFolderType ), kDontCreateFolder );
		
	#else
		
		if ( const char* home = getenv( "HOME" ) )
		{
			return N::Convert< N::FSDirSpec >( Path2FSS( home ) );
		}
		
		return N::RootDirectory( N::BootVolume() ) << "Users" << "jjuran";
		
	#endif
	}
	
	static N::FSDirSpec UserDeveloperFolder()
	{
		return CurrentUserHomeDir() << "Developer";
	}
	
	N::FSDirSpec UserProjectsFolder()
	{
		return UserDeveloperFolder() << "Projects";
	}
	
	static N::FSDirSpec UserLabFolder()
	{
		return CreateFolder( UserDeveloperFolder() & "Lab" );
	}
	
	bool ProjectHasSystemIncludes( const N::FSDirSpec& folder )
	{
		return N::FSpTestItemExists( folder & "SystemIncludes" );
	}
	
	static N::FSDirSpec ProjectControlFolder( const N::FSDirSpec& folder )
	{
		N::FSDirSpec source = folder;
		
		try
		{
			return source << "A-line.confd";
		}
		catch ( N::FNFErr )  {}
		
		try
		{
			return source << "A-line";
		}
		catch ( N::FNFErr )  {}
		
		try
		{
			return source << "Control";
		}
		catch ( N::FNFErr )  {}
		
		return source;
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
	
	N::FSDirSpec ProjectIncludesFolder( const N::FSDirSpec& folder )
	{
		N::FSDirSpec source = folder;
		
		try { return source << "SystemIncludes"; } catch ( N::FNFErr ) {}
		try { return source << "Includes";       } catch ( N::FNFErr ) {}
		try { return source << "include";        } catch ( N::FNFErr ) {}
		try { return source << "include";        } catch ( N::FNFErr ) {}
		
		return ProjectSourcesFolder( folder );
	}
	
	/*
	template <class String>
	V::DirSpec
	ProjectResourcesFolder(const String& name)
	{
		FSSpec proj = FindProjectFolder(name);
		FSSpec resources = proj + PSTR("Resources");
		if (V::Exists(resources)) {
			return resources;
		} else {
			return proj;
		}
	}
	*/
	
	FSSpec SourceDotListFile( const N::FSDirSpec& folder )
	{
		return ProjectControlFolder( folder ) & "Source.list";
	}
	
	static N::FSDirSpec ProjectLabFolder( const std::string& name )
	{
		return CreateFolder( UserLabFolder() & name );
	}
	
	static N::FSDirSpec ProjectTargetFolder( const std::string& proj, const std::string& target )
	{
		return CreateFolder( ProjectLabFolder( proj ) & target );
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
	
}

