/*	==========
 *	Process.cc
 *	==========
 */

#include "Genie/Process.hh"

// Standard C/C++
//#include <csignal>
#include <errno.h>
#include <signal.h>

// POSIX
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Sound.h"

// Backtrace
#include "Backtrace/StackCrawl.hh"
#include "Backtrace/MacsbugSymbols.hh"
#include "Backtrace/TracebackTables.hh"

// GetPathname
#include "GetPathname.hh"

// POSeven
#include "POSeven/Errno.hh"

// Kerosene/Common
#include "KEnvironment.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/TTY.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


#if defined(__MWERKS__) && defined(__csignal__)
	#undef SIGABRT
	#undef SIGFPE
	#undef SIGILL
	#undef SIGINT
	#undef SIGSEGV
	#undef SIGTERM
#endif


namespace Backtrace
{
	
	static std::string FindSymbolString( ReturnAddr68K addr )
	{
		MacsbugSymbolPtr symbolName = FindSymbolName( addr );
		
		return symbolName != NULL ? GetSymbolString( symbolName ) : "???";
	}
	
	static std::string FindSymbolString( ReturnAddrPPC addr )
	{
		TracebackTablePtr symbolName = FindSymbolName( addr );
		
		return symbolName != NULL ? GetSymbolString( symbolName ) : "???";
	}
	
	static void PrintTrace( unsigned offset, const void* addr, const char* arch, const char* name )
	{
		std::fprintf( stderr, "%d: 0x%.8x (%s) %s\n", offset, addr, arch, name );
	}
	
	
	static void Trace68K( unsigned offset, ReturnAddr68K addr )
	{
		std::string name = FindSymbolString( addr );
		
		PrintTrace( offset++, addr, "68K", name.c_str() );
	}
	
	static void TracePPC( unsigned offset, ReturnAddrPPC addr )
	{
		const ReturnAddrPPC mixedModeSwitch = (ReturnAddrPPC) 0xffcec400;
		
		std::string name = addr == mixedModeSwitch ? "MixedMode" : FindSymbolString( addr );
		
		PrintTrace( offset++, addr, "PPC", name.c_str() );
	}
	
}


static void DumpBacktrace( unsigned levelsToSkip = 0, const char* lastSymbol = "" )
{
	using Backtrace::CallRecord;
	
	const std::vector< CallRecord > trace = Backtrace::GetStackCrawl();
	
	typedef std::vector< CallRecord >::const_iterator Iter;
	
	const Iter begin = trace.begin() + levelsToSkip;
	
	unsigned offset = 0;
	
	// It's important to use < instead of != if we might skip past the end
	for ( Iter it = begin;  it < trace.end();  ++it, ++offset )
	{
		const CallRecord& call = *it;
		
		switch ( call.arch )
		{
			case Backtrace::kArchClassic68K:
				Backtrace::Trace68K( offset, call.addr68K );
				break;
			
			case Backtrace::kArchPowerPCCFM:
				Backtrace::TracePPC( offset, call.addrPPC );
				break;
			
			default:
				std::fprintf( stderr, "Trace: architecture %x for addres %.8x is unknown.\n", call.arch, call.addr68K );
				break;
		}
	}
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	namespace Sh = ShellShock;
	namespace K = Kerosene;
	
	
	GenieProcessTable gProcessTable;
	
	
	void SendSignalToProcessGroup( int sig, const ProcessGroup& group )
	{
		for ( GenieProcessTable::iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetProcessGroup().get() == &group )
			{
				proc.Raise( sig );
			}
		}
	}
	
	Process& GetProcess( pid_t pid )
	{
		if ( Process* process = FindProcess( pid ) )
		{
			return *process;
		}
		
		P7::ThrowErrno( ESRCH );
		
		// Not reached;
		throw;
	}
	
	Process* FindProcess( pid_t pid )
	{
		GenieProcessTable::iterator it = gProcessTable.Map().find( pid );
		
		if ( it == gProcessTable.end()  ||  it->second->GetLifeStage() == kProcessReleased )
		{
			return NULL;
		}
		
		return it->second.get();
	}
	
	
	static void* GetSystemCallFunctionPtr( const char* name )
	{
		return ( SystemCallsBegin() + LookUpSystemCallIndex( name ) )->function;
	}
	
	struct ToolScratchGlobals
	{
		int*    err;
		char**  env;
	};
	
	static ToolScratchGlobals& gToolScratchGlobals = *reinterpret_cast< ToolScratchGlobals* >( LMGetToolScratch() );
	
	static const char* FindSequenceInMemory( const char* mem_begin,
	                                         const char* mem_end,
	                                         const char* seq_begin,
	                                         const char* seq_end )
	{
		std::ptrdiff_t sequence_length = seq_end - seq_begin;
		
		while ( sequence_length <= mem_end - mem_begin )
		{
			if ( std::equal( seq_begin, seq_end, mem_begin ) )
			{
				return mem_begin;
			}
			
			++mem_begin;
		}
		
		return mem_end;
	}
	
	typedef long (*SetCurrentA4ProcPtr)();
	
	static SetCurrentA4ProcPtr GetCurrentA4Setter( void* mainPtr )
	{
		const char* codeBytes = (char*) mainPtr;
		
		const char* end = codeBytes + N::GetPtrSize( (Ptr) mainPtr );
		
		const std::ptrdiff_t patternOffset = 10;
		
		const UInt16 pattern[] = { 0x2008, 0xA055, 0xC18C, 0x4E75 };
		
		const char* pattern_location = FindSequenceInMemory( codeBytes,
		                                                     end,
		                                                     (const char*) pattern,
		                                                     (const char*) pattern + sizeof pattern );
		
		if ( pattern_location == end )
		{
			throw N::ParamErr();  // FIXME
		}
		
		const char* address_of_SetCurrentA4 = pattern_location - patternOffset;
		
		SetCurrentA4ProcPtr setCurrentA4 = (SetCurrentA4ProcPtr) address_of_SetCurrentA4;
		
		return setCurrentA4;
	}
	
	int ExternalProcessExecutor::operator()( ThreadContext& context ) const
	{
		context.processContext->InitThread();
		
		RegisterProcessContext( context.processContext );
		
		int result = -1;
		
		MainProcPtr mainPtr = context.externalMain;
		
		if ( mainPtr != NULL )
		{
			context.processContext->SetSchedule( kProcessRunning );
			
		#if 0 // TARGET_CPU_68K && !TARGET_RT_MAC_CFM
			
			ProcInfoType mainProcInfo = kCStackBased
			                          | RESULT_SIZE( SIZE_CODE( sizeof (int) ) )
			                          | STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof (int) ) )
			                          | STACK_ROUTINE_PARAMETER( 2, SIZE_CODE( sizeof (char**) ) )
			                          | STACK_ROUTINE_PARAMETER( 3, SIZE_CODE( sizeof (char**) ) );
			
			UniversalProcPtr upp = NewRoutineDescriptorTrap( (ProcPtr) mainPtr,
			                                                 mainProcInfo,
			                                                 kM68kISA | kCFM68kRTA );
			
			mainPtr = (MainProcPtr) upp;
			
		#endif
			
			gToolScratchGlobals.err = NULL;  // errno
			gToolScratchGlobals.env = context.processContext->Environ();  // environ
			
		#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
			
			{
				void* outOfBandData[3] = { GetSystemCallFunctionPtr, NULL, NULL };
				
				LMSetApplScratch( outOfBandData );
				
				SetCurrentA4ProcPtr setCurrentA4 = GetCurrentA4Setter( mainPtr );
				
				setCurrentA4();
			}
			
		#endif
			
			result = mainPtr( Sh::CountStringArray( context.argv ),
			                  context.argv,
			                  context.envp );
			
		#if 0 // TARGET_CPU_68K && !TARGET_RT_MAC_CFM
			
			DisposeRoutineDescriptorTrap( upp );
			
		#endif
		}
		
		// Accumulate any user time between last system call (if any) and return from main()
		context.processContext->EnterSystemCall( "*RETURN*" );
		
		context.processContext->Terminate( (result & 0xFF) << 8 );
		
		// Not reached
		return result;
	}
	
	struct ExecContext
	{
		FSSpec                      executable;
		std::vector< const char* >  argVector;
		std::string                 scriptPath;
		std::string                 interpreterPath;
		std::string                 interpreterArg;
		
		ExecContext()  {}
		
		ExecContext( const FSSpec&       executable,
		             char const* const*  argv )
		:
			executable( executable ),
			argVector ( argv, argv + Sh::CountStringArray( argv ) + 1 )
		{}
	};
	
	inline P7::Errno NotExecutable()  { return P7::Errno( EPERM ); }
	
	static ExecContext& Normalize( ExecContext& context, const FSTreePtr& cwd )
	{
		const OSType type = N::FSpGetFInfo( context.executable ).fdType;
		
		if ( type == 'Wish' )
		{
			return context;  // Already normalized
		}
		
		if ( type == 'TEXT' )
		{
			context.interpreterPath = "/bin/sh";  // default
			bool hasArg = false;
			
			NN::Owned< N::FSFileRefNum > script = io::open_for_reading( context.executable );
			
			char data[ 1024 + 1 ];
			data[1024] = '\0';
			
			size_t bytes = N::FSRead( script, 1024, data );
			
			N::FSClose( script );
			
			if ( bytes > 2 && data[0] == '#' && data[1] == '!' )
			{
				char* cr = std::strchr( data, '\r' );
				char* lf = std::strchr( data, '\n' );
				
				char* nl = (cr && lf) ? std::min( cr, lf ) : (cr ? cr : lf);
				
				if ( nl == NULL )
				{
					throw NotExecutable();  // #! line too long
				}
				
				*nl = '\0';
				
				char* space = std::strchr( data, ' ' );
				
				hasArg = space;
				
				context.interpreterPath = std::string( &data[2], space ? space : nl );
				context.interpreterArg  = std::string( space ? space + 1 : nl, nl );
			}
			
			const int newTokenCount = 1 + hasArg;
			
			// E.g. "$ script foo bar baz"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			context.argVector.resize( context.argVector.size() + newTokenCount );
			
			const char* const* const argv = &context.argVector.front();
			
			// argv == { "script", "foo", "bar", "baz", NULL, ?? }
			
			bool pathSearched = std::strchr( argv[0], '/' ) == NULL;
			
			const int skipCount = pathSearched;  // skip the script's name if we're overwriting it anyway
						
			std::copy_backward( context.argVector.begin() + skipCount,
			                    context.argVector.end() - newTokenCount,
			                    context.argVector.end() );
			
			// argv == { "script", "script", "foo", "bar", "baz", NULL }
			
			context.argVector[ 0 ] = context.interpreterPath.c_str();
			
			if ( hasArg )
			{
				context.argVector[ 1 ] = context.interpreterArg.c_str();
			}
			
			if ( pathSearched )
			{
				// Overwrite with full pathname
				context.scriptPath = GetPOSIXPathname( context.executable );
				
				context.argVector[ 1 + hasArg ] = context.scriptPath.c_str();
			}
			
			// argv == { "sh", "script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolvePathname( context.interpreterPath, cwd )->GetFSSpec();
		}
		else if ( type == 'MPST' )
		{
			context.scriptPath = GetMacPathname( context.executable );
			
			const int newTokenCount = 3;
			const int skipCount = 1;  // skip the script's name because we're overwriting it anyway
			
			// E.g. "$ script foo bar"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			context.argVector.resize( context.argVector.size() + newTokenCount );
			
			const char* const* const argv = &context.argVector.front();
			
			// argv == { "script", "foo", "bar", "baz", NULL, ??, ?? }
			
			std::copy_backward( context.argVector.begin() + skipCount,
			                    context.argVector.end() - newTokenCount,
			                    context.argVector.end() );
			
			// argv == { "script", "foo", "bar", "foo", "bar", "baz", NULL }
			
			context.argVector[ 0 ] = "/usr/bin/tlsrvr";
			context.argVector[ 1 ] = "--escape";
			context.argVector[ 2 ] = "--";
			context.argVector[ 3 ] = context.scriptPath.c_str();  // Overwrite with full pathname
			
			// argv == { "sh", "--", "/usr/bin/script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolvePathname( "/usr/bin/tlsrvr", cwd )->GetFSSpec();
		}
		else
		{
			throw NotExecutable();
		}
		
		return context;
	}
	
	
	static boost::shared_ptr< Session > NewSession( pid_t sid )
	{
		return boost::shared_ptr< Session >( new Session( sid ) );
	}
	
	static boost::shared_ptr< ProcessGroup > NewProcessGroup( pid_t pgid, const boost::shared_ptr< Session >& session )
	{
		return boost::shared_ptr< ProcessGroup >( new ProcessGroup( pgid, session ) );
	}
	
	static boost::shared_ptr< ProcessGroup > NewProcessGroup( pid_t pgid )
	{
		return NewProcessGroup( pgid, NewSession( pgid ) );
	}
	
	class NoSuchProcessGroup {};
	
	static boost::shared_ptr< ProcessGroup > FindProcessGroup( pid_t pgid )
	{
		for ( GenieProcessTable::iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetPGID() == pgid )
			{
				return proc.GetProcessGroup();
			}
		}
		
		throw NoSuchProcessGroup();
	}
	
	boost::shared_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, const boost::shared_ptr< Session >& session )
	{
		try
		{
			boost::shared_ptr< ProcessGroup > pgrp = FindProcessGroup( pgid );
			
			if ( pgrp->GetSession() != session )
			{
				P7::ThrowErrno( EPERM );
			}
			
			return pgrp;
		}
		catch ( const NoSuchProcessGroup& )
		{
			return NewProcessGroup( pgid, session );
		}
	}
	
	Process::Process( RootProcess ) 
	:
		itsPPID               ( 0 ),
		itsPID                ( gProcessTable.NewProcess( this ) ),
		itsForkedChildPID     ( 0 ),
		itsLongJmp            ( NULL ),
		itsProcessGroup       ( NewProcessGroup( itsPID ) ),
		itsTracingProcess     ( 0 ),
		itsAlarmClock         ( 0 ),
		itsLastTimerCheckpoint( 0 ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( "init" ),
		itsCWD                ( FSRoot() ),
		itsFileDescriptors    ( FileDescriptorMap() ),
		itsLifeStage          ( kProcessLive ),
		itsInterdependence    ( kProcessIndependent ),
		itsSchedule           ( kProcessSleeping ),
		itsResult             ( 0 ),
		itsEnvironStorage     ( new Sh::VarArray() ),
		itsCleanupHandler     (),
		itsErrnoData          ( NULL ),
		itsEnvironData        ( NULL )
	{
		itsFileDescriptors[ 0 ] =
		itsFileDescriptors[ 1 ] =
		itsFileDescriptors[ 2 ] = GetSimpleDeviceHandle( "null" );
	}
	
	Process::Process( Process& parent ) 
	:
		itsPPID               ( parent.GetPID() ),
		itsPID                ( gProcessTable.NewProcess( this ) ),
		itsForkedChildPID     ( 0 ),
		itsLongJmp            ( NULL ),
		itsProcessGroup       ( parent.GetProcessGroup() ),
		itsTracingProcess     ( 0 ),
		itsAlarmClock         ( 0 ),
		itsLastTimerCheckpoint( N::Microseconds() ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( parent.ProgramName() ),
		itsCWD                ( parent.GetCWD() ),
		itsFileDescriptors    ( parent.FileDescriptors() ),
		itsLifeStage          ( kProcessStarting ),
		itsInterdependence    ( kProcessForked ),
		itsSchedule           ( kProcessRunning ),
		itsResult             ( 0 ),
		itsEnvironStorage     ( new Sh::VarArray( parent.itsEnvironStorage->GetPointer() ) ),
		itsCleanupHandler     (),
		itsErrnoData          ( TARGET_RT_MAC_CFM ? parent.itsErrnoData : NULL ),
		itsEnvironData        ( parent.itsEnvironData )
	{
		parent.itsForkedChildPID = itsPID;
		
		if ( itsEnvironData == NULL )
		{
			itsEnvironData = &gToolScratchGlobals.env;
		}
		
		// The environment data and the pointer to the fragment's environ
		// variable have been copied from the parent.
		// The calling fragment is now associated with the new process.
		// Update its environ to point to our storage.
		if ( itsEnvironData != NULL )
		{
			*itsEnvironData = itsEnvironStorage->GetPointer();
		}
		
		RegisterProcessContext( this );
		
		parent.itsInterdependence = kProcessForking;
		parent.itsSchedule        = kProcessFrozen;
	}
	
	Process::~Process()
	{
	}
	
	void Process::AccumulateChildTimes( const Times& times )
	{
		itsTimes.child_user   += times.user   + times.child_user;
		itsTimes.child_system += times.system + times.child_system;
	}
	
	unsigned int Process::SetAlarm( unsigned int seconds )
	{
		UInt64 now = N::Microseconds();
		
		unsigned int remainder = 0;
		
		if ( itsAlarmClock )
		{
			remainder = (itsAlarmClock - now) / 1000000 + 1;
		}
		
		itsAlarmClock = seconds ? now + seconds * 1000000 : 0;
		
		return remainder;
	}
	
	void Process::InitThread()
	{
		N::CloseConnection( itsOldFragmentConnection );
		
		itsOldFragmentImage = BinaryImage();
	}
	
	template < class Type >
	bool LoadSymbol( N::CFragConnectionID fragment, ConstStr255Param symName, const Type& init )
	{
		
		try
		{
			Type* symbol = NULL;
			
			N::FindSymbol( fragment, symName, &symbol );
			
			*symbol = init;
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	class SymbolImporter
	{
		private:
			N::CFragConnectionID itsFragmentConnection;
		
		public:
			SymbolImporter( N::CFragConnectionID conn ) : itsFragmentConnection( conn )  {}
			
			bool operator()( SystemCallRegistry::value_type systemCall ) const
			{
				std::string name = systemCall.name;
				void*       func = systemCall.function;
				
				name += "_import_";
				
				return LoadSymbol( itsFragmentConnection, N::Str255( name ), func );
			}
	};
	
	static void ImportSystemCalls( N::CFragConnectionID fragmentConnection )
	{
		std::for_each( SystemCallsBegin(),
		               SystemCallsEnd(),
		               SymbolImporter( fragmentConnection ) );
	}
	
	static void CloseMarkedFileDescriptors( FileDescriptorMap& fileDescriptors )
	{
		// Close file descriptors with close-on-exec flag.
		typedef FileDescriptorMap::iterator FDIter;
		
		std::vector< int > toClose;
		
		for ( FDIter it = fileDescriptors.begin();  it != fileDescriptors.end();  ++it )
		{
			if ( it->second.closeOnExec )
			{
				toClose.push_back( it->first );
			}
		}
		
		typedef std::vector< int >::const_iterator ToCloseIter;
		
		for ( ToCloseIter it = toClose.begin();  it != toClose.end();  ++it )
		{
			fileDescriptors.erase( *it );
		}
	}
	
	NN::Owned< N::ThreadID >  Process::Exec( const FSSpec&        executable,
	                                         const char* const    argv[],
	                                         const char* const*   envp )
	{
		CloseMarkedFileDescriptors( itsFileDescriptors );
		
		// Do we take the name before or after normalization?
		ProgramName( NN::Convert< std::string >( executable.name ) );
		
		ExecContext context( executable, argv );
		
		Normalize( context, GetCWD() );
		
		itsProgramFile = context.executable;
		
		// Save the fragment image and the connection that we're running from.
		// We can't use stack storage because we run the risk of the thread terminating.
		itsOldFragmentImage      = itsFragmentImage;
		itsOldFragmentConnection = itsFragmentConnection;
		
		{
			BinaryImage binary = GetBinaryImage( itsProgramFile );
		}
		
		itsFragmentImage = GetBinaryImage( itsProgramFile );
		
		if ( !TARGET_CPU_68K || TARGET_RT_MAC_CFM )
		{
			itsFragmentConnection = N::GetMemFragment< N::kPrivateCFragCopy >( itsFragmentImage.Get(),
			                                                                   N::GetPtrSize( itsFragmentImage ) );
		}
		
		K::Versions assumedVersions;
		
		assumedVersions.current        = 2;
		assumedVersions.lastCompatible = 2;
		
		K::Versions* versions = &assumedVersions;
		
		MainProcPtr mainEntryPoint = NULL;
		
		if ( TARGET_CPU_68K && !TARGET_RT_MAC_CFM )
		{
			mainEntryPoint = (MainProcPtr) itsFragmentImage.Get().Get();
		}
		else
		{
			N::FindSymbol( itsFragmentConnection, "\p" "main", &mainEntryPoint );
		}
		
	#if 0
		
		try
		{
			N::FindSymbol( itsFragmentConnection, "\p" "gVersions", &versions );
		}
		catch ( ... )
		{
			// The practice of exporting main() postdates that of exporting gVersions.
			// Therefore, we can conclude that a drop-in which exports main() but omits
			// gVersions does so deliberately, whereas one which omits both we must
			// assume to be indefinitely old.
			// The latter case currently results in a warning, and subsequently
			// (once the protocol incompatibly changes) an error.
			// But the former indicates a program with zero dependence on the
			// kernel interface, and therefore ought not be version-checked.
			// (If such a program supplied version information, then a future version
			// of Genie with kLastCompatibleLibVersion augmented would needlessly
			// refuse to run it.)
			if ( mainEntryPoint == NULL )
			{
				//Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
				
				//Err << "No version data found.\n";
			}
		}
		
		using K::kCurrentVersion;
		using K::kLastCompatibleLibVersion;
		
		if ( kCurrentVersion > versions->current )
		{
			/*
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is older than what we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			*/
			
			if ( versions->current < kLastCompatibleLibVersion )
			{
				// It's too old
				//Err << "The plugin is too old\n";
				throw N::CFragImportTooOldErr();
			}
		}
		else if ( kCurrentVersion < versions->current )
		{
			/*
			Io::Stream< IORef > Err = FileDescriptors()[ 2 ].handle;
			
			// Plugin interface is newer than we know
			Err << "Plugin uses interface version "
			    << versions->current
			    << "\n";
			*/
			
			if ( kCurrentVersion < versions->lastCompatible )
			{
				// We're too old
				//Err << "The plugin is too new\n";
				throw N::CFragImportTooNewErr();
			}
		}
		
	#endif
		
		itsArgvStorage.reset( new Sh::StringArray( &context.argVector[ 0 ] ) );
		
		itsErrnoData = NULL;
		
		// We need to set the calling fragment's environ back to the parent
		// process' environment storage.
		if ( itsEnvironData != NULL )
		{
			*itsEnvironData = itsPPID > 0 ? GetProcess( itsPPID ).itsEnvironStorage->GetPointer()
			                              : NULL;
		}
		
		// Reset the environment storage as a copy of envp (or empty).
		// As an optimization, if the pointers match, then envp must be the
		// current environ, and the data are already there.
		// In that case, we just update the new fragment's environ to point to us.
		
		if ( envp == NULL )
		{
			itsEnvironStorage.reset( new Sh::VarArray );
		}
		else if ( envp != itsEnvironStorage->GetPointer() )
		{
			itsEnvironStorage.reset( new Sh::VarArray( envp ) );
		}
		
	#if TARGET_RT_MAC_CFM
		
		try
		{
			itsEnvironData = NULL;
			
			N::FindSymbol( itsFragmentConnection, "\p" "environ", &itsEnvironData );
			
			*itsEnvironData = itsEnvironStorage->GetPointer();
		}
		catch ( ... ) {}
		
		try
		{
			N::FindSymbol( itsFragmentConnection, "\p" "errno", &itsErrnoData );
		}
		catch ( ... ) {}
		
		ImportSystemCalls( itsFragmentConnection );
		
	#endif
		
		Result( 0 );
		
		ThreadContext threadContext( this,
		                             mainEntryPoint,
		                             itsArgvStorage->GetPointer(),
		                             itsEnvironStorage->GetPointer() );
		
		// We always spawn a new thread for the exec'ed process.
		// If we've forked, then the thread is null, but if not, it's the
		// current thread -- be careful!
		
		// Create the new thread
		std::auto_ptr< Thread > newThread( new Thread( threadContext ) );
		
		// Save the old thread
		NN::Owned< N::ThreadID > savedThreadID;
		
		if ( itsThread.get() )
		{
			savedThreadID = itsThread->HandOff();
		}
		
		// Make the new thread belong to this process
		itsThread = newThread;
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessSleeping;
		
		if ( IsBeingTraced() )
		{
			Raise( SIGSTOP );
			
			// We may have received a signal while stopped
			HandlePendingSignals();
		}
		
		SuspendTimer();
		
		return savedThreadID;
	}
	
	/*
	int Process::TryExec( const FSSpec&        executable,
	                      const char* const    argv[],
	                      const char* const*   envp )
	{
		try
		{
			return Exec( executable, argv, envp );
		}
		catch ( N::OSStatusErrMessage< N::CFragNoLibraryErr >& err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "cFragNoLibraryErr: "
			                                       << err.errMessage
			                                       << "\n";
			throw;
		}
		catch ( N::OSStatusErrMessage< N::CFragUnresolvedErr >& err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "cFragUnresolvedErr: "
			                                       << err.errMessage
			                                       << "\n";
			throw;
		}
		catch ( N::ErrMessage& err )
		{
			try
			{
				throw;
			}
			catch ( N::OSStatus status )
			{
				Io::S( FileDescriptors()[ 2 ].handle ) << "Caught OSStatus "
				                                       << status
				                                       << " with errMessage "
				                                       << err.errMessage
				                                       << ".\n";
			}
			catch ( ... )
			{
				Io::S( FileDescriptors()[ 2 ].handle ) << "Caught exception with errMessage "
				                                       << err.errMessage
				                                       << " which isn't an OSStatus.  This shouldn't happen.\n";
			}
			
			throw;
		}
		catch ( N::OSStatus err )
		{
			Io::S( FileDescriptors()[ 2 ].handle ) << "Couldn't load plugin:  " << err << "\n";
			throw;
		}
		
		// Not reached...
		return -1;
	}
	*/
	
	int Process::SetErrno( int errorNumber )
	{
		if ( itsErrnoData == NULL )
		{
			itsErrnoData = gToolScratchGlobals.err;
		}
		
		if ( itsErrnoData != NULL )
		{
			*itsErrnoData = errorNumber;
		}
		
		return errorNumber == 0 ? 0 : -1;
	}
	
	char* Process::GetEnv( const char* name )
	{
		char* result = NULL;
		
		Sh::SVector::const_iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		const char* end = Sh::EndOfVarName( var );
		
		// Did we find the right environment variable?
		if ( end != NULL  &&  *end == '='  &&  Sh::VarMatchesName( var, end, name ) )
		{
			itsLastEnv = var;
			itsLastEnv += "\0";  // make sure we have a trailing null
			
			std::size_t offset = end + 1 - var;
			
			result = &itsLastEnv[ offset ];
		}
		
		return result;
	}
	
	void Process::SetEnv( const char* name, const char* value, bool overwrite )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, Sh::MakeVar( name, value ) );
			
			if ( itsEnvironData )
			{
				*itsEnvironData = itsEnvironStorage->GetPointer();
			}
		}
		else if ( overwrite )
		{
			itsEnvironStorage->Overwrite( it, Sh::MakeVar( name, value ) );
		}
	}
	
	void Process::PutEnv( const char* string )
	{
		std::string name = string;
		name.resize( name.find( '=' ) );
		
		Sh::SVector::iterator it = itsEnvironStorage->Find( name.c_str() );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name.c_str() );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, string );
			
			if ( itsEnvironData )
			{
				*itsEnvironData = itsEnvironStorage->GetPointer();
			}
		}
		else
		{
			itsEnvironStorage->Overwrite( it, string );
		}
	}
	
	void Process::UnsetEnv( const char* name )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		if ( match )
		{
			itsEnvironStorage->Remove( it );
		}
	}
	
	void Process::ClearEnv()
	{
		itsEnvironStorage->Clear();
		
		if ( itsEnvironData )
		{
			*itsEnvironData = NULL;
		}
	}
	
	void Process::ChangeDirectory( const FSTreePtr& newCWD )
	{
		if ( !newCWD->IsDirectory() )
		{
			P7::ThrowErrno( ENOTDIR );
		}
		
		itsCWD = newCWD;
	}
	
	void Process::ResumeAfterFork()
	{
		//Status( kProcessRunning );
		itsInterdependence = Forked() ? kProcessForked
		                              : kProcessIndependent;
		
		itsSchedule        =            kProcessRunning;
		
		RegisterProcessContext( this );
		
		pid_t child = itsForkedChildPID;
		
		itsForkedChildPID = 0;
		
		if ( itsLongJmp != NULL  &&  child != 0 )
		{
			itsLongJmp( child );
		}
	}
	
	void Process::UsurpParent( int exit_status )
	{
		Process& parent = GetProcess( GetPPID() );
		
		ASSERT( itsLifeStage == kProcessStarting );
		
		ASSERT( parent.itsInterdependence == kProcessForking );
		ASSERT(        itsInterdependence == kProcessForked  );
		
		ASSERT( parent.itsSchedule == kProcessFrozen );
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessRunning;
		
		itsPPID = 1;  // temporary hack to avoid improper SIGHUP
		
		itsThread = parent.itsThread;
		
		parent.Terminate( exit_status << 8 );
	}
	
	void Process::Terminate()
	{
		itsLifeStage = kProcessTerminated;
		itsSchedule = kProcessUnscheduled;
		
		itsFileDescriptors.clear();
		
		if ( itsCleanupHandler != NULL )
		{
			itsCleanupHandler();
		}
		
		pid_t ppid = GetPPID();
		pid_t pid  = GetPID();
		pid_t sid  = GetSID();
		
		bool isSessionLeader = pid == sid;
		
		if ( isSessionLeader )
		{
			try
			{
				const boost::shared_ptr< IOHandle >& ctty = ControllingTerminal();
				
				if ( ctty.get() != NULL )
				{
					TTYHandle& tty = IOHandle_Cast< TTYHandle >( *ctty );
					
					tty.SaveLeaderWaitStatus( itsResult );
				}
			}
			catch ( ... )
			{
			}
		}
		
		if ( ppid > 1 )
		{
			GetProcess( ppid ).Raise( SIGCHLD );
		}
		else
		{
			Release();
		}
		
		typedef GenieProcessTable::ProcessMap::const_iterator const_iterator;
		
		for ( const_iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *( *it ).second;
			
			if ( isSessionLeader  &&  proc.GetSID() == pid )
			{
				proc.Raise( SIGHUP );
			}
			
			if ( proc.GetPPID() == pid )
			{
				proc.Orphan();
			}
		}
		
		if ( !Forked()  &&  itsThread.get() != NULL )
		{
			NN::Owned< N::ThreadID > savedThreadID = itsThread->HandOff();
		}
	}
	
	void Process::Terminate( int result )
	{
		itsResult = result;
		
		Terminate();
	}
	
	void Process::Orphan()
	{
		ASSERT( itsPID != 1 );
		
		itsPPID = 1;
		
		if ( itsLifeStage == kProcessTerminated )
		{
			Release();
		}
	}
	
	void Process::Release()
	{
		ASSERT( itsLifeStage == kProcessTerminated );
		
		itsPPID = 0;  // Don't match PPID comparisons
		itsLifeStage = kProcessReleased;
	}
	
	static void UpdateClock( UInt64& clock, UInt64& checkpoint )
	{
		UInt64 now = N::Microseconds();
		
		clock += now - checkpoint;
		
		checkpoint = now;
	}
	
	void Process::EnterSystemCall( const char* name )
	{
		UpdateClock( itsTimes.user, itsLastTimerCheckpoint );
	}
	
	void Process::LeaveSystemCall()
	{
		UpdateClock( itsTimes.system, itsLastTimerCheckpoint );
	}
	
	void Process::SuspendTimer()
	{
		UpdateClock( itsTimes.system, itsLastTimerCheckpoint );
	}
	
	void Process::ResumeTimer()
	{
		UInt64 now = N::Microseconds();
		
		itsLastTimerCheckpoint = now;
	}
	
	void Process::SetSchedule( ProcessSchedule schedule )
	{
		UInt64 now = N::Microseconds();
		
		if ( schedule == kProcessRunning )
		{
			if ( itsSchedule != kProcessRunning )
			{
				// starting
				itsLastTimerCheckpoint = now;
			}
		}
		else if ( itsSchedule == kProcessRunning )
		{
			// stopping
			UpdateClock( itsTimes.system, itsLastTimerCheckpoint );
		}
		
		itsSchedule = schedule;
	}
	
	sig_t Process::SetSignalAction( int signal, sig_t signalAction )
	{
		if ( signal == SIGKILL  ||  signal == SIGSTOP  ||  signalAction == SIG_ERR )
		{
			//errno = EINVAL;  // FIXME
			return SIG_ERR;
		}
		
		sig_t result = itsSignalMap[ signal ];
		
		itsSignalMap[ signal ] = signalAction;
		
		return result;
	}
	
	void Process::Raise( int signal )
	{
		if ( itsLifeStage >= kProcessTerminated  ||  itsResult != 0 )
		{
			return;
		}
		
		sig_t action = itsSignalMap[ signal ];
		
		if ( action == SIG_IGN )
		{
			return;
		}
		
		if ( action == SIG_DFL )
		{
			if ( GetPID() == 1 )
			{
				return;
			}
			
			switch ( signal )
			{
				case SIGQUIT:
				case SIGILL:
				case SIGTRAP:
				case SIGABRT:
				//case SIGEMT:
				case SIGFPE:
				case SIGBUS:
				case SIGSEGV:
				case SIGSYS:
					// create core image
					signal |= 0x80;
					// fall through
					//break;
				case SIGHUP:
				case SIGINT:
				case SIGKILL:
				case SIGPIPE:
				case SIGALRM:
				case SIGTERM:
				case SIGXCPU:
				case SIGXFSZ:
				case SIGVTALRM:
				case SIGPROF:
				case SIGUSR1:
				case SIGUSR2:
					// terminate process
					itsResult = signal;  // indicates fatal signal
					Continue();  // Wake the thread if it's stopped so it can die
					break;
				
				case SIGCONT:
					Continue();
					break;
				
				case SIGURG:
				case SIGCHLD:
				case SIGIO:
				case SIGWINCH:
				//case SIGINFO:
					// discard signal
					break;
				
				case SIGSTOP:
				case SIGTSTP:
				case SIGTTIN:
				case SIGTTOU:
					// stop process
					Stop();
					break;
				default:
					// bad signal
					break;
			}
		}
		else
		{
			// FIXME:  Block this signal during the function call
			itsPendingSignals |= 1 << signal - 1;
			
			Continue();
		}
	}
	
	bool Process::HandlePendingSignals()
	{
		if ( itsAlarmClock )
		{
			UInt64 now = N::Microseconds();
			
			if ( now > itsAlarmClock )
			{
				Raise( SIGALRM );
			}
		}
		
		if ( itsResult != 0 )
		{
			// Fatal signal received.  Terminate.
			if ( WCOREDUMP( itsResult ) )
			{
				DumpBacktrace( 1, "main" );
			}
			
			if ( itsInterdependence == kProcessForked )
			{
				Process& parent( GetProcess( itsPPID ) );
				
				Orphan();
				
				Terminate();
				
				parent.Terminate( itsResult );  // FIXME
				
				// Not reached
			}
			
			Terminate();
			
			// Not reached
		}
		
		itsPreviousSignals = itsPendingSignals;
		
		UInt32 bits = itsPendingSignals;
		int signal = 1;
		
		while ( bits )
		{
			if ( bits & 1 )
			{
				sig_t action = itsSignalMap[ signal ];
				
				ASSERT( action != SIG_IGN );
				ASSERT( action != SIG_DFL );
				
				action( signal );
				
				itsPendingSignals &= ~( 1 << signal - 1 );
			}
			
			bits >>= 1;
			++signal;
		}
		
		return itsPreviousSignals;
	}
	
	GenieProcessTable::GenieProcessTable() : itsNextPID( 1 )
	{
		new Process( Process::RootProcess() );
	}
	
	static void ReapProcesses()
	{
		std::vector< pid_t > hitlist;
		
		for ( GenieProcessTable::iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			pid_t pid = proc.GetPID();
			
			if ( proc.GetLifeStage() == kProcessReleased )
			{
				hitlist.push_back( pid );
			}
		}
		
		for ( std::vector< pid_t >::iterator it = hitlist.begin();  it != hitlist.end();  ++it )
		{
			pid_t pid = *it;
			
			gProcessTable.RemoveProcess( pid );
		}
	}
	
	namespace
	{
		
		void ReaperThreadEntry()
		{
			while ( true )
			{
				ReapProcesses();
				
				N::YieldToAnyThread();
			}
		}
		
	}
	
	pid_t GenieProcessTable::NewProcess( Process* process )
	{
		static NN::Owned< N::ThreadID > reaper = N::NewThread< ReaperThreadEntry >( N::kCooperativeThread );
		
		itsProcesses[ itsNextPID ] = boost::shared_ptr< Process >( process );
		
		return itsNextPID++;
	}
	
	void GenieProcessTable::RemoveProcess( pid_t pid )
	{
		iterator it = itsProcesses.find( pid );
		
		if ( it == itsProcesses.end() )
		{
			N::SysBeep();
		}
		else
		{
			itsProcesses.erase( it );
		}
	}
	
	void GenieProcessTable::KillAll()
	{
		for ( ProcessMap::iterator it = itsProcesses.begin();  it != itsProcesses.end();  ++it )
		{
			Process& proc = *it->second;
			
			proc.Raise( SIGKILL );
		}
		
		while ( itsProcesses.size() > 1 )
		{
			N::YieldToAnyThread();
		}
	}
	
	void Process::Stop()
	{
		ASSERT( itsSchedule == kProcessRunning );
		
		itsSchedule = kProcessStopped;
		
		SuspendTimer();
		
		StopThread( itsThread->Get() );
	}
	
	void Process::Continue()
	{
		ASSERT( itsThread.get() != NULL );
		
		if ( N::GetThreadState( itsThread->Get() ) == N::kStoppedThreadState )
		{
			ASSERT( itsSchedule == kProcessStopped );
			
			itsSchedule = kProcessSleeping;
			
			N::SetThreadState( itsThread->Get(), N::kReadyThreadState );
		}
	}
	
}

