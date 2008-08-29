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
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/Task.hh"


namespace ALine
{
	
	using namespace io::path_descent_operators;
	
	
	class CompilingTask : public FileTask
	{
		private:
			CompilerOptions  itsOptions;
			std::string      itsSourcePathname;
			const char*      itsCaption;
		
		public:
			CompilingTask( const CompilerOptions&  options,
			               const std::string&      source,
			               const std::string&      output,
			               const char*             caption ) : FileTask         ( output ),
			                                                   itsOptions       ( options ),
			                                                   itsSourcePathname( source  ),
			                                                   itsCaption       ( caption )
			{
			}
			
			bool UpToDate();
			
			void Make();
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
		// For our purposes here, ".c" is not a C file.
		
		return name.size() > 2  &&  std::equal( name.end() - 2,
		                                        name.end(),
		                                        ".c" );
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
		
		if ( options.Target().toolchain & toolchainGNU )
		{
			compile.push_back( "-Wno-deprecated-declarations" );  // since we're using legacy API's
			compile.push_back( "-Wno-long-double"             );
			
			bool cplusplus = !IsCFile( source_pathname );
			
			if ( cplusplus )
			{
				// We don't need this warning for C, and in fact GNU C complains about it
				compile.push_back( "-Wno-non-template-friend" );
			}
			
			compile.push_back( "-fvisibility=hidden" );
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
		
		RunCommand( command, diagnosticsFile.c_str(), caption + source_filename );
	}
	
	bool CompilingTask::UpToDate()
	{
		if ( io::item_exists( OutputPathname() ) )
		{
			time_t output_stamp = ModifiedDate( OutputPathname() );
			
			UpdateInputStamp( ModifiedDate( itsSourcePathname ) );
			
			if ( MoreRecent( output_stamp ) )
			{
				std::string source_filename = io::get_filename_string( itsSourcePathname );
				
				UpdateInputStamp( RecursivelyLatestDate( source_filename, itsSourcePathname ) );
				
				if ( MoreRecent( output_stamp ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	void CompilingTask::Make()
	{
		RunCompiler( itsOptions, itsSourcePathname, OutputPathname(), itsCaption );
	}
	
	
	static std::string PrecompiledHeaderImageFile( const ProjName&    projName,
	                                               std::string        pchSourceName,
	                                               const TargetInfo&  targetInfo )
	{
		std::string folder = ProjectPrecompiledDirPath( projName );
		
		const bool gnu = targetInfo.toolchain == toolchainGNU;
		
		std::string pchImageName = pchSourceName + (gnu ? ".gch"
		                                                : ".mwch");
		
		return folder / pchImageName;
	}
	
	static bool ProjectHasPrecompiledHeader( const ProjName& proj )
	{
		return GetProject( proj ).HasPrecompiledHeader();
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
		else if ( targetInfo.platform & CD::apiMacBlue )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_API_MAC_CARBON", 0 );
			options.DEFINE_MACRO( "TARGET_API_MAC_OS8" );
			
			if ( targetInfo.platform & CD::archPPC )
			{
				options.DEFINE_MACRO( "ACCESSOR_CALLS_ARE_FUNCTIONS" );
				options.DEFINE_MACRO( "OPAQUE_UPP_TYPES" );
			}
		}
		
		if ( options.Target().toolchain == toolchainMetrowerks )
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
	
	
	static const Project* ProjectProvidingPrecompiledHeader( const Project& project )
	{
		const Project* result = NULL;
		
		if ( project.HasPrecompiledHeader() )
		{
			result = &project;
		}
		else
		{
			// This project doesn't have a precompiled header, but maybe a used one does
			typedef std::vector< ProjName >::const_iterator const_iterator;
			
			const_iterator it = std::find_if( project.AllUsedProjects().begin(),
			                                  project.AllUsedProjects().end(),
			                                  std::ptr_fun( ProjectHasPrecompiledHeader ) );
			
			if ( it != project.AllUsedProjects().end() )
			{
				result = &GetProject( *it );
			}
		}
		
		return result;
	}
	
	class ToolTaskMaker
	{
		private:
			const CompilerOptions&  itsOptions;
			const TaskPtr&          itsPrecompileTask;
		
		public:
			ToolTaskMaker( const CompilerOptions&  options,
			               const TaskPtr&          precompile ) : itsOptions       ( options    ),
			                                                      itsPrecompileTask( precompile )
			{
			}
			
			TaskPtr operator()( const std::string& source_pathname, const std::string& object_pathname )
			{
				const char* caption = "Compiling: ";
				
				CompilerOptions source_options = itsOptions;
				
				source_options.AppendIncludeDir( io::get_preceding_directory( source_pathname ) );
				
				TaskPtr task( new CompilingTask( source_options, source_pathname, object_pathname, caption ) );
				
				itsPrecompileTask->AddDependent( task );
				
				return task;
			}
	};
	
	void CompileSources( const Project&           project,
	                     const TargetInfo&        targetInfo,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies )
	{
		CompilerOptions options( project.Name(), targetInfo );
		
		DefineMacros( options, targetInfo );
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options );
		
		const std::vector< ProjName >& allUsedProjects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( allUsedProjects.rbegin(),
		               allUsedProjects.rend(),
		               gatherer );
		
		CompilerOptions precompile_options = options;
		
		
		TaskPtr precompile_task( new NullTask() );
		
		// In case we have a toolkit with no common sources?
		precompile_task->AddDependent( source_dependency );
		
		const Project* projectProvidingPrecompiledHeader = ProjectProvidingPrecompiledHeader( project );
		
		if ( projectProvidingPrecompiledHeader != NULL )
		{
			std::string pchSourcePath = projectProvidingPrecompiledHeader->PrecompiledHeaderSource();
			
			std::string pchSourceName = io::get_filename_string( pchSourcePath );
			
			std::string pchImage = PrecompiledHeaderImageFile( projectProvidingPrecompiledHeader->Name(),
			                                                   pchSourceName,
			                                                   targetInfo );
			
			options.SetPrecompiledHeaderSource( pchSourceName );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			options.PrependIncludeDir( io::get_preceding_directory( pchImage ) );
			
			if ( project.HasPrecompiledHeader() )
			{
				// Locate the precompiled header image file.
				std::string pchSource = FindInclude( pchSourcePath );
				
				if ( pchSource.empty() )
				{
					std::fprintf( stderr, "Missing precompiled header '%s'\n", pchSourcePath.c_str() );
				}
				
				precompile_task.reset( new CompilingTask( precompile_options, pchSource, pchImage, "Precompiling: " ) );
			}
			else
			{
				precompile_task->UpdateInputStamp( ModifiedDate( pchImage ) );
			}
		}
		
		
		if ( Options().all )
		{
			precompile_task->UpdateInputStamp( 0x7FFFFFFF );
		}
		
		std::string outDir = ProjectObjectsDirPath( project.Name() );
		
		std::vector< std::string > source_paths;
		
		std::vector< std::string > objectFiles;
		
		const std::size_t n_tools = NameObjectFiles( project, source_paths, objectFiles );
		
		const std::vector< std::string >& sources = n_tools > 0 ? source_paths : project.Sources();
		
		tool_dependencies.resize( n_tools );
		
		std::transform( sources.begin(),
		                sources.begin() + n_tools,
		                objectFiles.begin(),
		                tool_dependencies.begin(),
		                ToolTaskMaker( options, precompile_task ) );
		
		std::vector< std::string >::const_iterator the_source, the_object, end = sources.end();
		
		for ( the_source = sources.begin() + n_tools,
		      the_object = objectFiles.begin();  the_source != end;  ++the_source,
		                                                             ++the_object )
		{
			// The source file
			const std::string& source_pathname = *the_source;
			
			const std::string& output_pathname = *the_object;
			
			CompilerOptions source_options = options;
			
			source_options.AppendIncludeDir( io::get_preceding_directory( source_pathname ) );
			
			const char* caption = "Compiling: ";
			
			TaskPtr task( new CompilingTask( source_options, source_pathname, output_pathname, caption ) );
			
			precompile_task->AddDependent( task );
			
			task->AddDependent( source_dependency );
		}
		
		AddReadyTask( precompile_task );
	}
	
}

