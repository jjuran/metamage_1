/*	==========
 *	Process.hh
 *	==========
 */

#ifndef GENIE_PROCESS_HH
#define GENIE_PROCESS_HH

// Standard C++
#include <map>
#include <string>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>

// Recall
#include "recall/stack_crawl.hh"

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/Threads.h"

// Genie
#include "Genie/Exec/MainEntry.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/Process/LongJumper.hh"
#include "Genie/Process/memory_mapping_holder.hh"
#include "Genie/Process/SavedRegisters.hh"
#include "Genie/Process/SignalReceiver.hh"
#include "Genie/Process/TraceTarget.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	typedef std::map< int, FileDescriptor > FileDescriptorMap;
	
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
	
	typedef void (*CleanupHandlerProc)();
	
	typedef CleanupHandlerProc CleanupHandler;
	
	typedef int (*Reexec_Function)( void* _1,
	                                void* _2,
	                                void* _3,
	                                void* _4,
	                                void* _5,
	                                void* _6,
	                                void* _7 );
	
	class FlatArgVector
	{
		private:
			std::string itsStorage;
		
		public:
			void Assign( char const *const *argv );
			
			const std::string& Data() const  { return itsStorage; }
	};
	
	struct Parameters
	{
			FlatArgVector itsCmdLine;
			FlatArgVector itsEnviron;
			
			std::vector< const char* > itsArgV;
			std::vector< const char* > itsEnvP;
	};
	
	class Process : public SignalReceiver,
	                public LongJumper,
	                public memory_mapping_holder,
	                public TraceTarget
	{
		public:
			enum
			{
				kNoProcessPID   = 0,
				kRootProcessPID = 1
			};
		
		private:
			pid_t itsPPID;
			pid_t itsPID;
			pid_t itsForkedChildPID;
			
			boost::shared_ptr< ProcessGroup > itsProcessGroup;
			
			int* itsErrno;
			
			recall::stack_frame_pointer itsStackBottomPtr;
			recall::stack_frame_pointer itsStackFramePtr;
			recall::stack_frame_pointer itsVForkFramePtr;
			
			SavedRegisters itsSavedRegisters;
			
			UInt64 itsAlarmClock;
			
			std::string itsName;
			
			boost::shared_ptr< IOHandle > itsCWD;
			
			FileDescriptorMap itsFileDescriptors;
			
			ProcessLifeStage        itsLifeStage;
			ProcessInterdependence  itsInterdependence;
			ProcessSchedule         itsSchedule;
			
			int itsResult;
			
			unsigned itsAsyncOpCount;
		
		private:
			FSTreePtr itsProgramFile;
			
			MainEntry itsMainEntry;
			MainEntry itsOldMainEntry;
			
			boost::shared_ptr< Parameters > itsParameters;
			
			void* itsReexecArgs[8];
			
			Nucleus::Owned< Nitrogen::ThreadID > itsThread;
			
			CleanupHandler itsCleanupHandler;
			
			bool itMayDumpCore;
		
		private:
			void Suspend();
			void Resume();
			void Pause( ProcessSchedule newSchedule );
			
			void Terminate();
			void Terminate( int wait_status );
			
			void Orphan();
		
		public:
			struct RootProcess {};
			
			Process( RootProcess );
			Process( Process& parent, pid_t pid );
			
			~Process();
			
			int Run();
			
			const std::string& GetCmdLine() const  { return itsParameters->itsCmdLine.Data(); }
			
			void SetCleanupHandler( CleanupHandlerProc cleanup )  { itsCleanupHandler = cleanup; }
			
			bool MayDumpCore() const  { return itMayDumpCore; }
			
			void SuppressCoreDump()  { itMayDumpCore = false; }
			void AllowCoreDump   ()  { itMayDumpCore = true;  }
			
			void SetErrnoPtr( int* address )  { itsErrno = address; }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			
			pid_t GetPGID() const;
			pid_t GetSID()  const;
			
			const boost::shared_ptr< ProcessGroup >& GetProcessGroup() const  { return itsProcessGroup; }
			
			void SetProcessGroup( const boost::shared_ptr< ProcessGroup >& pgrp )  { itsProcessGroup = pgrp; }
			
			SavedRegisters& GetSavedRegisters()  { return itsSavedRegisters; }
			
			recall::stack_frame_pointer GetStackFramePointer() const  { return itsStackFramePtr; }
			recall::stack_frame_pointer GetStackBottomPointer() const  { return itsStackBottomPtr; }
			
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
			
			std::string ProgramName()  const  { return itsName;   }
			
			const FSTreePtr& ProgramFile() const  { return itsProgramFile; }
			
			FileDescriptorMap& FileDescriptors()  { return itsFileDescriptors; }
			
			unsigned int SetAlarm( unsigned int seconds );
			
			void DeliverSignal( int signal );
			
			void Raise( int signal );
			
			void SuspendForFork( pid_t childPID );
			void ResumeAfterFork();
			
			void UsurpParent( int exit_status );
			
			void Exit( int exit_status );
		
		public:
			bool Forked() const  { return itsThread.Get() == Nitrogen::kNoThreadID; }
			
			bool Forking() const  { return itsForkedChildPID != 0; }
			
			Nitrogen::ThreadID GetThread() const;
			
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
	
	boost::shared_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, const boost::shared_ptr< Session >& session );
	
	class ProcessList
	{
		public:
			typedef std::map< pid_t, boost::shared_ptr< Process > >  Map;
			typedef Map::value_type                                  value_type;
			typedef Map::const_iterator                              const_iterator;
			typedef Map::iterator                                    iterator;
		
		private:
			Map    itsMap;
			pid_t  itsLastPID;
		
		public:
			ProcessList();
			
			~ProcessList();
			
			const boost::shared_ptr< Process >& NewProcess( Process::RootProcess );
			const boost::shared_ptr< Process >& NewProcess( Process& parent );
			
			void RemoveProcess( pid_t pid );
			
			void KillAll();
			
			Map const& GetMap() const  { return itsMap; }
			Map      & GetMap()        { return itsMap; }
			
			const_iterator begin() const  { return itsMap.begin(); }
			const_iterator end  () const  { return itsMap.end  (); }
			
			iterator begin()  { return itsMap.begin(); }
			iterator end  ()  { return itsMap.end  (); }
	};
	
	typedef ProcessList GenieProcessTable;
	
	extern ProcessList gProcessTable;
	
	ProcessList& GetProcessList();
	
	inline const boost::shared_ptr< Process >& NewProcess( Process& parent )
	{
		return GetProcessList().NewProcess( parent );
	}
	
	void SendSignalToProcessGroup( int sig, const ProcessGroup& group );
	
	Process& CurrentProcess();
	
	Process& GetInitProcess();
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
	void DeliverFatalSignal( int signo );
	
	void Yield( Interruptibility interrupting );
	
	void Breathe();
	
	void TryAgainLater( bool isNonblocking );
	
}

#endif

