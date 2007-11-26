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
#include "Orion/StandardIO.hh"

// CompileDriver
#include "CompileDriver/ProjectCatalog.hh"
#include "CompileDriver/ProjectConfig.hh"
#include "CompileDriver/ProjectDotConf.hh"

// A-line
#include "A-line/Exceptions.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Compile.hh"
#include "A-line/Link.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"


namespace O = Orion;


namespace ALine
{
	
	//namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	//namespace NX = NitrogenExtras;
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
	
	
	static void ExecuteCommand( const std::string& command )
	{
		if ( gOptions.verbose || gDryRun )
		{
			std::string command_line = command + "\n";
			
			p7::write( p7::stdout_fileno, command_line.data(), command_line.size() );
		}
		
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
		
		int wait_result = system( command.c_str() );
		
		if ( wait_result != 0 )
		{
			bool signaled = WIFSIGNALED( wait_result );
			
			const char* ended  = signaled ? "terminated via signal" : "exited with status";
			int         status = signaled ? WTERMSIG( wait_result ) : WEXITSTATUS( wait_result );
			
			std::fprintf( stderr, "The last command %s %d.  Aborting.\n", ended, status );
			
			O::ThrowExitStatus( signaled ? 2 : 1 );
		}
	}
	
	void QueueCommand( const std::string& command )
	{
		//gCommands.push_back( command );
		
		ExecuteCommand( command );
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
		bool needToBuild = project.Product() != productNotBuilt;
		
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
	
	CD::AddPendingSubproject( UserProjectsPath() );
	
	if ( gOptions.catalog )
	{
		//(void)GetProjectCatalog();
	}
	
	CD::ApplyPlatformDefaults( targetPlatform );
	
	int fail = 0;
	
	for ( int i = 0;  freeArgs[ i ] != NULL;  ++i )
	{
		const std::string& proj = freeArgs[ i ];
		
		gOptions.platform = targetPlatform;
		
		try
		{
			Project& project = GetProject( proj );
			BuildTarget( project, MakeTargetInfo( project, buildVariety ) );
			
			//Io::Out << "Executing commands\n";
			
			std::for_each( gCommands.begin(),
			               gCommands.end(),
			               std::ptr_fun( ExecuteCommand ) );
			
			gCommands.clear();
		}
		catch ( const NoSuchProject& )
		{
			Io::Err << argv[ 0 ] << ": No such project " << q( proj ) << "\n";
			fail++;
		}
		catch ( const CD::NoSuchProject& )
		{
			Io::Err << argv[ 0 ] << ": No such project " << q( proj ) << "\n";
			fail++;
		}
		catch ( const NoSuchUsedProject& ex )
		{
			Io::Err << argv[ 0 ] 
				<< ": No such project " << q( ex.used ) 
				<< " used by " << ex.projName << "\n";
			fail++;
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
			Io::Err << argv[ 0 ] << ": Error in " << proj << ":\n";
			
			throw;
		}
	}
	
	return fail != 0;
}

