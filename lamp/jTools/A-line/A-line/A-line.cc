/*	=========
 *	A-line.cc
 *	=========
 */

#include "A-line/A-line.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <vector>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/waitpid.hh"
#include "POSeven/functions/_exit.hh"

// Nitrogen Extras / Utilities
//#include "Utilities/Processes.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"

// CompileDriver
#include "CompileDriver/ProjectCatalog.hh"
#include "CompileDriver/ProjectConfig.hh"
#include "CompileDriver/ProjectDotConf.hh"

// A-line
#include "A-line/Commands.hh"
#include "A-line/Exceptions.hh"
#include "A-line/Compile.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/TargetNames.hh"
#include "A-line/Task.hh"
#include "CompileDriver/ProjectCatalog.hh"
#include "CompileDriver/ProjectConfig.hh"


namespace O = Orion;


namespace ALine
{
	
	namespace p7 = poseven;
	namespace CD = CompileDriver;
	
	
	using namespace io::path_descent_operators;
	
	
	static bool gDryRun = false;
	
	static OptionsRecord gOptions;
	
	OptionsRecord& Options()
	{
		return gOptions;
	}
	
	/*
	static void SwapFrontProcess( const ProcessSerialNumber& from, const ProcessSerialNumber& to )
	{
		try
		{
			if ( N::SameProcess( from, N::GetFrontProcess() ) )
			{
				N::SetFrontProcess( to );
			}
		}
		catch ( ... )
		{
		}
	}
	
	class ApplicationLayerSwitch
	{
		private:
			ProcessSerialNumber itsTargetApp;
			
			ApplicationLayerSwitch           ( const ApplicationLayerSwitch& );
			ApplicationLayerSwitch& operator=( const ApplicationLayerSwitch& );
		
		public:
			ApplicationLayerSwitch() : itsTargetApp( N::CurrentProcess() )  {}
			
			ApplicationLayerSwitch( N::OSType signature ) : itsTargetApp( NX::LaunchApplication( signature ) )
			{
				SwapFrontProcess( N::CurrentProcess(), itsTargetApp );
			}
			
			~ApplicationLayerSwitch()
			{
				SwapFrontProcess( itsTargetApp, N::CurrentProcess() );
			}
			
			void Switch( const ProcessSerialNumber& newTarget )
			{
				SwapFrontProcess( itsTargetApp, newTarget );
				
				itsTargetApp = newTarget;
			}
			
			void Switch( N::OSType newTarget )
			{
				Switch( NX::LaunchApplication( newTarget ) );
			}
	};
	*/
	
	
	inline std::string MakeTargetName( const TargetInfo& info )
	{
		return MakeTargetName( info.platform & CD::archMask,
		                       info.platform & CD::runtimeMask,
		                       info.platform & CD::apiMask,
		                       info.build );
	}
	
	
	static std::string ShellEscapedWord( const std::string& word )
	{
		std::string result;
		
		result.reserve( word.size() );
		
		for ( const char* p = word.c_str();  *p != '\0';  ++p )
		{
			if ( !std::isalnum( *p )  &&  *p != '_' )
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
	
	static void PrintCommand( const std::vector< const char* >& command )
	{
		//ASSERT( command.size() > 1 ) );
		
		std::string command_line = command.front();
		
		typedef std::vector< const char* >::const_iterator Iter;
		
		Iter end = command.end() - (command.back() == NULL ? 1 : 0);
		
		for ( Iter it = command.begin() + 1;  it != end;  ++it )
		{
			command_line += ' ';
			command_line += *it;
		}
		
		command_line += "\n";
		
		p7::write( p7::stdout_fileno, command_line.data(), command_line.size() );
	}
	
	static void PrintCommandForShell( const std::vector< const char* >& command )
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
				(void) execv( command[0], (char**) command );
				
				_exit( 127 );
			}
			
			(void) p7::waitpid( pid );
		}
	}
	
	static void mkdir_path( const std::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path, 0777 );
		}
	}
	
	void ExecuteCommand( const std::vector< const char* >& command, const char* diagnosticsFilename )
	{
		ASSERT( command.size() > 1 );
		ASSERT( command.back() == NULL );
		
		PrintCommandForShell( command );
		
		if ( gDryRun )
		{
			return;
		}
		
		/*
		ApplicationLayerSwitch activateToolServerForCommand;
		
		bool shouldSwitchLayers = !TARGET_RT_MAC_MACHO  &&  (command.substr( 0, 6 ) == "tlsrvr"  ||  command.substr( 0, 4 ) == "mwcc" );
		
		if ( shouldSwitchLayers )
		{
			const N::OSType sigToolServer = N::OSType( 'MPSX' );
			
			//static ApplicationLayerSwitch activateToolServerForSession( sigToolServer );
			
			activateToolServerForCommand.Switch( sigToolServer );
		}
		*/
		
		const bool has_diagnostics_file = diagnosticsFilename != NULL  &&  diagnosticsFilename[0] != '\0';
		
		if ( has_diagnostics_file )
		{
			std::string diagnostics_dir = io::get_preceding_directory( diagnosticsFilename );
			
			mkdir_path( diagnostics_dir );
		}
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			if ( has_diagnostics_file )
			{
				int diagnostics = ::open( diagnosticsFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
				
				if ( diagnostics < 0 )
				{
					std::perror( diagnosticsFilename );
					
					p7::_exit( p7::exit_failure );
				}
				
				dup2( diagnostics, STDERR_FILENO );
				
				close( diagnostics );
			}
			
			execvp( command.front(), const_cast< char** >( &command.front() ) );
			
			_exit( 127 );
		}
		
		if ( has_diagnostics_file )
		{
			SetEditorSignature( diagnosticsFilename );
		}
		
		p7::wait_t wait_status = p7::waitpid( pid );
		
		const bool had_errors = wait_status != 0;
		
		if ( has_diagnostics_file )
		{
			struct ::stat stat_buffer;
			
			int status = ::stat( diagnosticsFilename, &stat_buffer );
			
			if ( status == 0 )
			{
				const size_t size = stat_buffer.st_size;
				
				if ( size == 0 )
				{
					// empty file; delete, ignore errors
					(void) unlink( diagnosticsFilename );
				}
				else
				{
					const char* stuff = had_errors ? "errors" : "warnings";
					
					char path_buffer[ 4096 ];
					
					const char* pathname = realpath( diagnosticsFilename, path_buffer );
					
					if ( pathname == NULL )
					{
						pathname = diagnosticsFilename;
					}
					
					std::fprintf( stderr, "#\n# %d bytes of %s\n#\n" "    report %s\n#\n",
					                            size,       stuff,               pathname );
				}
			}
			else
			{
				// something's wrong with the diagnostics file
				// shouldn't really happen, so screw it
			}
		}
		
		if ( had_errors )
		{
			const bool signaled = p7::wifsignaled( wait_status );
			
			const char* ended  = signaled ? "terminated via signal" : "exited with status";
			int         status = signaled ? WTERMSIG( wait_status ) : WEXITSTATUS( wait_status );
			
			std::fprintf( stderr, "The last command %s %d.  Aborting.\n", ended, status );
			
			O::ThrowExitStatus( signaled ? 2 : p7::exit_failure );
		}
	}
	
	
	class project_builder
	{
		private:
			const TargetInfo& its_target_info;
		
		public:
			project_builder( const TargetInfo& info ) : its_target_info( info )
			{
			}
			
			void operator()( const std::string& project_name ) const;
	};
	
	void project_builder::operator()( const std::string& project_name ) const
	{
		Project& project = GetProject( project_name, its_target_info.platform );
		
		bool needToBuild = ProductGetsBuilt( project.Product() );
		
		if ( needToBuild )
		{
			TaskPtr project_base_task( new NullTask() );
			TaskPtr source_dependency( new NullTask() );
			
			std::vector< TaskPtr > tool_dependencies;
			
			CompileSources( project, its_target_info, project_base_task, source_dependency, tool_dependencies );
			LinkProduct   ( project, its_target_info, project_base_task, source_dependency, tool_dependencies );
			
			AddReadyTask( project_base_task );
		}
	}
	
	static void BuildTarget( const Project& project, const TargetInfo& targetInfo )
	{
		std::string targetName = MakeTargetName( targetInfo );
		
		std::string targetDir = TargetDirPath( targetName );
		
		PrintCommandForShell( MakeCommand( "cd", targetDir.c_str() ) );
		
		chdir( targetDir.c_str() );
		
		const std::vector< std::string >& prereqs = project.AllUsedProjects();
		std::for_each
		(
			prereqs.begin(), 
			prereqs.end(), 
			project_builder( targetInfo )
		);
	}
	
	static void ApplyTargetDefaults( TargetInfo& target )
	{
		CD::ApplyPlatformDefaults( target.platform );
		
		if ( target.build == buildDefault )
		{
			target.build = buildDebug;
		}
		
		if ( !ALINE_CROSS_DEVELOPMENT  ||  target.platform & CD::runtimeMachO )
		{
			target.envType   = envUnix;
		}
		else
		{
			target.envType   = envLamp;
		}
		
		// (a) Metrowerks/Mach-O is fully untested and almost certainly broken
		// (b) This doesn't consider MPW compilers
		
		if ( !ALINE_CROSS_DEVELOPMENT  ||  target.platform & CD::runtimeMachO )
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
	
}

namespace O = Orion;

using namespace ALine;

int O::Main( int argc, argv_t argv )
{
	if ( argc <= 1 )  return 0;
	
	CD::Platform arch    = CD::platformUnspecified;
	CD::Platform runtime = CD::platformUnspecified;
	CD::Platform macAPI  = CD::platformUnspecified;
	
	BuildVariety buildVariety = buildDefault;
	
	// General
	
	O::BindOption( "-v", gOptions.verbose );
	
	O::AliasOption( "-v", "--verbose" );
	
	// Actions
	
	O::BindOption( "-a", gOptions.all );
	
	O::AliasOption( "-a", "--all" );
	
	O::BindOption( "-n", gDryRun );
	
	O::AliasOption( "-n", "--dry-run" );
	
	O::BindOption( "-t", gOptions.catalog );
	
	O::AliasOption( "-t", "--catalog" );
	
	// Targeting
	
	O::BindOption( "-6", arch, CD::arch68K );
	O::BindOption( "-P", arch, CD::archPPC );
	O::BindOption( "-8", arch, CD::archX86 );
	
	O::BindOption( "-4", runtime, CD::runtimeA4CodeResource );
	O::BindOption( "-5", runtime, CD::runtimeA5CodeSegments );
	O::BindOption( "-F", runtime, CD::runtimeCodeFragments  );
	O::BindOption( "-O", runtime, CD::runtimeMachO          );
	
	O::BindOption( "-B", macAPI, CD::apiMacBlue   );
	O::BindOption( "-C", macAPI, CD::apiMacCarbon );
	
	O::BindOption( "-g", buildVariety, buildDebug   );
	O::BindOption( "-R", buildVariety, buildRelease );
	O::BindOption( "-D", buildVariety, buildDemo    );
	
	O::AliasOption( "-6", "--68k" );
	O::AliasOption( "-P", "--ppc" );
	O::AliasOption( "-8", "--x86" );
	
	O::AliasOption( "-4", "--a4"    );
	O::AliasOption( "-5", "--a5"    );
	O::AliasOption( "-F", "--cfm"   );
	O::AliasOption( "-O", "--macho" );
	
	O::AliasOption( "-B", "--blue"   );
	O::AliasOption( "-C", "--carbon" );
	
	O::AliasOption( "-g", "--debug"   );
	O::AliasOption( "-R", "--release" );
	O::AliasOption( "-D", "--demo"    );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	CD::Platform targetPlatform = arch | runtime | macAPI;
	
	CD::AddPendingSubproject( UserSrcTreePath() );
	
	std::string catalog_cache_pathname = get_user_cache_pathname() / "catalog";
	
	bool cache_was_written = false;
	
	if ( gOptions.catalog || !io::file_exists( catalog_cache_pathname ) )
	{
		p7::write( p7::stdout_fileno, STR_LEN( "# Catalogging project configs..." ) );
		
		while ( AddPendingSubprojects() )
		{
			continue;
		}
		
		write_catalog_cache( p7::open( catalog_cache_pathname,
		                               p7::o_wronly | p7::o_creat | p7::o_trunc,
		                               0644 ) );
		
		cache_was_written = true;
		
		p7::write( p7::stdout_fileno, STR_LEN( "done\n" ) );
	}
	else
	{
		read_catalog_cache( p7::open( catalog_cache_pathname, p7::o_rdonly ) );
	}
	
	p7::write( p7::stdout_fileno, STR_LEN( "# Loading project data..." ) );
	
	CD::ApplyPlatformDefaults( targetPlatform );
	
	for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
	{
		const std::string& project_name = freeArgs[ i ];
		
		try
		{
			Project& project = GetProject( project_name, targetPlatform );
		}
		catch ( const CD::NoSuchProject& )
		{
			std::fprintf( stderr, "A-line: No such project '%s'\n", project_name.c_str() );
			
			if ( !cache_was_written )
			{
				std::fprintf( stderr, "%s\n", "A-line: (use 'A-line -t' to refresh the project catalog)" );
			}
			
			return EXIT_FAILURE;
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
			
			return EXIT_FAILURE;
		}
		catch ( const p7::errno_t& err )
		{
			std::fprintf( stderr, "A-line: %s: %s\n", project_name.c_str(), std::strerror( err ) );
			
			throw;
		}
	}
	
	p7::write( p7::stdout_fileno, STR_LEN( "done.\n" ) );
	
	p7::write( p7::stdout_fileno, STR_LEN( "# Generating task graph..." ) );
	
	TargetInfo target_info( targetPlatform, buildVariety );
	
	ApplyTargetDefaults( target_info );
	
	for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
	{
		const std::string& proj = freeArgs[ i ];
		
		try
		{
			Project& project = GetProject( proj, targetPlatform );
			
			BuildTarget( project, target_info );
		}
		catch ( const p7::errno_t& err )
		{
			std::fprintf( stderr, "A-line: %s: %s\n", proj.c_str(), std::strerror( err ) );
			
			throw;
		}
	}
	
	p7::write( p7::stdout_fileno, STR_LEN( "done.\n" ) );
	
	while ( RunNextTask() )
	{
		continue;
	}
	
	return EXIT_SUCCESS;
}

