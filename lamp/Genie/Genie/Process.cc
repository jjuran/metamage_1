/*	==========
 *	Process.cc
 *	==========
 */

#include "Genie/Process.hh"

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// Standard C++
#include <vector>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// Lamp
#include "lamp/syscalls.h"

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Aliases.hh"
#include "Nitrogen/OSStatus.hh"
#include "Nitrogen/Threads.hh"
#include "Nitrogen/Timer.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// GetPathname
#include "GetPathname.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"

// Recall
#include "recall/backtrace.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/Exec/MainEntryPoint.hh"
#include "Genie/Faults.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/IO/Base.hh"
#include "Genie/ProcessList.hh"
#include "Genie/Process/AsyncYield.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemConsole.hh"
#include "Genie/Utilities/AsyncIO.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif

#if defined(__MWERKS__) && defined(__csignal__)
	#undef SIGABRT
	#undef SIGFPE
	#undef SIGILL
	#undef SIGINT
	#undef SIGSEGV
	#undef SIGTERM
#endif


static void DumpBacktrace()
{
	using namespace recall;
	
	const unsigned frame_capacity = 64;
	
	frame_data stack_crawl[ frame_capacity ];
	
	const unsigned n_frames = make_stack_crawl( stack_crawl, frame_capacity );
	
	const frame_data* begin = stack_crawl;
	const frame_data* end   = stack_crawl + n_frames;
	
	++begin;  // skip DumpBacktrace( void )
	
	plus::var_string report;
	
	make_report_from_stack_crawl( report, begin, end );
	
	(void) Genie::WriteToSystemConsole( report.data(), report.size() );
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	Process* gCurrentProcess;  // extern, declared in Faults.cc
	
	Process& CurrentProcess()
	{
		if ( gCurrentProcess == NULL )
		{
			p7::throw_errno( ESRCH );
		}
		
		return *gCurrentProcess;
	}
	
	
	struct signal_param
	{
		pid_t  pgid;
		int    signo;
	};
	
	static void* signal_process_in_group( void* param, pid_t, Process& process )
	{
		signal_param& pb = *(signal_param*) param;
		
		if ( process.GetPGID() == pb.pgid )
		{
			process.Raise( pb.signo );
		}
		
		return NULL;
	}
	
	void SendSignalToProcessGroup( int sig, pid_t pgid )
	{
		signal_param param = { pgid, sig };
		
		for_each_process( &signal_process_in_group, &param );
	}
	
	Process& GetProcess( pid_t pid )
	{
		if ( Process* process = FindProcess( pid ) )
		{
			return *process;
		}
		
		throw p7::errno_t( ESRCH );
	}
	
	static void* find_pid( void* param, pid_t pid, Process& process )
	{
		return *(pid_t*) param == pid ? &process
		                              : NULL;
	}
	
	Process* FindProcess( pid_t pid )
	{
		if ( Process* result = (Process*) for_each_process( &find_pid, &pid ) )
		{
			if ( result->GetLifeStage() != kProcessReleased )
			{
				return result;
			}
		}
		
		return NULL;
	}
	
	void DeliverFatalSignal( int signo )
	{
		bool caught_exception = false;
		
		if ( gCurrentProcess != NULL )
		{
			try
			{
				Process& current = *gCurrentProcess;
				
				// first chance -- program can siglongjmp() out of signal handler
				current.Raise( signo );
				current.HandlePendingSignals( kInterruptNever );
				
				// This should be fatal
				current.ResetSignalAction( signo );
				current.Raise( signo );
				current.HandlePendingSignals( kInterruptNever );
			}
			catch ( ... )
			{
				caught_exception = true;
			}
		}
		
		// Either we're in the main thread, or signal handling failed.  Bail.
		
		if ( TARGET_CONFIG_DEBUGGING )
		{
			const unsigned char* message = !gCurrentProcess ? "\p" "Fatal condition occurred on main thread"
			                             : caught_exception ? "\p" "Exception caught trying to deliver fatal signal"
			                             :                    "\p" "Invalid return from delivering fatal signal";
			
			::DebugStr( message );
		}
		else
		{
			Mac::SysBeep();
			Mac::SysBeep();
			Mac::SysBeep();
		}
		
		::ExitToShell();  // not messing around
	}
	
#ifdef __LAMP__
	
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
	
#if TARGET_CPU_68K
	
	static asm void DispatchSystemCall( ... )
	{
		// D0 contains the system call number
		
		CMP.W	gLastSystemCall,D0
		BLT		in_range
		
		MOVE.W	gLastSystemCall,D0
		
	in_range:
		MOVEA.L	gSystemCallArray,A0
		MOVEA.L	(A0,D0.W*8),A0
		
		JMP		(A0)
	}
	
#endif
	
#if TARGET_CPU_PPC
	
	enum { kSystemCallSize = sizeof (SystemCall) };
	
	static asm void DispatchSystemCall( ... )
	{
		nofralloc
		
		// r11 contains the requested system call number
		// r3-r10 are up to 8 arguments
		
		// r12.last = gLastSystemCall;
		lwz		r12,gLastSystemCall
		lwz		r12,0(r12)
		
		// if ( r11.index > r12.last )
		cmpl	cr0,r11,r12
		blt+	cr0,in_range
		// r11.index = r12.last;
		mr		r11,r12
		
	in_range:
		// r12.array = gSystemCallArray;
		lwz		r12,gSystemCallArray
		lwz		r12,0(r12)
		
		// r12.f = r12.array[ r11.index ].function
		mulli	r11,r11,kSystemCallSize
		add		r12,r12,r11
		lwz		r12,0(r12)
		
		// load system call address for local jump
		lwz		r0,0(r12)
		
		// jump to system call
		mtctr	r0
		bctr
	}
	
#endif
	
#else  // #ifdef __LAMP__
	
	// Dummy declarations so we compile on OS X
	
	static void SaveRegisters( SavedRegisters* saved )
	{
	}
	
	static void DispatchSystemCall( ... )
	{
	}
	
#endif
	
	int Process::Run()
	{
		// Accumulate any system time between start and entry to main()
		LeaveSystemCall();
		
		int exit_status = 0;
		
		if ( Reexec_Function f = (Reexec_Function) itsReexecArgs[ 0 ] )
		{
			exit_status = f( itsReexecArgs[ 1 ],
			                 itsReexecArgs[ 2 ],
			                 itsReexecArgs[ 3 ],
			                 itsReexecArgs[ 4 ],
			                 itsReexecArgs[ 5 ],
			                 itsReexecArgs[ 6 ],
			                 itsReexecArgs[ 7 ] );
		}
		else
		{
			int    argc = its_memory_data->get_argc();
			char** argv = its_memory_data->get_argv();
			char** envp = its_memory_data->get_envp();
			
			exit_status = itsMainEntry->Invoke( argc, argv, envp, &DispatchSystemCall );
			
			// Not reached by regular tools, since they call exit()
		}
		
		// Accumulate any user time between last system call (if any) and return from main()
		EnterSystemCall( "*RETURN*" );
		
		// For code fragments, static destruction occurs here.
		itsMainEntry.reset();
		
		return exit_status;
	}
	
	
	recall::stack_frame_pointer Init_Thread();
	
#if TARGET_CPU_PPC && TARGET_RT_MAC_CFM
	
	asm recall::stack_frame_pointer Init_Thread()
	{
		lwz r4,0(sp)
		li  r0,0
		lwz r3,0(r4)
		stw r0,0(r3)
	}
	
#elif TARGET_CPU_68K
	
	asm recall::stack_frame_pointer Init_Thread()
	{
		MOVEA.L (A6),A0
		RTS
	}
	
#else
	
	inline recall::stack_frame_pointer Init_Thread()
	{
		return NULL;
	}
	
#endif
	
	
	pascal void* Process::ThreadEntry( void* param )
	{
		Process* process = reinterpret_cast< Process* >( param );
		
		process->itsStackBottomPtr = Init_Thread();
		
		try
		{
			process->InitThread();
			
			int exit_status = process->Run();
			
			process->Exit( exit_status );
			
			// Not reached
		}
		catch ( ... )
		{
			abort();
		}
		
		// Not reached
		
		return NULL;
	}
	
	
	static std::size_t argv_length( char const* const* argv )
	{
		std::size_t result = 0;
		
		while ( *argv++ )
		{
			++result;
		}
		
		return result;
	}
	
	struct ExecContext
	{
		FSTreePtr                   executable;
		std::vector< const char* >  argVector;
		plus::string                scriptPath;
		plus::string                interpreterPath;
		plus::string                interpreterArg;
		
		ExecContext()  {}
		
		ExecContext( const FSTreePtr&    executable,
		             char const* const*  argv )
		:
			executable( executable ),
			argVector ( argv, argv + argv_length( argv ) + 1 )
		{}
	};
	
	static inline p7::errno_t NotExecutable()  { return p7::errno_t( EPERM ); }
	
	static void Normalize( const char* path, ExecContext& context, const FSTreePtr& cwd )
	{
		FSSpec fileSpec;
		
		OSType type = 'Wish';
		
		try
		{
			fileSpec = GetFSSpecFromFSTree( context.executable );
			
			type = N::FSpGetFInfo( fileSpec ).fdType;
		}
		catch ( ... )
		{
			// Assume that non-FSSpec executables are binaries, not scripts
		}
		
		if ( type == 'Wish' )
		{
			return;  // Already normalized
		}
		
		if ( type == 'TEXT' )
		{
			context.interpreterPath = "/bin/sh";  // default
			bool hasArg = false;
			
			char data[ 1024 + 1 ];
			data[1024] = '\0';
			
			n::owned< N::FSFileRefNum > script = N::FSpOpenDF( fileSpec, N::fsRdPerm );
			
			size_t bytes = N::FSRead( script, 1024, data, N::ThrowEOF_Never() );
			
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
				
				context.interpreterPath.assign( &data[2], space ? space : nl );
				context.interpreterArg .assign( space ? space + 1 : nl, nl );
			}
			
			// E.g. "$ script foo bar baz"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			if ( std::strchr( context.argVector[ 0 ], '/' ) == NULL )
			{
				// Overwrite with path
				context.argVector[ 0 ] = path;
				
				// argv == { "/path/to/script", "foo", "bar", "baz", NULL }
			}
			
			context.argVector.insert( context.argVector.begin(),
			                          context.interpreterPath.c_str() );
			
			// argv == { "sh", "script", "foo", "bar", "baz", NULL }
			
			if ( hasArg )
			{
				context.argVector.insert( context.argVector.begin() + 1,
				                          context.interpreterArg.c_str() );
			}
			
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
			
			context.argVector[ 0 ] = "/Developer/Tools/tlsrvr";
			context.argVector[ 1 ] = "--escape";
			context.argVector[ 2 ] = "--";
			context.argVector[ 3 ] = context.scriptPath.c_str();  // Overwrite with full pathname
			
			// argv == { "sh", "--", "/usr/bin/script", "foo", "bar", "baz", NULL }
			
			context.executable = ResolveAbsolutePath( STR_LEN( "/Developer/Tools/tlsrvr" ) );
		}
		else
		{
			throw NotExecutable();
		}
	}
	
	
	static boost::intrusive_ptr< Session > NewSession( pid_t sid )
	{
		return boost::intrusive_ptr< Session >( new Session( sid ) );
	}
	
	static boost::intrusive_ptr< ProcessGroup > NewProcessGroup( pid_t pgid, const boost::intrusive_ptr< Session >& session )
	{
		return boost::intrusive_ptr< ProcessGroup >( new ProcessGroup( pgid, session ) );
	}
	
	static boost::intrusive_ptr< ProcessGroup > NewProcessGroup( pid_t pgid )
	{
		return NewProcessGroup( pgid, NewSession( pgid ) );
	}
	
	static void* find_process_group( void* param, pid_t, Process& process )
	{
		const pid_t pgid = *(pid_t*) param;
		
		if ( process.GetPGID() == pgid )
		{
			return process.GetProcessGroup().get();
		}
		
		return NULL;
	}
	
	boost::intrusive_ptr< ProcessGroup > FindProcessGroup( pid_t pgid )
	{
		void* result = for_each_process( &find_process_group, &pgid );
		
		ProcessGroup* group = (ProcessGroup*) result;
		
		return boost::intrusive_ptr< ProcessGroup >( group );
	}
	
	boost::intrusive_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, const boost::intrusive_ptr< Session >& session )
	{
		boost::intrusive_ptr< ProcessGroup > pgrp = FindProcessGroup( pgid );
		
		if ( pgrp.get() == NULL )
		{
			return NewProcessGroup( pgid, session );
		}
		
		if ( pgrp->GetSession() != session )
		{
			p7::throw_errno( EPERM );
		}
		
		return pgrp;
	}
	
	static inline boost::intrusive_ptr< memory_data > root_memory_data()
	{
		boost::intrusive_ptr< memory_data > result( memory_data::create() );
		
		char const *const argv[] = { "init", NULL };
		
		result->set_argv( argv );
		result->set_envp( NULL );
		
		return result;
	}
	
	Process::Process( RootProcess ) 
	:
		itsPPID               ( 0 ),
		itsPID                ( 1 ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( NewProcessGroup( itsPID ) ),
		itsErrno              ( NULL ),
		itsStackBottomPtr     ( NULL ),
		itsStackFramePtr      ( NULL ),
		itsAlarmClock         ( 0 ),
		itsName               ( "init" ),
		itsCWD                ( FSRoot()->ChangeToDirectory() ),
		itsFileDescriptors    ( fd_table::create() ),
		itsLifeStage          ( kProcessLive ),
		itsInterdependence    ( kProcessIndependent ),
		itsSchedule           ( kProcessSleeping ),
		itsResult             ( 0 ),
		itsAsyncOpCount       ( 0 ),
		itsProgramFile        ( FSRoot() ),
		its_memory_data       ( root_memory_data() ),
		itsCleanupHandler     (),
		itMayDumpCore         ()
	{
		itsReexecArgs[0] =
		itsReexecArgs[1] =
		itsReexecArgs[2] =
		itsReexecArgs[3] =
		itsReexecArgs[4] =
		itsReexecArgs[5] =
		itsReexecArgs[6] =
		itsReexecArgs[7] = NULL;
		
		fd_table& fds = *itsFileDescriptors;
		
		fds[ 0 ] =
		fds[ 1 ] =
		fds[ 2 ] = GetSimpleDeviceHandle( "null" );
		
		InstallExceptionHandlers();
	}
	
	Process::Process( Process& parent, pid_t pid ) 
	:
		SignalReceiver        ( parent ),
		itsPPID               ( parent.GetPID() ),
		itsPID                ( pid ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( parent.GetProcessGroup() ),
		itsErrno              ( parent.itsErrno ),
		itsStackBottomPtr     ( NULL ),
		itsStackFramePtr      ( NULL ),
		itsAlarmClock         ( 0 ),
		itsName               ( parent.ProgramName() ),
		itsCWD                ( parent.itsCWD ),
		itsFileDescriptors    ( duplicate( *parent.itsFileDescriptors ) ),
		itsLifeStage          ( kProcessStarting ),
		itsInterdependence    ( kProcessForked ),
		itsSchedule           ( kProcessRunning ),
		itsResult             ( 0 ),
		itsAsyncOpCount       ( 0 ),
		itsProgramFile        ( parent.itsProgramFile ),
		itsMainEntry          ( parent.itsMainEntry ),
		its_memory_data       ( parent.its_memory_data ),
		itsCleanupHandler     (),
		itMayDumpCore         ( true )
	{
		itsReexecArgs[0] =
		itsReexecArgs[1] =
		itsReexecArgs[2] =
		itsReexecArgs[3] =
		itsReexecArgs[4] =
		itsReexecArgs[5] =
		itsReexecArgs[6] =
		itsReexecArgs[7] = NULL;
		
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
		
		while ( process->itsThread.get() == N::kNoThreadID )
		{
			pid_t ppid = process->GetPPID();
			
			if ( ppid == 1 )
			{
				return N::kNoThreadID;
			}
			
			process = &GetProcess( ppid );
		}
		
		return process->itsThread.get();
	}
	
	static void close_fd_on_exec( void* keep, int fd, FileDescriptor& desc )
	{
		if ( desc.closeOnExec  &&  fd != *(int*) keep )
		{
			desc.handle.reset();
		}
	}
	
	static void CloseMarkedFileDescriptors( FileDescriptorMap& fileDescriptors, int keep_fd = -1 )
	{
		// Close file descriptors with close-on-exec flag.
		
		fileDescriptors.for_each( &close_fd_on_exec, &keep_fd );
	}
	
	static void CheckProgramFile( const FSTreePtr& programFile )
	{
		struct ::stat sb;
		
		programFile->Stat( sb );
		
		if ( S_ISDIR( sb.st_mode ) )
		{
			p7::throw_errno( EISDIR );
		}
		
		if ( (sb.st_mode & S_IXUSR) == 0 )
		{
			p7::throw_errno( EACCES );
		}
	}
	
	static std::size_t DefaultThreadStackSize()
	{
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		return size;
	}
	
	void Process::Exec( const char*         path,
	                    const char* const   argv[],
	                    const char* const*  envp )
	{
		// Declare this first so it goes out of scope last
		n::owned< N::ThreadID > looseThread;
		
		// Somehow (not GetCWD()) this fails in non-debug 68K in 7.6
		FSTreePtr programFile = ResolvePathname( path, GetCWD() );
		
		ResolveLinks_InPlace( programFile );
		
		CheckProgramFile( programFile );
		
		// Do we take the name before or after normalization?
		itsName = programFile->Name();
		
		ExecContext context( programFile, argv );
		
		Normalize( path, context, GetCWD() );
		
		int script_fd = -1;
		
		if ( !context.interpreterPath.empty() )
		{
			const bool has_arg = !context.interpreterArg.empty();
			
			const char* script_path = context.argVector[ 1 + has_arg ];
			
			if ( std::memcmp( script_path, STR_LEN( "/dev/fd/" ) ) == 0 )
			{
				const char* fd_name = script_path + STRLEN( "/dev/fd/" );
				
				script_fd = iota::parse_unsigned_decimal( fd_name );
			}
		}
		
		its_memory_data->clear_memory_mappings();
		
		CloseMarkedFileDescriptors( *itsFileDescriptors, script_fd );
		
		ClearPendingSignals();
		
		ResetSignalHandlers();
		
		// Members of argv and envp could be living in its_memory_data
		boost::intrusive_ptr< memory_data > new_memory_data( memory_data::create() );
		
		new_memory_data->set_argv( &context.argVector.front() );
		
		new_memory_data->set_envp( envp );
		
		using std::swap;
		
		swap( its_memory_data, new_memory_data );
		
		itsProgramFile = context.executable;
		
		// Really the new main entry.  We'll swap later.
		itsOldMainEntry = itsProgramFile->GetMainEntry();
		
		// We always spawn a new thread for the exec'ed process.
		// If we've forked, then the thread is null, but if not, it's the
		// current thread -- be careful!
		
		const std::size_t defaultStackSize = DefaultThreadStackSize();
		
		const std::size_t minimumStackSize = 64 * 1024;
		
		const std::size_t stackSize = std::max( defaultStackSize, minimumStackSize );
		
		// Create the new thread
		looseThread = N::NewThread< Process::ThreadEntry >( this, stackSize );
		
		if ( itsCleanupHandler != NULL )
		{
			itsCleanupHandler();
			
			itsCleanupHandler = NULL;
		}
		
		// Make the new thread belong to this process and save the old one
		itsThread.swap( looseThread );
		
		// Save the binary image that we're running from and set the new one.
		// We can't use stack storage because we run the risk of the thread terminating.
		swap( itsOldMainEntry, itsMainEntry );
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessRunning;  // a new process is runnable
		
		Ped::AdjustSleepForActivity();
		
		Suspend();
	}
	
	void Process::Reexec( Reexec_Function f, void* _1,
	                                         void* _2,
	                                         void* _3,
	                                         void* _4,
	                                         void* _5,
	                                         void* _6,
	                                         void* _7 )
	{
		itsReexecArgs[0] = (void*) f;
		itsReexecArgs[1] = _1;
		itsReexecArgs[2] = _2;
		itsReexecArgs[3] = _3;
		itsReexecArgs[4] = _4;
		itsReexecArgs[5] = _5;
		itsReexecArgs[6] = _6;
		itsReexecArgs[7] = _7;
		
		// Declare this first so it goes out of scope last
		n::owned< N::ThreadID > looseThread;
		
		its_memory_data->clear_memory_mappings();
		
		CloseMarkedFileDescriptors( *itsFileDescriptors );
		
		ClearPendingSignals();
		
		ResetSignalHandlers();
		
		// We always spawn a new thread for the exec'ed process.
		// If we've forked, then the thread is null, but if not, it's the
		// current thread -- be careful!
		
		const std::size_t defaultStackSize = DefaultThreadStackSize();
		
		const std::size_t minimumStackSize = 64 * 1024;
		
		const std::size_t stackSize = std::max( defaultStackSize, minimumStackSize );
		
		// Create the new thread
		looseThread = N::NewThread< Process::ThreadEntry >( this, stackSize );
		
		// Make the new thread belong to this process and save the old one
		itsThread.swap( looseThread );
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessRunning;  // a new process is runnable
		
		Ped::AdjustSleepForActivity();
		
		Suspend();
	}
	
	int Process::SetErrno( int errorNumber )
	{
		if ( itsErrno != NULL )
		{
			*itsErrno = errorNumber;
		}
		
		return errorNumber == 0 ? 0 : -1;
	}
	
	pid_t Process::GetPGID() const
	{
		return itsProcessGroup.get() ? itsProcessGroup->ID() : 0;
	}
	
	pid_t Process::GetSID()  const
	{
		return itsProcessGroup.get() ? itsProcessGroup->GetSID() : 0;
	}
	
	const boost::shared_ptr< IOHandle >& Process::ControllingTerminal() const
	{
		if ( itsProcessGroup.get() )
		{
			return GetProcessGroup()->GetSession()->GetControllingTerminal();
		}
		
		static boost::shared_ptr< IOHandle > null;
		
		return null;
	}
	
	FSTreePtr Process::GetCWD() const
	{
		return itsCWD->GetFile();
	}
	
	void Process::ChangeDirectory( const FSTreePtr& newCWD )
	{
		itsCWD = newCWD->ChangeToDirectory();
	}
	
	void Process::SuspendForFork( pid_t childPID )
	{
		itsForkedChildPID = childPID;
		
		itsInterdependence = kProcessForking;
		itsSchedule        = kProcessFrozen;
		
		itsStackFramePtr = get_vfork_frame_pointer();
		
		SaveRegisters( &itsSavedRegisters );
		
		Suspend();
	}
	
	void Process::ResumeAfterFork()
	{
		ASSERT( itsInterdependence == kProcessForking );
		ASSERT( itsSchedule        == kProcessFrozen  );
		
		ASSERT( itsForkedChildPID != 0 );
		
		using recall::get_stack_frame_pointer;
		
		recall::stack_frame_pointer vfork_fp = get_vfork_frame_pointer(   );
		recall::stack_frame_pointer stack_fp = get_stack_frame_pointer( 2 );
		
		// Stack grows down
		const bool stack_fault = stack_fp > vfork_fp;
		
		Resume();
		
		itsInterdependence = Forked() ? kProcessForked
		                              : kProcessIndependent;
		
		pid_t child = itsForkedChildPID;
		
		itsForkedChildPID = 0;
		
		if ( itsPID == 1 )
		{
			itsSchedule = kProcessSleeping;
			
			return;
		}
		
		if ( stack_fault )
		{
			DeliverFatalSignal( SIGSTKFLT );
		}
		
		LeaveSystemCall();
		
		resume_vfork( child );
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
		
		ASSERT( itsCleanupHandler == NULL );
		
		using std::swap;
		
		swap( itsCleanupHandler, parent.itsCleanupHandler );
		
		parent.Exit( exit_status );
	}
	
	struct notify_param
	{
		pid_t  pid;
		bool   is_session_leader;
	};
	
	void* Process::notify_process( void* param, pid_t, Process& process )
	{
		const notify_param& pb = *(notify_param*) param;
		
		if ( pb.is_session_leader  &&  process.GetSID() == pb.pid )
		{
			process.Raise( SIGHUP );
		}
		
		if ( process.GetPPID() == pb.pid )
		{
			process.Orphan();
		}
		
		return NULL;
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
		
		its_memory_data->clear_memory_mappings();
		
		// This could yield, e.g. in OTCloseProvider() with sync idle events
		itsFileDescriptors.reset();
		
		itsCWD.reset();
		
		itsProcessGroup.reset();
		
		if ( itsCleanupHandler != NULL )
		{
			itsCleanupHandler();
		}
		
		itsLifeStage = kProcessZombie;
		
		Process& parent = GetProcess( ppid );
		
		if ( ppid > 1  &&  parent.WaitsForChildren() )
		{
			parent.Raise( SIGCHLD );
		}
		else
		{
			Release();
		}
		
		notify_param param = { pid, isSessionLeader };
		
		for_each_process( &notify_process, &param );
		
		Ped::AdjustSleepForActivity();
		
		if ( !Forked() )
		{
			n::owned< N::ThreadID > savedThreadID = itsThread;
			
			gCurrentProcess = NULL;
			
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
		
		notify_reaper();
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
		
		itsSchedule = kProcessRunning;
		
		ResumeTimer();
	}
	
	void Process::Pause( ProcessSchedule newSchedule )
	{
		itsSchedule = newSchedule;
		
		Suspend();
		
		itsStackFramePtr = recall::get_stack_frame_pointer();
		
		SaveRegisters( &itsSavedRegisters );
		
		if ( newSchedule == kProcessStopped )
		{
			N::SetThreadState( GetThread(), N::kStoppedThreadState );
		}
		else
		{
			// Ignore errors so we don't throw in critical sections
			(void) ::YieldToAnyThread();
		}
		
		Resume();
	}
	
	void Process::DeliverSignal( int signo )
	{
		typedef void (*signal_handler_t)(int);
		
		signal_handler_t action = GetSignalAction( signo ).sa_handler;
		
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
			
			switch ( signo )
			{
				case SIGQUIT:
				case SIGILL:
				case SIGTRAP:
				case SIGABRT:
				//case SIGEMT:
				case SIGFPE:
				case SIGBUS:
				case SIGSEGV:
				case SIGSTKFLT:
				case SIGSYS:
					// create core image
					signo |= 0x80;
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
					itsResult = signo;  // indicates fatal signal
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
			AddPendingSignal( signo );
			
			Continue();
		}
	}
	
	// Doesn't return if the process was current and receives a fatal signal while stopped.
	// But always returns when *raising* a fatal signal.
	void Process::Raise( int signo )
	{
		if ( itsLifeStage >= kProcessTerminating  ||  itsResult != 0 )
		{
			return;
		}
		
		if ( IsBeingTraced()  &&  signo != SIGKILL )
		{
			Stop();
		}
		else
		{
			DeliverSignal( signo );
		}
	}
	
	// This function doesn't return if the process receives a fatal signal.
	bool Process::HandlePendingSignals( Interruptibility interrupting )
	{
		if ( itsLifeStage > kProcessLive )
		{
			return false;  // Don't try to handle signals in terminated processes
		}
		
		if ( itsAlarmClock )
		{
			UInt64 now = N::Microseconds();
			
			if ( now > itsAlarmClock )
			{
				itsAlarmClock = 0;
				
				Raise( SIGALRM );
			}
		}
		
		if ( itsResult != 0 )
		{
			// Fatal signal received.  Terminate.
			if ( itMayDumpCore && WCOREDUMP( itsResult ) )
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
		
		return DeliverPendingSignals( interrupting );
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
			
			// Stoppers (pause, sigsuspend) never restart, but don't throw
			HandlePendingSignals( kInterruptNever );
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
	
	
	static const UInt32 gMinimumSleepIntervalTicks = 2;
	
	void Process::Breathe()
	{
		Pause( kProcessRunning );
	}
	
	void Process::Yield()
	{
		Pause( kProcessSleeping );
	}
	
	void Process::AsyncYield()
	{
		++itsAsyncOpCount;
		
		Yield();
		
		--itsAsyncOpCount;
	}
	
	// This function doesn't return if we received a fatal signal.
	void Yield( Interruptibility interrupting )
	{
		ASSERT( gCurrentProcess != NULL );
		
		gCurrentProcess->Yield();
		
		// Doesn't return if we received a fatal signal.
		gCurrentProcess->HandlePendingSignals( interrupting );
	}
	
	// declared in Process/AsyncYield.hh
	void AsyncYield()
	{
		if ( gCurrentProcess == NULL )
		{
			// Ignore errors so we don't throw in critical sections
			(void) ::YieldToAnyThread();
		}
		else
		{
			Ped::AdjustSleepForActivity();
			
			gCurrentProcess->AsyncYield();
		}
	}
	
	// This function doesn't return if we received a fatal signal.
	void Breathe()
	{
		ASSERT( gCurrentProcess != NULL );
		
		// Check for fatal signals; don't throw EINTR
		gCurrentProcess->HandlePendingSignals( kInterruptNever );
		
		const UInt64 now = N::Microseconds();
		
		if ( now - gCurrentProcess->GetTimeOfLastResume() > 20000 )
		{
			Ped::AdjustSleepForActivity();
			
			gCurrentProcess->Breathe();
			
			// Check for fatal signals again
			gCurrentProcess->HandlePendingSignals( kInterruptNever );
		}
	}
	
	void TryAgainLater( bool isNonblocking )
	{
		if ( isNonblocking )
		{
			p7::throw_errno( EAGAIN );
		}
		
		// I/O is restartable
		Yield( kInterruptUnlessRestarting );
	}
	
}

