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

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Sound.h"
#include "Nitrogen/Threads.h"

// Backtrace
#include "Backtrace/Backtrace.hh"

// GetPathname
#include "GetPathname.hh"

// POSeven
#include "POSeven/Errno.hh"

// Kerosene/Common
#include "KEnvironment.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemConsole.hh"
#include "Genie/Yield.hh"


#if defined(__MWERKS__) && defined(__csignal__)
	#undef SIGABRT
	#undef SIGFPE
	#undef SIGILL
	#undef SIGINT
	#undef SIGSEGV
	#undef SIGTERM
#endif


#define ENVIRON_IS_SHARED  (TARGET_CPU_68K && !TARGET_RT_MAC_CFM)


static void DumpBacktrace()
{
	using namespace Backtrace;
	
	std::vector< ReturnAddress > stackCrawl = MakeStackCrawl();
	
	std::vector< ReturnAddress >::const_iterator begin = stackCrawl.begin();
	std::vector< ReturnAddress >::const_iterator end   = stackCrawl.end();
	
	++begin;  // skip DumpBacktrace( void )
	
	std::vector< CallInfo > callChain;
	
	callChain.reserve( end - begin );
	
	for ( ;  begin < end;  ++begin )
	{
		callChain.push_back( GetCallInfoFromReturnAddress( *begin ) );
		
		if ( callChain.back().itsUnmangledName == "main" )
		{
			break;
		}
	}
	
	std::string report = MakeReportFromCallChain( callChain.begin(),
	                                              callChain.end() );
	
	(void) Genie::WriteToSystemConsole( report.data(), report.size() );
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Sh = ShellShock;
	namespace K = Kerosene;
	namespace Ped = Pedestal;
	
	
	static Process* gCurrentProcess;
	
	Process& CurrentProcess()
	{
		if ( gCurrentProcess == NULL )
		{
			p7::throw_errno( ESRCH );
		}
		
		return *gCurrentProcess;
	}
	
	
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
		
		p7::throw_errno( ESRCH );
		
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
		int*             err;
		iota::environ_t  env;
	};
	
	static ToolScratchGlobals& gToolScratchGlobals = *reinterpret_cast< ToolScratchGlobals* >( LMGetToolScratch() );
	
	inline void SwapInEnvironValue( iota::environ_t envp )
	{
		if ( ENVIRON_IS_SHARED )
		{
			gToolScratchGlobals.env = envp;
		}
	}
	
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
		
		int exit_status = -1;
		
		MainProcPtr mainPtr = context.externalMain;
		
		ASSERT( mainPtr != NULL );
		
		gToolScratchGlobals.err = NULL;  // errno
		
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		{
			void* outOfBandData[3] = { GetSystemCallFunctionPtr, NULL, NULL };
			
			LMSetApplScratch( outOfBandData );
			
			SetCurrentA4ProcPtr setCurrentA4 = GetCurrentA4Setter( mainPtr );
			
			setCurrentA4();
		}
		
	#endif
		
		exit_status = mainPtr( Sh::CountStringArray( context.argv ),
		                       context.argv,
		                       context.envp );
		
		// Accumulate any user time between last system call (if any) and return from main()
		context.processContext->EnterSystemCall( "*RETURN*" );
		
		context.processContext->Exit( exit_status );
		
		// Not reached
		return exit_status;
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
	
	inline p7::errno_t NotExecutable()  { return p7::errno_t( EPERM ); }
	
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
				char* end = data + bytes;
				
				char* cr = std::find( data, end, '\r' );
				char* lf = std::find( data, end, '\n' );
				
				char* nl = std::min( cr, lf );
				
				if ( nl == end )
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
				p7::throw_errno( EPERM );
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
		itsProcessGroup       ( NewProcessGroup( itsPID ) ),
		itsStackFramePtr      ( NULL ),
		itsTracingProcess     ( 0 ),
		itsAlarmClock         ( 0 ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( "init" ),
		itsCWD                ( FSRoot() ),
		itsFileDescriptors    ( FileDescriptorMap() ),
		itsLifeStage          ( kProcessLive ),
		itsInterdependence    ( kProcessIndependent ),
		itsSchedule           ( kProcessSleeping ),
		itsResult             ( 0 ),
		itsCleanupHandler     (),
		itsErrnoData          ( NULL )
	{
		itsFileDescriptors[ 0 ] =
		itsFileDescriptors[ 1 ] =
		itsFileDescriptors[ 2 ] = GetSimpleDeviceHandle( "null" );
	}
	
	Process::Process( Process& parent ) 
	:
		Environ( parent ),
		itsPPID               ( parent.GetPID() ),
		itsPID                ( gProcessTable.NewProcess( this ) ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( parent.GetProcessGroup() ),
		itsStackFramePtr      ( NULL ),
		itsTracingProcess     ( 0 ),
		itsAlarmClock         ( 0 ),
		itsPendingSignals     ( 0 ),
		itsPreviousSignals    ( 0 ),
		itsName               ( parent.ProgramName() ),
		itsCWD                ( parent.GetCWD() ),
		itsFileDescriptors    ( parent.FileDescriptors() ),
		itsLifeStage          ( kProcessStarting ),
		itsInterdependence    ( kProcessForked ),
		itsSchedule           ( kProcessRunning ),
		itsResult             ( 0 ),
		itsCleanupHandler     (),
		itsErrnoData          ( TARGET_RT_MAC_CFM ? parent.itsErrnoData : NULL )
	{
		parent.itsForkedChildPID = itsPID;
		
		gCurrentProcess = this;
		
		parent.itsInterdependence = kProcessForking;
		parent.itsSchedule        = kProcessFrozen;
	}
	
	Process::~Process()
	{
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
		Resume();
		
		itsOldMainEntry.reset();
		
		if ( IsBeingTraced() )
		{
			// This stops the thread immediately.
			// If we receive a fatal signal while stopped, the thread dies.
			Raise( SIGSTOP );
		}
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
	
	NN::Owned< N::ThreadID >  Process::Exec( const FSTreePtr&    executable,
	                                         const char* const   argv[],
	                                         const char* const*  envp )
	{
		CloseMarkedFileDescriptors( itsFileDescriptors );
		
		// Do we take the name before or after normalization?
		itsName = executable->Name();
		
		FSSpec fileSpec = executable->GetFSSpec();
		
		ExecContext context( fileSpec, argv );
		
		Normalize( context, GetCWD() );
		
		itsProgramFile = FSTreeFromFSSpec( context.executable );
		
		// Save the binary image that we're running from.
		// We can't use stack storage because we run the risk of the thread terminating.
		itsOldMainEntry = itsMainEntry;
		
		itsMainEntry = GetMainEntryFromFile( itsProgramFile );
		
		K::Versions assumedVersions;
		
		assumedVersions.current        = 2;
		assumedVersions.lastCompatible = 2;
		
		K::Versions* versions = &assumedVersions;
		
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
		
		MainProcPtr mainEntryPoint = itsMainEntry->GetMainPtr();
		
		itsArgvStorage.reset( new Sh::StringArray( &context.argVector[ 0 ] ) );
		
		itsErrnoData = itsMainEntry->GetErrnoPtr();
		
		iota::environ_t* environ_address = ENVIRON_IS_SHARED ? &gToolScratchGlobals.env : itsMainEntry->GetEnvironPtr();
		
		ResetEnviron( envp, environ_address );
		
		itsResult = 0;
		
		ThreadContext threadContext( this,
		                             mainEntryPoint,
		                             itsArgvStorage->GetPointer(),
		                             GetEnviron() );
		
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
		
		Suspend();
		
		return savedThreadID;
	}
	
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
	
	void Process::ChangeDirectory( const FSTreePtr& newCWD )
	{
		if ( !newCWD->IsDirectory() )
		{
			p7::throw_errno( newCWD->Exists() ? ENOTDIR : ENOENT );
		}
		
		itsCWD = newCWD;
	}
	
	void Process::ResumeAfterFork()
	{
		Resume();
		
		itsInterdependence = Forked() ? kProcessForked
		                              : kProcessIndependent;
		
		itsSchedule        =            kProcessRunning;
		
		// Restore environ (which was pointing into the child's environ storage)
		UpdateEnvironValue();
		
		pid_t child = itsForkedChildPID;
		
		itsForkedChildPID = 0;
		
		LongJmp jump = GetLongJmp();
		
		ASSERT( jump != NULL  ||  child == 0 );
		
		if ( jump != NULL  &&  child != 0 )
		{
			LeaveSystemCall();
			
			jump( child );
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
		
		parent.Exit( exit_status );
	}
	
	// This function doesn't return if the process is current and not forked.
	void Process::Terminate()
	{
		itsLifeStage = kProcessTerminating;
		itsSchedule  = kProcessUnscheduled;
		
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
					TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *ctty );
					
					terminal.SaveLeaderWaitStatus( itsResult );
				}
			}
			catch ( ... )
			{
			}
		}
		
		// This could yield, e.g. in OTCloseProvider() with sync idle events
		itsFileDescriptors.clear();
		
		if ( itsCleanupHandler != NULL )
		{
			itsCleanupHandler();
		}
		
		itsLifeStage = kProcessZombie;
		
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
			
			// Unforked process' thread dies here
		}
	}
	
	// This function doesn't return if the process is current and not forked.
	void Process::Terminate( int wait_status )
	{
		itsResult = wait_status;
		
		Terminate();
	}
	
	// This function doesn't return if the process is current and not forked.
	void Process::Exit( int exit_status )
	{
		Terminate( (exit_status & 0xFF) << 8 );
	}
	
	void Process::Orphan()
	{
		ASSERT( itsPID != 1 );
		
		itsPPID = 1;
		
		if ( itsLifeStage == kProcessZombie )
		{
			Release();
		}
	}
	
	void Process::Release()
	{
		ASSERT( itsLifeStage == kProcessZombie );
		
		itsPPID = 0;  // Don't match PPID comparisons
		itsLifeStage = kProcessReleased;
	}
	
	void Process::Suspend()
	{
		SuspendTimer();
		
		gCurrentProcess = NULL;
	}
	
	void Process::Resume()
	{
		gCurrentProcess = this;
		
		itsStackFramePtr = NULL;  // We don't track this while running
		
		SwapInEnvironValue( GetEnviron() );
		
		itsSchedule = kProcessRunning;
		
		ResumeTimer();
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
	
	// Doesn't return if the process was current and receives a fatal signal while stopped.
	// But always returns when *raising* a fatal signal.
	void Process::Raise( int signal )
	{
		if ( itsLifeStage >= kProcessTerminating  ||  itsResult != 0 )
		{
			return;
		}
		
		if ( IsBeingTraced()  &&  signal != SIGKILL )
		{
			Stop();
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
	
	// This function doesn't return if the process receives a fatal signal.
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
				DumpBacktrace();
			}
			
			if ( itsInterdependence == kProcessForked )
			{
				Process& parent( GetProcess( itsPPID ) );
				
				Terminate();
				
				parent.ResumeAfterFork();  // Calls longjmp()
				
				// Not reached
			}
			
			Terminate();  // Kills the thread
			
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
	
	// Doesn't return if the process was current and receives a fatal signal while stopped.
	void Process::Stop()
	{
		ASSERT( itsThread.get() != NULL );
		
		if ( gCurrentProcess == this )
		{
			ASSERT( itsSchedule == kProcessRunning );
			
			ASSERT( N::GetCurrentThread() == itsThread->Get() );
			
			itsStackFramePtr = Backtrace::GetStackFramePointer();
			
			Suspend();
		}
		
		itsSchedule = kProcessStopped;
		
		// Yields if this is the current thread
		N::SetThreadState( itsThread->Get(), N::kStoppedThreadState );
		
		if ( N::GetCurrentThread() == itsThread->Get() )
		{
			Resume();
			
			// Doesn't return if we received a fatal signal.
			HandlePendingSignals();
		}
	}
	
	void Process::Continue()
	{
		if ( itsThread.get() == NULL )
		{
			WriteToSystemConsole( STR_LEN( "Genie: Process::Continue(): no thread assigned\n" ) );
			
			return;
		}
		
		if ( N::GetThreadState( itsThread->Get() ) == N::kStoppedThreadState )
		{
			ASSERT( itsSchedule == kProcessStopped );
			
			itsSchedule = kProcessSleeping;
			
			N::SetThreadState( itsThread->Get(), N::kReadyThreadState );
		}
	}
	
	
	static UInt32 gTickCountOfLastSleep = 0;
	
	static const UInt32 gMinimumSleepIntervalTicks = 15;  // Sleep every quarter second
	
	// This function doesn't return if we received a fatal signal.
	void Process::Yield()
	{
		itsSchedule = kProcessSleeping;
		
		itsStackFramePtr = Backtrace::GetStackFramePointer();
		
		Suspend();
		
		N::YieldToAnyThread();
		
		Resume();
		
		gTickCountOfLastSleep = ::TickCount();
		
		// Doesn't return if we received a fatal signal.
		bool signalled = HandlePendingSignals();
		
		if ( signalled )
		{
			p7::throw_errno( EINTR );
		}
	}
	
	// This function doesn't return if we received a fatal signal.
	void Yield()
	{
		ASSERT( gCurrentProcess != NULL );
		
		gCurrentProcess->Yield();
	}
	
	// This function doesn't return if we received a fatal signal.
	void Breathe()
	{
		UInt32 now = ::TickCount();
		
		if ( now - gTickCountOfLastSleep > gMinimumSleepIntervalTicks )
		{
			Ped::AdjustSleepForActivity();
			
			Yield();
			
			gTickCountOfLastSleep = now;
		}
	}
	
	void TryAgainLater( bool isNonblocking )
	{
		if ( isNonblocking )
		{
			throw io::no_input_pending();
		}
		
		Yield();
	}
	
}

