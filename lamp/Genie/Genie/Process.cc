/*	==========
 *	Process.cc
 *	==========
 */

#include "Genie/Process.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

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
#include <time.h>

// POSIX
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// Relix
#include "relix/syscalls.h"
#include "relix/config/syscall_stacks.hh"
#include "relix/signal/signal_traits.hh"

// Iota
#include "iota/strings.hh"

// gear
#include "gear/parse_decimal.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// Recall
#include "recall/stack_frame.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Aliases.hh"
#include "Nitrogen/Threads.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"

// Recall
#include "recall/backtrace.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/primitives/stat.hh"

// vfs-relix
#include "vfs/primitives/exec.hh"

// relix-kernel
#include "relix/config/mini.hh"

// Genie
#include "Genie/caught_signal.hh"
#include "Genie/Devices.hh"
#include "Genie/Dispatch/system_call.68k.hh"
#include "Genie/Dispatch/system_call.ppc.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/Faults.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/open.hh"
#include "Genie/FS/opendir.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/ProcessList.hh"
#include "Genie/Process/AsyncYield.hh"
#include "Genie/scheduler.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemConsole.hh"
#include "Genie/userland.hh"
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
	
	
	static uint64_t microseconds()
	{
		return clock();
	}
	
	extern "C" _relix_system_parameter_block global_parameter_block;
	
	_relix_system_parameter_block global_parameter_block =
	{
		NULL,  // current user
		
		sizeof (_relix_system_parameter_block),
		sizeof (_relix_user_parameter_block),
		
		TARGET_CPU_68K ? &dispatch_68k_system_call :
		TARGET_CPU_PPC ? &dispatch_ppc_system_call
		               : NULL,
		
		&microseconds
	};
	
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
		return get_process( pid );
	}
	
	Process* FindProcess( pid_t pid )
	{
		return lookup_process( pid );
	}
	
	void DeliverFatalSignal( int signo )
	{
		typedef void (*signal_handler_t)(int);
		
		if ( gCurrentProcess != NULL )
		{
			signal_handler_t handler = gCurrentProcess->GetSignalAction( signo ).sa_handler;
			
			if ( handler != SIG_DFL  &&  handler != SIG_IGN )
			{
				call_signal_handler( handler, signo );
			}
			
			gCurrentProcess->Terminate( signo | 0x80 );
		}
		
		if ( TARGET_CONFIG_DEBUGGING )
		{
			::DebugStr( "\p" "Fatal condition occurred on main thread" );
		}
		else
		{
			Mac::SysBeep();
			Mac::SysBeep();
			Mac::SysBeep();
		}
		
		::ExitToShell();  // not messing around
	}
	
	
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
			
			relix_entry relix_main = its_exec_handle->get_main_entry_point();
			
			ENTER_USERMAIN();
			
			exit_status = relix_main( argc,
			                          argv,
			                          envp,
			                          &global_parameter_block );
			
			EXIT_USERMAIN();
			
			// Not reached by regular tools, since they call exit()
		}
		
		// Accumulate any user time between last system call (if any) and return from main()
		EnterSystemCall();
		
		// For code fragments, static destruction occurs here.
		its_exec_handle.reset();
		
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
	
	
	static void* measure_stack_limit()
	{
	#ifdef __MACOS__
		
		const unsigned extra_stack = TARGET_CPU_68K * 10;
		
		return   (char*) recall::get_frame_pointer()
		       - (N::ThreadCurrentStackSpace( N::GetCurrentThread() ) + extra_stack);
		
	#endif
		
		return NULL;
	}
	
	pascal void* Process::ThreadEntry( void* param )
	{
		Process* process = reinterpret_cast< Process* >( param );
		
		process->its_pb.stack_bottom = Init_Thread();
		process->its_pb.stack_limit  = measure_stack_limit();
		
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
	
	static plus::string first_disk_block( const vfs::node* file )
	{
		const size_t buffer_length = 512;
		
		plus::var_string result;
		
		char* p = result.reset( buffer_length );
		
		ssize_t n_read = 0;
		
		try
		{
			const vfs::filehandle_ptr fh = open( file, O_RDONLY, 0 );
			
			StreamHandle& stream = IOHandle_Cast< StreamHandle >( *fh.get() );
			
			n_read = stream.Read( p, buffer_length );
		}
		catch ( ... )
		{
		}
		
		result.resize( n_read );
		
		return result.move();
	}
	
	static void Normalize( const char* path, ExecContext& context, const FSTree* cwd )
	{
		OSType type = 0;
		
		try
		{
			const FSSpec fileSpec = GetFSSpecFromFSTree( context.executable );
			
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
		
		const plus::string block = first_disk_block( context.executable.get() );
		
		const ssize_t bytes = block.size();
		
		const char* data = block.c_str();
		
		const bool has_shebang = bytes > 2 && data[0] == '#' && data[1] == '!';
		
		if ( type == 'TEXT'  ||  (type == 0  &&  has_shebang) )
		{
			context.interpreterPath = "/bin/sh";  // default
			bool hasArg = false;
			
			if ( has_shebang )
			{
				const char* end = data + bytes;
				
				const char* cr = std::find( data, end, '\r' );
				const char* lf = std::find( data, end, '\n' );
				
				const char* nl = std::min( cr, lf );
				
				if ( nl == end )
				{
					throw NotExecutable();  // #! line too long
				}
				
				const char* space = std::find( data, nl, ' ' );
				
				hasArg = space != nl;
				
				context.interpreterPath.assign( &data[2], space );
				
				if ( hasArg )
				{
					context.interpreterArg.assign( space + 1, nl );
				}
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
		else if ( type != 0 )
		{
			throw NotExecutable();
		}
	}
	
	
	static boost::intrusive_ptr< ProcessGroup > NewProcessGroup( pid_t pgid, Session& session )
	{
		return new ProcessGroup( pgid, session );
	}
	
	static void* find_process_group( void* param, pid_t, Process& process )
	{
		const pid_t pgid = *(pid_t*) param;
		
		if ( process.GetPGID() == pgid )
		{
			return &process.GetProcessGroup();
		}
		
		return NULL;
	}
	
	ProcessGroup* FindProcessGroup( pid_t pgid )
	{
		void* result = for_each_process( &find_process_group, &pgid );
		
		ProcessGroup* group = (ProcessGroup*) result;
		
		return group;
	}
	
	boost::intrusive_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, Session& session )
	{
		ProcessGroup* pgrp = FindProcessGroup( pgid );
		
		if ( pgrp == NULL )
		{
			return NewProcessGroup( pgid, session );
		}
		
		if ( &pgrp->get_session() != &session )
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
	
	static inline _relix_user_parameter_block user_pb_for_init()
	{
		_relix_user_parameter_block pb = { NULL };
		
		return pb;
	}
	
	static inline _relix_user_parameter_block copy_user_pb( const _relix_user_parameter_block& pb )
	{
		_relix_user_parameter_block result = pb;
		
		result.cleanup = NULL;
		
		return result;
	}
	
	Process::Process( RootProcess ) 
	:
		its_pb                ( user_pb_for_init() ),
		itsPPID               ( 0 ),
		itsPID                ( 1 ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( NewProcessGroup( itsPID, *NewSession( itsPID ) ) ),
		itsStackFramePtr      ( NULL ),
		itsAlarmClock         ( 0 ),
		itsName               ( "init" ),
		its_fs_info           ( relix::fs_info::create( opendir( FSRoot() ) ) ),
		itsFileDescriptors    ( relix::fd_table::create() ),
		its_signal_handlers   ( relix::signal_handlers::create() ),
		itsLifeStage          ( kProcessLive ),
		itsInterdependence    ( kProcessIndependent ),
		itsSchedule           ( kProcessSleeping ),
		itsResult             ( 0 ),
		itsAsyncOpCount       ( 0 ),
		itsProgramFile        ( FSRoot() ),
		its_memory_data       ( root_memory_data() ),
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
		
		relix::fd_table& fds = *itsFileDescriptors;
		
		fds[ 0 ] =
		fds[ 1 ] = GetSimpleDeviceHandle( "null"    );
		fds[ 2 ] = GetSimpleDeviceHandle( "console" );
		
		InstallExceptionHandlers();
	}
	
	Process::Process( Process& parent, pid_t pid, pid_t ppid, pid_t tid ) 
	:
		TimeKeeper            (),  // Reset resource utilization on fork
		SignalReceiver        ( parent ),
		its_pb                ( copy_user_pb( parent.its_pb ) ),
		itsPPID               ( ppid ? ppid : parent.GetPID() ),
		itsPID                ( pid ),
		itsForkedChildPID     ( 0 ),
		itsProcessGroup       ( parent.itsProcessGroup ),
		itsStackFramePtr      ( NULL ),
		itsAlarmClock         ( 0 ),
		itsName               ( parent.ProgramName() ),
		its_fs_info           ( parent.its_fs_info ),
		itsFileDescriptors    ( parent.itsFileDescriptors ),
		its_signal_handlers   ( parent.its_signal_handlers ),
		itsLifeStage          ( kProcessStarting ),
		itsInterdependence    ( kProcessIndependent ),
		itsSchedule           ( kProcessRunning ),
		itsResult             ( 0 ),
		itsAsyncOpCount       ( 0 ),
		itsProgramFile        ( parent.itsProgramFile ),
		its_exec_handle       ( parent.its_exec_handle ),
		its_memory_data       ( parent.its_memory_data ),
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
		
		mark_process_active( pid );
	}
	
	Process::~Process()
	{
	}
	
	void Process::unshare_fs_info()
	{
		its_fs_info = duplicate( *its_fs_info );
	}
	
	void Process::unshare_files()
	{
		itsFileDescriptors = duplicate( *itsFileDescriptors );
	}
	
	void Process::unshare_signal_handlers()
	{
		its_signal_handlers = duplicate( *its_signal_handlers );
	}
	
	unsigned int Process::SetAlarm( unsigned int seconds )
	{
		uint64_t now = clock();
		
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
	
	Process& Process::vfork()
	{
		Process& child = NewProcess( *this );
		
		child.unshare_fs_info();
		child.unshare_files();
		child.unshare_signal_handlers();
		
		// suspend parent for vfork
		
		itsForkedChildPID = child.GetPID();
		
		itsInterdependence = kProcessForking;
		itsSchedule        = kProcessFrozen;
		
		itsStackFramePtr = (recall::stack_frame_pointer) get_vfork_frame_pointer();
		
		Suspend();
		
		// activate child
		
		child.itsInterdependence = kProcessForked;
		
		gCurrentProcess = &child;
		
		global_parameter_block.current_user = &child.its_pb;
		
		return child;
		
	}
	
	static void close_fd_on_exec( void* keep, int fd, vfs::file_descriptor& desc )
	{
		if ( desc.will_close_on_exec()  &&  fd != *(int*) keep )
		{
			desc.handle.reset();
		}
	}
	
	static void CloseMarkedFileDescriptors( relix::fd_table& fileDescriptors, int keep_fd = -1 )
	{
		// Close file descriptors with close-on-exec flag.
		
		fileDescriptors.for_each( &close_fd_on_exec, &keep_fd );
	}
	
	static void CheckProgramFile( const FSTreePtr& programFile )
	{
		struct ::stat sb;
		
		stat( programFile.get(), sb );
		
		if ( S_ISDIR( sb.st_mode ) )
		{
			p7::throw_errno( EISDIR );
		}
		
		if ( (sb.st_mode & S_IXUSR) == 0 )
		{
			p7::throw_errno( EACCES );
		}
	}
	
	static std::size_t ThreadStackSize()
	{
		const ::Size minimumStackSize = (CONFIG_MINI ? 32 : 64) * 1024;
		
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		return std::max( size, minimumStackSize );
	}
	
	class thing_that_may_resume_after_vfork
	{
		private:
			pid_t its_ppid;
		
		public:
			thing_that_may_resume_after_vfork() : its_ppid( 0 )
			{
			}
			
			void enable( pid_t ppid )
			{
				its_ppid = ppid;
			}
			
			~thing_that_may_resume_after_vfork();
	};
	
	thing_that_may_resume_after_vfork::~thing_that_may_resume_after_vfork()
	{
		if ( its_ppid )
		{
			GetProcess( its_ppid ).ResumeAfterFork();
		}
	}
	
	void Process::Exec( const char*         path,
	                    const char* const   argv[],
	                    const char* const*  envp )
	{
		// Declare this first so it goes out of scope last
		thing_that_may_resume_after_vfork resume;
		
		n::owned< N::ThreadID > looseThread;
		
		FSTreePtr cwd = GetCWD();
		
		// Somehow (not GetCWD()) this fails in non-debug 68K in 7.6
		FSTreePtr programFile = ResolvePathname( path, cwd.get() );
		
		ResolveLinks_InPlace( programFile );
		
		CheckProgramFile( programFile );
		
		// Do we take the name before or after normalization?
		itsName = programFile->name();
		
		ExecContext context( programFile, argv );
		
		Normalize( path, context, cwd.get() );
		
		int script_fd = -1;
		
		if ( !context.interpreterPath.empty() )
		{
			const bool has_arg = !context.interpreterArg.empty();
			
			const char* script_path = context.argVector[ 1 + has_arg ];
			
			if ( std::memcmp( script_path, STR_LEN( "/dev/fd/" ) ) == 0 )
			{
				const char* fd_name = script_path + STRLEN( "/dev/fd/" );
				
				script_fd = gear::parse_unsigned_decimal( fd_name );
			}
		}
		
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
		
		shared_exec_handle executable = exec( itsProgramFile.get() );
		
		// We always spawn a new thread for the exec'ed process.
		// If we've forked, then the thread is null, but if not, it's the
		// current thread -- be careful!
		
		const std::size_t stackSize = ThreadStackSize();
		
		// Create the new thread
		looseThread = N::NewThread< Process::ThreadEntry >( this, stackSize );
		
		if ( its_pb.cleanup != NULL )
		{
			ENTER_USERLAND();
			
			its_pb.cleanup();
			
			EXIT_USERLAND();
			
			its_pb.cleanup = NULL;
		}
		
		// Make the new thread belong to this process and save the old one
		itsThread.swap( looseThread );
		
		// Save the binary image that we're running from and set the new one.
		swap( executable, its_exec_handle );
		
		// Lose the current executable.  If we're not vforked and the
		// execution unit isn't cached, it's now gone.  But that's okay
		// since the thread terminates in execve().
		executable.reset();
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessRunning;  // a new process is runnable
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		Suspend();
		
		if ( looseThread.get() == N::kNoThreadID )
		{
			resume.enable( itsPPID );
		}
	}
	
	void Process::Reexec( Reexec_Function f, void* _1,
	                                         void* _2,
	                                         void* _3,
	                                         void* _4,
	                                         void* _5,
	                                         void* _6,
	                                         void* _7 )
	{
		thing_that_may_resume_after_vfork resume;
		
		n::owned< N::ThreadID > looseThread = SpawnThread( (Clone_Function) f, _1 );
		
	//	itsReexecArgs[0] = (void*) f;
	//	itsReexecArgs[1] = _1;
		itsReexecArgs[2] = _2;
		itsReexecArgs[3] = _3;
		itsReexecArgs[4] = _4;
		itsReexecArgs[5] = _5;
		itsReexecArgs[6] = _6;
		itsReexecArgs[7] = _7;
		
		CloseMarkedFileDescriptors( *itsFileDescriptors );
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		Suspend();
		
		if ( looseThread.get() == N::kNoThreadID )
		{
			resume.enable( itsPPID );
		}
	}
	
	n::owned< N::ThreadID > Process::SpawnThread( Clone_Function f, void* arg )
	{
		itsReexecArgs[0] = (void*) f;
		itsReexecArgs[1] = arg;
		itsReexecArgs[2] = 
		itsReexecArgs[3] =
		itsReexecArgs[4] =
		itsReexecArgs[5] =
		itsReexecArgs[6] =
		itsReexecArgs[7] = NULL;
		
		ClearPendingSignals();
		
		ResetSignalHandlers();
		
		const std::size_t stackSize = ThreadStackSize();
		
		// Create the new thread
		n::owned< N::ThreadID > looseThread = N::NewThread< Process::ThreadEntry >( this, stackSize );
		
		// Make the new thread belong to this process and save the old one
		itsThread.swap( looseThread );
		
		itsLifeStage       = kProcessLive;
		itsInterdependence = kProcessIndependent;
		itsSchedule        = kProcessRunning;  // a new process is runnable
		
		return looseThread;
	}
	
	int Process::SetErrno( int errorNumber )
	{
		if ( its_pb.errno_var != NULL )
		{
			*its_pb.errno_var = errorNumber;
		}
		
		return errorNumber == 0 ? 0 : -1;
	}
	
	const plus::string& Process::GetCmdLine() const
	{
		return its_memory_data.get() ? its_memory_data.get()->get_cmdline()
		                             : plus::string::null;
	}
	
	pid_t Process::GetPGID() const
	{
		return itsProcessGroup.get() ? itsProcessGroup->id() : 0;
	}
	
	pid_t Process::GetSID()  const
	{
		return itsProcessGroup.get() ? itsProcessGroup->getsid() : 0;
	}
	
	const IOPtr& Process::ControllingTerminal() const
	{
		if ( itsProcessGroup.get() )
		{
			return GetProcessGroup().get_session().get_ctty();
		}
		
		static IOPtr null;
		
		return null;
	}
	
	FSTreePtr Process::GetCWD() const
	{
		return its_fs_info->getcwd()->GetFile();
	}
	
	void Process::ChangeDirectory( const FSTreePtr& newCWD )
	{
		its_fs_info->chdir( opendir( newCWD.get() ) );
	}
	
	void Process::ResumeAfterFork()
	{
		ASSERT( itsInterdependence == kProcessForking );
		ASSERT( itsSchedule        == kProcessFrozen  );
		
		ASSERT( itsForkedChildPID != 0 );
		
		const int depth = 4 + TARGET_CPU_68K;
		
		using recall::get_stack_frame_pointer;
		
		typedef recall::stack_frame_pointer fp_t;
		
		fp_t vfork_fp = (fp_t) get_vfork_frame_pointer(       );
		fp_t stack_fp =        get_stack_frame_pointer( depth );
		
		// Stack grows down
		const bool stack_fault = !CONFIG_SYSCALL_STACKS  &&  stack_fp > vfork_fp;
		
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
	
	void Process::fork_and_exit( int exit_status )
	{
		Process& parent = *this;
		
		Process& child = NewProcess( parent );
		
		child.itsLifeStage       = kProcessLive;
		
		child.itsThread = parent.itsThread;
		
		ASSERT( child.its_pb.cleanup == NULL );
		
		using std::swap;
		
		swap( child.its_pb.cleanup, parent.its_pb.cleanup );
		
		parent.Exit( exit_status );
		
		child.Resume();
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
	
	void Process::ResetSignalHandlers()
	{
		its_signal_handlers->reset_handlers();
	}
	
	const struct sigaction& Process::GetSignalAction( int signo ) const
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		return its_signal_handlers->get( signo - 1 );
	}
	
	void Process::SetSignalAction( int signo, const struct sigaction& action )
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		its_signal_handlers->set( signo - 1, action );
		
		if ( action.sa_handler == SIG_IGN )
		{
			ClearPendingSignalSet( 1 << signo - 1 );
		}
	}
	
	void Process::ResetSignalAction( int signo )
	{
		const struct sigaction default_sigaction = { SIG_DFL };
		
		SetSignalAction( signo, default_sigaction );
	}
	
	bool Process::WaitsForChildren() const
	{
		const struct sigaction& chld = GetSignalAction( SIGCHLD );
		
		enum
		{
			sa_nocldwait
			
		#ifdef SA_NOCLDWAIT
			
			= SA_NOCLDWAIT
			
		#endif
			
		};
		
		return chld.sa_handler != SIG_IGN  &&  (chld.sa_flags & sa_nocldwait) == 0;
	}
	
	// This function doesn't return if the process is current.
	void Process::Terminate()
	{
		mark_process_inactive( GetPID() );
		
		if ( WCOREDUMP( itsResult )  &&  itMayDumpCore )
		{
			DumpBacktrace();
		}
		
		itsLifeStage = kProcessTerminating;
		itsSchedule  = kProcessUnscheduled;
		
		pid_t ppid = GetPPID();
		pid_t pid  = GetPID();
		pid_t sid  = GetSID();
		
		bool isSessionLeader = pid == sid;
		
		// This could yield, e.g. in OTCloseProvider() with sync idle events
		itsFileDescriptors.reset();
		
		its_fs_info.reset();
		
		itsProcessGroup.reset();
		
		if ( its_pb.cleanup != NULL )
		{
			ENTER_USERLAND();
			
			its_pb.cleanup();
			
			EXIT_USERLAND();
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
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		Suspend();
		
		/*
			For a vforked process (with null thread) this does nothing.
			Otherwise, reset() is safe because it swaps with a temporary
			before destroying the thread (so the copy that doesn't get
			nulled out when the thread terminates is on the stack).
		*/
		
		itsThread.reset();
		
		// We get here if this is a vforked child, or fork_and_exit().
		
		if ( itsInterdependence == kProcessForked )
		{
			GetProcess( itsPPID ).ResumeAfterFork();  // Calls longjmp()
		}
	}
	
	// This function doesn't return if the process is current.
	void Process::Terminate( int wait_status )
	{
		itsResult = wait_status;
		
		Terminate();
	}
	
	// This function doesn't return if the process is current.
	void Process::Exit( int exit_status )
	{
		itsResult = (exit_status & 0xFF) << 8;
		
		Terminate();
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
		
		global_parameter_block.current_user = NULL;
	}
	
	void Process::Resume()
	{
		gCurrentProcess = this;
		
		global_parameter_block.current_user = &its_pb;
		
		itsStackFramePtr = NULL;  // We don't track this while running
		
		itsSchedule = kProcessRunning;
		
		ResumeTimer();
	}
	
	void Process::Pause( ProcessSchedule newSchedule )
	{
		itsSchedule = newSchedule;
		
		Suspend();
		
		itsStackFramePtr = recall::get_stack_frame_pointer();
		
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
		if ( GetPID() == 1 )
		{
			return;
		}
		
		typedef void (*signal_handler_t)(int);
		
		signal_handler_t action = GetSignalAction( signo ).sa_handler;
		
		if ( action == SIG_IGN )
		{
			return;
		}
		
		AddPendingSignal( signo );
		
		Continue();
	}
	
	// Doesn't return if the process was current and receives a fatal signal while stopped.
	// But always returns when *raising* a fatal signal.
	void Process::Raise( int signo )
	{
		if ( itsLifeStage >= kProcessTerminating  ||  itsResult != 0 )
		{
			return;
		}
		
		DeliverSignal( signo );
	}
	
	// This function doesn't return if the process receives a fatal signal.
	bool Process::HandlePendingSignals( bool may_throw )
	{
		if ( itsLifeStage > kProcessLive )
		{
			return false;  // Don't try to handle signals in terminated processes
		}
		
		if ( itsAlarmClock )
		{
			uint64_t now = clock();
			
			if ( now > itsAlarmClock )
			{
				itsAlarmClock = 0;
				
				Raise( SIGALRM );
			}
		}
		
		if ( itsResult != 0 )
		{
			// Fatal signal received.  Terminate.
			
			Terminate();  // Kills the thread
			
			// Not reached
		}
		
		return DeliverPendingSignals( may_throw );
	}
	
	bool Process::DeliverPendingSignals( bool may_throw )
	{
		bool signal_was_caught = false;
		
		for ( int signo = 1;  signo < NSIG;  ++signo )
		{
			const sigset_t active_signals = GetPendingSignals() & ~GetBlockedSignals();
			
			if ( !active_signals )
			{
				return false;
			}
			
			const sigset_t signo_mask = 1 << signo - 1;
			
			if ( active_signals & signo_mask )
			{
				const struct sigaction& action = GetSignalAction( signo );
				
				if ( action.sa_handler == SIG_IGN )
				{
					continue;
				}
				
				if ( action.sa_handler == SIG_DFL )
				{
					using namespace relix;
					
					const signal_traits traits = global_signal_traits[ signo ];
					
					switch ( traits & signal_default_action_mask )
					{
						case signal_discard:
							break;
						
						case signal_terminate:
							Terminate( signo | (traits & signal_core) );
							break;
						
						case signal_stop:
							Stop();
							break;
						
						case signal_continue:
							Continue();
							break;
					}
					
					continue;
				}
				
				signal_was_caught = true;
				
				if ( !may_throw )
				{
					continue;
				}
				
				const caught_signal caught = { signo, action };
				
				if ( action.sa_flags & SA_RESETHAND  &&  signo != SIGILL  &&  signo != SIGTRAP )
				{
					ResetSignalAction( signo );
				}
				
				throw caught;
			}
		}
		
		return signal_was_caught;
	}
	
	// Stops the process' thread.  Eventually returns.
	void Process::Stop()
	{
		ASSERT( gCurrentProcess == this );
		
		mark_process_inactive( itsPID );
		
		Pause( kProcessStopped );
		
		mark_process_active( itsPID );
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
			gCurrentProcess->AsyncYield();
		}
	}
	
}

