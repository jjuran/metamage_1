/*	==========
 *	Compile.cc
 *	==========
 */

#include "A-line/Compile.hh"

// C Standard Library
#include <stdlib.h>

// C++
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>

// POSIX
#include "unistd.h"

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Aliases.h"
#include "Nitrogen/AppleEvents.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Str.h"

// Nitrogen Extras / Operators
#include "Operators/AEDataModel.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Processes.h"

// BitsAndBytes
#include "StringFilters.hh"

// Orion
#include "Orion/StandardIO.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/Locations.hh"
#include "A-line/MWScripter.hh"
#include "A-line/ProjectCommon.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	
	// FIXME:  Way duplicate
	/*
	static FSSpec DescendPathToFile( N::FSDirSpec dir, const std::string& path )
	{
		typedef std::string::size_type size_type;
		const size_type npos = std::string::npos;
		
		size_type start = 0;
		
		while ( true )
		{
			size_type stop = path.find( '/', start );
			std::string unit = path.substr( start, stop - start );
			
			if ( stop == npos )
			{
				return dir & unit;
			}
			
			dir = dir << unit;
			start = stop + 1;
		}
		
		// Not reached
		return FSSpec();
	}
	*/
	
	static std::string MakeMacroDefinition( const std::pair< const std::string, std::string >& value )
	{
		return "-D" + value.first + "=" + q( value.second );
	}
	
	static std::string MakeIncludeDirOption( const std::string& pathname )
	{
		// Turn a pathname into a -I option.
		// Works for gcc and CW Pro 6; doesn't work for CW Pro 2, MrC, or SC.
		
		return std::string( "-I" ) + q( pathname );
	}
	
	static std::string MakeIncludeDirOptions( const std::vector< std::string >& pathnames )
	{
		return join( pathnames.begin(),
		             pathnames.end(),
		             " ",
		             std::ptr_fun( MakeIncludeDirOption ) );
	}
	
	
	class IncludeDirGatherer
	{
		private:
			std::vector< N::FSDirSpec >& includes;
			std::vector< N::FSDirSpec >& systemIncludes;
		
		public:
			IncludeDirGatherer( std::vector< N::FSDirSpec >& includes, std::vector< N::FSDirSpec >& systemIncludes )
			:
				includes      ( includes ), 
				systemIncludes( systemIncludes )
			{}
				
			void operator()( const ProjName& projName );
	};
	
	void IncludeDirGatherer::operator()( const ProjName& projName )
	{
		const std::vector< N::FSDirSpec >& searchDirs( GetProject( projName ).SearchDirs() );
		
		std::copy( searchDirs.begin(),
		           searchDirs.end(),
		           std::back_inserter( includes ) );
	}
	
	static std::string BuildSourceFileWithCodeWarrior( const CompilerOptions& options, const FSSpec& file )
	{
		std::vector< std::string > includeDirPathnames;
		
		includeDirPathnames.resize( options.UserOnlyIncludeDirs().size() );
		
		std::transform( options.UserOnlyIncludeDirs().begin(),
		                options.UserOnlyIncludeDirs().end(),
		                includeDirPathnames.begin(),
		                std::ptr_fun( N::FSDirGetMacPathname ) );
		
		std::string inc = MakeIncludeDirOptions( includeDirPathnames );
		
		inc << "-I-";
		
		if ( options.UserAndSystemIncludeDirs().size() > 0 )
		{
			includeDirPathnames.clear();
			
			includeDirPathnames.resize( options.UserAndSystemIncludeDirs().size() );
			
			std::transform( options.UserAndSystemIncludeDirs().begin(),
			                options.UserAndSystemIncludeDirs().end(),
			                includeDirPathnames.begin(),
			                std::ptr_fun( N::FSDirGetMacPathname ) );
			
			inc << MakeIncludeDirOptions( includeDirPathnames );
		}
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = cmdgen.CompilerName();
		
		command << cmdgen.AllCompilerOptions();
		
		if ( options.NeedsCwdSource() )
		{
			command << "-cwd source";
		}
		
		/*
		for ( Map::const_iterator it = options.Macros().begin();  it != options.Macros().end();  ++it )
		{
			command << MakeMacroDefinition( *it );
		}
		*/
		
		command << join( options.Macros().begin(),
		                 options.Macros().end(),
		                 " ",
		                 std::ptr_fun( MakeMacroDefinition ) );
		
		command << inc;
		
		if ( options.HasPrecompiledHeaderImage() )
		{
			command << cmdgen.Prefix( N::FSpGetMacPathname( options.PrecompiledHeaderImage() ) );
		}
		
		command << cmdgen.Output( N::FSDirGetMacPathname( options.Output() ) );
		
		// Add the source file to the command line
		command << cmdgen.Input( N::FSpGetMacPathname( file ) );
		
		return EnvironmentTraits< kMPWEnvironment >::MakeNativeCommand( command );
	}
	
	static bool IsCFile( const std::string& name )
	{
		return name.substr( name.size() - 2, name.size() ) == ".c";
	}
	
	static N::FSDirSpec CreateFolder( const FSSpec& folder )
	{
		if ( N::FSpTestItemExists( folder ) )
		{
			return NN::Convert< N::FSDirSpec >( folder );
		}
		
		return N::FSpDirCreate( folder );
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		std::string::size_type dot = filename.find_last_of( "." );
		return filename.substr( 0, dot ) + ".txt";
	}
	
	static std::string BuildSourceFileWithGCC( const CompilerOptions& options, const FSSpec& file )
	{
		std::vector< std::string > includeDirPathnames;
		
		includeDirPathnames.resize( options.UserOnlyIncludeDirs().size() );
		
		std::transform( options.UserOnlyIncludeDirs().begin(),
		                options.UserOnlyIncludeDirs().end(),
		                includeDirPathnames.begin(),
		                std::ptr_fun( N::FSDirGetPOSIXPathname ) );
		
		std::string inc = MakeIncludeDirOptions( includeDirPathnames );
		
		//inc << "-I-";  // This makes gcc pissy
		
		if ( options.UserAndSystemIncludeDirs().size() > 0 )
		{
			includeDirPathnames.clear();
			
			includeDirPathnames.resize( options.UserAndSystemIncludeDirs().size() );
			
			std::transform( options.UserAndSystemIncludeDirs().begin(),
			                options.UserAndSystemIncludeDirs().end(),
			                includeDirPathnames.begin(),
			                std::ptr_fun( N::FSDirGetPOSIXPathname ) );
			
			inc << MakeIncludeDirOptions( includeDirPathnames );
		}
		
		inc << "-I/Developer/Headers/FlatCarbon";
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = cmdgen.CompilerName();
		
		command << cmdgen.AllCompilerOptions();
		
		std::string filename = NN::Convert< std::string >( file.name );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command << "-Wno-non-template-friend";
		}
		
		command << "-Wno-long-double";
		
		command << join( options.Macros().begin(),
		                 options.Macros().end(),
		                 " ",
		                 std::ptr_fun( MakeMacroDefinition ) );
		
		if ( cmdgen.PrecompiledHeaderIsSearched() )
		{
			if ( options.HasPrecompiledHeaderSource() )
			{
				// Specify by name only, so gcc will search for the .gch image.
				std::string pchSourceName = NN::Convert< std::string >( options.PrecompiledHeaderSource().name );
				
				//command << "-include" << q( pchSourceName );
				command << cmdgen.Prefix( pchSourceName );
			}
		}
		else
		{
			if ( options.HasPrecompiledHeaderImage() )
			{
				command << cmdgen.Prefix( N::FSpGetMacPathname( options.PrecompiledHeaderImage() ) );
			}
		}
		
		
		command << inc;
		
		using namespace NN::Operators;
		
		command << cmdgen.Output( N::FSpGetPOSIXPathname( options.Output() & ObjectFileName( filename ) ) );
		
		// Add the source file to the command line
		command << cmdgen.Input( N::FSpGetPOSIXPathname( file ) );
		
		N::FSDirSpec diagnosticsFolder = CreateFolder( N::FSDirGetParent( options.Output() ) & "Diagnostics" );
		FSSpec diagnosticsFile = diagnosticsFolder & DiagnosticsFilenameFromSourceFilename( filename );
		
		command << "> " << q( N::FSpGetPOSIXPathname( diagnosticsFile ) ) << " 2>&1";
		
		command = EnvironmentTraits< kNativePOSIXEnvironment >::MakeNativeCommand( command );
		
		return command;
	}
	
	static N::OSType EditorSignature()
	{
		try
		{
			const char* envMacEditorSignature = getenv( "MAC_EDITOR_SIGNATURE" );
			
			if ( envMacEditorSignature )
			{
				N::OSType signature = NN::Convert< N::OSType, std::string >( envMacEditorSignature );
				
				return signature;
			}
		}
		catch ( ... )
		{
		}
		
		const N::OSType sigTextWrangler = '!Rch';
		
		return sigTextWrangler;
	}
	
	static void BuildSourceFile( CompilerOptions options, const FSSpec& file )
	{
		options.DefineMacro( "__ALINE_MAC_FILE__",  qq( N::FSpGetMacPathname  ( file ) ) );
		options.DefineMacro( "__ALINE_UNIX_FILE__", qq( N::FSpGetPOSIXPathname( file ) ) );
		
		std::string command;
		
		switch ( options.Target().toolkit )
		{
			case toolkitCodeWarrior:
				command = BuildSourceFileWithCodeWarrior( options, file );
				break;
			case toolkitGNU:
				command = BuildSourceFileWithGCC( options, file );
				break;
			default:
				Io::Err << "Unsupported toolkit\n";
				throw N::ParamErr();
		}
		
		std::string filename = NN::Convert< std::string >( file.name );
		
		QueueCommand( "echo Compiling:  " + filename );
		QueueCommand( command );
		
		if ( options.Target().toolkit == toolkitGNU )
		{
			FSSpec diagnostics = N::FSDirGetParent( options.Output() )
			                     << "Diagnostics"
			                     & DiagnosticsFilenameFromSourceFilename( filename );
			
			if ( N::GetEOF( N::FSpOpenDF( diagnostics, fsRdPerm ) ) > 0 )
			{
				N::AESend( N::AECreateAppleEvent( kCoreEventClass, 
				                                  kAEOpenDocuments, 
				                                  N::AECreateDesc< typeProcessSerialNumber >( NX::LaunchApplication( EditorSignature() ) ) ) 
				           << keyDirectObject + ( N::AECreateList< false >() 
				                                  << N::AECreateDesc< typeAlias >( N::NewAlias( diagnostics ) ) ), 
				           kAENoReply | kAECanInteract );
				
			}
			else
			{
				try
				{
					N::FSpDelete( diagnostics );
				}
				catch ( N::OSStatus )
				{
				}
			}
		}
	}
	
	static std::string PrecompileWithCodeWarrior( const CompilerOptions& options, const FSSpec& file )
	{
		std::vector< std::string > includeDirPathnames;
		
		includeDirPathnames.resize( options.UserOnlyIncludeDirs().size() );
		
		std::transform( options.UserOnlyIncludeDirs().begin(),
		                options.UserOnlyIncludeDirs().end(),
		                includeDirPathnames.begin(),
		                std::ptr_fun( N::FSDirGetMacPathname ) );
		
		std::string inc = MakeIncludeDirOptions( includeDirPathnames );
		
		inc << "-I-";
		
		if ( options.UserAndSystemIncludeDirs().size() > 0 )
		{
			includeDirPathnames.clear();
			
			includeDirPathnames.resize( options.UserAndSystemIncludeDirs().size() );
			
			std::transform( options.UserAndSystemIncludeDirs().begin(),
			                options.UserAndSystemIncludeDirs().end(),
			                includeDirPathnames.begin(),
			                std::ptr_fun( N::FSDirGetMacPathname ) );
			
			inc << MakeIncludeDirOptions( includeDirPathnames );
		}
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = cmdgen.CompilerName();
		
		command << cmdgen.AllCompilerOptions();
		
		command << join( options.Macros().begin(),
		                 options.Macros().end(),
		                 " ",
		                 std::ptr_fun( MakeMacroDefinition ) );
		
		// Add the include directories
		command << inc;
		
		command << cmdgen.PrecompiledOutput( N::FSpGetMacPathname( options.PrecompiledHeaderImage() ) );
		
		// Add the source file to the command line
		command << cmdgen.Input( N::FSpGetMacPathname( file ) );
		
		return EnvironmentTraits< kMPWEnvironment >::MakeNativeCommand( command );
	}
	
	static std::string PrecompileWithGCC( const CompilerOptions& options, const FSSpec& file )
	{
		std::vector< std::string > includeDirPathnames;
		
		includeDirPathnames.resize( options.UserOnlyIncludeDirs().size() );
		
		std::transform( options.UserOnlyIncludeDirs().begin(),
		                options.UserOnlyIncludeDirs().end(),
		                includeDirPathnames.begin(),
		                std::ptr_fun( N::FSDirGetPOSIXPathname ) );
		
		std::string inc = MakeIncludeDirOptions( includeDirPathnames );
		
		//inc << "-I-";  // This makes gcc pissy
		
		if ( options.UserAndSystemIncludeDirs().size() > 0 )
		{
			includeDirPathnames.clear();
			
			includeDirPathnames.resize( options.UserAndSystemIncludeDirs().size() );
			
			std::transform( options.UserAndSystemIncludeDirs().begin(),
			                options.UserAndSystemIncludeDirs().end(),
			                includeDirPathnames.begin(),
			                std::ptr_fun( N::FSDirGetPOSIXPathname ) );
			
			inc << MakeIncludeDirOptions( includeDirPathnames );
		}
		
		inc << "-I/Developer/Headers/FlatCarbon";
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = cmdgen.CompilerName();
		
		command << cmdgen.AllCompilerOptions();
		
		std::string filename = NN::Convert< std::string >( file.name );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command << "-Wno-non-template-friend";
		}
		
		command << "-Wno-long-double";
		
		command << join( options.Macros().begin(),
		                 options.Macros().end(),
		                 " ",
		                 std::ptr_fun( MakeMacroDefinition ) );
		
		command << inc;
		
		command << cmdgen.PrecompiledOutput( N::FSpGetPOSIXPathname( options.PrecompiledHeaderImage() ) );
		
		command << cmdgen.Input( N::FSpGetPOSIXPathname( file ) );
		
		using namespace NN::Operators;
		
		N::FSDirSpec diagnosticsFolder = CreateFolder( N::FSDirGetParent( options.Output() ) & "Diagnostics" );
		FSSpec diagnosticsFile = diagnosticsFolder & DiagnosticsFilenameFromSourceFilename( filename );
		
		command << "> " << q( N::FSpGetPOSIXPathname( diagnosticsFile ) ) << " 2>&1";
		
		command = EnvironmentTraits< kNativePOSIXEnvironment >::MakeNativeCommand( command );
		
		return command;
	}
	
	static void Precompile( const CompilerOptions& options, const FSSpec& file )
	{
		std::string command;
		
		switch ( options.Target().toolkit )
		{
			case toolkitCodeWarrior:
				command = PrecompileWithCodeWarrior( options, file );
				break;
			case toolkitGNU:
				command = PrecompileWithGCC( options, file );
				break;
			default:
				Io::Err << "Unsupported toolkit\n";
				throw N::ParamErr();
		}
		
		QueueCommand( "echo Precompiling:  " + NN::Convert< std::string >( file.name ) );
		QueueCommand( command );
	}
	
	static FSSpec PrecompiledHeaderImageFile( const ProjName&    projName,
	                                          std::string        pchSourceName,
	                                          const TargetInfo&  targetInfo )
	{
		N::FSDirSpec folder = ProjectPrecompiledFolder( projName, MakeTargetName( targetInfo ) );
		
		bool gnu = targetInfo.toolkit == toolkitGNU;
		
		std::string pchImageName = gnu ? pchSourceName + ".gch"
		                               : projName      + ".phi";
		
		using namespace NN::Operators;
		
		return folder & pchImageName;
	}
	
	static bool ProjectHasPrecompiledHeader( const ProjName& proj )
	{
		IncludePath pchSource = GetProject( proj ).PrecompiledHeaderSource();
		return !pchSource.empty();
	}
	
	void CompileSources( const Project& project, TargetInfo targetInfo )
	{
		CompilerOptions options( targetInfo );
		
		MacDate pchImageDate = 0;
		bool needToPrecompile = false;
		IncludePath pchSourcePath = project.PrecompiledHeaderSource();
		bool thisProjectProvidesPrecompiledHeader = !pchSourcePath.empty();
		FSSpec pchSource;
		FSSpec pchImage;
		
		if ( thisProjectProvidesPrecompiledHeader )
		{
			// Locate the file and return the latest modification date of any file referenced
			MacDate pchSourceDate = RecursivelyLatestDate( pchSourcePath );
			
			// Locate the precompiled header image file.
			pchSource = IncludeLocation( pchSourcePath );
			std::string pchSourceName = NN::Convert< std::string >( pchSource.name );
			
			pchImage = PrecompiledHeaderImageFile( project.Name(),
			                                       pchSourceName,
			                                       targetInfo );
			
			options.SetPrecompiledHeaderImage( pchImage );
			
			bool pchiExists = N::FSpTestFileExists( pchImage );
			
			// If the image doesn't exist, use a date that will always be newer
			// (for comparison to source files)
			pchImageDate = pchiExists ? ModifiedDate( pchImage )
			                          : 0xFFFFFFFF;
			
			// Compare to the modification dates.
			needToPrecompile = !pchiExists || pchImageDate <= pchSourceDate;
		}
		else
		{
			// This project doesn't have a precompiled header, but maybe a used one does
			typedef std::vector< ProjName >::const_iterator const_iterator;
			
			const_iterator found = std::find_if( project.AllUsedProjects().begin(),
			                                     project.AllUsedProjects().end(),
			                                     N::PtrFun( ProjectHasPrecompiledHeader ) );
			
			if ( found != project.AllUsedProjects().end() )
			{
				pchSourcePath = GetProject( *found ).PrecompiledHeaderSource();
				pchSource = IncludeLocation( pchSourcePath );
				std::string pchSourceName = NN::Convert< std::string >( pchSource.name );
				
				pchImage = PrecompiledHeaderImageFile( *found,
				                                       pchSourceName,
				                                       targetInfo );
				
				pchImageDate = N::FSpTestFileExists( pchImage ) ? ModifiedDate( pchImage )
				                                                : 0xFFFFFFFF;
				
				options.SetPrecompiledHeaderImage( pchImage );
			}
		}
		
		N::FSDirSpec outDir = ProjectObjectsFolder( project.Name(), MakeTargetName( targetInfo ) );
		
		options.SetOutput( outDir );
		
		SetCurrentSourceDir( false );
		
		std::vector< FSSpec > dirtyFiles;
		
		// See which source files need to be compiled,
		// caching include information in the process.
		std::vector< FSSpec >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			using namespace NN::Operators;
			
			sleep( 0 );
			
			// What we do in this loop:
			//  * We set the current source directory to the parent directory
			//    of the current file if the project requires that information.
			//  * If a file needs to be compiled, we add it to the dirty files list.
			//  * Dependency information may be acquired and cached, in RecursivelyLatestDate().
			
			// The source file
			const FSSpec& sourceFile( *it );
			
			if ( project.NeedsCwdSourceOption() )
			{
				SetCurrentSourceDir( N::FSpGetParent( sourceFile ) );
			}
			
			// The file's name
			const std::string& sourceName = NN::Convert< std::string >( sourceFile.name );
			
			// The object file for this source file, which may or may not exist yet.
			FSSpec objectFile = outDir & ObjectFileName( sourceName );
			
			// If object file is nonexistent or older than source,
			// then it must be compiled, and we can skip the includes.
			// Otherwise, compare against the latest include date.
			
			bool needToCompile = true;
			
			// If the object file doesn't exist, we definitely need to compile.
			// But if it does...
			if ( N::FSpTestItemExists( objectFile ) )
			{
				// The effective modification date of the file, considering only
				// a precompiled header (if available).  If the precompiled header
				// has been modified, it saves us extracting the includes.
				// Premature optimization?  Maybe.
				MacDate sourceDate = std::max( pchImageDate, ModifiedDate( sourceFile ) );
				
				MacDate objectDate = ModifiedDate( objectFile );
				
				// If the object file is more recent than the source,
				// (considering first the actual mod date and then the effectove mod date),
				// then it's up to date.
				
				if (    objectDate > sourceDate
				     && objectDate > RecursivelyLatestDate( sourceName, sourceFile ) )
				{
					needToCompile = false;
				}
			}
			
			if ( needToCompile )
			{
				dirtyFiles.push_back( sourceFile );
			}
		}
		
		if ( !needToPrecompile && dirtyFiles.size() == 0 )  return;
		
		options.DefineMacro( "__ALINE__" );
		options.DefineMacro( "JOSHUA_JURAN_EXPERIMENTAL" );
		options.DefineMacro( "IF_ITS_THAT_COMPLICATED_IT_MUST_BE_WRONG" );
		
		if ( targetInfo.platform.api == CD::apiMacCarbon )
		{
			options.DefineMacro( "TARGET_API_MAC_CARBON" );
		}
		else
		{
			options.DefineMacro( "TARGET_API_MAC_CARBON", false );
			options.DefineMacro( "TARGET_API_MAC_OS8" );
			
			if ( targetInfo.platform.arch == CD::archPPC )
			{
				options.DefineMacro( "ACCESSOR_CALLS_ARE_FUNCTIONS" );
				options.DefineMacro( "OPAQUE_UPP_TYPES" );
			}
		}
		
		if ( options.Target().toolkit == toolkitCodeWarrior )
		{
			// Assume CW Pro 6
			options.DefineMacro( "NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS" );
		}
		
		// Select the includes and system includes belonging to the projects we use
		IncludeDirGatherer gatherer
		(
			options.UserOnlyIncludeDirs(), 
			options.UserAndSystemIncludeDirs()
		);
		
		const std::vector< ProjName >& allUsedProjects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( allUsedProjects.rbegin(),
		               allUsedProjects.rend(),
		               gatherer );
		
		if ( targetInfo.build == buildDebug )
		{
			options.DefineMacro( "ALINE_DEBUG", true );
		}
		else if ( targetInfo.build == buildDemo )
		{
			options.DefineMacro( "BUILD_DEMO", true );
		}
		
		if ( project.NeedsCwdSourceOption() )
		{
			options.SetCwdSource();
		}
		
		if ( needToPrecompile )
		{
			Precompile( options, pchSource );
		}
		
		if ( targetInfo.toolkit == toolkitGNU  &&  !pchSourcePath.empty() )
		{
			options.SetPrecompiledHeaderSource( pchSource );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			std::vector< N::FSDirSpec >& userOnlyIncludeDirs = options.UserOnlyIncludeDirs();
			
			userOnlyIncludeDirs.insert( userOnlyIncludeDirs.begin(),
			                            N::FSpGetParent( pchImage ) );
		}
		
		std::for_each( dirtyFiles.begin(),
		               dirtyFiles.end(),
		               std::bind1st( N::PtrFun( BuildSourceFile ),
		                             options ) );
	}
	
}

