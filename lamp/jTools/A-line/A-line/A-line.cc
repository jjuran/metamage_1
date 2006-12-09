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

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Shared.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Threads.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace CD = CompileDriver;
	
	using std::string;
	using std::vector;
	
	using BitsAndBytes::q;
	
	static void MyOSStatusLogger( N::OSStatus error, const char *file, int line )
	{
		static int level = 0;
		
		if ( error != -43 )
		{
			return;
		}
		
		if ( std::string( file ) == "Files.cp"  &&  ( line == 114 || line == 282 ) )
		{
			return;
		}
		
		++level;
		
		ASSERT( level < 5 );
		
		try
		{
			Io::Err << "# LOG: OSStatus " << error << ".  \n"
					   "File '" << file << "'; Line " << line << "\n";
			
			sleep( 0 );
		}
		catch ( ... )
		{
			::SysBeep( 30 );
		}
		
		--level;
	}
	
	static OptionsRecord gOptions;
	
	static std::vector< std::string > gCommands;
	
	OptionsRecord& Options()
	{
		return gOptions;
	}
	
	static void ExecuteCommand( const std::string& command )
	{
		if ( gOptions.verbose )
		{
			Io::Out << "  " << command << "\n";
		}
		
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
			Io::Out << "Checking project " << q( project.Name() ) << ":\n";
		}
		
		if ( needToBuild )
		{
			//QueueCommand( "echo Building project " + q( project.Name() ) + ":" );
		}
		
		if ( !needToBuild )  return;
		
		sleep( 0 );
		
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
		const vector< ProjName >& prereqs = project.AllUsedProjects();
		std::for_each
		(
			prereqs.begin(), 
			prereqs.end(), 
			std::bind2nd
			(
				N::PtrFun( BuildJob ), 
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
		
		if ( target.platform.runtime == CD::runtimeMachO )
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
	
	enum
	{
		optVerbose,
		optCatalog,
		optFile,
		optArch, 
		optRuntime, 
		optMacAPI, 
		optBuild
	};
	
}

namespace O = Orion;

using namespace ALine;

static O::Options DefineOptions()
{
	O::Options options;
	
	// General
	
	options.DefineSetFlag( "--verbose", optVerbose );
	options.DefineSetFlag( "-v",        optVerbose );
	
	// Actions
	
	options.DefineSetFlag( "--catalog", optCatalog );
	options.DefineSetFlag( "-t",        optCatalog );
	
	options.DefineAppendToStringList( "--file", optFile );
	options.DefineAppendToStringList( "-f",     optFile );
	
	// Targeting
	
	options.DefineSelectEnum( "--68k", optArch, CD::arch68K );
	options.DefineSelectEnum( "-6",    optArch, CD::arch68K );
	options.DefineSelectEnum( "--ppc", optArch, CD::archPPC );
	options.DefineSelectEnum( "-P",    optArch, CD::archPPC );
	options.DefineSelectEnum( "--x86", optArch, CD::archX86 );
	options.DefineSelectEnum( "-8",    optArch, CD::archX86 );
	
	options.DefineSelectEnum( "--a4",    optRuntime, CD::runtimeA4CodeResource );
	options.DefineSelectEnum( "-4",      optRuntime, CD::runtimeA4CodeResource );
	options.DefineSelectEnum( "--a5",    optRuntime, CD::runtimeA5CodeSegments );
	options.DefineSelectEnum( "-5",      optRuntime, CD::runtimeA5CodeSegments );
	options.DefineSelectEnum( "--cfm",   optRuntime, CD::runtimeCodeFragments  );
	options.DefineSelectEnum( "-F",      optRuntime, CD::runtimeCodeFragments  );
	options.DefineSelectEnum( "--macho", optRuntime, CD::runtimeMachO          );
	options.DefineSelectEnum( "-O",      optRuntime, CD::runtimeMachO          );
	
	options.DefineSelectEnum( "--classic", optMacAPI, CD::apiMacToolbox );
	options.DefineSelectEnum( "-B",        optMacAPI, CD::apiMacToolbox );
	options.DefineSelectEnum( "--carbon",  optMacAPI, CD::apiMacCarbon  );
	options.DefineSelectEnum( "-C",        optMacAPI, CD::apiMacCarbon  );
	
	options.DefineSelectEnum( "--debug",   optBuild, buildDebug   );
	options.DefineSelectEnum( "-g",        optBuild, buildDebug   );
	options.DefineSelectEnum( "--release", optBuild, buildRelease );
	options.DefineSelectEnum( "-R",        optBuild, buildRelease );
	options.DefineSelectEnum( "--demo",    optBuild, buildDemo    );
	options.DefineSelectEnum( "-D",        optBuild, buildDemo    );
	
	return options;
}

int O::Main( int argc, char const* const argv[] )
{
	if ( argc <= 1 )  return 0;
	
	int fail = 0;
	
	//N::SetOSStatusLoggingProc( MyOSStatusLogger );
	
	O::Options options = DefineOptions();
	options.GetOptions( argc, argv );
	
	const vector< const char* >& params = options.GetFreeParams();
	
	CD::Platform targetPlatform;
	BuildVariety buildVariety = buildDefault;
	
	if ( options.GetEnum( optArch ) != 0 )
	{
		targetPlatform.arch = CD::Architecture( options.GetEnum( optArch ) );
	}
	
	if ( options.GetEnum( optRuntime ) != 0 )
	{
		targetPlatform.runtime = CD::Runtime( options.GetEnum( optRuntime ) );
	}
	
	if ( options.GetEnum( optMacAPI ) != 0 )
	{
		targetPlatform.api = CD::MacAPI( options.GetEnum( optMacAPI ) );
	}
	
	if ( options.GetEnum( optBuild ) != 0 )
	{
		buildVariety = BuildVariety( options.GetEnum( optBuild ) );
	}
	
	gOptions.verbose = options.GetFlag( optVerbose );
	gOptions.catalog = options.GetFlag( optCatalog );
	
	gOptions.files = options.GetStringList( optFile );
	
	CD::AddPendingSubproject( UserProjectsFolder() );
	
	if ( gOptions.catalog )
	{
		//(void)GetProjectCatalog();
	}
	
	targetPlatform;
	
	for ( int i = 0;  i < params.size();  ++i )
	{
		const string& proj = params[ i ];
		
		gOptions.platform = targetPlatform;
		
		try
		{
			Project& project = GetProject( proj );
			BuildTarget( project, MakeTargetInfo( project, buildVariety ) );
			
			//Io::Out << "Executing commands\n";
			
			std::for_each( gCommands.begin(),
			               gCommands.end(),
			               N::PtrFun( ExecuteCommand ) );
			
			gCommands.clear();
		}
		catch ( NoSuchProject )
		{
			Io::Err << argv[ 0 ] << ": No such project " << q( proj ) << "\n";
			fail++;
		}
		catch ( CD::NoSuchProject )
		{
			Io::Err << argv[ 0 ] << ": No such project " << q( proj ) << "\n";
			fail++;
		}
		catch ( NoSuchUsedProject& ex )
		{
			Io::Err << argv[ 0 ] 
				<< ": No such project " << q( ex.used ) 
				<< " used by " << ex.projName << "\n";
			fail++;
		}
		catch ( BadSourceAlias& ex )
		{
			Io::Err << argv[ 0 ] 
				<< ": Unresolvable source alias " << q( NN::Convert< string >( ex.alias.name ) )
				<< " in " << ex.proj.Name() << "\n";
		}
		catch ( N::OSStatus err )
		{
			Io::Err << argv[ 0 ] << ": Unrecognized error " << err << " in " << proj << "\n";
		}
	}
	
	return fail != 0;
}

