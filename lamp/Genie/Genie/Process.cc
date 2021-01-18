/*	==========
 *	Process.cc
 *	==========
 */

#include "Genie/Process.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// Standard C++
#include <vector>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

// POSIX
#include <fcntl.h>
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// mac-sys-utils
#include "mac_sys/exit_to_shell.hh"

// relix-include
#include "relix/syscalls.h"

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

// Recall
#include "recall/backtrace.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/file_descriptor.hh"
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/primitives/open.hh"
#include "vfs/primitives/stat.hh"

// MacVFS
#include "MacVFS/util/get_Mac_type_code.hh"

// relix-kernel
#include "relix/api/deliver_fatal_signal.hh"
#include "relix/api/getcwd.hh"
#include "relix/api/get_process_group.hh"
#include "relix/api/root.hh"
#include "relix/api/terminate_current_process.hh"
#include "relix/api/waits_for_children.hh"
#include "relix/config/mini.hh"
#include "relix/config/reexec.hh"
#include "relix/config/syscall_stacks.hh"
#include "relix/fs/console.hh"
#include "relix/glue/system_call.68k.hh"
#include "relix/glue/system_call.ppc.hh"
#include "relix/glue/userland.hh"
#include "relix/signal/signal_process_group.hh"
#include "relix/task/A5_world.hh"
#include "relix/task/alarm_clock.hh"
#include "relix/task/fd_map.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/process_image.hh"
#include "relix/task/process_resources.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/session.hh"
#include "relix/time/cpu_time_checkpoint.hh"

// Genie
#include "Genie/Faults.hh"
#include "Genie/ProcessList.hh"
#include "Genie/Process/AsyncYield.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
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
	
	(void) relix::console::log( report.data(), report.size() );
}

namespace relix
{
	
	Genie::Process* gCurrentProcess;  // extern, declared in Faults.cc
	
}

namespace Genie
{
	
	namespace p7 = poseven;
	
	
	using relix::memory_data;
	using relix::gCurrentProcess;
	
	
	static uint64_t microseconds()
	{
		return clock();
	}
	
	static
	long runctl( long selector )
	{
		switch ( selector )
		{
			case runctl_get_max:
				return runctl_end - 1;
			
		#if CONFIG_SYSCALL_STACKS
			
			case runctl_allocate_syscall_stack:
				if ( gCurrentProcess )
				{
					gCurrentProcess->allocate_syscall_stack();
					return 0;
				}
				break;
			
		#endif
			
			default:
				break;
		}
		
		return -1;
	}
	
	extern "C" _relix_system_parameter_block global_parameter_block;
	
	_relix_system_parameter_block global_parameter_block =
	{
		NULL,  // current user
		
		sizeof (_relix_system_parameter_block),
		sizeof (_relix_user_parameter_block),
		
		TARGET_CPU_68K ? &relix::dispatch_68k_system_call :
		TARGET_CPU_PPC ? &relix::dispatch_ppc_system_call
		               : NULL,
		
		&microseconds,
		&runctl,
	};
	
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
	
}

namespace relix
{
	
	void deliver_fatal_signal( int signo )
	{
		using namespace Genie;
		
		typedef void (*signal_handler_t)(int);
		
		if ( gCurrentProcess != NULL )
		{
			relix::process& proc = gCurrentProcess->get_process();
			
			signal_handler_t handler = proc.get_sigaction( signo ).sa_handler;
			
			if ( handler != SIG_DFL  &&  handler != SIG_IGN )
			{
				relix::call_signal_handler( handler, signo );
			}
			
			gCurrentProcess->Terminate( signo | 0x80 );
		}
		
		if ( CONFIG_DEBUGGING )
		{
			::DebugStr( "\p" "Fatal condition occurred on main thread" );
		}
		else
		{
			Mac::SysBeep();
			Mac::SysBeep();
			Mac::SysBeep();
		}
		
		mac::sys::exit_to_shell();  // not messing around
	}
	
}

namespace Genie
{
	
	// This function doesn't return if the process is current.
	static inline
	void exit_process( Process& process, int exit_status )
	{
		const int wait_status = (exit_status & 0xFF) << 8;
		
		process.Terminate( wait_status );
	}
	
	static int reexec_start( void* args[] )
	{
		Reexec_Function f = (Reexec_Function) args[ 0 ];
		
		int exit_status = f( args[ 1 ],
		                     args[ 2 ],
		                     args[ 3 ],
		                     args[ 4 ],
		                     args[ 5 ],
		                     args[ 6 ],
		                     args[ 7 ] );
		
		return exit_status;
	}
	
	
	void* Process::thread_start( void* param, const void* bottom, const void* limit )
	{
		Process* process = reinterpret_cast< Process* >( param );
		
		relix::process_image& image = process->get_process().get_process_image();
		
		_relix_user_parameter_block& pb = image.param_block();
		
		global_parameter_block.current_user = &pb;
		
		// Accumulate any system time between start and entry to main()
		relix::leave_system();
		
		int exit_status = 0;
		
		if ( CONFIG_REEXEC  &&  process->itsReexecArgs[ 0 ] )
		{
			exit_status = reexec_start( process->itsReexecArgs );
		}
		else
		{
			exit_status = image.enter_start_routine( &global_parameter_block );
			
			// Not reached by regular tools, since they call exit()
		}
		
		// Accumulate any time between last syscall (if any) and return from userspace
		relix::enter_system();
		
		exit_process( *process, exit_status );
		
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
		vfs::node_ptr               executable;
		std::vector< const char* >  argVector;
		plus::var_string            interpreter;
		
		ExecContext()  {}
		
		ExecContext( const vfs::node&    executable,
		             char const* const*  argv )
		:
			executable( &executable ),
			argVector ( argv, argv + argv_length( argv ) + 1 )
		{}
	};
	
	static inline p7::errno_t NotExecutable()  { return p7::errno_t( EPERM ); }
	
	static plus::string first_disk_block( const vfs::node& file )
	{
		const size_t buffer_length = 512;
		
		plus::var_string result;
		
		char* p = result.reset( buffer_length );
		
		ssize_t n_read = 0;
		
		try
		{
			const vfs::filehandle_ptr fh = open( file, O_RDONLY, 0 );
			
			n_read = pread( *fh, p, buffer_length, 0 );
		}
		catch ( ... )
		{
		}
		
		result.resize( n_read );
		
		return result.move();
	}
	
	static void Normalize( const char* path, ExecContext& context, const vfs::node& cwd )
	{
		OSType type = 0;
		
		try
		{
			type = vfs::get_Mac_type_code( *context.executable );
		}
		catch ( ... )
		{
			// Assume that non-FSSpec executables are binaries, not scripts
		}
		
		if ( type == 'Tool' )
		{
			return;  // Already normalized
		}
		
		const plus::string block = first_disk_block( *context.executable );
		
		const ssize_t bytes = block.size();
		
		const char* data = block.c_str();
		
		const bool has_shebang = bytes > 2 && data[0] == '#' && data[1] == '!';
		
		if ( type == 'TEXT'  ||  (type == 0  &&  has_shebang) )
		{
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
				
				context.interpreter.assign( &data[ 2 ], nl );
			}
			else
			{
				context.interpreter = "/bin/sh";  // default
			}
			
			// E.g. "$ script foo bar baz"
			// argv == { "script", "foo", "bar", "baz", NULL }
			
			if ( std::strchr( context.argVector[ 0 ], '/' ) == NULL )
			{
				// Overwrite with path
				context.argVector[ 0 ] = path;
				
				// argv == { "/path/to/script", "foo", "bar", "baz", NULL }
			}
			
			char* data = &context.interpreter[ 0 ];
			
			context.argVector.insert( context.argVector.begin(), data );
			
			// argv == { "sh", "script", "foo", "bar", "baz", NULL }
			
			char* p = data;
			
			int i = 0;
			
			while ( char* space = strchr( p, ' ' ) )
			{
				*space = '\0';
				
				p = space + 1;
				
				context.argVector.insert( context.argVector.begin() + ++i, p );
			}
			
			plus::string path = context.interpreter.substr( 0, strlen( data ) );
			
			context.executable = resolve_pathname( *relix::root(), path, cwd );
		}
		else if ( type != 0 )
		{
			throw NotExecutable();
		}
	}
	
	
	static void* find_process_group( void* param, pid_t, Process& process )
	{
		const pid_t pgid = *(pid_t*) param;
		
		relix::process_group& process_group = process.get_process().get_process_group();
		
		if ( process_group.id() == pgid )
		{
			return &process_group;
		}
		
		return NULL;
	}
	
	static boost::intrusive_ptr< relix::process_image >
	//
	new_process_image( const vfs::node&    exe,
	                   const char* const*  argv,
	                   const char* const*  envp )
	{
		return new relix::process_image( exe, argv, envp );
	}
	
	static vfs::filehandle_ptr open_device( const char* path, size_t length )
	{
		return open( *vfs::resolve_absolute_path( *relix::root(), path, length ), O_RDWR, 0 );
	}
	
	Process::Process( RootProcess ) 
	:
		relix::thread( 1,
		               0,
		               *new relix::process() ),
		itsPID                ( 1 ),
		itsForkedChildPID     ( 0 ),
		its_vfork_parent      ( 0 ),
		itsLifeStage          ( kProcessLive ),
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
		
		relix::fd_map& fds = FileDescriptors();
		
		fds[ 2 ] = open_device( STR_LEN( "/dev/console" ) );
		fds[ 1 ] =
		fds[ 0 ] = open_device( STR_LEN( "/dev/null"    ) );
		
		relix::InstallExceptionHandlers();
		
		relix::save_the_A5_world();
	}
	
	Process::Process( Process& parent, pid_t pid, pid_t tid ) 
	:
		relix::thread( tid,
		               parent.signals_blocked(),
		               tid == pid ? *new relix::process( pid, parent.get_process() )
		                          : parent.get_process() ),
		itsPID                ( pid ),
		itsForkedChildPID     ( 0 ),
		its_vfork_parent      ( 0 ),
		itsLifeStage          ( kProcessStarting ),
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
		
	#ifdef __RELIX__
		
		create_task( this );
		
	#endif
	}
	
	Process& Process::vfork()
	{
		Process& child = NewProcess( *this );
		
		child.get_process().unshare_per_fork();
		
		// suspend parent for vfork
		
		suspend_task( this );
		
		itsForkedChildPID = child.GetPID();
		
		child.its_vfork_parent = this;
		
		mark_vfork_stack_frame();
		
		// activate child
		
		child.share_os_thread( *this );
		
		update_os_thread_param( &child );
		
		gCurrentProcess = &child;
		
		return child;
		
	}
	
	static void close_fd_on_exec( void* keep, int fd, vfs::file_descriptor& desc )
	{
		if ( desc.will_close_on_exec() )
		{
			desc.handle.reset();
		}
	}
	
	static void CloseMarkedFileDescriptors( relix::fd_map& fileDescriptors )
	{
		// Close file descriptors with close-on-exec flag.
		
		fileDescriptors.for_each( &close_fd_on_exec, NULL );
	}
	
	static void CheckProgramFile( const vfs::node& programFile )
	{
		struct stat sb;
		
		stat( programFile, sb );
		
		if ( S_ISDIR( sb.st_mode ) )
		{
			p7::throw_errno( EISDIR );
		}
		
		if ( (sb.st_mode & S_IXUSR) == 0 )
		{
			p7::throw_errno( EACCES );
		}
	}
	
	static inline std::size_t minimum_stack_size()
	{
		return (CONFIG_MINI ? 32 : 64) * 1024;
	}
	
	class thing_that_may_resume_after_vfork
	{
		private:
			Process* its_parent;
		
		public:
			thing_that_may_resume_after_vfork() : its_parent()
			{
			}
			
			void enable( Process* parent )
			{
				its_parent = parent;
			}
			
			~thing_that_may_resume_after_vfork();
	};
	
	thing_that_may_resume_after_vfork::~thing_that_may_resume_after_vfork()
	{
		if ( its_parent )
		{
			its_parent->ResumeAfterFork();
		}
	}
	
	static void thread_switch_in( void* param )
	{
		gCurrentProcess = NULL;
		
		destroy_pending();
		
		Process& thread = *(Process*) param;
		
		gCurrentProcess = &thread;
		
		thread.clear_stack_frame_mark();  // We don't track this while running
		
		thread.switch_in();
	}
	
	static void thread_switch_out( void* param )
	{
		Process& thread = *(Process*) param;
		
		thread.switch_out();
		
		gCurrentProcess = NULL;
	}
	
	static relix::os_thread_box new_thread( Process& task )
	{
		const std::size_t min_stack = minimum_stack_size();
		
		return new_os_thread( &Process::thread_start,
		                      &task,
		                      min_stack,
		                      &thread_switch_in,
		                      &thread_switch_out );
	}
	
	void Process::Exec( const char*         path,
	                    const char* const   argv[],
	                    const char* const*  envp )
	{
		// Declare this first so it goes out of scope last
		thing_that_may_resume_after_vfork resume;
		
		relix::os_thread_box looseThread;
		
		relix::process& proc = get_process();
		
		vfs::node_ptr cwd = getcwd( proc );
		
		vfs::node_ptr programFile = resolve_pathname( *relix::root(), path, *cwd );
		
		vfs::resolve_links_in_place( *relix::root(), programFile );
		
		CheckProgramFile( *programFile );
		
		// Do we take the name before or after normalization?
		proc.set_name( programFile->name() );
		
		ExecContext context( *programFile, argv );
		
		Normalize( path, context, *cwd );
		
		CloseMarkedFileDescriptors( FileDescriptors() );
		
		proc.reset_signal_handlers();
		
		// We always spawn a new thread for the exec'ed process.
		
		// Create the new thread
		looseThread = new_thread( *this );
		
		// Save the process image that we're running from and set the new one.
		boost::intrusive_ptr< relix::process_image > old_image = &proc.get_process_image();
		
		proc.set_process_image( *new_process_image( *context.executable,
		                                            &context.argVector.front(),
		                                            envp ) );
		
		// Make the new thread belong to this process and save the old one
		swap_os_thread( looseThread );
		
		// Lose the current process image.  If we're not vforked and the
		// execution unit isn't cached, it's now gone.  But that's okay
		// since the thread terminates in execve().
		old_image.reset();
		
		itsLifeStage       = kProcessLive;
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		if ( its_vfork_parent )
		{
			resume_task  ( its_vfork_parent );
			resume.enable( its_vfork_parent );
			
			its_vfork_parent = NULL;
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
		
		relix::os_thread_box looseThread = SpawnThread( (Clone_Function) f, _1 );
		
	//	itsReexecArgs[0] = (void*) f;
	//	itsReexecArgs[1] = _1;
		itsReexecArgs[2] = _2;
		itsReexecArgs[3] = _3;
		itsReexecArgs[4] = _4;
		itsReexecArgs[5] = _5;
		itsReexecArgs[6] = _6;
		itsReexecArgs[7] = _7;
		
		CloseMarkedFileDescriptors( FileDescriptors() );
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		if ( its_vfork_parent )
		{
			resume_task  ( its_vfork_parent );
			resume.enable( its_vfork_parent );
			
			its_vfork_parent = NULL;
		}
	}
	
	relix::os_thread_box Process::SpawnThread( Clone_Function f, void* arg )
	{
		itsReexecArgs[0] = (void*) f;
		itsReexecArgs[1] = arg;
		itsReexecArgs[2] = 
		itsReexecArgs[3] =
		itsReexecArgs[4] =
		itsReexecArgs[5] =
		itsReexecArgs[6] =
		itsReexecArgs[7] = NULL;
		
		// Create the new thread
		relix::os_thread_box looseThread = new_thread( *this );
		
		// Make the new thread belong to this process and save the old one
		swap_os_thread( looseThread );
		
		itsLifeStage       = kProcessLive;
		
		return looseThread;
	}
	
	pid_t Process::GetPPID() const
	{
		return get_process().getppid();
	}
	
	pid_t Process::GetPGID() const
	{
		return get_process().get_process_group().id();
	}
	
	char Process::run_state_code() const
	{
		if ( gCurrentProcess == this )
		{
			return 'R';
		}
		
		if ( itsLifeStage == kProcessReleased )
		{
			return 'X';
		}
		
		if ( async_op_count() > 0 )
		{
			return 'D';
		}
		
		if ( itsForkedChildPID != 0 )
		{
			return 'V';
		}
		
		relix::process& proc = get_process();
		
		if ( proc.is_stopped() )
		{
			return 'T';
		}
		
		if ( relix::os_thread_id thread = get_os_thread() )
		{
			if ( relix::is_os_thread_stopped( thread ) )
			{
				return 'W';
			}
		}
		
		if ( is_on_run_queue( this ) )
		{
			return 'Q';
		}
		
		if ( proc.is_zombie() )
		{
			return 'Z';
		}
		
		return 'S';
	}
	
	relix::fd_map& Process::FileDescriptors()
	{
		return get_process().get_process_resources().get_fd_map();
	}
	
	void Process::ResumeAfterFork()
	{
		ASSERT( itsForkedChildPID != 0 );
		
		const int depth = 4 + TARGET_CPU_68K;
		
		using recall::get_stack_frame_pointer;
		
		typedef recall::stack_frame_pointer fp_t;
		
		fp_t vfork_fp = (fp_t) get_vfork_frame_pointer(       );
		fp_t stack_fp =        get_stack_frame_pointer( depth );
		
		// Stack grows down
		const bool stack_fault = !CONFIG_SYSCALL_STACKS  &&  stack_fp >= vfork_fp;
		
		update_os_thread_param( this );
		
		gCurrentProcess = this;
		
		pid_t child = itsForkedChildPID;
		
		itsForkedChildPID = 0;
		
		if ( stack_fault )
		{
			relix::deliver_fatal_signal( SIGSTKFLT );
		}
		
		relix::leave_system();
		
		resume_vfork( child );
	}
	
	void Process::fork_and_exit( int exit_status )
	{
		Process& parent = *this;
		
		Process& child = NewProcess( parent );
		
		child.itsLifeStage       = kProcessLive;
		
		child.swap_os_thread( parent );
		
		exit_process( parent, exit_status );
		
		child.update_os_thread_param( &child );
		
		gCurrentProcess = &child;
	}
	
	struct notify_param
	{
		pid_t  pid;
		bool   is_session_leader;
	};
	
	void* Process::notify_process( void* param, pid_t, Process& process )
	{
		// `process` is actually a thread
		
		const notify_param& pb = *(notify_param*) param;
		
		const pid_t that_pid = process.GetPID();
		
		if ( that_pid == pb.pid  &&  process.gettid() != that_pid )
		{
			// This is one of our threads; kill it.
			
			if ( process.itsLifeStage < kProcessTerminating )
			{
				process.Terminate( 0 );  // singly recursive call
			}
			
			if ( process.itsLifeStage == kProcessZombie )
			{
				process.Release();
			}
		}
		
		relix::process&  proc    = process.get_process();
		relix::session&  session = proc.get_process_group().get_session();
		
		if ( pb.is_session_leader  &&  session.id() == pb.pid )
		{
			process.Raise( SIGHUP );
		}
		
		if ( proc.getppid() == pb.pid )
		{
			proc.orphan();
			
			if ( process.itsLifeStage == kProcessZombie )
			{
				process.Release();
			}
		}
		
		return NULL;
	}
	
	// This function doesn't return if the process is current.
	void Process::Terminate( int wait_status )
	{
		if ( WCOREDUMP( wait_status )  &&  itMayDumpCore )
		{
			// prevent reentry if backtrace causes exception
			itMayDumpCore = false;
			
			DumpBacktrace();
		}
		
		itsLifeStage = kProcessTerminating;
		
		relix::process& process = get_process();
		
		process.set_status( wait_status );
		
		pid_t ppid = process.getppid();
		pid_t pid  = GetPID();
		pid_t sid  = process.get_process_group().get_session().id();
		
		bool isSessionLeader = pid == sid;
		
		if ( gettid() == pid )
		{
			process.zombify();
		}
		
		itsLifeStage = kProcessZombie;
		
		if ( gettid() == pid )
		{
			notify_param param = { pid, isSessionLeader };
			
			for_each_process( &notify_process, &param );
			
			Process& parent = GetProcess( ppid );
			
			resume_task( &parent );
			
			if ( ppid > 1  &&  waits_for_children( parent.get_process() ) )
			{
				parent.Raise( SIGCHLD );
			}
			else
			{
				Release();
			}
		}
		
		if ( gCurrentProcess != this )
		{
			return;
		}
		
		relix::restore_the_A5_world();
		
		/*
			For a vforked process (with null thread) this does nothing.
			Otherwise, reset() is safe because it swaps with a temporary
			before destroying the thread (so the copy that doesn't get
			nulled out when the thread terminates is on the stack).
			
			Mark the thread inactive now, in case it doesn't get joined
			right away.  If the OS thread still lives after reset, mark
			the thread active again.
		*/
		
		remove_task( this );
		
		reset_os_thread();
		
		// We get here if this is a vforked child, or fork_and_exit().
		
		if ( its_vfork_parent )
		{
			ASSERT( its_vfork_parent->itsForkedChildPID != 0 );
			ASSERT( its_vfork_parent->itsForkedChildPID == pid );
			
			its_vfork_parent->ResumeAfterFork();  // Calls longjmp()
		}
	}
	
	void Process::Release()
	{
		ASSERT( itsLifeStage == kProcessZombie );
		
		if ( gettid() == GetPID() )
		{
			get_process().clear_ppid();  // Don't match PPID comparisons
		}
		
		itsLifeStage = kProcessReleased;
		
		notify_reaper( this );
	}
	
	void Process::Raise( int signo )
	{
		relix::process& proc = get_process();
		
		if ( GetPID() == 1 )
		{
			return;
		}
		
		proc.set_pending_signal( signo );
		
		relix::woken_os_thread( get_os_thread() );
	}
	
	// declared in Process/AsyncYield.hh
	void AsyncYield()
	{
		if ( gCurrentProcess == NULL )
		{
			relix::os_thread_yield();
		}
		else
		{
			gCurrentProcess->async_yield();
		}
	}
	
}

namespace relix
{
	
	process_group* get_process_group( pid_t pgid )
	{
		using namespace Genie;
		
		return (process_group*) for_each_process( &find_process_group, &pgid );
	}
	
	void signal_process_group( int signo, pid_t pgid )
	{
		if ( pgid != no_pgid )
		{
			Genie::SendSignalToProcessGroup( signo, pgid );
		}
	}
	
	void terminate_current_process( int wait_status )
	{
		Genie::gCurrentProcess->Terminate( wait_status );
	}
	
}
