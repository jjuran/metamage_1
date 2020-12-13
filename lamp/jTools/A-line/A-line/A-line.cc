/*	=========
 *	A-line.cc
 *	=========
 */

#include "A-line/A-line.hh"

// Standard C++
#include <algorithm>
#include <map>
#include <vector>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include <sys/wait.h>

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/chdir.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/waitpid.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/_exit.hh"

// pfiles
#include "pfiles/common.hh"

// Orion
#include "Orion/Main.hh"

// A-line
#include "A-line/Commands.hh"
#include "A-line/Exceptions.hh"
#include "A-line/Compile.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCatalog.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/ProjectConfig.hh"
#include "A-line/ProjectDotConf.hh"
#include "A-line/TargetNames.hh"
#include "A-line/Task.hh"


using namespace command::constants;

using tool::OptionsRecord;
using tool::Platform;
using tool::BuildVariety;
using tool::platformUnspecified;
using tool::buildDefault;

enum
{
	Option_A4      = '4',
	Option_A5      = '5',
	Option_68K     = '6',
	Option_x86     = '8',
	Option_Blue    = 'B',
	Option_Carbon  = 'C',
	Option_CFM     = 'F',
	Option_MachO   = 'O',
	Option_PPC     = 'P',
	Option_release = 'R',
	Option_sym     = 'X',
	Option_all     = 'a',
	Option_debug   = 'g',
	Option_n_jobs  = 'j',
	Option_dry_run = 'n',
	Option_cpp     = 'p',
	Option_catalog = 't',
	Option_verbose = 'v',
};

static command::option options[] =
{
	{ "", Option_n_jobs, command::Param_required },
	
	{ "all",     Option_all     },
	{ "cpp",     Option_cpp     },
	{ "catalog", Option_catalog },
	{ "dry-run", Option_dry_run },
	{ "verbose", Option_verbose },
	
	{ "68k", Option_68K },
	{ "ppc", Option_PPC },
	{ "x86", Option_x86 },
	
	{ "a4",    Option_A4    },
	{ "a5",    Option_A5    },
	{ "cfm",   Option_CFM   },
	{ "macho", Option_MachO },
	
	{ "blue",   Option_Blue   },
	{ "carbon", Option_Carbon },
	
	{ "sym",     Option_sym     },
	{ "debug",   Option_debug   },
	{ "release", Option_release },
	
	{ NULL }
};

static bool gDryRun = false;

static OptionsRecord gOptions;

static std::size_t global_job_limit = 1;

static Platform arch    = platformUnspecified;
static Platform runtime = platformUnspecified;
static Platform macAPI  = platformUnspecified;

static BuildVariety buildVariety = buildDefault;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		using namespace tool;
		
		using command::global_result;
		
		switch ( opt )
		{
			case Option_all:
				gOptions.all = true;
				break;
			
			case Option_catalog:
				gOptions.catalog = true;
				break;
			
			case Option_dry_run:
				gDryRun = true;
				break;
			
			case Option_verbose:
				gOptions.verbose = true;
				break;
			
			case Option_cpp:
				gOptions.preprocess = true;
				break;
			
			case Option_n_jobs:
				global_job_limit = gear::parse_decimal( global_result.param );
				break;
			
			case Option_68K:  arch = arch68K;  break;
			case Option_PPC:  arch = archPPC;  break;
			case Option_x86:  arch = archX86;  break;
			
			case Option_A4:     runtime = runtimeA4CodeResource;  break;
			case Option_A5:     runtime = runtimeA5CodeSegments;  break;
			case Option_CFM:    runtime = runtimeCodeFragments;   break;
			case Option_MachO:  runtime = runtimeMachO;           break;
			
			case Option_Blue:    macAPI = apiMacBlue;    break;
			case Option_Carbon:  macAPI = apiMacCarbon;  break;
			
			case Option_sym:      buildVariety = buildSymbolics;  break;
			case Option_debug:    buildVariety = buildDebug;      break;
			case Option_release:  buildVariety = buildRelease;    break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	OptionsRecord& Options()
	{
		return gOptions;
	}
	
	
	static inline plus::string MakeTargetName( const TargetInfo& info )
	{
		return MakeTargetName( info.platform & archMask,
		                       info.platform & runtimeMask,
		                       info.platform & apiMask,
		                       info.build );
	}
	
	
	static plus::string ShellEscapedWord( const plus::string& word )
	{
		plus::var_string result;
		
		result.reserve( word.size() );
		
		for ( const char* p = word.c_str();  *p != '\0';  ++p )
		{
			if ( !iota::is_alnum( *p )  &&  *p != '_' )
			{
				switch ( *p )
				{
					case '_':
					case '+':
					case '-':
					case '/':
					case '.':
					case ',':
					case '@':
					case '%':
						// shell-safe character; don't escape
						break;
					
					default:
						// insert a backslash before an unsafe character
						result += '\\';
						break;
				}
			}
			
			result += *p;
		}
		
		return result;
	}
	
	static void PrintCommand( const StrConVector& command )
	{
		//ASSERT( command.size() > 1 ) );
		
		plus::var_string command_line = command.front();
		
		typedef StrConVector::const_iterator Iter;
		
		Iter end = command.end() - (command.back() == NULL ? 1 : 0);
		
		for ( Iter it = command.begin() + 1;  it != end;  ++it )
		{
			command_line += ' ';
			command_line += *it;
		}
		
		command_line += "\n";
		
		p7::write( p7::stdout_fileno, command_line );
	}
	
	static void PrintCommandForShell( const StrConVector& command )
	{
		if ( gOptions.verbose || gDryRun )
		{
			PrintCommand( command );
		}
	}
	
	static void SetEditorSignature( const char* filename )
	{
		if ( const char* editor_sig = getenv( "MAC_EDITOR_SIGNATURE" ) )
		{
			const char* command[] = { "/Developer/Tools/SetFile", "-t", "TEXT", "-c", editor_sig, filename, NULL };
			
			p7::pid_t pid = POSEVEN_VFORK();
			
			if ( pid == 0 )
			{
				p7::execv( command );
			}
			
			(void) p7::waitpid( pid );
		}
	}
	
	static bool open_diagnostics_file( const char* path )
	{
		int fd = ::open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
		
		if ( fd < 0 )
		{
			return false;
		}
		
		dup2( fd, STDERR_FILENO );
		
		close( fd );
		
		return true;
	}
	
	static inline bool is_null( const char* string )
	{
		return string == NULL  ||  string[0] == '\0';
	}
	
	
	static p7::pid_t launch_job( const StrConVector& command, const char* diagnostics_path )
	{
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			if ( !is_null( diagnostics_path ) && !open_diagnostics_file( diagnostics_path ) )
			{
				std::perror( diagnostics_path );
				
				p7::_exit( p7::exit_failure );
			}
			
			p7::execvp( &command.front() );
		}
		
		return pid;
	}
	
	static void report_diagnostics( size_t size, bool had_errors, const char* diagnostics_path )
	{
		const char* stuff = had_errors ? "errors" : "warnings";
		
		char path_buffer[ 4096 ];
		
		const char* pathname = realpath( diagnostics_path, path_buffer );
		
		if ( pathname == NULL )
		{
			pathname = diagnostics_path;
		}
		
		std::fprintf( stderr, "#\n# %ld bytes of %s\n#\n" "    report %s\n#\n",
		                            size,        stuff,               pathname );
	}
	
	void check_diagnostics( bool succeeded, const char* diagnostics_path )
	{
		if ( !is_null( diagnostics_path ) )
		{
			struct stat stat_buffer = p7::stat( diagnostics_path );
			
			if ( const size_t size = stat_buffer.st_size )
			{
				report_diagnostics( size, !succeeded, diagnostics_path );
				
				return;
			}
			
			// empty file; delete, ignore errors
			(void) unlink( diagnostics_path );
		}
	}
	
	
	static std::map< p7::pid_t, TaskPtr > global_running_tasks;
	
	
	static inline bool is_user_break( p7::wait_t wait_status )
	{
		return p7::wifexited( wait_status )  &&  p7::wexitstatus( wait_status ) == 128;
	}
	
	static inline bool is_plain_error( p7::wait_t wait_status )
	{
		return p7::wifexited( wait_status )  &&  p7::wexitstatus( wait_status ) == 1;
	}
	
	static void end_task( p7::pid_t pid, p7::wait_t wait_status )
	{
		std::map< p7::pid_t, TaskPtr >::iterator it = global_running_tasks.find( pid );
		
		ASSERT( it != global_running_tasks.end() );
		
		TaskPtr task = it->second;
		
		global_running_tasks.erase( it );
		
		if ( wait_status == 0 )
		{
			task->Success();
		}
		else if ( is_plain_error( wait_status ) )
		{
			task->Failure();
		}
		else
		{
			const bool signaled = p7::wifsignaled( wait_status );
			
			const char* ended  = signaled ? "terminated via signal" : "exited with status";
			int         status = signaled ? WTERMSIG( wait_status ) : WEXITSTATUS( wait_status );
			
			std::fprintf( stderr, "The last command %s %d.  Aborting.\n", ended, status );
			
			throw signaled ? p7::exit_t( 2 ) : p7::exit_failure;
		}
	}
	
	static bool wait_and_end_task( bool nonblocking )
	{
		p7::wait_t wait_status;
		
		if ( p7::pid_t pid = p7::waitpid( p7::pid_t( -1 ), wait_status, nonblocking ? WNOHANG : 0 ) )
		{
			if ( is_user_break( wait_status ) )
			{
				p7::write( p7::stderr_fileno, STR_LEN( "### Aborting on user break via ToolServer.\n" ) );
				
				_exit( 128 );
			}
			
			end_task( pid, wait_status );
			
			return true;
		}
		
		return false;
	}
	
	static bool reap_job( bool nonblocking )
	{
		return !global_running_tasks.empty() && wait_and_end_task( nonblocking );
	}
	
	static void reap_jobs( bool nonblocking )
	{
		while ( reap_job( nonblocking ) )
		{
			continue;
		}
	}
	
	static void wait_for_jobs()
	{
		// If all the available slots are taken, wait for a job to exit
		while ( global_running_tasks.size() >= global_job_limit )
		{
			wait_and_end_task( false );
		}
		
		reap_jobs( true );
	}
	
	void ExecuteCommand( const TaskPtr&       task,
	                     const plus::string&  caption,
	                     const StrConVector&  command,
	                     const char*          diagnostics_file_path )
	{
		ASSERT( command.size() > 1 );
		ASSERT( command.back() == NULL );
		
		std::printf( "%s\n", caption.c_str() );
		
		PrintCommandForShell( command );
		
		if ( gDryRun )
		{
			return;
		}
		
		const bool has_diagnostics_file = !is_null( diagnostics_file_path );
		
		if ( has_diagnostics_file )
		{
			plus::string diagnostics_dir = io::get_preceding_directory( diagnostics_file_path );
			
			mkdir_path( diagnostics_dir );
		}
		
		p7::pid_t pid = launch_job( command, diagnostics_file_path );
		
		global_running_tasks[ pid ] = task;
		
	#ifdef __APPLE__
		
		if ( has_diagnostics_file )
		{
			SetEditorSignature( diagnostics_file_path );
		}
		
	#endif
		
		wait_for_jobs();
	}
	
	
	class project_builder
	{
		private:
			const TargetInfo& its_target_info;
		
		public:
			project_builder( const TargetInfo& info ) : its_target_info( info )
			{
			}
			
			void operator()( const plus::string& project_name ) const;
	};
	
	void project_builder::operator()( const plus::string& project_name ) const
	{
		Project& project = GetProject( project_name, its_target_info.platform );
		
		bool needToBuild = ProductGetsBuilt( project.Product() );
		
		if ( needToBuild )
		{
			TaskPtr project_base_task = seize_ptr( new NullTask() );
			TaskPtr source_dependency = seize_ptr( new NullTask() );
			
			std::vector< TaskPtr > tool_dependencies;
			
			CompileSources( project, its_target_info, project_base_task, source_dependency, tool_dependencies );
			LinkProduct   ( project, its_target_info, project_base_task, source_dependency, tool_dependencies );
			
			AddReadyTask( project_base_task );
		}
	}
	
	static void BuildTarget( const Project& project, const TargetInfo& targetInfo )
	{
		plus::string targetName = MakeTargetName( targetInfo );
		
		plus::string targetDir = TargetDirPath( targetName );
		
		PrintCommandForShell( MakeCommand( "cd", targetDir.c_str() ) );
		
		p7::chdir( targetDir.c_str() );
		
		const StringVector& prereqs = project.AllUsedProjects();
		
		std::for_each
		(
			prereqs.begin(), 
			prereqs.end(), 
			project_builder( targetInfo )
		);
	}
	
	static void ApplyTargetDefaults( TargetInfo& target )
	{
		ApplyPlatformDefaults( target.platform );
		
		if ( target.build == buildDefault )
		{
			target.build = buildDebug;
		}
		
		if ( !ALINE_CROSS_DEVELOPMENT  ||  target.platform & runtimeMachO )
		{
			target.envType   = envUnix;
		}
		else
		{
			target.envType   = envRelix;
		}
		
		// (a) Metrowerks/Mach-O is fully untested and almost certainly broken
		// (b) This doesn't consider MPW compilers
		
		if ( !ALINE_CROSS_DEVELOPMENT  ||  target.platform & runtimeMachO )
		{
			target.toolchain = toolchainGNU;
		}
		else
		{
			target.toolchain = toolchainMetrowerks;
		}
	}
	
	static TargetInfo MakeTargetInfo( const Project& project, Platform platform, BuildVariety build )
	{
		TargetInfo targetInfo( platform, build );
		
		//targetInfo.DeduceFromProduct( project.Product() );
		//targetInfo.ApplyOptions( gOptions.target );
		
		ApplyTargetDefaults( targetInfo );
		
		return targetInfo;
	}
	
	int Main( int argc, char** argv )
	{
		if ( argc <= 1 )  return 0;
		
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		char const *const *freeArgs = args;
		
		if ( gOptions.preprocess  &&  buildVariety == buildSymbolics )
		{
			const char* msg = "A-line:  Symbolics and separate preprocessing are incompatible";
			
			fprintf( stderr, "%s\n", msg );
			
			return 2;
		}
		
	#if TARGET_API_MAC_CARBON
		
		/*
			Bringing ToolServer frontmost to run commands seems to improve
			performance when MacRelix is running natively in OS X, but hurt
			it in OS 9 (including Classic).
		*/
		
		setenv( "TLSRVR_FRONTMOST", "always", 0 );
		
	#endif
		
	#if defined( __APPLE__ )  &&  defined( __POWERPC__ )
		
		// Developer Tools 5247 (Xcode 2.2) complains if this is not set
		// (root cause:  it defaults to 10.1)
		setenv( "MACOSX_DEPLOYMENT_TARGET", "10.2", 0 );
		
	#endif
		
		Platform targetPlatform = arch | runtime | macAPI;
		
		AddPendingSubproject( UserSrcTreePath() );
		
		plus::string catalog_cache_pathname = get_user_cache_pathname() / "catalog";
		
		bool cache_was_written = false;
		
		if ( gOptions.catalog || !io::file_exists( catalog_cache_pathname ) )
		{
			p7::write( p7::stdout_fileno, STR_LEN( "# Catalogging project configs..." ) );
			
			while ( AddPendingSubprojects() )
			{
				continue;
			}
			
			write_catalog_cache( p7::open( catalog_cache_pathname,
			                               p7::o_wronly | p7::o_creat | p7::o_trunc ) );
			
			cache_was_written = true;
			
			p7::write( p7::stdout_fileno, STR_LEN( "done\n" ) );
		}
		else
		{
			read_catalog_cache( p7::open( catalog_cache_pathname, p7::o_rdonly ) );
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( "# Loading project data..." ) );
		
		ApplyPlatformDefaults( targetPlatform );
		
		for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
		{
			const char* project_name = freeArgs[ i ];
			
			try
			{
				Project& project = GetProject( project_name, targetPlatform );
			}
			catch ( const NoSuchProject& )
			{
				std::fprintf( stderr, "A-line: No such project '%s'\n", project_name );
				
				if ( !cache_was_written )
				{
					std::fprintf( stderr, "%s\n", "A-line: (use 'A-line -t' to refresh the project catalog)" );
				}
				
				return 1;
			}
			catch ( const NoSuchUsedProject& ex )
			{
				std::fprintf( stderr, "A-line: No such project '%s' used by %s\n",
				                                                ex.used.c_str(),
				                                                            ex.projName.c_str() );
				
				if ( !cache_was_written )
				{
					std::fprintf( stderr, "%s\n", "A-line: (use 'A-line -t' to refresh the project catalog)" );
				}
				
				return 1;
			}
			catch ( const missing_project_config& missing )
			{
				std::fprintf( stderr, "A-line: project %s's config %s doesn't exist\n",
				                                       missing.project_name.c_str(),
				                                                   missing.config_pathname.c_str() );
				
				if ( !cache_was_written )
				{
					std::fprintf( stderr, "%s\n", "A-line: (use 'A-line -t' to refresh the project catalog)" );
				}
				
				throw p7::exit_t( 3 );
			}
			catch ( const circular_dependency& circular )
			{
				std::fprintf( stderr, "A-line: Circular dependency on '%s'\n", circular.project_name.c_str() );
				
				throw p7::exit_t( 4 );
			}
			catch ( const p7::errno_t& err )
			{
				std::fprintf( stderr, "A-line: %s: %s\n", project_name, std::strerror( err ) );
				
				throw;
			}
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( "done.\n" ) );
		
		p7::write( p7::stdout_fileno, STR_LEN( "# Generating task graph..." ) );
		
		TargetInfo target_info( targetPlatform, buildVariety );
		
		ApplyTargetDefaults( target_info );
		
		for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
		{
			const char* proj = freeArgs[ i ];
			
			try
			{
				Project& project = GetProject( proj, targetPlatform );
				
				BuildTarget( project, target_info );
			}
			catch ( const p7::errno_t& err )
			{
				std::fprintf( stderr, "A-line: %s: %s\n", proj, std::strerror( err ) );
				
				throw;
			}
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( "done.\n" ) );
		
		while ( StartNextTask() || reap_job( false ) )
		{
			continue;
		}
		
		reap_jobs( false );
		
		if ( std::size_t n = CountFailures() )
		{
			std::fprintf( stderr, "###\n"
			                      "### A-line: %ld task%s had errors\n"
			                      "###\n",     n,      n == 1 ? ""
			                                                  : "s" );
			
			return 1;
		}
		
		return 0;
	}
	
}
