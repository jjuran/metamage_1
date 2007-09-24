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

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/CodeFragments.h"

// Nitrogen Extras / Utilities
#include "Utilities/Threads.h"

// ShellShock
#include "ShellShock/VarArray.hh"

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/Process/Environ.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace Sh = ShellShock;
	
	
	typedef void ( *sig_t )( int );
	
	typedef std::map< int, FileDescriptor > FileDescriptorMap;
	
	class NotExecutableError {};
	
	
	typedef int (*MainProcPtr)( int, char const* const*, char const* const* );
	
	class Process;
	
	struct ThreadContext
	{
		Process*      processContext;
		MainProcPtr   externalMain;
		iota::argp_t  argv;
		iota::envp_t  envp;
		
		ThreadContext()
		:
			processContext(),
			externalMain  (),
			argv          (),
			envp          ()
		{}
		
		ThreadContext( Process*      process,
		               MainProcPtr   externalMain,
		               iota::argp_t  argv,
		               iota::envp_t  envp )
		:
			processContext( process       ),
			externalMain  ( externalMain  ),
			argv          ( argv          ),
			envp          ( envp          )
		{}
	};
	
	struct ExternalProcessExecutor
	{
		int operator()( ThreadContext& context ) const;
	};
	
	struct Thread_Details
	{
		typedef ThreadContext  Parameter;
		typedef int            Result;
		
		typedef ExternalProcessExecutor Function;
	};
	
	typedef NX::Thread< Thread_Details > Thread;
	
	typedef int* ErrnoDataPtr;
	
	class TTYHandle;
	
	enum ProcessLifeStage
	{
		kProcessStarting,
		kProcessLive,
		kProcessTerminated,
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
	
	struct Times
	{
		UInt64 user;
		UInt64 system;
		UInt64 child_user;
		UInt64 child_system;
		
		Times() : user(), system(), child_user(), child_system()  {}
	};
	
	class Process : public Environ
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
			
			void (*itsLongJmp)(int);
			
			boost::shared_ptr< ProcessGroup > itsProcessGroup;
			
			pid_t itsTracingProcess;
			
			UInt64 itsAlarmClock;
			
			UInt64 itsLastTimerCheckpoint;
			
			Times itsTimes;
			
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
			FSSpec itsProgramFile;
			BinaryImage itsFragmentImage;
			BinaryImage itsOldFragmentImage;
			NN::Owned< CFragConnectionID > itsFragmentConnection;
			NN::Owned< CFragConnectionID > itsOldFragmentConnection;
			
			std::auto_ptr< Sh::StringArray > itsArgvStorage;
			std::auto_ptr< Thread > itsThread;
			
			CleanupHandler itsCleanupHandler;
			
			ErrnoDataPtr   itsErrnoData;
			
			std::string itsLastEnv;
		
		private:
			void Orphan();
		
		public:
			struct RootProcess {};
			
			Process( RootProcess );
			Process( Process& parent );
			
			~Process();
			
			void SetCleanupHandler( CleanupHandlerProc cleanup )  { itsCleanupHandler = cleanup; }
			
			void SetLongJmp( void (*LongJmp)(int) )  { itsLongJmp = LongJmp; }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			pid_t GetPGID() const  { return itsProcessGroup->ID();     }
			pid_t GetSID()  const  { return itsProcessGroup->GetSID(); }
			
			const Times& GetTimes() const  { return itsTimes; }
			
			void AccumulateChildTimes( const Times& times );
			
			const boost::shared_ptr< ProcessGroup >& GetProcessGroup() const  { return itsProcessGroup; }
			
			void SetProcessGroup( const boost::shared_ptr< ProcessGroup >& pgrp )  { itsProcessGroup = pgrp; }
			
			pid_t GetTracingProcess() const  { return itsTracingProcess; }
			bool IsBeingTraced() const  { return itsTracingProcess != 0; }
			
			void StartTracing( pid_t tracingProcess )  { itsTracingProcess = tracingProcess; }
			
			void StopTracing()  { itsTracingProcess = 0; }
			
			ProcessLifeStage  GetLifeStage() const  { return itsLifeStage; }
			ProcessSchedule   GetSchedule () const  { return itsSchedule;  }
			
			int Result() const  { return itsResult; }
			
			void Release();
			
			void EnterSystemCall( const char* name );
			
			void LeaveSystemCall();
			
			void SuspendTimer();
			void ResumeTimer();
			
			void SetSchedule( ProcessSchedule schedule );
			
			void Result( int result )  { itsResult = result; }
			
			const boost::shared_ptr< IOHandle >& ControllingTerminal() const  { return GetProcessGroup()->GetSession()->GetControllingTerminal(); }
			
			FSTreePtr GetCWD() const  { return itsCWD; }
			
			void ChangeDirectory( const FSTreePtr& newCWD );
			
			std::string ProgramName()  const  { return itsName;   }
			
			void ProgramName( const std::string& name )  { itsName = name; }
			
			FSSpec ProgramFile() const  { return itsProgramFile; }
			
			FileDescriptorMap& FileDescriptors()  { return itsFileDescriptors; }
			
			unsigned int SetAlarm( unsigned int seconds );
			
			sig_t SetSignalAction( int signal, sig_t signalAction );
			
			void Raise( int signal );
			
			void ResumeAfterFork();
			
			void UsurpParent( int exit_status );
			
			void Terminate();
			void Terminate( int result );
		
		public:
			bool Forked() const  { return itsThread.get() == NULL; }
			
			bool Forking() const  { return itsForkedChildPID != 0; }
			
			NN::Owned< N::ThreadID > Exec( const FSSpec&       progFile,
			                               const char* const   argv[],
			                               const char* const*  envp );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals();
	};
	
	boost::shared_ptr< ProcessGroup > GetProcessGroupInSession( pid_t pgid, const boost::shared_ptr< Session >& session );
	
	class GenieProcessTable
	{
		public:
			typedef std::map< pid_t, boost::shared_ptr< Process > > ProcessMap;
			typedef ProcessMap::const_iterator const_iterator;
			typedef ProcessMap::iterator iterator;
		
		private:
			ProcessMap itsProcesses;
			pid_t itsNextPID;
		
		public:
			GenieProcessTable();
			
			~GenieProcessTable()
			{
				//ASSERT( itsProcesses.size() == 1 );
			}
			
			pid_t NewProcess( Process* process );
			
			void RemoveProcess( pid_t pid );
			
			void KillAll();
			
			ProcessMap const& Map() const  { return itsProcesses; }
			ProcessMap      & Map()        { return itsProcesses; }
			
			const_iterator begin() const  { return itsProcesses.begin(); }
			const_iterator end  () const  { return itsProcesses.end  (); }
			
			iterator begin()  { return itsProcesses.begin(); }
			iterator end  ()  { return itsProcesses.end  (); }
	};
	
	extern GenieProcessTable gProcessTable;
	
	void SendSignalToProcessGroup( int sig, const ProcessGroup& group );
	
	Process& GetProcess( pid_t pid );
	
	Process* FindProcess( pid_t pid );
	
}

#endif

