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

// MoreFunctional
#include "PointerToFunction.hh"

// BitsAndBytes
#include "StringFilters.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"


namespace ALine
{
	
	using namespace io::path_descent_operators;
	
	using BitsAndBytes::q;
	
	
	class IncludeDirGatherer
	{
		private:
			CompilerOptions& itsOptions;
		
		public:
			IncludeDirGatherer( CompilerOptions& options ) : itsOptions( options )  {}
				
			void operator()( const ProjName& projName );
	};
	
	void IncludeDirGatherer::operator()( const ProjName& projName )
	{
		const Project& project = GetProject( projName );
		
		if ( project.Product() == productNotBuilt )
		{
			return;
		}
		
		const std::vector< std::string >& searchDirs( project.SearchDirs() );
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = searchDirs.begin();  it != searchDirs.end();  ++it )
		{
			itsOptions.AppendIncludeDir( *it );
		}
	}
	
	
	static bool IsCFile( const std::string& name )
	{
		return name.substr( name.size() - 2, name.size() ) == ".c";
	}
	
	static std::string CreateFolder( const std::string& folder )
	{
		if ( !io::item_exists( folder ) )
		{
			int made = mkdir( folder.c_str(), 0700 );
		}
		
		return folder;
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		return filename + ".txt";
	}
	
	static std::string DiagnosticsFilePathname( const std::string&  proj,
	                                            const std::string&  filename )
	{
		std::string diagnosticsDir = ProjectDiagnosticsDirPath( proj );
		
		std::string diagnosticsFile = diagnosticsDir / DiagnosticsFilenameFromSourceFilename( filename );
		
		return diagnosticsFile;
	}
	
	static Command MakeCompileCommand( const CompilerOptions& options )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command compile = cmdgen.CompilerName();
		
		AugmentCommand( compile, cmdgen.AllCompilerOptions() );
		
		if ( options.Target().platform & CD::runtimeMachO )
		{
			compile.push_back( "-Wno-deprecated-declarations" );  // since we're using legacy API's
			compile.push_back( "-Wno-long-double"             );
		}
		
		AugmentCommand( compile, options.GetMacros() );
		
		AugmentCommand( compile, options.IncludeDirOptions() );
		
		return compile;
	}
	
	static void Echo( const char* a, const char* b )
	{
		Command echo;
		
		echo.push_back( "/bin/echo" );
		echo.push_back( a );
		echo.push_back( b );
		echo.push_back( NULL );
		
		ExecuteCommand( echo );
	}
	
	static void BuildSourceFile( CompilerOptions options, const std::string& file )
	{
		options.AppendIncludeDir( io::get_preceding_directory( file ) );
		
		CommandGenerator cmdgen( options.Target() );
		
		Command command = MakeCompileCommand( options );
		
		std::string filename = io::get_filename_string( file );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command.push_back( "-Wno-non-template-friend" );
		}
		
		std::string pchSourceName;
		
		if ( options.HasPrecompiledHeaderSource() )
		{
			// Specify by name only, so gcc will search for the .gch image.
			pchSourceName = io::get_filename_string( options.PrecompiledHeaderSource() );
			
			AugmentCommand( command, cmdgen.Prefix( pchSourceName.c_str() ) );
		}
		
		std::string outputFile = options.Output() / ObjectFileName( filename );
		
		AugmentCommand( command, OutputOption( outputFile.c_str() ) );
		
		command.push_back( file.c_str() );
		
		std::string diagnosticsFile = DiagnosticsFilePathname( options.Name(), filename );
		
		Echo( "Compiling:", filename.c_str() );
		
		command.push_back( NULL );
		
		ExecuteCommand( command, diagnosticsFile.c_str() );
	}
	
	static void Precompile( const CompilerOptions&  options,
	                        const std::string&      pathname_to_precompiled_header_source )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command command = MakeCompileCommand( options );
		
		std::string filename = io::get_filename_string( pathname_to_precompiled_header_source );
		
		if ( !IsCFile( filename ) )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			command.push_back( "-Wno-non-template-friend" );
		}
		
		AugmentCommand( command, OutputOption( options.PrecompiledHeaderImage().c_str() ) );
		
		// Add the source file to the command line
		command.push_back( pathname_to_precompiled_header_source.c_str() );
		
		std::string diagnosticsFile = DiagnosticsFilePathname( options.Name(), filename );
		
		Echo( "Precompiling:", filename.c_str() );
		
		command.push_back( NULL );
		
		ExecuteCommand( command, diagnosticsFile.c_str() );
	}
	
	static std::string PrecompiledHeaderImageFile( const ProjName&    projName,
	                                               std::string        pchSourceName,
	                                               const TargetInfo&  targetInfo )
	{
		std::string folder = ProjectPrecompiledDirPath( projName );
		
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
	
	#define DEFINE_MACRO_VALUE( macro, value )  AddDefinedMacro( "-D" macro "=" #value )
	
	#define DEFINE_MACRO( macro )  DEFINE_MACRO_VALUE( macro, 1 )
	
	void CompileSources( const Project& project, TargetInfo targetInfo )
	{
		CompilerOptions options( project.Name(), targetInfo );
		
		time_t pchImageDate = 0;
		bool needToPrecompile = false;
		IncludePath pchSourcePath = project.PrecompiledHeaderSource();
		bool thisProjectProvidesPrecompiledHeader = !pchSourcePath.empty();
		std::string pchSource;
		std::string pchImage;
		
		if ( thisProjectProvidesPrecompiledHeader )
		{
			// Locate the precompiled header image file.
			pchSource = FindInclude( pchSourcePath );
			
			if ( pchSource.empty() )
			{
				std::fprintf( stderr, "Missing precompiled header '%s'\n", pchSourcePath.c_str() );
			}
			
			// Locate the file and return the latest modification date of any file referenced
			time_t pchSourceDate = RecursivelyLatestDate( pchSourcePath, pchSource );
			
			std::string pchSourceName = io::get_filename_string( pchSourcePath );
			
			pchImage = PrecompiledHeaderImageFile( project.Name(),
			                                       pchSourceName,
			                                       targetInfo );
			
			options.SetPrecompiledHeaderImage( pchImage );
			
			needToPrecompile = true;
			
			// If the image doesn't exist, use a date that will always be newer
			// (for comparison to source files)
			pchImageDate = 0xFFFFFFFFU;
			
			if ( !Options().all && io::file_exists( pchImage ) )
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
			                                     std::ptr_fun( ProjectHasPrecompiledHeader ) );
			
			if ( found != project.AllUsedProjects().end() )
			{
				pchSourcePath = GetProject( *found ).PrecompiledHeaderSource();
				
				std::string pchSourceName = io::get_filename_string( pchSourcePath );
				
				pchImage = PrecompiledHeaderImageFile( *found,
				                                       pchSourceName,
				                                       targetInfo );
				
				pchImageDate = io::file_exists( pchImage ) ? ModifiedDate( pchImage )
				                                           : 0xFFFFFFFF;
				
				options.SetPrecompiledHeaderImage( pchImage );
			}
		}
		
		std::string outDir = ProjectObjectsDirPath( project.Name() );
		
		options.SetOutput( outDir );
		
		std::vector< std::string > dirtyFiles;
		
		// If we're compiling the precompiled header, then recompile all source
		bool compilingEverything = Options().all  ||  thisProjectProvidesPrecompiledHeader && needToPrecompile;
		
		// See which source files need to be compiled,
		// caching include information in the process.
		std::vector< std::string >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			// What we do in this loop:
			//  * We set the current source directory to the parent directory
			//    of the current file if the project requires that information.
			//  * If a file needs to be compiled, we add it to the dirty files list.
			//  * Dependency information may be acquired and cached, in RecursivelyLatestDate().
			
			// The source file
			std::string sourceFile( *it );
			
			// We need to compile the source file if any of these apply:
			// * we're compiling everything anyway
			// * its object file doesn't exist
			// * the object file is out of date
			
			// Below, we run the negation of each test.
			// Any negative result ends the tests and we proceed to add the file to the list.
			// If all tests pass then the object file is up to date and we skip it.
			
			if ( !compilingEverything )
			{
				// The file's name
				std::string sourceName = io::get_filename_string( sourceFile );
				
				// The object file for this source file, which may or may not exist yet.
				std::string objectFile = outDir / ObjectFileName( sourceName );
				
				if ( io::item_exists( objectFile ) )
				{
					// The effective modification date of the file, considering only
					// a precompiled header (if available).  If the precompiled header
					// has been modified, it saves us extracting the includes.
					// Premature optimization?  Maybe.
					time_t sourceDate = std::max( pchImageDate, ModifiedDate( sourceFile ) );
					
					time_t objectDate = ModifiedDate( objectFile );
					
					// If the object file is more recent than the source,
					// (considering first the actual mod date and then the effective mod date),
					// then it's up to date.
					
					if (    objectDate > sourceDate
					     && objectDate > RecursivelyLatestDate( sourceName, sourceFile ) )
					{
						// Object file is newer than source file and newest header,
						// so it's up to date and we can skip it
						continue;
					}
				}
			}
			
			dirtyFiles.push_back( sourceFile );
		}
		
		if ( !needToPrecompile && dirtyFiles.size() == 0 )  return;
		
		options.DEFINE_MACRO( "__ALINE__" );
		options.DEFINE_MACRO( "JOSHUA_JURAN_EXPERIMENTAL" );
		
		options.DEFINE_MACRO( "NUCLEUS_USES_BACKTRACE" );
		
		if ( targetInfo.platform & CD::apiMacCarbon )
		{
			options.DEFINE_MACRO( "TARGET_API_MAC_CARBON" );
		}
		else if ( targetInfo.platform & CD::apiMacToolbox )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_API_MAC_CARBON", 0 );
			options.DEFINE_MACRO( "TARGET_API_MAC_OS8" );
			
			if ( targetInfo.platform & CD::archPPC )
			{
				options.DEFINE_MACRO( "ACCESSOR_CALLS_ARE_FUNCTIONS" );
				options.DEFINE_MACRO( "OPAQUE_UPP_TYPES" );
			}
		}
		
		if ( options.Target().toolkit == toolkitCodeWarrior )
		{
			// Assume CW Pro 6
			options.DEFINE_MACRO( "NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS" );
		}
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options );
		
		const std::vector< ProjName >& allUsedProjects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( allUsedProjects.rbegin(),
		               allUsedProjects.rend(),
		               gatherer );
		
		if ( targetInfo.build == buildDebug )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_CONFIG_DEBUGGING", 1 );
		}
		else
		{
			options.DEFINE_MACRO_VALUE( "TARGET_CONFIG_DEBUGGING", 0 );
		}
		
		if ( targetInfo.build == buildDemo )
		{
			options.DEFINE_MACRO( "BUILD_DEMO" );
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
			
			options.PrependIncludeDir( io::get_preceding_directory( pchImage ) );
		}
		
		std::for_each( dirtyFiles.begin(),
		               dirtyFiles.end(),
		               std::bind1st( more::ptr_fun( BuildSourceFile ),
		                             options ) );
	}
	
}

