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

// Iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Io
#include "io/io.hh"
#include "io/files.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/Pathnames.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/Includes.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/Task.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	static std::string diagnostics_file_path( const std::string&  dir_path,
	                                          const std::string&  target_path )
	{
		return dir_path / io::get_filename( target_path ) + ".txt";
	}
	
	class CompilingTask : public FileTask
	{
		private:
			const Project&   its_project;
			CompilerOptions  its_options;
			std::string      its_source_pathname;
			std::string      its_diagnostics_file_path;
			const char*      its_caption;
		
		public:
			CompilingTask( const Project&          project,
			               const CompilerOptions&  options,
			               const std::string&      source,
			               const std::string&      output,
			               const std::string&      diagnostics,
			               const char*             caption )
			: FileTask                 ( output ),
			  its_project              ( project ),
			  its_options              ( options ),
			  its_source_pathname      ( source  ),
			  its_diagnostics_file_path( diagnostics_file_path( diagnostics, source ) ),
			  its_caption              ( caption )
			{
				if ( project.SourceDirs().empty() )
				{
					its_options.AppendIncludeDir( io::get_preceding_directory( source ) );
				}
			}
			
			bool UpToDate();
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	class IncludeDirGatherer
	{
		private:
			CompilerOptions& its_options;
			bool&            it_needs_include_union;
		
		public:
			IncludeDirGatherer( CompilerOptions& options, bool& needs_union )
			:
				its_options( options ),
				it_needs_include_union( needs_union )
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
		// For our purposes here, ".c" is not a C file.  (Basename can't be empty.)
		// Any one-character extension passes the test, including 'c', 'h', and 'm'.
		
		return name.size() > 2  &&  *(name.end() - 2) == '.';
	}
	
	static Command MakeCompileCommand( const CompilerOptions&  options,
	                                   const std::string&      source_pathname,
	                                   const std::string&      output_pathname )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command compile = cmdgen.CompilerName();
		
		AugmentCommand( compile, cmdgen.AllCompilerOptions() );
		
		if ( !ALINE_LAMP_DEVELOPMENT  ||  options.Target().toolchain & toolchainGNU )
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
	
	static void get_recursive_includes( const Project&            project,
	                                    const std::string&        source_pathname,
	                                    std::set< std::string >&  result )
	{
		const std::vector< std::string >& includes = GetIncludes( source_pathname ).user;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			const std::string& include_path = *it;
			
			std::string pathname = project.FindIncludeRecursively( include_path );
			
			if ( !pathname.empty() )
			{
				if ( result.find( pathname ) == result.end() )
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
		
		size_t length = 0;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			length += it->length() + 1;
		}
		
		std::string contents;
		
		try
		{
			contents.reserve( length );
		}
		catch ( ... )
		{
		}
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			contents += *it;
			contents += '\n';
		}
		
		p7::write( output, contents );
	}
	
	static void read_dependencies_file( p7::fd_t input_fd, std::set< std::string >& includes )
	{
		text_input::feed feed;
		
		p7::fd_reader reader( input_fd );
		
		while ( const std::string* s = get_line_from_feed( feed, reader ) )
		{
			std::string include_path( s->begin(), s->end() - 1 );
			
			includes.insert( include_path );
		}
	}
	
	static time_t get_memoized_timestamp( const std::string& pathname )
	{
		static std::map< std::string, time_t > map;
		
		time_t& value = map[ pathname ];
		
		if ( value == 0 )
		{
			struct stat include_stat;
			
			const bool exists = p7::stat( pathname, include_stat );
			
			// If an include is missing, ensure the .d gets refreshed by returning max
			value = exists ? include_stat.st_mtime : 0x7fffffff;
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
			
			time_t stamp = get_memoized_timestamp( pathname );
			
			if ( stamp > result )
			{
				result = stamp;
				
				if ( result == 0x7fffffff )
				{
					// The .d file is out of date, and that's that.
					break;
				}
			}
		}
		
		return result;
	}
	
	bool CompilingTask::UpToDate()
	{
		struct stat output_stat;
		
		const bool output_exists = p7::stat( OutputPath(), output_stat );
		
		if ( output_exists )
		{
			// Memoize this once we have multi-platform builds
			const time_t source_stamp = p7::stat( its_source_pathname ).st_mtime;
			
			UpdateInputStamp( source_stamp );
			
			if ( MoreRecent( output_stat.st_mtime ) )
			{
				std::string source_filename = io::get_filename_string( its_source_pathname );
				
				std::string dependencies_dir = get_project_dependencies_pathname( its_project.Name() );
				
				std::string dependencies_pathname = dependencies_dir / source_filename + ".d";
				
				struct stat dependencies_stat;
				
				bool has_dot_d = p7::stat( dependencies_pathname, dependencies_stat )  &&  dependencies_stat.st_mtime > source_stamp;
				
				time_t includes_stamp;
				
				std::set< std::string > includes;
				
				if ( has_dot_d )
				{
					read_dependencies_file( p7::open( dependencies_pathname, p7::o_rdonly ), includes );
					
					includes_stamp = get_collective_timestamp( includes.begin(), includes.end() );
					
					if ( includes_stamp >= dependencies_stat.st_mtime )
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
					write_dependencies_file( p7::open( dependencies_pathname,
					                                   p7::o_wronly | p7::o_creat | p7::o_trunc ),
					                         includes );
				}
				
				UpdateInputStamp( includes_stamp );
				
				if ( MoreRecent( output_stat.st_mtime ) )
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	void CompilingTask::Make()
	{
		Command command = MakeCompileCommand( its_options, its_source_pathname, OutputPath() );
		
		const char* source_path = std::strstr( its_source_pathname.c_str(), "//" );
		
		if ( source_path == NULL )
		{
			source_path = its_source_pathname.c_str();
		}
		else
		{
			source_path += STRLEN( "//" );
		}
		
		std::string full_caption = its_caption;
		
		full_caption += source_path;
		
		ExecuteCommand( shared_from_this(), full_caption, command, its_diagnostics_file_path.c_str() );
	}
	
	void CompilingTask::Return( bool succeeded )
	{
		check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	static std::string get_prefix_image_pathname( const std::string&  project_name,
	                                              std::string         prefix_source_filename,
	                                              const TargetInfo&   target_info )
	{
		std::string prefix_dir_pathname = ProjectPrecompiledDirPath( project_name );
		
		const bool gnu = target_info.toolchain == toolchainGNU;
		
		const char* extension = (gnu ? ".gch" : ".mwch");
		
		std::string prefix_image_filename = prefix_source_filename + extension;
		
		return prefix_dir_pathname / prefix_image_filename;
	}
	
	static bool project_has_prefix( const std::string& project_name, Platform platform )
	{
		return GetProject( project_name, platform ).HasPrecompiledHeader();
	}
	
	#define DEFINE_MACRO_VALUE( macro, value )  AddDefinedMacro( "-D" macro "=" #value )
	
	#define DEFINE_MACRO( macro )  DEFINE_MACRO_VALUE( macro, 1 )
	
	static void DefineMacros( CompilerOptions& options, const TargetInfo& target_info )
	{
		options.DEFINE_MACRO( "__ALINE__" );
		
		options.DEFINE_MACRO( "NUCLEUS_USES_BACKTRACE" );
		
		if ( target_info.platform & apiMacCarbon )
		{
			options.DEFINE_MACRO( "TARGET_API_MAC_CARBON" );
		}
		else if ( target_info.platform & apiMacBlue )
		{
			options.DEFINE_MACRO_VALUE( "TARGET_API_MAC_CARBON", 0 );
			options.DEFINE_MACRO( "TARGET_API_MAC_OS8" );
			
			if ( target_info.platform & archPPC )
			{
				options.DEFINE_MACRO( "ACCESSOR_CALLS_ARE_FUNCTIONS" );
				options.DEFINE_MACRO( "OPAQUE_UPP_TYPES" );
			}
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
	
	
	static const Project* get_project_providing_prefix( const Project& project, Platform platform )
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
			                                  std::bind2nd( more::ptr_fun( project_has_prefix ),
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
				
				TaskPtr task( new CompilingTask( its_project,
				                                 its_options,
				                                 source_pathname,
				                                 object_pathname,
				                                 ProjectDiagnosticsDirPath( its_project.Name() ),
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
		
		bool needs_include_union = false;
		
		// Select the includes belonging to the projects we use
		IncludeDirGatherer gatherer( options, needs_include_union );
		
		const std::vector< std::string >& all_used_projects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		std::for_each( all_used_projects.rbegin(),
		               all_used_projects.rend(),
		               gatherer );
		
		CompilerOptions precompile_options = options;
		
		const std::string& diagnostics_dir_path = ProjectDiagnosticsDirPath( project.Name() );
		
		TaskPtr precompile_task( new NullTask() );
		
		// In case we have a toolkit with no common sources?
		precompile_task->AddDependent( source_dependency );
		
		// For width reasons, we call the precompiled header a 'prefix'.
		
		const Project* project_providing_prefix = get_project_providing_prefix( project, target_info.platform );
		
		if ( project_providing_prefix != NULL )
		{
			std::string prefix_source_path = project_providing_prefix->PrecompiledHeaderSource();
			
			std::string prefix_source_filename = io::get_filename_string( prefix_source_path );
			
			std::string pchImage = get_prefix_image_pathname( project_providing_prefix->Name(),
			                                                  prefix_source_filename,
			                                                  target_info );
			
			options.SetPrecompiledHeaderSource( prefix_source_filename );
			// Theory:
			// We need to include the pch source by name only, not path.
			// Therefore, we need its parent directory to be in the search path,
			// but that must already be so to have found it in the first place.
			// We also need the parent of the image file, so gcc can find that.
			
			options.PrependIncludeDir( io::get_preceding_directory( pchImage ) );
			
			if ( project.HasPrecompiledHeader() )
			{
				// Locate the precompiled header image file.
				std::string prefix_source_pathname = project.FindIncludeRecursively( prefix_source_path );
				
				if ( prefix_source_pathname.empty() )
				{
					std::fprintf( stderr, "Missing precompiled header '%s'\n", prefix_source_path.c_str() );
				}
				else
				{
					// Insert a "//" sentinel before the relative path
					prefix_source_pathname.insert( prefix_source_pathname.end() - prefix_source_path.size(), '/' );
				}
				
				precompile_task.reset( new CompilingTask( project,
				                                          precompile_options,
				                                          prefix_source_pathname,
				                                          pchImage,
				                                          diagnostics_dir_path,
				                                          "Precompiling: " ) );
				
				project.set_precompile_task( precompile_task );
			}
			else
			{
				ASSERT( !project_providing_prefix->get_precompile_task().expired() );
				
				precompile_task = project_providing_prefix->get_precompile_task().lock();
			}
		}
		
		
		if ( Options().all )
		{
			precompile_task->UpdateInputStamp( 0x7FFFFFFF );
		}
		
		mkdir_path( get_project_dependencies_pathname( project.Name() ) );
		
		std::string outDir = ProjectObjectsDirPath( project.Name() );
		
		std::vector< std::string > object_paths;
		
		NameObjectFiles( project, object_paths );
		
		const std::vector< std::string >& sources = project.Sources();
		
		const std::size_t n_tools = project.ToolCount();
		
		tool_dependencies.resize( n_tools );
		
		std::transform( sources.begin(),
		                sources.begin() + n_tools,
		                object_paths.begin(),
		                tool_dependencies.begin(),
		                ToolTaskMaker( project, options, precompile_task ) );
		
		std::vector< std::string >::const_iterator the_source, the_object, end = sources.end();
		
		for ( the_source = sources     .begin() + n_tools,
		      the_object = object_paths.begin() + n_tools;  the_source != end;  ++the_source,
		                                                                        ++the_object )
		{
			// The source file
			const std::string& source_pathname = *the_source;
			
			const std::string& output_path = *the_object;
			
			const char* caption = "Compiling: ";
			
			TaskPtr task( new CompilingTask( project,
			                                 options,
			                                 source_pathname,
			                                 output_path,
			                                 diagnostics_dir_path,
			                                 caption ) );
			
			precompile_task->AddDependent( task );
			
			task->AddDependent( source_dependency );
		}
		
		project_base_task->AddDependent( precompile_task );
	}
	
}

