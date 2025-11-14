/*	==========
 *	Compile.cc
 *	==========
 */

#include "A-line/Compile.hh"

// Standard C++
#include <algorithm>
#include <set>
#include <vector>

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// gear
#include "gear/find.hh"

// plus
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Io
#include "io/io.hh"
#include "io/files.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/basename.hh"
#include "poseven/functions/fstatat.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/symlinkat.hh"
#include "poseven/functions/unlinkat.hh"
#include "poseven/functions/write.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Commands.hh"
#include "A-line/CompilerOptions.hh"
#include "A-line/derived_filename.hh"
#include "A-line/Includes.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/prefix.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/Task.hh"


#define STRLEN( s )  (sizeof "" s - 1)


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	typedef Command (*CompileCommandMaker)( const CompilerOptions&  options,
	                                        const plus::string&     source_pathname,
	                                        const plus::string&     output_pathname );
	
	static
	bool using_nexus()
	{
		static bool nexus_peered = getenv( "NEXUS_PEER" );
		
		return nexus_peered;
	}
	
	static plus::string diagnostics_file_path( const plus::string&  dir_path,
	                                           const plus::string&  target_path )
	{
		return derived_pathname( dir_path, target_path, ".txt" );
	}
	
	class CompilingTask : public FileTask
	{
		private:
			const Project&   its_project;
			CompilerOptions  its_options;
			plus::string     its_source_pathname;
			plus::string     its_diagnostics_file_path;
			const char*      its_caption;
			CompileCommandMaker  its_command_maker;
			bool                 it_has_no_includes;
		
		public:
			CompilingTask( const Project&          project,
			               const CompilerOptions&  options,
			               const plus::string&     source,
			               const plus::string&     output,
			               const plus::string&     diagnostics,
			               const char*             caption,
			               CompileCommandMaker     maker,
			               bool                    no_includes )
			: FileTask                 ( output ),
			  its_project              ( project ),
			  its_options              ( options ),
			  its_source_pathname      ( source  ),
			  its_diagnostics_file_path( diagnostics_file_path( diagnostics, source ) ),
			  its_caption              ( caption ),
			  its_command_maker        ( maker   ),
			  it_has_no_includes       ( no_includes )
			{
				if ( !it_has_no_includes  &&  project.SourceDirs().empty() )
				{
					its_options.AppendIncludeDir( io::get_preceding_directory( source ) );
				}
				
				if ( options.Target().envType & envRelix  &&  !options.GetMacros().empty() )
				{
					/*
						Search Metrowerks' includes before Apple's
						so the MSL libc headers take precedence.
						
						Finally, search "Interfaces", so includes like
						<CWANSIIncludes/ctime> and <CIncludes/Threads.h>
						work without requiring any other configuration.
					*/
					
					static const plus::string cwansiincludes = get_Interfaces_subdir( "CWANSIIncludes" );
					static const plus::string cwcincludes    = get_Interfaces_subdir( "CWCIncludes" );
					static const plus::string cincludes      = get_Interfaces_subdir( "CIncludes" );
					static const plus::string interfaces     = get_Interfaces_subdir( "" );
					
					its_options.AppendIncludeDir( cwansiincludes );
					its_options.AppendIncludeDir( cwcincludes    );
					its_options.AppendIncludeDir( cincludes      );
					its_options.AppendIncludeDir( interfaces     );
				}
			}
			
			bool UpToDate();
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	
	static const char* basename( const char* path, size_t length )
	{
		if ( const char* match = gear::find_last_match( path, length, '/' ) )
		{
			return match + 1;
		}
		
		return path;
	}
	
	static void populate_include_union( const Project& project )
	{
		if ( !project.SourceDirs().empty() )
		{
			const StringVector source_dirs = project.SourceDirs();
			
			plus::string includes_union_pathname = get_includes_union_pathname();
			
			n::owned< p7::fd_t > include_fd = p7::open( includes_union_pathname,
			                                            p7::o_rdonly | p7::o_directory );
			
			typedef StringVector::const_iterator Iter;
			
			for ( Iter it = source_dirs.begin();  it != source_dirs.end();  ++it )
			{
				const plus::string& dir_pathname_string = *it;
				
				const char* dir_pathname = dir_pathname_string.c_str();
				
				const char* dir_name = basename( dir_pathname,
				                                 dir_pathname_string.size() );
				
				plus::string include_link = includes_union_pathname / dir_name;
				
				// Source dir must exist (or it would have been culled)
				struct stat dir_stat = p7::stat( dir_pathname );
				
				struct stat link_stat;
				
				const bool target_exists = p7::fstatat( include_fd, dir_name, link_stat );
				
				if ( target_exists  &&  memcmp( &dir_stat, &link_stat, sizeof (struct stat) ) == 0 )
				{
					// They stat the same.  Assume that one is a symlink.
					
					continue;
				}
				
				const bool link_exists = p7::fstatat( include_fd,
				                                      dir_name,
				                                      link_stat,
				                                      p7::at_symlink_nofollow );
				
				if ( link_exists  &&  S_ISLNK( link_stat.st_mode ) )
				{
					// Stale symlink.
					
					p7::unlinkat( include_fd, dir_name );
				}
				else
				{
					// symlinkat() will throw EEXIST if a non-link is present
				}
				
				p7::symlinkat( dir_pathname, include_fd, dir_name );
			}
		}
		
	}
	
#ifdef __APPLE__
	
	static bool include_union_is_usable = true;
	
#else
	
	const bool include_union_is_usable = true;
	
#endif
	
	static
	void gather_includes( CompilerOptions&     options,
	                      bool&                needs_union,
	                      const plus::string&  project_name )
	{
		const Project& project = GetProject( project_name, options.Target().platform );
		
		const ProductType product = project.Product();
		
		if ( product == productNotBuilt  ||  product == productDropIn )
		{
			// Pseudo-projects and drop-in code don't export headers
			return;
		}
		
		if ( include_union_is_usable  &&  ! project.SourceDirs().empty() )
		{
			// Omit projects with 'sources' dirs, since those will be unioned
			needs_union = true;
			
			populate_include_union( project );
			
			return;
		}
		
		const StringVector& search_dirs( project.SearchDirs() );
		
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = search_dirs.begin();  it != search_dirs.end();  ++it )
		{
			options.AppendIncludeDir( *it );
		}
	}
	
	
	static bool IsCFile( const plus::string& name )
	{
		// For our purposes here, ".c" is not a C file.  (Basename can't be empty.)
		// Any one-character extension passes the test, including 'c', 'h', and 'm'.
		
		return name.size() > 2  &&  *(name.end() - 2) == '.';
	}
	
	static Command MakePreprocessCommand( const CompilerOptions&  options,
	                                      const plus::string&     source_pathname,
	                                      const plus::string&     output_pathname )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command preprocess;
		
		preprocess.push_back( "mxcpp" );
		
		const char* arch = cmdgen.ppc  ? "--ppc"
		                 : cmdgen.m68k ? "--68k"
		                 :               "--x86";
		
		preprocess.push_back( arch );
		
		if ( cmdgen.cfm )
		{
			preprocess.push_back( "--cfm" );
		}
		
		if ( cmdgen.a4 )
		{
			preprocess.push_back( "--a4" );
		}
		
		if ( const bool cplusplus = !IsCFile( source_pathname ) )
		{
			preprocess.push_back( "--c++" );
		}
		
		AugmentCommand( preprocess, options.GetMacros() );
		
		AugmentCommand( preprocess, options.IncludeDirOptions() );
		
		if ( options.Target().envType & envRelix )
		{
			// CW Pro 5 barfs on LF newlines, so use CR
			preprocess.push_back( "--cr" );
			
		#ifdef __RELIX__
			
			// For now, mxcpp relies on _realpathat() for this.
			
			preprocess.push_back( "--mac-lines" );
			
		#endif
			
			preprocess.push_back( "-D" "macintosh=1" );
			
			if ( cmdgen.ppc )
			{
				preprocess.push_back( "-D" "powerc=1" );
			}
		}
		else if ( options.Target().envType & envUnix )
		{
			preprocess.push_back( "-I/usr/include" );
		}
		
		if ( false )
		{
			preprocess.push_back( "--precompile" );
		}
		
		AugmentCommand( preprocess, OutputOption( output_pathname.c_str() ) );
		
		// Add the source file to the command line
		preprocess.push_back( source_pathname.c_str() );
		
		preprocess.push_back( NULL );
		
		return preprocess;
	}
	
	static Command MakeCompileCommand( const CompilerOptions&  options,
	                                   const plus::string&     source_pathname,
	                                   const plus::string&     output_pathname )
	{
		CommandGenerator cmdgen( options.Target() );
		
		Command compile = cmdgen.CompilerName();
		
		AugmentCommand( compile, cmdgen.AllCompilerOptions() );
		
		if ( !ALINE_RELIX_DEVELOPMENT  ||  cmdgen.gnu )
		{
			compile.push_back( "-Wno-deprecated-declarations" );  // since we're using legacy API's
			
		#ifdef __POWERPC__
			
			compile.push_back( "-Wno-long-double"             );
			
			bool cplusplus = !IsCFile( source_pathname );
			
			if ( cplusplus )
			{
				// We don't need this warning for C, and in fact GNU C complains about it
				compile.push_back( "-Wno-non-template-friend" );
			}
			
		#endif
			
			compile.push_back( "-fvisibility=hidden" );
		}
		
		AugmentCommand( compile, options.GetMacros() );
		
		AugmentCommand( compile, options.IncludeDirOptions() );
		
		// False while precompiling
		if ( options.HasPrecompiledHeaderSource() )
		{
			// Specify by name only, so gcc will search for the .gch image.
			const plus::string& pchSourceName = options.PrecompiledHeaderSource();
			
			compile.push_back( "-include"            );
			compile.push_back( pchSourceName.c_str() );
		}
		
		AugmentCommand( compile, OutputOption( output_pathname.c_str() ) );
		
		// Add the source file to the command line
		compile.push_back( source_pathname.c_str() );
		
		compile.push_back( NULL );
		
		return compile;
	}
	
	static void get_recursive_includes( const Project&             project,
	                                    const plus::string&        source_pathname,
	                                    std::set< plus::string >&  result )
	{
		const StringVector& includes = GetIncludes( source_pathname ).user;
		
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			const plus::string& include_path = *it;
			
			plus::string pathname = project.FindIncludeRecursively( include_path );
			
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
	
	static void write_dependencies_file( p7::fd_t output, const std::set< plus::string >& includes )
	{
		typedef std::set< plus::string >::const_iterator Iter;
		
		size_t length = 0;
		
		for ( Iter it = includes.begin();  it != includes.end();  ++it )
		{
			length += it->length() + 1;
		}
		
		plus::var_string contents;
		
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
	
	static void read_dependencies_file( p7::fd_t input_fd, std::set< plus::string >& includes )
	{
		text_input::feed feed;
		
		p7::fd_reader reader( input_fd );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string include_path( *s );
			
			includes.insert( include_path );
		}
	}
	
	static time_t get_memoized_timestamp( const plus::string& pathname )
	{
		static std::map< plus::string, time_t > map;
		
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
			const plus::string& pathname = *begin++;
			
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
				if ( it_has_no_includes )
				{
					return true;
				}
				
				plus::string dependencies_dir = get_project_dependencies_pathname( its_project.Name() );
				
				plus::string dependencies_pathname = derived_pathname( dependencies_dir,
				                                                       its_source_pathname,
				                                                       ".d" );
				
				struct stat dependencies_stat;
				
				bool has_dot_d = p7::stat( dependencies_pathname, dependencies_stat )  &&  dependencies_stat.st_mtime > source_stamp;
				
				time_t includes_stamp;
				
				std::set< plus::string > includes;
				
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
		Command command = its_command_maker( its_options, its_source_pathname, OutputPath() );
		
		const char* source_path = strstr( its_source_pathname.c_str(), "//" );
		
		if ( source_path == NULL )
		{
			source_path = its_source_pathname.c_str();
		}
		else
		{
			source_path += STRLEN( "//" );
		}
		
		plus::string full_caption = plus::concat( its_caption, source_path );
		
		ExecuteCommand( shared_from_this(), full_caption, command, its_diagnostics_file_path.c_str() );
	}
	
	void CompilingTask::Return( bool succeeded )
	{
		check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	static plus::string get_prefix_image_pathname( const plus::string&  project_name,
	                                               plus::string         prefix_source_filename,
	                                               const TargetInfo&    target_info )
	{
		plus::string prefix_dir_pathname = ProjectPrecompiledDirPath( project_name );
		
		const bool gnu = target_info.toolchain == toolchainUnix;
		
		const char* extension = (gnu ? ".gch" : ".mwch");
		
		plus::string prefix_image_filename = prefix_source_filename + extension;
		
		return prefix_dir_pathname / prefix_image_filename;
	}
	
	#define DEFINE_MACRO_VALUE( macro, value )  AddDefinedMacro( "-D" macro "=" #value )
	
	#define DEFINE_MACRO( macro )  DEFINE_MACRO_VALUE( macro, 1 )
	
	static void DefineMacros( CompilerOptions& options, const TargetInfo& target_info )
	{
		if ( options.Target().envType & envRelix )
		{
			options.DEFINE_MACRO( "__RELIX__" );
		}
		
		if ( target_info.platform & apiMacCarbon )
		{
			if ( target_info.platform & runtimeMachO )
			{
				options.DEFINE_MACRO_VALUE( "MAC_OS_X_VERSION_MIN_REQUIRED", MAC_OS_X_VERSION_10_2 );
			}
			
			options.DEFINE_MACRO( "TARGET_API_MAC_CARBON" );
		}
		
		if ( target_info.build != buildRelease )
		{
			options.DEFINE_MACRO_VALUE( "CONFIG_DEBUGGING", 1 );
		}
		else
		{
			options.DEFINE_MACRO_VALUE( "CONFIG_DEBUGGING", 0 );
		}
	}
	
	
	static
	TaskPtr add_cpp_and_cc_tasks( const Project&          project,
	                              const CompilerOptions&  cpp_options,
	                              const CompilerOptions&  cc_options,
	                              const plus::string&     source_path,
	                              const plus::string&     cpp_path,
	                              const plus::string&     output_path,
	                              const plus::string&     diagnostics_dir_path,
	                              const TaskPtr&          precompile_task )
	{
		const bool preprocessing = !cpp_path.empty();
		
		TaskPtr cc_task = seize_ptr( new CompilingTask( project,
		                                                cc_options,
		                                                preprocessing ? cpp_path : source_path,
		                                                output_path,
		                                                diagnostics_dir_path,
		                                                "CC    ",
		                                                &MakeCompileCommand,
		                                                preprocessing ) );
		
		if ( preprocessing )
		{
			TaskPtr cpp_task = seize_ptr( new CompilingTask( project,
			                                                 cpp_options,
			                                                 source_path,
			                                                 cpp_path,
			                                                 diagnostics_dir_path,
			                                                 "CPP   ",
			                                                 &MakePreprocessCommand,
			                                                 false ) );
			
			precompile_task->AddDependent( cpp_task );
			
			cpp_task->AddDependent( cc_task );
		}
		else
		{
			precompile_task->AddDependent( cc_task );
		}
		
		return cc_task;
	}
	
	static
	TaskPtr make_unit_tasks( const Project&          project,
	                         const CompilerOptions&  cpp_options,
	                         const CompilerOptions&  cc_options,
	                         const TaskPtr&          precompile,
	                         const plus::string&     source_pathname,
	                         const plus::string&     cppout_pathname,
	                         const plus::string&     object_pathname )
	{
		TaskPtr task = add_cpp_and_cc_tasks( project,
		                                     cpp_options,
		                                     cc_options,
		                                     source_pathname,
		                                     cppout_pathname,
		                                     object_pathname,
		                                     ProjectDiagnosticsDirPath( project.Name() ),
		                                     precompile );
		
		return task;
	}
	
	static inline
	bool can_use_symlinks( const TargetInfo& info )
	{
	#ifdef __APPLE__
		
		if ( can_run_Classic()  &&  info.toolchain == toolchainMetrowerks )
		{
			/*
				If we're targeting the Metrowerks compiler MPW tool
				(which implies the Classic environment), then we
				can't give it include paths with symlinks.  So we
				can only use symlinks if we preprocess separately.
				
				TODO:  In MacRelix, the --cpp option is overridden
				by setting a precompiled header in A-line.conf.
				Native A-line in Mac OS X is currently exhibiting
				the opposite behavior (--cpp overrides `prefix`),
				but we may wish to change that in the future.
			*/
			
			return Options().preprocess;
		}
		
		return true;
		
	#endif
		
	#ifdef __RELIX__
		
		/*
			The symlinks we create in MacRelix are also aliases,
			and the MPW-hosted compiler has no problem with them.
		*/
		
		return true;
		
	#endif
		
		/*
			We assume, if we get here, that we're not running
			in any kind of Mac OS, and further, that targeting
			classic Mac OS isn't an option on this platform.
			
			The first assumption would be invalidated by using
			Metrowerks' compiler targeting Mach-O.  The second
			could be invalidated by using a modified tlsrvr
			that somehow communicates with ToolServer running
			over a network or in an emulator, or possibly with
			a reimplementation of the MPW environment that also
			includes emulation and can run MPW tools and scripts.
		*/
		
		return true;
	}
	
	void CompileSources( Project&                 project,
	                     const TargetInfo&        target_info,
	                     const TaskPtr&           project_base_task,
	                     const TaskPtr&           source_dependency,
	                     std::vector< TaskPtr >&  tool_dependencies )
	{
		CompilerOptions basic_options( project.Name(), target_info );
		
		CompilerOptions preprocess_options = basic_options;
		
		DefineMacros( preprocess_options, target_info );
		
	#ifdef __APPLE__
		
		include_union_is_usable = can_use_symlinks( target_info );
		
	#endif
		
		bool needs_include_union = false;
		
		// Select the includes belonging to the projects we use
		
		const StringVector& all_used_projects = project.AllUsedProjects();
		
		// Reverse direction so projects can override Prefix.hh
		for ( size_t i = all_used_projects.size();  i > 0;  )
		{
			gather_includes( preprocess_options,
			                 needs_include_union,
			                 all_used_projects[ --i ] );
		}
		
		if ( needs_include_union )
		{
			preprocess_options.PrependIncludeDir( get_includes_union_pathname() );
		}
		
		// Currently, precompiling disables separate preprocessing, so the compiler always needs
		// the preprocessor options.
		
		CompilerOptions precompile_options = preprocess_options;
		
		const plus::string& diagnostics_dir_path = ProjectDiagnosticsDirPath( project.Name() );
		
		TaskPtr precompile_task = seize_ptr( new NullTask() );
		
		// In case we have a toolkit with no common sources
		precompile_task->AddDependent( source_dependency );
		
		// For width reasons, we call the precompiled header a 'prefix'.
		
		const Project* project_providing_prefix = get_project_providing_prefix( project, target_info.platform );
		
		const bool preprocessing = Options().preprocess  &&  !project_providing_prefix;
		
		// If we're preprocessing, then only the preprocessor gets macros and includes,
		// and the compiler gets just the basics.  Otherwise, the compiler gets the preprocessing
		// options as well.
		
		CompilerOptions& options = preprocessing ? basic_options : preprocess_options;
		
		if ( project_providing_prefix != NULL )
		{
			plus::string prefix_source_path = project_providing_prefix->PrecompiledHeaderSource();
			
			plus::string prefix_source_filename = p7::basename( prefix_source_path );
			
			plus::string pchImage = get_prefix_image_pathname( project_providing_prefix->Name(),
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
				plus::var_string prefix_source_pathname = project.FindIncludeRecursively( prefix_source_path );
				
				if ( prefix_source_pathname.empty() )
				{
					fprintf( stderr, "Missing precompiled header '%s'\n", prefix_source_path.c_str() );
				}
				else
				{
					// Insert a "//" sentinel before the relative path
					prefix_source_pathname.insert( prefix_source_pathname.end() - prefix_source_path.size(), '/' );
				}
				
				precompile_task = seize_ptr( new CompilingTask( project,
				                                                precompile_options,
				                                                prefix_source_pathname,
				                                                pchImage,
				                                                diagnostics_dir_path,
				                                                "PCH   ",
				                                                &MakeCompileCommand,
				                                                false ) );
				
				project.set_precompile_task( precompile_task );
			}
			else
			{
				precompile_task = project_providing_prefix->get_precompile_task().lock();
			}
		}
		
		
		if ( Options().all )
		{
			precompile_task->UpdateInputStamp( 0x7FFFFFFF );
		}
		
		mkdir_path( get_project_dependencies_pathname( project.Name() ) );
		
		plus::string outDir = ProjectObjectsDirPath( project.Name() );
		
		plus::string cpp_dir;
		
		if ( preprocessing )
		{
			cpp_dir = mkdir_path( "cpp" / project.Name() );
		}
		
		StringVector cppout_paths;
		StringVector object_paths;
		
		if ( can_run_Classic() )
		{
			avoid_long_names = target_info.toolchain == toolchainMetrowerks;
		}
		
		NameObjectFiles( project, object_paths, preprocessing );
		
		const StringVector& source_paths = project.Sources();
		
		if ( preprocessing )
		{
			const char* _i;
			const char* _ii;
			
			bool in_MacRelix = true;
			
		#ifndef __RELIX__
			
			_i  = ".i";
			_ii = ".ii";
			
			in_MacRelix = false;
			
		#endif
			
			/*
				We're preprocessing separately, which already implies
				targeting classic Mac OS.  The question is whether we
				use native tools that directly drive ToolServer (running
				on this machine in Classic), or the nexus wrappers that
				shuttle commands and their inputs to a Mac OS 9 host via
				a shared directory (e.g. over AFP or SheepShaver's extfs).
				
				If the former, we need to avoid using ".i" and ".ii" as
				extensions for source filenames, as that will confuse the
				Metrowerks compiler and yield warnings.  Therefore, we
				should only use those extensions if we're using nexus.
				(If we can't run Classic, we can assume the use of nexus.)
			*/
			
			if ( in_MacRelix  ||  (can_run_Classic()  &&  ! using_nexus()) )
			{
				_i = _ii = ".cpp";
			}
			
			FillOutputFiles( cpp_dir, source_paths, cppout_paths, _i, _ii );
		}
		
		const size_t n_tools = project.ToolCount();
		
		tool_dependencies.reserve( n_tools );
		
		for ( size_t i = 0;  i < n_tools;  ++i )
		{
			const plus::string& cppout = preprocessing ? cppout_paths[ i ]
			                                           : plus::string::null;
			
			tool_dependencies.push_back( make_unit_tasks( project,
			                                              preprocess_options,
			                                              options,
			                                              precompile_task,
			                                              source_paths[ i ],
			                                              cppout,
			                                              object_paths[ i ] ) );
		}
		
		const size_t n = source_paths.size();
		
		for ( size_t i = n_tools;  i < n;  ++i )
		{
			const plus::string& cppout = preprocessing ? cppout_paths[ i ]
			                                           : plus::string::null;
			
			TaskPtr task = make_unit_tasks( project,
			                                preprocess_options,
			                                options,
			                                precompile_task,
			                                source_paths[ i ],
			                                cppout,
			                                object_paths[ i ] );
			
			task->AddDependent( source_dependency );
		}
		
		project_base_task->AddDependent( precompile_task );
	}
	
}
