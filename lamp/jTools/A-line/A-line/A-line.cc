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

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Shared.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// Nitrogen Extras / Utilities
//#include "Utilities/Processes.h"

// BitsAndBytes
#include "StringFilters.hh"

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
#include "A-line/BuildCommon.hh"
#include "A-line/Compile.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"


namespace O = Orion;


namespace ALine
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace CD = CompileDriver;
	
	using BitsAndBytes::q;
	
	
	static bool gDryRun = false;
	
	static OptionsRecord gOptions;
	
	static std::vector< std::string > gCommands;
	
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
			std::string command = "/Developer/Tools/SetFile -t TEXT -c $MAC_EDITOR_SIGNATURE " + q( filename );
			
			(void) system( command.c_str() );
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
		
		bool has_diagnostics_file = diagnosticsFilename != NULL  &&  diagnosticsFilename[0] != '\0';
		
		pid_t pid = p7::throw_posix_result( vfork() );
		
		if ( pid == 0 )
		{
			if ( has_diagnostics_file )
			{
				int diagnostics = ::open( diagnosticsFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
				
				if ( diagnostics < 0 )
				{
					std::perror( diagnosticsFilename );
					
					_exit( EXIT_FAILURE );
				}
				
				SetEditorSignature( diagnosticsFilename );
				
				dup2( diagnostics, STDERR_FILENO );
				
				close( diagnostics );
			}
			
			execvp( command.front(), const_cast< char** >( &command.front() ) );
			
			_exit( 127 );
		}
		
		int wait_status = -1;
		
		pid_t resultpid = p7::throw_posix_result( ::waitpid( pid, &wait_status, 0 ) );
		
		bool had_errors = wait_status != 0;
		
		if ( has_diagnostics_file )
		{
			struct ::stat stat_buffer;
			
			int status = ::stat( diagnosticsFilename, &stat_buffer );
			
			if ( status == 0 )
			{
				const size_t size = stat_buffer.st_size;
				
				if ( size == 0 )
				{
					// empty file; delete
					unlink( diagnosticsFilename );
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
			bool signaled = WIFSIGNALED( wait_status );
			
			const char* ended  = signaled ? "terminated via signal" : "exited with status";
			int         status = signaled ? WTERMSIG( wait_status ) : WEXITSTATUS( wait_status );
			
			std::fprintf( stderr, "The last command %s %d.  Aborting.\n", ended, status );
			
			O::ThrowExitStatus( signaled ? 2 : 1 );
		}
	}
	
	
	class Job
	{
		private:
			Project& project;
			TargetInfo info;
			int failures;
		
		public:
			Job( Project& proj, const TargetInfo& targetInfo )
			: 
				project ( proj ), 
				info    ( targetInfo ), 
				failures( 0 )
			{}
			
			void Build();
	};
	
	void Job::Build()
	{
		bool needToBuild = ProductGetsBuilt( project.Product() );
		
		if ( needToBuild || Options().verbose )
		{
			std::string checking = "# Checking project " + q( project.Name() ) + ":\n";
			
			(void) p7::write( p7::stdout_fileno, checking.data(), checking.size() );
		}
		
		if ( needToBuild )
		{
			//QueueCommand( "echo Building project " + q( project.Name() ) + ":" );
		}
		
		if ( !needToBuild )  return;
		
		CompileSources( project, info );
		LinkProduct   ( project, info );
	}
	
	typedef std::map< ProjName, NN::Shared< Job*, NN::DisposeWithDelete > > JobSubMap;
	typedef std::map< TargetName, JobSubMap > JobMap;
	
	static Job& BuildJob( const ProjName& projName, const TargetInfo& targetInfo )
	{
		static JobMap gJobs;
		
		JobSubMap& subMap = gJobs[ MakeTargetName( targetInfo ) ];
		JobSubMap::iterator it = subMap.find( projName );
		
		if ( it != subMap.end() )
		{
			return *it->second.Get();
		}
		else
		{
			Job& job = *( subMap[ projName ] = NN::Owned< Job*, NN::DisposeWithDelete >::Seize
			(
				new Job( GetProject( projName ), targetInfo ) )
			);
			
			job.Build();
			
			return job;
		}
	}
	
	static void BuildTarget( const Project& project, TargetInfo targetInfo )
	{
		std::string targetName = MakeTargetName( targetInfo );
		
		std::string targetDir = TargetDirPath( targetName );
		
		PrintCommandForShell( MakeCommand( "cd", targetDir.c_str() ) );
		
		chdir( targetDir.c_str() );
		
		const std::vector< ProjName >& prereqs = project.AllUsedProjects();
		std::for_each
		(
			prereqs.begin(), 
			prereqs.end(), 
			std::bind2nd
			(
				more::ptr_fun( BuildJob ), 
				targetInfo
			)
		);
	}
	
	static void ApplyTargetDefaults( TargetInfo& target )
	{
		CD::ApplyPlatformDefaults( target.platform );
		
		if ( target.build == buildDefault )
		{
			target.build = buildDebug;
		}
		
		// This is wrong for Metrowerks targeting Mach-O, but does anyone care?
		if ( !ALINE_CROSS_DEVELOPMENT  ||  target.platform & CD::runtimeMachO )
		{
			target.toolkit = toolkitGNU;
		}
		else
		{
			target.toolkit = toolkitCodeWarrior;
		}
	}
	
	static TargetInfo MakeTargetInfo( const Project& project, BuildVariety build )
	{
		TargetInfo targetInfo( gOptions.platform, build );
		
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
	
	O::BindOption( "-f", gOptions.files );
	
	O::AliasOption( "-f", "--file" );
	
	// Targeting
	
	O::BindOption( "-6", arch, CD::arch68K );
	O::BindOption( "-P", arch, CD::archPPC );
	O::BindOption( "-8", arch, CD::archX86 );
	
	O::BindOption( "-4", runtime, CD::runtimeA4CodeResource );
	O::BindOption( "-5", runtime, CD::runtimeA5CodeSegments );
	O::BindOption( "-F", runtime, CD::runtimeCodeFragments  );
	O::BindOption( "-O", runtime, CD::runtimeMachO          );
	
	O::BindOption( "-B", macAPI, CD::apiMacToolbox );
	O::BindOption( "-C", macAPI, CD::apiMacCarbon  );
	
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
	
	if ( gOptions.catalog )
	{
		//(void)GetProjectCatalog();
	}
	
	CD::ApplyPlatformDefaults( targetPlatform );
	
	for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
	{
		const std::string& proj = freeArgs[ i ];
		
		gOptions.platform = targetPlatform;
		
		try
		{
			Project& project = GetProject( proj );
			BuildTarget( project, MakeTargetInfo( project, buildVariety ) );
		}
		catch ( const CD::NoSuchProject& )
		{
			std::fprintf( stderr, "A-line: No such project '%s'\n", proj.c_str() );
			
			return EXIT_FAILURE;
		}
		catch ( const NoSuchUsedProject& ex )
		{
			std::fprintf( stderr, "A-line: No such project '%s' used by %s\n",
			                                                ex.used.c_str(),
			                                                            ex.projName.c_str() );
			
			return EXIT_FAILURE;
		}
		/*
		catch ( BadSourceAlias& ex )
		{
			Io::Err << argv[ 0 ] 
				<< ": Unresolvable source alias " << q( NN::Convert< std::string >( ex.alias.name ) )
				<< " in " << ex.proj.Name() << "\n";
		}
		*/
		catch ( const p7::errno_t& err )
		{
			std::fprintf( stderr, "A-line: %s: %s\n", proj.c_str(), std::strerror( err ) );
			
			throw;
		}
	}
	
	return EXIT_SUCCESS;
}

