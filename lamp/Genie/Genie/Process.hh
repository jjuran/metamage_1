/*	==========
 *	Process.hh
 *	==========
 */

#ifndef GENIE_PROCESS_HH
#define GENIE_PROCESS_HH

// Lamp
#include "lamp/parameter_block.h"

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"
#include "plus/var_string.hh"

// Boost
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#include "Mac/Threads/Types/ThreadID.hh"

// Genie
#include "Genie/Exec/MainEntry.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/Process/SavedRegisters.hh"
#include "Genie/Process/SignalReceiver.hh"
#include "Genie/Process/TraceTarget.hh"
#include "Genie/Process/vfork_context.hh"
#include "Genie/ProcessGroup.hh"
#include "Genie/task/fd_table.hh"
#include "Genie/task/memory_data.hh"


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
	
	class Process : public plus::ref_count< Process >,
	                public SignalReceiver,
	                public vfork_context,
	                public TraceTarget
	{
		public:
			enum
			{
				kNoProcessPID   = 0,
				kRootProcessPID = 1
			};
		
		private:
			_lamp_user_parameter_block its_pb;
			
			pid_t itsPPID;
			pid_t itsPID;
			pid_t itsForkedChildPID;
			
			boost::intrusive_ptr< ProcessGroup > itsProcessGroup;
			
			recall::stack_frame_pointer itsStackFramePtr;
			
			SavedRegisters itsSavedRegisters;
			
			UInt64 itsAlarmClock;
			
			plus::string itsName;
			
			boost::shared_ptr< IOHandle > itsCWD;
			
			boost::intrusive_ptr< fd_table > itsFileDescriptors;
			
			ProcessLifeStage        itsLifeStage;
			ProcessInterdependence  itsInterdependence;
			ProcessSchedule         itsSchedule;
			
			int itsResult;
			
			unsigned itsAsyncOpCount;
		
		private:
			FSTreePtr itsProgramFile;
			
			MainEntry itsMainEntry;
			MainEntry itsOldMainEntry;
			
			boost::intrusive_ptr< memory_data > its_memory_data;
			
			void* itsReexecArgs[8];
			
			nucleus::owned< Mac::ThreadID > itsThread;
			
			bool itMayDumpCore;
		
		private:
			void Suspend();
			void Resume();
			void Pause( ProcessSchedule newSchedule );
			
			void Terminate();
			void Terminate( int wait_status );
			
			static void* notify_process( void* param, pid_t, Process& process );
			
			void Orphan();
		
		public:
			struct RootProcess {};
			
			Process( RootProcess );
			Process( Process& parent, pid_t pid, pid_t ppid = 0 );
			
			~Process();
			
			static pascal void* ThreadEntry( void* param );
			
			int Run();
			
			const plus::string& GetCmdLine() const  { return its_memory_data->get_cmdline(); }
			
			typedef void* addr_t;
			
			addr_t add_memory_mapping( const memory_mapping* mapping )
			{
				return its_memory_data->add_memory_mapping( mapping );
			}
			
			void remove_memory_mapping( addr_t key )
			{
				its_memory_data->remove_memory_mapping( key );
			}
			
			bool MayDumpCore() const  { return itMayDumpCore; }
			
			void SuppressCoreDump()  { itMayDumpCore = false; }
			void AllowCoreDump   ()  { itMayDumpCore = true;  }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			
			pid_t GetPGID() const;
			pid_t GetSID()  const;
			
			const boost::intrusive_ptr< ProcessGroup >& GetProcessGroup() const  { return itsProcessGroup; }
			
			void SetProcessGroup( const boost::intrusive_ptr< ProcessGroup >& pgrp )  { itsProcessGroup = pgrp; }
			
			SavedRegisters& GetSavedRegisters()  { return itsSavedRegisters; }
			
			recall::stack_frame_pointer GetStackFramePointer() const  { return itsStackFramePtr; }
			
			ProcessLifeStage  GetLifeStage() const  { return itsLifeStage; }
			ProcessSchedule   GetSchedule () const  { return itsSchedule;  }
			
			int Result() const  { return itsResult; }
			
			unsigned CountAsyncOps() const  { return itsAsyncOpCount; }
			
			void Release();
			
			void Breathe();
			void Yield();
			void AsyncYield();
			
			const boost::shared_ptr< IOHandle >& ControllingTerminal() const;
			
			FSTreePtr GetCWD() const;
			
			void ChangeDirectory( const FSTreePtr& newCWD );
			
			const plus::string& ProgramName()  const  { return itsName;   }
			
			const FSTreePtr& ProgramFile() const  { return itsProgramFile; }
			
			fd_table& FileDescriptors()  { return *itsFileDescriptors; }
			
			unsigned int SetAlarm( unsigned int seconds );
			
			void DeliverSignal( int signal );
			
			void Raise( int signal );
			
			void SuspendForFork( pid_t childPID );
			void ResumeAfterFork();
			
			void UsurpParent( int exit_status );
			
			void Exit( int exit_status );
		
		public:
			bool Forked() const  { return itsThread.get() == Mac::kNoThreadID; }
			
			bool Forking() const  { return itsForkedChildPID != 0; }
			
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
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals( Interruptibility interrupting );
	};
	
	boost::intrusive_ptr< ProcessGroup > FindProcessGroup( pid_t pgid );
	
	boost::intrusive_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, const boost::intrusive_ptr< Session >& session );
	
	void SendSignalToProcessGroup( int sig, pid_t pgid );
	
	Process& CurrentProcess();
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
	void Yield( Interruptibility interrupting );
	
	void Breathe();
	
	void TryAgainLater( bool isNonblocking );
	
}

#endif

