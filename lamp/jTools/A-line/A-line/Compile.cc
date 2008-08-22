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

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/Task.hh"


namespace ALine
{
	
	using namespace io::path_descent_operators;
	
	
	class CompilingTask : public Task
	{
		private:
			CompilerOptions  itsOptions;
			std::string      itsSourcePathname;
			std::string      itsOutputPathname;
			const char*      itsCaption;
		
		public:
			CompilingTask( const CompilerOptions&  options,
			               const std::string&      source,
			               const std::string&      output,
			               const char*             caption ) : itsOptions       ( options ),
			                                                   itsSourcePathname( source  ),
			                                                   itsOutputPathname( output  ),
			                                                   itsCaption       ( caption )
			{
			}
			
			void Main();
	};
	
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
	
	static Command MakeCompileCommand( const CompilerOptions&  options,
	                                   const std::string&      source_pathname,
	                                   const std::string&      output_pathname )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command compile = cmdgen.CompilerName();
		
		AugmentCommand( compile, cmdgen.AllCompilerOptions() );
		
		compile.push_back( "-Wno-deprecated-declarations" );  // since we're using legacy API's
		compile.push_back( "-Wno-long-double"             );
		
		bool cplusplus = !IsCFile( source_pathname );
		
		if ( cplusplus )
		{
			// We don't need this warning for C, and in fact GNU C complains about it
			compile.push_back( "-Wno-non-template-friend" );
		}
		
		AugmentCommand( compile, options.GetMacros() );
		
		AugmentCommand( compile, options.IncludeDirOptions() );
		
		// False while precompiling
		if ( options.HasPrecompiledHeaderSource() )
		{
			// Specify by name only, so gcc will search for the .gch image.
			const std::string& pchSourceName = options.PrecompiledHeaderSource();
			
			compile.push_back( "-include"            );
			compile.push_back( pchSourceName.c_str() );
		}
		
		AugmentCommand( compile, OutputOption( output_pathname.c_str() ) );
		
		// Add the source file to the command line
		compile.push_back( source_pathname.c_str() );
		
		compile.push_back( NULL );
		
		return compile;
	}
	
	static void RunCompiler( const CompilerOptions&  options,
	                         const std::string&      source_pathname,
	                         const std::string&      output_pathname,
	                         const char*             caption )
	{
		Command command = MakeCompileCommand( options, source_pathname, output_pathname );
		
		std::string source_filename = io::get_filename_string( source_pathname );
		
		std::string diagnosticsFile = DiagnosticsFilePathname( options.Name(), source_filename );
		
		TaskPtr task( new CommandTask( command, diagnosticsFile, caption + source_filename ) );
		
		task->Main();
	}
	
	void CompilingTask::Main()
	{
		RunCompiler( itsOptions, itsSourcePathname, itsOutputPathname, itsCaption );
	}
	
	static void CompileSource( const CompilerOptions& options, const std::string& source_pathname )
	{
		std::string source_filename = io::get_filename_string( source_pathname );
		
		
		std::string output_pathname = options.Output() / ObjectFileName( source_filename );
		
		const char* caption = "Compiling: ";
		
		
		TaskPtr task( new CompilingTask( options, source_pathname, output_pathname, caption ) );
		
		task->Main();
	}
	
	static void Compile( CompilerOptions options, const std::string& source_pathname )
	{
		options.AppendIncludeDir( io::get_preceding_directory( source_pathname ) );
		
		CompileSource( options, source_pathname );
	}
	
	
	static void Precompile( const CompilerOptions&  options,
	                        const std::string&      source_pathname )
	{
		const std::string& output_pathname = options.PrecompiledHeaderImage();
		
		const char* caption = "Precompiling: ";
		
		
		TaskPtr task( new CompilingTask( options, source_pathname, output_pathname, caption ) );
		
		task->Main();
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
	
	static void DefineMacros( CompilerOptions& options, const TargetInfo& targetInfo )
	{
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
	}
	
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
		
		DefineMacros( options, targetInfo );
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options );
		
		const std::vector< ProjName >& allUsedProjects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( allUsedProjects.rbegin(),
		               allUsedProjects.rend(),
		               gatherer );
		
		if ( needToPrecompile )
		{
			Precompile( options, pchSource );
		}
		
		if ( !pchSourcePath.empty() )
		{
			options.SetPrecompiledHeaderSource( io::get_filename_string( pchSourcePath ) );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			options.PrependIncludeDir( io::get_preceding_directory( pchImage ) );
		}
		
		std::for_each( dirtyFiles.begin(),
		               dirtyFiles.end(),
		               std::bind1st( more::ptr_fun( Compile ),
		                             options ) );
	}
	
}

