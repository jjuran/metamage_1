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

// Lamp
#include "lamp/syscalls.h"

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

// ShellShock
#include "ShellShock/VarArray.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemConsole.hh"


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
	
	
	ProcessList& GetProcessList()
	{
		static ProcessList theProcessList;
		
		return theProcessList;
	}
	
	
	void CmdLine::Assign( char const *const *argv )
	{
		itsStorage.clear();
		
		while ( *argv )
		{
			//itsStorage += *argv++;  // This fails to copy **argv using MSL
			
			itsStorage += std::string( *argv++ );
			
			itsStorage += '\0';
		}
	}
	
	
	void SendSignalToProcessGroup( int sig, const ProcessGroup& group )
	{
		for ( ProcessList::iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetProcessGroup().get() == &group )
			{
				proc.Raise( sig );
			}
		}
	}
	
	Process& GetInitProcess()
	{
		static Process* init = new Process( Process::RootProcess() );
		
		return *init;
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
		ProcessList::iterator it = GetProcessList().GetMap().find( pid );
		
		if ( it == GetProcessList().end()  ||  it->second->GetLifeStage() == kProcessReleased )
		{
			return NULL;
		}
		
		return it->second.get();
	}
	
	
#if TARGET_CPU_68K

	static asm void SaveRegisters( SavedRegisters* saved )
	{
		MOVE.W		SR,-(SP)
		MOVE.L		A0,-(SP)
		MOVEA.L		10(SP),A0
		ADD.L		#64,A0
		MOVEM.L		D0-D7/A0-A7,-(A0)
		MOVE.L		(SP),32(A0)
		MOVE.W		4(SP),68(A0)
		MOVE.L		6(SP),64(A0)
		MOVEA.L		(SP)+,A0
		MOVE.W		(SP)+,CCR
		RTS
	}

#endif

#if TARGET_CPU_PPC

	static asm void SaveRegisters( SavedRegisters* saved )
	{
		stw		r0,0(r3)
		stw		r1,4(r3)
		stw		r2,8(r3)
		stw		r3,12(r3)
		stw		r4,16(r3)
		stw		r5,20(r3)
		stw		r6,24(r3)
		stw		r7,28(r3)
		stw		r8,32(r3)
		stw		r9,36(r3)
		stw		r10,40(r3)
		stw		r11,44(r3)
		stw		r12,48(r3)
		stw		r13,52(r3)
		stw		r14,56(r3)
		stw		r15,60(r3)
		stw		r16,64(r3)
		stw		r17,68(r3)
		stw		r18,72(r3)
		stw		r19,76(r3)
		stw		r20,80(r3)
		stw		r21,84(r3)
		stw		r22,88(r3)
		stw		r23,92(r3)
		stw		r24,96(r3)
		stw		r25,100(r3)
		stw		r26,104(r3)
		stw		r27,108(r3)
		stw		r28,112(r3)
		stw		r29,116(r3)
		stw		r30,120(r3)
		stw		r31,124(r3)
		
		mflr	r0
		stw		r0,128(r3)
		
		mfctr	r0
		stw		r0,132(r3)
		
		lwz		r0,0(r3)
		
		blr
	}

#endif
	
	static void Die()
	{
		Process& current = CurrentProcess();
		
		current.Raise( SIGSYS );
		
		current.HandlePendingSignals();
		
		current.Raise( SIGKILL );
		
		current.HandlePendingSignals();
		
		N::SysBeep();
		
		::ExitToShell();  // not messing around
	}
	
	static void* GetSystemCallAddress( unsigned index )
	{
		const SystemCall* syscall = GetSystemCall( index );
		
		if ( syscall == NULL  ||  syscall->function == NULL )
		{
			Die();
		}
		
		return syscall->function;
	}
	
#if TARGET_CPU_68K
	
	static asm void DispatchSystemCall( unsigned index )
	{
		JSR		GetSystemCallAddress  // index is already on the stack
		
		ADDQ	#4,SP  // pop the stack
		
		JMP		(A0)  // system call address is already in A0
	}
	
#endif
	
#if TARGET_CPU_PPC
	
	static asm void DispatchSystemCall()
	{
		// save caller's return address
		mflr	r0
		stw		r0,8(SP)
		
		// save system call arguments into stack
		stw		r3,24(SP)
		stw		r4,28(SP)
		stw		r5,32(SP)
		stw		r6,36(SP)
		stw		r7,40(SP)
		stw		r8,44(SP)
		stw		r9,48(SP)
		stw		r10,52(SP)
		
		// allocate stack frame
		stwu	SP,-32(SP)
		
		// get system call address (from its index) or die
		mr		r3,r11
		bl		GetSystemCallAddress
		mr		r12,r3
		
		// deallocate stack frame
		addi	SP,SP,32
		
		// restore system call arguments
		lwz		r3,24(SP)
		lwz		r4,28(SP)
		lwz		r5,32(SP)
		lwz		r6,36(SP)
		lwz		r7,40(SP)
		lwz		r8,44(SP)
		lwz		r9,48(SP)
		lwz		r10,52(SP)
		
		// prepare for hyperspace jump (restore caller's return address to link register)
		lwz		r0,8(SP)
		mtlr	r0
		
		// load system call address
		lwz		r0,0(r12)
		
		// jump to system call
		mtctr	r0
		bctr
	}
	
#endif
	
	typedef void (*Dispatcher)( const char* );
	
	typedef void (*DispatcherByIndex)( unsigned );
	
	struct ToolScratchGlobals
	{
		Dispatcher       dispatcher;
		DispatcherByIndex  dispatcherByIndex;
	};
	
	struct ApplScratchGlobals
	{
		iota::environ_t  env;
		void*            reserved1;
		void*            reserved2;
	};
	
#if TARGET_API_MAC_CARBON
	
	#define LMGetApplScratch() NULL
	
#endif
	
	static ApplScratchGlobals& gApplScratchGlobals = *reinterpret_cast< ApplScratchGlobals* >( LMGetApplScratch() );
	
	inline void SwapInEnvironValue( iota::environ_t envp )
	{
		if ( ENVIRON_IS_SHARED )
		{
			gApplScratchGlobals.env = envp;
		}
	}
	
	
	static std::vector< const char* > ArgVectorFromCmdLine( const std::string& cmdLine )
	{
		std::vector< const char* > result;
		
		const char* begin = &*cmdLine.begin();
		const char* end   = &*cmdLine.end();
		
		while ( begin < end )
		{
			const char* null = std::find( begin, end, '\0' );
			
			ASSERT( null != end );
			
			result.push_back( begin );
			
			begin = null + 1;
		}
		
		result.push_back( NULL );
		
		return result;
	}
	
	static int RunFromContext( Process* process )
	{
		Main3 mainPtr = process->GetMain();
		
		ASSERT( mainPtr != NULL );
		
		std::vector< const char* > argVector = ArgVectorFromCmdLine( process->GetCmdLine() );
		
		int argc = argVector.size() - 1;  // don't count trailing NULL
		
		iota::argp_t argv = &argVector.front();
		
		iota::environ_t envp = process->GetEnviron();
		
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		const void* applScratch[3] = { envp, NULL, NULL };
		
		LMSetApplScratch( applScratch );
		
	#endif
		
		// This is a separate function so registers get saved and restored
		int exit_status = mainPtr( argc, argv, envp );
		
		return exit_status;
	}
	
	namespace
	{
		
		void ProcessThreadEntry( Process* process )
		{
			process->InitThread();
			
			int exit_status = RunFromContext( process );
			
			// Accumulate any user time between last system call (if any) and return from main()
			process->EnterSystemCall( "*RETURN*" );
			
			process->Exit( exit_status );
			
			// Not reached
		}
		
	}
	
	struct ExecContext
	{
		FSTreePtr                   executable;
		std::vector< const char* >  argVector;
		std::string                 scriptPath;
		std::string                 interpreterPath;
		std::string                 interpreterArg;
		
		ExecContext()  {}
		
		ExecContext( const FSTreePtr&    executable,
		             char const* const*  argv )
		:
			executable( executable ),
			argVector ( argv, argv + Sh::CountStringArray( argv ) + 1 )
		{}
	};
	
	inline p7::errno_t NotExecutable()  { return p7::errno_t( EPERM ); }
	
	static ExecContext& Normalize( ExecContext& context, const FSTreePtr& cwd )
	{
		FSSpec fileSpec;
		
		OSType type = 'Wish';
		
		try
		{
			fileSpec = context.executable->GetFSSpec();
			
			type = N::FSpGetFInfo( fileSpec ).fdType;
		}
		catch ( ... )
		{
			// Assume that non-FSSpec executables are binaries, not scripts
		}
		
		if ( type == 'Wish' )
		{
			return context;  // Already normalized
		}
		
		if ( type == 'TEXT' )
		{
			context.interpreterPath = "/bin/sh";  // default
			bool hasArg = false;
			
			NN::Owned< N::FSFileRefNum > script = io::open_for_reading( fileSpec );
			
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
				context.scriptPath = GetPOSIXPathname( fileSpec );
				
				context.argVector[ 1 + hasArg ] = context.scriptPath.c_str();
			}
			
			// argv == { "sh", "script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolvePathname( context.interpreterPath, cwd );
		}
		else if ( type == 'MPST' )
		{
			context.scriptPath = GetMacPathname( fileSpec );
			
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
			
			context.executable = ResolvePathname( "/usr/bin/tlsrvr", cwd );
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
		for ( ProcessList::iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
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
		itsPID                ( GetProcessList().NewProcess( this ) ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( NewProcessGroup( itsPID ) ),
		itsStackFramePtr      ( NULL ),
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
		itsProgramFile        ( FSRoot() ),
		itsCleanupHandler     ()
	{
		*reinterpret_cast< void** >( LMGetToolScratch() ) = DispatchSystemCall;
		
		char const *const argv[] = { "init", NULL };
		
		itsCmdLine.Assign( argv );
		
		itsFileDescriptors[ 0 ] =
		itsFileDescriptors[ 1 ] =
		itsFileDescriptors[ 2 ] = GetSimpleDeviceHandle( "null" );
	}
	
	Process::Process( Process& parent ) 
	:
		Environ( parent ),
		itsPPID               ( parent.GetPID() ),
		itsPID                ( GetProcessList().NewProcess( this ) ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( parent.GetProcessGroup() ),
		itsStackFramePtr      ( NULL ),
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
		itsMainEntry          ( parent.itsMainEntry ),
		itsCleanupHandler     ()
	{
		parent.SuspendForFork( itsPID );
		
		gCurrentProcess = this;
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
			Stop();
		}
	}
	
	Nitrogen::ThreadID Process::GetThread() const
	{
		const Process* process = this;
		
		while ( process->itsThread.Get() == N::kNoThreadID )
		{
			pid_t ppid = process->GetPPID();
			
			if ( ppid == 1 )
			{
				return N::kNoThreadID;
			}
			
			process = &GetProcess( ppid );
		}
		
		return process->itsThread.Get();
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
		
		ExecContext context( executable, argv );
		
		Normalize( context, GetCWD() );
		
		itsProgramFile = context.executable;
		
		// Save the binary image that we're running from.
		// We can't use stack storage because we run the risk of the thread terminating.
		itsOldMainEntry = itsMainEntry;
		
		itsMainEntry = itsProgramFile->GetMainEntry();
		
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
		
		itsCmdLine.Assign( &context.argVector.front() );
		
		//itsErrnoData = itsMainEntry->GetErrnoPtr();
		
		iota::environ_t* environ_address = ENVIRON_IS_SHARED ? &gApplScratchGlobals.env : itsMainEntry->GetEnvironPtr();
		
		ResetEnviron( envp, environ_address );
		
		itsResult = 0;
		
		// We always spawn a new thread for the exec'ed process.
		// If we've forked, then the thread is null, but if not, it's the
		// current thread -- be careful!
		
		N::Size stackSize = N::GetDefaultThreadStackSize( N::kCooperativeThread );
		
		stackSize = std::max( stackSize, 32768ul );  // at least 32K of stack
		
		// Create the new thread
		NN::Owned< N::ThreadID > newThread = N::NewThread< Process*, ProcessThreadEntry >( N::kCooperativeThread,
		                                                                                   this,
		                                                                                   stackSize );
		
		// Save the old thread
		NN::Owned< N::ThreadID > savedThreadID = itsThread;
		
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
		if ( int* p_errno = itsMainEntry->GetErrnoPtr() )
		{
			*p_errno = errorNumber;
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
	
	void Process::SuspendForFork( pid_t childPID )
	{
		itsForkedChildPID = childPID;
		
		itsInterdependence = kProcessForking;
		itsSchedule        = kProcessFrozen;
		
		itsStackFramePtr = Backtrace::GetStackFramePointer( 3 );
		
		SaveRegisters( &itsSavedRegisters );
		
		Suspend();
	}
	
	void Process::ResumeAfterFork()
	{
		Resume();
		
		itsInterdependence = Forked() ? kProcessForked
		                              : kProcessIndependent;
		
		// Restore environ (which was pointing into the child's environ storage)
		UpdateEnvironValue();
		
		pid_t child = itsForkedChildPID;
		
		itsForkedChildPID = 0;
		
		if ( itsPID == 1 )
		{
			itsSchedule = kProcessSleeping;
			
			return;
		}
		
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
		
		Process& parent = GetProcess( ppid );
		
		if ( ppid > 1  &&  parent.itsSignalMap[ SIGCHLD ] != SIG_IGN )
		{
			parent.Raise( SIGCHLD );
		}
		else
		{
			Release();
		}
		
		typedef ProcessList::Map::const_iterator const_iterator;
		
		for ( const_iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
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
		
		if ( !Forked() )
		{
			NN::Owned< N::ThreadID > savedThreadID = itsThread;
			
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
	
	bool Process::Pause( ProcessSchedule newSchedule )
	{
		itsSchedule = newSchedule;
		
		Suspend();
		
		itsStackFramePtr = Backtrace::GetStackFramePointer();
		
		SaveRegisters( &itsSavedRegisters );
		
		if ( newSchedule == kProcessStopped )
		{
			N::SetThreadState( GetThread(), N::kStoppedThreadState );
		}
		else
		{
			N::YieldToAnyThread();
		}
		
		Resume();
		
		return HandlePendingSignals();
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
	
	void Process::DeliverSignal( int signal )
	{
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
		else
		{
			DeliverSignal( signal );
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
	
	ProcessList::ProcessList() : itsNextPID( 1 )
	{
	}
	
	static void ReapProcesses()
	{
		std::vector< pid_t > hitlist;
		
		for ( ProcessList::iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
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
			
			GetProcessList().RemoveProcess( pid );
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
	
	pid_t ProcessList::NewProcess( Process* process )
	{
		static NN::Owned< N::ThreadID > reaper = N::NewThread< ReaperThreadEntry >( N::kCooperativeThread );
		
		itsMap[ itsNextPID ] = boost::shared_ptr< Process >( process );
		
		return itsNextPID++;
	}
	
	void ProcessList::RemoveProcess( pid_t pid )
	{
		iterator it = itsMap.find( pid );
		
		if ( it == itsMap.end() )
		{
			N::SysBeep();
		}
		else
		{
			itsMap.erase( it );
		}
	}
	
	void ProcessList::KillAll()
	{
		for ( Map::iterator it = itsMap.begin();  it != itsMap.end();  ++it )
		{
			Process& proc = *it->second;
			
			proc.Raise( SIGKILL );
		}
		
		while ( itsMap.size() > 1 )
		{
			N::YieldToAnyThread();
		}
	}
	
	// Doesn't return if the process was current and receives a fatal signal while stopped.
	void Process::Stop()
	{
		N::ThreadID thread = GetThread();
		
		ASSERT( thread != N::kNoThreadID );
		
		if ( gCurrentProcess == this )
		{
			ASSERT( itsSchedule == kProcessRunning );
			
			ASSERT( N::GetCurrentThread() == thread );
			
			Pause( kProcessStopped );
		}
		else
		{
			itsSchedule = kProcessStopped;
			
			N::SetThreadState( thread, N::kStoppedThreadState );
		}
	}
	
	void Process::Continue()
	{
		N::ThreadID thread = GetThread();
		
		if ( thread == N::kNoThreadID )
		{
			WriteToSystemConsole( STR_LEN( "Genie: Process::Continue(): no thread assigned\n" ) );
			
			return;
		}
		
		if ( itsSchedule == kProcessFrozen )
		{
			return;
		}
		
		if ( N::GetThreadState( thread ) == N::kStoppedThreadState )
		{
			ASSERT( itsSchedule == kProcessStopped );
			
			itsSchedule = kProcessSleeping;
			
			N::SetThreadState( thread, N::kReadyThreadState );
		}
	}
	
	
	static UInt32 gTickCountOfLastSleep = 0;
	
	static const UInt32 gMinimumSleepIntervalTicks = 15;  // Sleep every quarter second
	
	// This function doesn't return if we received a fatal signal.
	void Process::Yield()
	{
		// Doesn't return if we received a fatal signal.
		bool signalled = Pause( kProcessSleeping );
		
		gTickCountOfLastSleep = ::TickCount();
		
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

