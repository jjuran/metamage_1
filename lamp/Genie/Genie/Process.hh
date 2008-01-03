/*	===============
 *	GenieProcess.hh
 *	===============
 *	
 *	Implemented by GenieProcess.cc
 */

#ifndef GENIE_PROCESS_HH
#define GENIE_PROCESS_HH

// Universal Interfaces
#include <AEDataModel.h>

// Standard C++
#include <map>
#include <string>

// Iota
#include "iota/argv.hh"
#include "iota/environ.hh"

// Boost
#include <boost/shared_ptr.hpp>

// Backtrace
#include "Backtrace/StackCrawl.hh"

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/Threads.h"

// Genie
#include "Genie/Exec/MainEntry.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/Process/Environ.hh"
#include "Genie/Process/LongJumper.hh"
#include "Genie/Process/TimeKeeper.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	typedef void ( *sig_t )( int );
	
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
	
#if TARGET_CPU_68K
	
	typedef CleanupHandlerProc CleanupHandler;
	
#else
	
	struct CleanupHandler
	{
		CleanupHandler& operator=( CleanupHandlerProc )  { return *this; }
		
		operator CleanupHandlerProc() const  { return NULL; }
		
		void operator()() const  {}
	};
	
#endif
	
	class CmdLine
	{
		private:
			std::string itsStorage;
		
		public:
			void Assign( char const *const *argv );
			
			const std::string& Data() const  { return itsStorage; }
	};
	
	class Process : public Environ,
	                public TimeKeeper,
	                public LongJumper
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
			
			Backtrace::StackFramePtr itsStackFramePtr;
			
			pid_t itsTracingProcess;
			
			UInt64 itsAlarmClock;
			
			UInt32 itsPendingSignals;
			UInt32 itsPreviousSignals;
			
			std::string itsName;
			
			FSTreePtr itsCWD;
			
			FileDescriptorMap itsFileDescriptors;
			
			ProcessLifeStage        itsLifeStage;
			ProcessInterdependence  itsInterdependence;
			ProcessSchedule         itsSchedule;
			
			int itsResult;
			
			std::map< int, sig_t > itsSignalMap;
		
		private:
			FSTreePtr itsProgramFile;
			MainEntry itsMainEntry;
			MainEntry itsOldMainEntry;
			
			CmdLine itsCmdLine;
			
			Nucleus::Owned< Nitrogen::ThreadID > itsThread;
			
			CleanupHandler itsCleanupHandler;
			
			std::string itsLastEnv;
		
		private:
			void Suspend();
			void Resume();
			
			void Terminate();
			void Terminate( int wait_status );
			
			void Orphan();
		
		public:
			struct RootProcess {};
			
			Process( RootProcess );
			Process( Process& parent );
			
			~Process();
			
			Main3 GetMain() const  { return itsMainEntry->GetMainPtr(); }
			
			const std::string& GetCmdLine() const  { return itsCmdLine.Data(); }
			
			void SetCleanupHandler( CleanupHandlerProc cleanup )  { itsCleanupHandler = cleanup; }
			
			void SetErrnoPtr( int* address )  { itsMainEntry->SetErrnoPtr( address ); }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			pid_t GetPGID() const  { return itsProcessGroup->ID();     }
			pid_t GetSID()  const  { return itsProcessGroup->GetSID(); }
			
			const boost::shared_ptr< ProcessGroup >& GetProcessGroup() const  { return itsProcessGroup; }
			
			void SetProcessGroup( const boost::shared_ptr< ProcessGroup >& pgrp )  { itsProcessGroup = pgrp; }
			
			Backtrace::StackFramePtr GetStackFramePointer() const  { return itsStackFramePtr; }
			
			pid_t GetTracingProcess() const  { return itsTracingProcess; }
			bool IsBeingTraced() const  { return itsTracingProcess != 0; }
			
			void StartTracing( pid_t tracingProcess )  { itsTracingProcess = tracingProcess; }
			
			void StopTracing()  { itsTracingProcess = 0; }
			
			ProcessLifeStage  GetLifeStage() const  { return itsLifeStage; }
			ProcessSchedule   GetSchedule () const  { return itsSchedule;  }
			
			int Result() const  { return itsResult; }
			
			void Release();
			
			void Yield();
			
			const boost::shared_ptr< IOHandle >& ControllingTerminal() const  { return GetProcessGroup()->GetSession()->GetControllingTerminal(); }
			
			FSTreePtr GetCWD() const  { return itsCWD; }
			
			void ChangeDirectory( const FSTreePtr& newCWD );
			
			std::string ProgramName()  const  { return itsName;   }
			
			const FSTreePtr& ProgramFile() const  { return itsProgramFile; }
			
			FileDescriptorMap& FileDescriptors()  { return itsFileDescriptors; }
			
			unsigned int SetAlarm( unsigned int seconds );
			
			sig_t SetSignalAction( int signal, sig_t signalAction );
			
			void Raise( int signal );
			
			void SuspendForFork( pid_t childPID );
			void ResumeAfterFork();
			
			void UsurpParent( int exit_status );
			
			void Exit( int exit_status );
		
		public:
			bool Forked() const  { return itsThread.Get() == Nitrogen::kNoThreadID; }
			
			bool Forking() const  { return itsForkedChildPID != 0; }
			
			Nitrogen::ThreadID GetThread() const;
			
			NN::Owned< N::ThreadID > Exec( const FSTreePtr&    executable,
			                               const char* const   argv[],
			                               const char* const*  envp );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals();
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
			pid_t  itsNextPID;
		
		public:
			ProcessList();
			
			~ProcessList()
			{
				//ASSERT( itsMap.size() == 1 );
			}
			
			pid_t NewProcess( Process* process );
			
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
	
	void SendSignalToProcessGroup( int sig, const ProcessGroup& group );
	
	Process& CurrentProcess();
	
	Process& GetInitProcess();
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
	void Yield();
	
	void TryAgainLater( bool isNonblocking );
	
}

#endif

