/*	==========
 *	Process.hh
 *	==========
 */

#ifndef GENIE_PROCESS_HH
#define GENIE_PROCESS_HH

// Relix
#include "relix/parameter_block.h"
#include "relix/config/syscall_stacks.hh"
#include "relix/task/fd_table.hh"
#include "relix/task/fs_info.hh"
#include "relix/task/memory_data.hh"
#include "relix/task/signal_handlers.hh"
#include "relix/task/syscall_stack.hh"
#include "relix/task/vfork_context.hh"

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// Recall
#include "recall/stack_crawl.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_THREADS_TYPES_THREADID_HH
#include "Mac/Threads/Types/ThreadID.hh"
#endif

// vfs
#include "vfs/program_ptr.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/Process/SignalReceiver.hh"
#include "Genie/Process/TimeKeeper.hh"
#include "Genie/ProcessGroup.hh"


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
	
	class Process : public plus::ref_count< Process >,
	                public TimeKeeper,
	                public SignalReceiver,
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
			
			pid_t itsPPID;
			pid_t itsPID;
			pid_t itsTID;
			pid_t itsForkedChildPID;
			
		#if CONFIG_SYSCALL_STACKS
			
			relix::syscall_stack its_syscall_stack;
			
		#endif
			
			boost::intrusive_ptr< ProcessGroup > itsProcessGroup;
			
			recall::stack_frame_pointer itsStackFramePtr;
			
			UInt64 itsAlarmClock;
			
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
			FSTreePtr itsProgramFile;
			
			vfs::program_ptr  its_exec_handle;
			
			boost::intrusive_ptr< relix::memory_data > its_memory_data;
			
			void* itsReexecArgs[8];
			
			nucleus::owned< Mac::ThreadID > itsThread;
			
			bool itMayDumpCore;
		
		private:
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
			
			static pascal void* ThreadEntry( void* param );
			
			int Run();
			
			const plus::string& GetCmdLine() const;
			
			typedef void* addr_t;
			
			addr_t add_memory_mapping( const vfs::memory_mapping* mapping )
			{
				return its_memory_data->add_memory_mapping( mapping );
			}
			
			void msync_memory_mapping( addr_t addr, size_t len, int flags )
			{
				its_memory_data->msync_memory_mapping( addr, len, flags );
			}
			
			void remove_memory_mapping( addr_t key )
			{
				its_memory_data->remove_memory_mapping( key );
			}
			
			bool MayDumpCore() const  { return itMayDumpCore; }
			
			void SuppressCoreDump()  { itMayDumpCore = false; }
			void AllowCoreDump   ()  { itMayDumpCore = true;  }
			
			pid_t gettid() const  { return itsTID; }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			
			pid_t GetPGID() const;
			pid_t GetSID()  const;
			
			ProcessGroup& GetProcessGroup() const  { return *itsProcessGroup; }
			
			void SetProcessGroup( ProcessGroup& pgrp )  { itsProcessGroup = &pgrp; }
			
			recall::stack_frame_pointer GetStackFramePointer() const  { return itsStackFramePtr; }
			
			ProcessLifeStage  GetLifeStage() const  { return itsLifeStage; }
			ProcessSchedule   GetSchedule () const  { return itsSchedule;  }
			
			int Result() const  { return itsResult; }
			
			unsigned CountAsyncOps() const  { return itsAsyncOpCount; }
			
			void Release();
			
			void Breathe();
			void Yield();
			void AsyncYield();
			
			const IOPtr& ControllingTerminal() const;
			
			FSTreePtr GetCWD() const;
			
			void ChangeDirectory( const FSTreePtr& newCWD );
			
			const plus::string& ProgramName()  const  { return itsName;   }
			
			const FSTreePtr& ProgramFile() const  { return itsProgramFile; }
			
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
			bool Forked() const  { return itsThread.get() == Mac::kNoThreadID; }
			
			Mac::ThreadID GetThread() const;
			
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
			
			nucleus::owned< Mac::ThreadID > SpawnThread( Clone_Function f, void* arg );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals( bool may_throw );
	};
	
	ProcessGroup* FindProcessGroup( pid_t pgid );
	
	boost::intrusive_ptr< ProcessGroup >
	//
	GetProcessGroupInSession( pid_t pgid, relix::session& session );
	
	void SendSignalToProcessGroup( int sig, pid_t pgid );
	
	Process& CurrentProcess();
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
}

#endif

