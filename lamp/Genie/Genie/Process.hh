/*	==========
 *	Process.hh
 *	==========
 */

#ifndef GENIE_PROCESS_HH
#define GENIE_PROCESS_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// relix-include
#include "relix/parameter_block.h"

// plus
#include "plus/string.hh"

// Recall
#include "recall/stack_crawl.hh"

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/memory_mapping_fwd.hh"
#include "vfs/node_ptr.hh"
#include "vfs/program_ptr.hh"

// relix-kernel
#include "relix/api/os_thread_api.hh"
#include "relix/api/os_thread_box.hh"
#include "relix/config/syscall_stacks.hh"
#include "relix/task/syscall_stack.hh"
#include "relix/task/thread.hh"
#include "relix/task/vfork_context.hh"

// Genie
#include "Genie/Process/TimeKeeper.hh"


struct sigaction;


namespace relix
{
	
	class fd_table;
	class fs_info;
	class memory_data;
	class process_group;
	class session;
	class signal_handlers;
	
}

namespace Genie
{
	
	class NotExecutableError {};
	
	
	enum ProcessLifeStage
	{
		kProcessStarting,
		kProcessLive,
		kProcessTerminating,
		kProcessZombie,
		kProcessReleased
	};
	
	enum ProcessInterdependence
	{
		kProcessIndependent,
		kProcessForking,
		kProcessForked
	};
	
	enum ProcessSchedule
	{
		kProcessRunning,
		kProcessSleeping,
		kProcessStopped,
		kProcessFrozen,
		kProcessUnscheduled
	};
	
	typedef int (*Reexec_Function)( void* _1,
	                                void* _2,
	                                void* _3,
	                                void* _4,
	                                void* _5,
	                                void* _6,
	                                void* _7 );
	
	typedef int (*Clone_Function)( void* arg );
	
	// Genie::Process is actually a thread, not a process.
	
	class Process : public relix::thread,
	                public TimeKeeper,
	                public relix::vfork_context
	{
		public:
			enum
			{
				kNoProcessPID   = 0,
				kRootProcessPID = 1
			};
		
		private:
			_relix_user_parameter_block its_pb;
			
			pid_t itsPID;
			pid_t itsForkedChildPID;
			
		#if CONFIG_SYSCALL_STACKS
			
			relix::syscall_stack its_syscall_stack;
			
		#endif
			
			recall::stack_frame_pointer itsStackFramePtr;
			
			uint64_t itsAlarmClock;
			
			plus::string itsName;
			
			boost::intrusive_ptr< relix::fs_info > its_fs_info;
			
			boost::intrusive_ptr< relix::fd_table > itsFileDescriptors;
			
			boost::intrusive_ptr< relix::signal_handlers > its_signal_handlers;
			
			ProcessLifeStage        itsLifeStage;
			ProcessInterdependence  itsInterdependence;
			ProcessSchedule         itsSchedule;
			
			int itsResult;
			
			unsigned itsAsyncOpCount;
		
		private:
			vfs::node_ptr itsProgramFile;
			
			vfs::program_ptr  its_exec_handle;
			
			boost::intrusive_ptr< relix::memory_data > its_memory_data;
			
			void* itsReexecArgs[8];
			
			relix::os_thread_box itsThread;
			
			bool itMayDumpCore;
		
		private:
			// non-copyable
			Process           ( const Process& );
			Process& operator=( const Process& );
			
			void Suspend();
			void Resume();
			void Pause( ProcessSchedule newSchedule );
			
			bool DeliverPendingSignals( bool may_throw );
			
			bool WaitsForChildren() const;
			
			void Terminate();
			
			static void* notify_process( void* param, pid_t, Process& process );
			
			void Orphan();
		
		public:
			bool IsBeingTraced() const  { return false; }
			
			void Terminate( int wait_status );
			
			struct RootProcess {};
			
			Process( RootProcess );
			Process( Process& parent, pid_t pid, pid_t ppid, pid_t tid );
			
			~Process();
			
			void unshare_fs_info();
			void unshare_files();
			void unshare_signal_handlers();
			
			static void* thread_start( void* param, const void* bottom, const void* limit );
			
			int Run();
			
			const plus::string& GetCmdLine() const;
			
			typedef void* addr_t;
			
			addr_t add_memory_mapping( const vfs::memory_mapping* mapping );
			
			void msync_memory_mapping( addr_t addr, size_t len, int flags );
			
			void remove_memory_mapping( addr_t key );
			
			bool MayDumpCore() const  { return itMayDumpCore; }
			
			void SuppressCoreDump()  { itMayDumpCore = false; }
			void AllowCoreDump   ()  { itMayDumpCore = true;  }
			
			pid_t gettid() const  { return id(); }
			
			pid_t GetPPID() const;
			pid_t GetPID()  const  { return itsPID;  }
			
			pid_t GetPGID() const;
			pid_t GetSID()  const;
			
			relix::process_group& GetProcessGroup() const;
			
			void SetProcessGroup( relix::process_group& pgrp );
			
			recall::stack_frame_pointer GetStackFramePointer() const  { return itsStackFramePtr; }
			
			ProcessLifeStage  GetLifeStage() const  { return itsLifeStage; }
			ProcessSchedule   GetSchedule () const  { return itsSchedule;  }
			
			int Result() const  { return itsResult; }
			
			unsigned CountAsyncOps() const  { return itsAsyncOpCount; }
			
			void Release();
			
			void Breathe();
			void Yield();
			void AsyncYield();
			
			const vfs::filehandle_ptr& ControllingTerminal() const;
			
			vfs::node_ptr GetCWD() const;
			
			void ChangeDirectory( const vfs::node& new_cwd );
			
			const plus::string& ProgramName()  const  { return itsName;   }
			
			const vfs::node_ptr& ProgramFile() const  { return itsProgramFile; }
			
			relix::fd_table& FileDescriptors()  { return *itsFileDescriptors; }
			
		#if CONFIG_SYSCALL_STACKS
			
			void* get_syscall_stack_memory() const  { return its_syscall_stack.memory; }
			
		#else
			
			void* get_syscall_stack_memory() const  { return NULL; }
			
		#endif
			
			unsigned int SetAlarm( unsigned int seconds );
			
			void ResetSignalHandlers();
			
			const struct sigaction& GetSignalAction( int signo ) const;
			
			void SetSignalAction( int signo, const struct sigaction& action );
			
			void ResetSignalAction( int signo );
			
			void DeliverSignal( int signal );
			
			void Raise( int signal );
			
			void ResumeAfterFork();
			
			void fork_and_exit( int exit_status );
			
			void Exit( int exit_status );
		
		public:
			bool Forked() const  { return itsThread.get() == 0; }
			
			relix::os_thread_id GetThread() const;
			
			Process& vfork();
			
			void Exec( const char*         path,
			           const char* const   argv[],
			           const char* const*  envp );
			
			void Reexec( Reexec_Function f, void* _1,
			                                void* _2,
			                                void* _3,
			                                void* _4,
			                                void* _5,
			                                void* _6,
			                                void* _7 );
			
			relix::os_thread_box SpawnThread( Clone_Function f, void* arg );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals( bool may_throw );
	};
	
	relix::process_group* FindProcessGroup( pid_t pgid );
	
	boost::intrusive_ptr< relix::process_group >
	//
	GetProcessGroupInSession( pid_t pgid, relix::session& session );
	
	void SendSignalToProcessGroup( int sig, pid_t pgid );
	
	Process& CurrentProcess();
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
	boost::intrusive_ptr< relix::session > NewSession( pid_t sid );
	
}

#endif

