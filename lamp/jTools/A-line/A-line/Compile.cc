/*	==========
 *	Compile.cc
 *	==========
 */

#include "A-line/Compile.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <vector>

// Standard C
#include <stdlib.h>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Io
#include "io/io.hh"
#include "io/files.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// Io
#include "Io/TextInput.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/Includes.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/Task.hh"


namespace ALine
{
	
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	class CompilingTask : public FileTask
	{
		private:
			const Project&   its_project;
			CompilerOptions  its_options;
			std::string      its_source_pathname;
			const char*      its_caption;
		
		public:
			CompilingTask( const Project&          project,
			               const CompilerOptions&  options,
			               const std::string&      source,
			               const std::string&      output,
			               const char*             caption ) : FileTask           ( output ),
			                                                   its_project        ( project ),
			                                                   its_options        ( options ),
			                                                   its_source_pathname( source  ),
			                                                   its_caption        ( caption )
			{
			}
			
			bool UpToDate();
			
			void Make();
	};
	
	class IncludeDirGatherer
	{
		private:
			CompilerOptions& its_options;
		
		public:
			IncludeDirGatherer( CompilerOptions& options ) : its_options( options )
			{
			}
			
			void operator()( const std::string& project_name );
	};
	
	void IncludeDirGatherer::operator()( const std::string& project_name )
	{
		const Project& project = GetProject( project_name, its_options.Target().platform );
		
		if ( project.Product() == productNotBuilt )
		{
			return;
		}
		
		const std::vector< std::string >& search_dirs( project.SearchDirs() );
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = search_dirs.begin();  it != search_dirs.end();  ++it )
		{
			its_options.AppendIncludeDir( *it );
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
			p7::mkdir( folder, 0700 );
		}
		
		return folder;
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		return filename + ".txt";
	}
	
	static std::string DiagnosticsPathnameFromSourceFilename( const std::string&  project_name,
	                                                          const std::string&  source_filename )
	{
		std::string diagnostics_dir = ProjectDiagnosticsDirPath( project_name );
		
		return diagnostics_dir / DiagnosticsFilenameFromSourceFilename( source_filename );
	}
	
	static Command MakeCompileCommand( const CompilerOptions&  options,
	                                   const std::string&      source_pathname,
	                                   const std::string&      output_pathname )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command compile = cmdgen.CompilerName();
		
		AugmentCommand( compile, cmdgen.AllCompilerOptions() );
		
		if ( !ALINE_LAMP_DEVELOPMENT  &&  options.Target().toolchain & toolchainGNU )
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
	                         const std::string&      output_path,
	                         const char*             caption )
	{
		Command command = MakeCompileCommand( options, source_pathname, output_path );
		
		std::string source_filename = io::get_filename_string( source_pathname );
		
		std::string diagnostics_pathname = DiagnosticsPathnameFromSourceFilename( options.Name(), source_filename );
		
		RunCommand( command, diagnostics_pathname.c_str(), caption + source_filename );
	}
	
	static void get_recursive_includes( const Project&            project,
	                                    const std::string&        source_pathname,
	                                    std::set< std::string >&  result )
	{
		const std::vector< std::string >& includes = GetIncludes( source_pathname ).user;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			const std::string& include_path = *it;
			
			if ( result.find( include_path ) == result.end() )
			{
				std::string pathname = project.FindIncludeRecursively( include_path );
				
				if ( !pathname.empty() )
				{
					result.insert( pathname );
					
					get_recursive_includes( project, pathname, result );
				}
			}
		}
	}
	
	static void write_dependencies_file( p7::fd_t output, const std::set< std::string >& includes )
	{
		typedef std::set< std::string >::const_iterator Iter;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			std::string line = *it + '\n';
			
			p7::write( output, line.data(), line.length() );
		}
	}
	
	static void read_dependencies_file( p7::fd_t input_fd, std::set< std::string >& includes )
	{
		Io::TextInputAdapter< p7::fd_t > input( input_fd );
		
		while ( input.Ready() )
		{
			std::string include_path = input.Read();
			
			includes.insert( include_path );
		}
	}
	
	static time_t get_memoized_timestamp( const std::string& pathname )
	{
		static std::map< std::string, time_t > map;
		
		time_t& value = map[ pathname ];
		
		if ( value == 0 )
		{
			value = ModifiedDate( pathname );
		}
		
		return value;
	}
	
	template < class Iter >
	static time_t get_collective_timestamp( Iter begin, Iter end )
	{
		time_t result = 0;
		
		while ( begin != end )
		{
			const std::string& pathname = *begin++;
			
			if ( !io::file_exists( pathname ) )
			{
				// A missing include file means the .d file is out of date
				return 0x7fffffff;
			}
			
			time_t stamp = get_memoized_timestamp( pathname );
			
			if ( stamp > result )
			{
				result = stamp;
			}
		}
		
		return result;
	}
	
	bool CompilingTask::UpToDate()
	{
		if ( io::item_exists( OutputPath() ) )
		{
			time_t output_stamp = ModifiedDate( OutputPath() );
			
			// Memoize this once we have multi-platform builds
			UpdateInputStamp( ModifiedDate( its_source_pathname ) );
			
			if ( MoreRecent( output_stamp ) )
			{
				std::string source_filename = io::get_filename_string( its_source_pathname );
				
				std::string dependencies_dir = get_project_dependencies_pathname( its_project.Name() );
				
				std::string dependencies_pathname = dependencies_dir / source_filename + ".d";
				
				bool has_dot_d = io::file_exists( dependencies_pathname );
				
				time_t includes_stamp;
				
				std::set< std::string > includes;
				
				if ( has_dot_d )
				{
					read_dependencies_file( p7::open( dependencies_pathname, p7::o_rdonly ), includes );
					
					includes_stamp = get_collective_timestamp( includes.begin(), includes.end() );
					
					time_t dependencies_stamp = ModifiedDate( dependencies_pathname );
					
					if ( includes_stamp >= dependencies_stamp )
					{
						// .d file is out of date
						includes.clear();
						
						has_dot_d = false;
					}
				}
				
				if ( !has_dot_d )
				{
					get_recursive_includes( its_project, its_source_pathname, includes );
					
					includes_stamp = get_collective_timestamp( includes.begin(), includes.end() );
					
					// Write .d file for next time
					write_dependencies_file( p7::open( dependencies_pathname, p7::o_wronly | p7::o_creat, 0644 ), includes );
				}
				
				UpdateInputStamp( includes_stamp );
				
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
		RunCompiler( its_options, its_source_pathname, OutputPath(), its_caption );
	}
	
	
	static std::string PrecompiledHeaderImageFile( const std::string&  project_name,
	                                               std::string         precompiled_header_source_filename,
	                                               const TargetInfo&   target_info )
	{
		std::string precompiled_header_dir_pathname = ProjectPrecompiledDirPath( project_name );
		
		const bool gnu = target_info.toolchain == toolchainGNU;
		
		const char* extension = (gnu ? ".gch" : ".mwch");
		
		std::string precompiled_header_image_filename = precompiled_header_source_filename + extension;
		
		return precompiled_header_dir_pathname / precompiled_header_image_filename;
	}
	
	static bool ProjectHasPrecompiledHeader( const std::string& project_name, Platform platform )
	{
		return GetProject( project_name, platform ).HasPrecompiledHeader();
	}
	
	#define DEFINE_MACRO_VALUE( macro, value )  AddDefinedMacro( "-D" macro "=" #value )
	
	#define DEFINE_MACRO( macro )  DEFINE_MACRO_VALUE( macro, 1 )
	
	static void DefineMacros( CompilerOptions& options, const TargetInfo& target_info )
	{
		options.DEFINE_MACRO( "__ALINE__" );
		options.DEFINE_MACRO( "JOSHUA_JURAN_EXPERIMENTAL" );
		
		options.DEFINE_MACRO( "NUCLEUS_USES_BACKTRACE" );
		
		if ( target_info.platform & CD::apiMacCarbon )
		{
			options.DEFINE_MACRO( "TARGET_API_MAC_CARBON" );
		}
		else if ( target_info.platform & CD::apiMacBlue )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_API_MAC_CARBON", 0 );
			options.DEFINE_MACRO( "TARGET_API_MAC_OS8" );
			
			if ( target_info.platform & CD::archPPC )
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
		
		if ( target_info.build == buildDebug )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_CONFIG_DEBUGGING", 1 );
		}
		else
		{
			options.DEFINE_MACRO_VALUE( "TARGET_CONFIG_DEBUGGING", 0 );
		}
		
		if ( target_info.build == buildDemo )
		{
			options.DEFINE_MACRO( "BUILD_DEMO" );
		}
	}
	
	
	static const Project* ProjectProvidingPrecompiledHeader( const Project& project, Platform platform )
	{
		const Project* result = NULL;
		
		if ( project.HasPrecompiledHeader() )
		{
			result = &project;
		}
		else
		{
			// This project doesn't have a precompiled header, but maybe a used one does
			typedef std::vector< std::string >::const_iterator const_iterator;
			
			const_iterator it = std::find_if( project.AllUsedProjects().begin(),
			                                  project.AllUsedProjects().end(),
			                                  std::bind2nd( more::ptr_fun( ProjectHasPrecompiledHeader ),
			                                                platform ) );
			
			if ( it != project.AllUsedProjects().end() )
			{
				result = &GetProject( *it, platform );
			}
		}
		
		return result;
	}
	
	class ToolTaskMaker
	{
		private:
			const Project&          its_project;
			const CompilerOptions&  its_options;
			const TaskPtr&          its_precompile_task;
		
		public:
			ToolTaskMaker( const Project&          project,
			               const CompilerOptions&  options,
			               const TaskPtr&          precompile ) : its_project        ( project    ),
			                                                      its_options        ( options    ),
			                                                      its_precompile_task( precompile )
			{
			}
			
			TaskPtr operator()( const std::string& source_pathname, const std::string& object_pathname )
			{
				const char* caption = "Compiling: ";
				
				CompilerOptions source_options = its_options;
				
				source_options.AppendIncludeDir( io::get_preceding_directory( source_pathname ) );
				
				TaskPtr task( new CompilingTask( its_project,
				                                 source_options,
				                                 source_pathname,
				                                 object_pathname,
				                                 caption ) );
				
				its_precompile_task->AddDependent( task );
				
				return task;
			}
	};
	
	void CompileSources( Project&                 project,
	                     const TargetInfo&        target_info,
	                     const TaskPtr&           project_base_task,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies )
	{
		CompilerOptions options( project.Name(), target_info );
		
		DefineMacros( options, target_info );
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options );
		
		const std::vector< std::string >& all_used_projects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( all_used_projects.rbegin(),
		               all_used_projects.rend(),
		               gatherer );
		
		CompilerOptions precompile_options = options;
		
		
		TaskPtr precompile_task( new NullTask() );
		
		// In case we have a toolkit with no common sources?
		precompile_task->AddDependent( source_dependency );
		
		const Project* project_providing_precompiled_header = ProjectProvidingPrecompiledHeader( project, target_info.platform );
		
		if ( project_providing_precompiled_header != NULL )
		{
			std::string precompiled_header_source_path = project_providing_precompiled_header->PrecompiledHeaderSource();
			
			std::string precompiled_header_source_filename = io::get_filename_string( precompiled_header_source_path );
			
			std::string pchImage = PrecompiledHeaderImageFile( project_providing_precompiled_header->Name(),
			                                                   precompiled_header_source_filename,
			                                                   target_info );
			
			options.SetPrecompiledHeaderSource( precompiled_header_source_filename );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			options.PrependIncludeDir( io::get_preceding_directory( pchImage ) );
			
			if ( project.HasPrecompiledHeader() )
			{
				// Locate the precompiled header image file.
				std::string precompiled_header_source_pathname = project.FindIncludeRecursively( precompiled_header_source_path );
				
				if ( precompiled_header_source_pathname.empty() )
				{
					std::fprintf( stderr, "Missing precompiled header '%s'\n", precompiled_header_source_path.c_str() );
				}
				
				precompile_task.reset( new CompilingTask( project,
				                                          precompile_options,
				                                          precompiled_header_source_pathname,
				                                          pchImage,
				                                          "Precompiling: " ) );
				
				project.set_precompile_task( precompile_task );
			}
			else
			{
				ASSERT( !project_providing_precompiled_header->get_precompile_task().expired() );
				
				precompile_task = project_providing_precompiled_header->get_precompile_task().lock();
			}
		}
		
		
		if ( Options().all )
		{
			precompile_task->UpdateInputStamp( 0x7FFFFFFF );
		}
		
		std::string outDir = ProjectObjectsDirPath( project.Name() );
		
		std::vector< std::string > source_paths;
		
		std::vector< std::string > object_paths;
		
		const std::size_t n_tools = NameObjectFiles( project, source_paths, object_paths );
		
		const std::vector< std::string >& sources = n_tools > 0 ? source_paths : project.Sources();
		
		tool_dependencies.resize( n_tools );
		
		std::transform( sources.begin(),
		                sources.begin() + n_tools,
		                object_paths.begin(),
		                tool_dependencies.begin(),
		                ToolTaskMaker( project, options, precompile_task ) );
		
		std::vector< std::string >::const_iterator the_source, the_object, end = sources.end();
		
		for ( the_source = sources.begin() + n_tools,
		      the_object = object_paths.begin();  the_source != end;  ++the_source,
		                                                              ++the_object )
		{
			// The source file
			const std::string& source_pathname = *the_source;
			
			const std::string& output_path = *the_object;
			
			CompilerOptions source_options = options;
			
			source_options.AppendIncludeDir( io::get_preceding_directory( source_pathname ) );
			
			const char* caption = "Compiling: ";
			
			TaskPtr task( new CompilingTask( project, source_options, source_pathname, output_path, caption ) );
			
			precompile_task->AddDependent( task );
			
			task->AddDependent( source_dependency );
		}
		
		project_base_task->AddDependent( precompile_task );
	}
	
}

