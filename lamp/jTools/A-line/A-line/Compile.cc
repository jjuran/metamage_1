/*	==========
 *	Compile.cc
 *	==========
 */

#include "A-line/Compile.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Io
#include "io/io.hh"
#include "io/files.hh"

// POSeven
#include "POSeven/Pathnames.hh"

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Aliases.h"
#include "Nitrogen/AppleEvents.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Str.h"

// GetPathname
#include "GetPathname.hh"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// BitsAndBytes
#include "StringFilters.hh"

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
	
	using namespace io::path_descent_operators;
	
	using BitsAndBytes::q;
	
	
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
		
		public:
			IncludeDirGatherer( std::vector< N::FSDirSpec >& includes ) : includes( includes )  {}
				
			void operator()( const ProjName& projName );
	};
	
	void IncludeDirGatherer::operator()( const ProjName& projName )
	{
		const std::vector< N::FSDirSpec >& searchDirs( GetProject( projName ).SearchDirs() );
		
		std::copy( searchDirs.begin(),
		           searchDirs.end(),
		           std::back_inserter( includes ) );
	}
	
	
	static bool IsCFile( const std::string& name )
	{
		return name.substr( name.size() - 2, name.size() ) == ".c";
	}
	
	static N::FSDirSpec CreateFolder( const FSSpec& folder )
	{
		if ( io::item_exists( folder ) )
		{
			return N::FSDirSpec( folder );
		}
		
		return N::FSpDirCreate( folder );
	}
	
	static std::string GetFilenameBase( const std::string& filename )
	{
		std::size_t dot = filename.find_last_of( "." );
		
		return filename.substr( 0, dot );
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		return GetFilenameBase( filename ) + ".txt";
	}
	
	static std::string DiagnosticsFilePathname( const N::FSDirSpec& outputDir, const std::string& filename )
	{
		N::FSDirSpec diagnosticsFolder = CreateFolder( io::get_preceding_directory( outputDir ) / "Diagnostics" );
		FSSpec diagnosticsFile = diagnosticsFolder / DiagnosticsFilenameFromSourceFilename( filename );
		
		return GetPOSIXPathname( diagnosticsFile );
	}
	
	static std::string IncludeDirString( const CompilerOptions& options )
	{
		std::vector< std::string > includeDirPathnames;
		
		includeDirPathnames.resize( options.UserOnlyIncludeDirs().size() );
		
		std::transform( options.UserOnlyIncludeDirs().begin(),
		                options.UserOnlyIncludeDirs().end(),
		                includeDirPathnames.begin(),
		                std::ptr_fun( static_cast< std::string (*)(const N::FSDirSpec&) >( GetPOSIXPathname ) ) );
		
		return MakeIncludeDirOptions( includeDirPathnames );
	}
	
	static std::string MakeCompileCommand( const CompilerOptions& options )
	{
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = cmdgen.CompilerName();
		
		command << cmdgen.AllCompilerOptions();
		
		if ( options.NeedsCwdSource() )
		{
			command << "-cwd source";
		}
		
		command << "-Wno-deprecated-declarations -Wno-long-double";
		
		command << join( options.Macros().begin(),
		                 options.Macros().end(),
		                 " ",
		                 std::ptr_fun( MakeMacroDefinition ) );
		
		command << IncludeDirString( options );
		
		return command;
	}
	
	static void BuildSourceFile( CompilerOptions options, const FSSpec& file )
	{
		bool gnu = options.Target().toolkit == toolkitGNU;
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = MakeCompileCommand( options );
		
		std::string filename = io::get_filename_string( file );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command << "-Wno-non-template-friend";
		}
		
		if ( options.HasPrecompiledHeaderSource() )
		{
			// Specify by name only, so gcc will search for the .gch image.
			std::string pchSourceName = io::get_filename_string( options.PrecompiledHeaderSource() );
			
			//command << "-include" << q( pchSourceName );
			command << cmdgen.Prefix( pchSourceName );
		}
		
		command << cmdgen.Output( GetPOSIXPathname( options.Output() / ObjectFileName( filename ) ) );
		
		command << cmdgen.Input( GetPOSIXPathname( file ) );
		
		if ( gnu )
		{
			command += " > " + q( DiagnosticsFilePathname( options.Output(), filename ) ) + " 2>&1";
		}
		
		QueueCommand( "echo Compiling:  " + filename );
		QueueCommand( command );
		
		if ( gnu )
		{
			std::string diagnostics = DiagnosticsFilePathname( options.Output(), filename );
			
			struct ::stat sb;
			
			int status = ::stat( diagnostics.c_str(), &sb );
			
			if ( status == 0 && sb.st_size > 0 )
			{
				system( ( "edit " + q( diagnostics ) ).c_str() );
			}
			else if ( status == 0 )
			{
				//try { io::delete_file( diagnostics ); } catch ( ... ) {}
				unlink( diagnostics.c_str() );
			}
		}
	}
	
	static void Precompile( const CompilerOptions&  options,
	                        const std::string&      pathname_to_precompiled_header_source )
	{
		bool gnu = options.Target().toolkit == toolkitGNU;
		
		CommandGenerator cmdgen( options.Target() );
		
		std::string command = MakeCompileCommand( options );
		
		std::string filename = io::get_filename_string( pathname_to_precompiled_header_source );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command << "-Wno-non-template-friend";
		}
		
		command << cmdgen.Output( GetPOSIXPathname( options.PrecompiledHeaderImage() ) );
		
		// Add the source file to the command line
		command << cmdgen.Input( pathname_to_precompiled_header_source );
		
		if ( gnu )
		{
			command << "> " << q( DiagnosticsFilePathname( options.Output(), filename ) ) << " 2>&1";
		}
		
		QueueCommand( "echo Precompiling:  " + filename );
		QueueCommand( command );
	}
	
	static FSSpec PrecompiledHeaderImageFile( const ProjName&    projName,
	                                          std::string        pchSourceName,
	                                          const TargetInfo&  targetInfo )
	{
		N::FSDirSpec folder = ProjectPrecompiledFolder( projName, MakeTargetName( targetInfo ) );
		
		bool gnu = targetInfo.toolkit == toolkitGNU;
		
		std::string pchImageName = pchSourceName + (gnu ? ".gch"
		                                                : ".mwch");
		
		return folder / pchImageName;
	}
	
	static bool ProjectHasPrecompiledHeader( const ProjName& proj )
	{
		IncludePath pchSource = GetProject( proj ).PrecompiledHeaderSource();
		return !pchSource.empty();
	}
	
	void CompileSources( const Project& project, TargetInfo targetInfo )
	{
		CompilerOptions options( targetInfo );
		
		time_t pchImageDate = 0;
		bool needToPrecompile = false;
		IncludePath pchSourcePath = project.PrecompiledHeaderSource();
		bool thisProjectProvidesPrecompiledHeader = !pchSourcePath.empty();
		std::string pchSource;
		FSSpec pchImage;
		
		if ( thisProjectProvidesPrecompiledHeader )
		{
			// Locate the file and return the latest modification date of any file referenced
			time_t pchSourceDate = RecursivelyLatestDate( pchSourcePath );
			
			// Locate the precompiled header image file.
			pchSource = IncludeLocation( pchSourcePath );
			std::string pchSourceName = io::get_filename_string( pchSource );
			
			pchImage = PrecompiledHeaderImageFile( project.Name(),
			                                       pchSourceName,
			                                       targetInfo );
			
			options.SetPrecompiledHeaderImage( pchImage );
			
			needToPrecompile = true;
			
			// If the image doesn't exist, use a date that will always be newer
			// (for comparison to source files)
			pchImageDate = 0xFFFFFFFF;
			
			if ( io::file_exists( pchImage ) )
			{
				time_t pchImageModifiedDate = ModifiedDate( pchImage );
				
				if ( pchImageModifiedDate > pchSourceDate )
				{
					needToPrecompile = false;
					pchImageDate = pchImageModifiedDate;
				}
			}
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
				std::string pchSourceName = io::get_filename_string( pchSource );
				
				pchImage = PrecompiledHeaderImageFile( *found,
				                                       pchSourceName,
				                                       targetInfo );
				
				pchImageDate = io::file_exists( pchImage ) ? ModifiedDate( pchImage )
				                                           : 0xFFFFFFFF;
				
				options.SetPrecompiledHeaderImage( pchImage );
			}
		}
		
		N::FSDirSpec outDir = ProjectObjectsFolder( project.Name(), MakeTargetName( targetInfo ) );
		
		options.SetOutput( outDir );
		
		//SetCurrentSourceDir( false );
		
		std::vector< FSSpec > dirtyFiles;
		
		// See which source files need to be compiled,
		// caching include information in the process.
		std::vector< FSSpec >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			// What we do in this loop:
			//  * We set the current source directory to the parent directory
			//    of the current file if the project requires that information.
			//  * If a file needs to be compiled, we add it to the dirty files list.
			//  * Dependency information may be acquired and cached, in RecursivelyLatestDate().
			
			// The source file
			const FSSpec& sourceFile( *it );
			
			if ( project.NeedsCwdSourceOption() )
			{
				//SetCurrentSourceDir( io::get_preceding_directory( sourceFile ) );
			}
			
			// The file's name
			std::string sourceName = io::get_filename_string( sourceFile );
			
			// The object file for this source file, which may or may not exist yet.
			FSSpec objectFile = outDir / ObjectFileName( sourceName );
			
			// If object file is nonexistent or older than source,
			// then it must be compiled, and we can skip the includes.
			// Otherwise, compare against the latest include date.
			
			bool needToCompile = true;
			
			// If the object file doesn't exist, we definitely need to compile.
			// But if it does...
			if ( io::item_exists( objectFile ) )
			{
				// The effective modification date of the file, considering only
				// a precompiled header (if available).  If the precompiled header
				// has been modified, it saves us extracting the includes.
				// Premature optimization?  Maybe.
				time_t sourceDate = std::max( pchImageDate, ModifiedDate( sourceFile ) );
				
				time_t objectDate = ModifiedDate( objectFile );
				
				// If the object file is more recent than the source,
				// (considering first the actual mod date and then the effectove mod date),
				// then it's up to date.
				
				if (    objectDate > sourceDate
				     && objectDate > RecursivelyLatestDate( sourceName, GetPOSIXPathname( sourceFile ) ) )
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
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options.UserOnlyIncludeDirs() );
		
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
		
		if ( !pchSourcePath.empty() )
		{
			options.SetPrecompiledHeaderSource( pchSource );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			std::vector< N::FSDirSpec >& userOnlyIncludeDirs = options.UserOnlyIncludeDirs();
			
			userOnlyIncludeDirs.insert( userOnlyIncludeDirs.begin(),
			                            io::get_preceding_directory( pchImage ) );
		}
		
		std::for_each( dirtyFiles.begin(),
		               dirtyFiles.end(),
		               std::bind1st( N::PtrFun( BuildSourceFile ),
		                             options ) );
	}
	
}

