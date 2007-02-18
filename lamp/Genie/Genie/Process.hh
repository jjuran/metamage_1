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

// Boost
#include <boost/shared_ptr.hpp>

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/CodeFragments.h"

// Nitrogen Extras / Utilities
#include "Utilities/Threads.h"

// Silver
#include "Silver/ProcTypes.hh"

// ShellShock
#include "ShellShock/VarArray.hh"

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace Ag = Silver;
	namespace Sh = ShellShock;
	
	
	typedef void ( *sig_t )( int );
	
	typedef std::map< int, FileDescriptor > FileDescriptorMap;
	
	class NotExecutableError {};
	
	
	typedef Ag::ProcType3< int, int, char const* const*, char const* const* >::CProcPtr  MainProcPtr;
	
	class Process;
	
	struct ThreadContext
	{
		Process* processContext;
		MainProcPtr externalMain;
		const char* const* argv;
		const char* const* envp;
		
		ThreadContext()
		:
			processContext(),
			externalMain  (),
			argv          (),
			envp          ()
		{}
		
		ThreadContext( Process*            process,
		               MainProcPtr         externalMain,
		               const char* const*  argv,
		               const char* const*  envp )
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
	
	typedef int*     ErrnoDataPtr;
	typedef char***  EnvironDataPtr;
	
	class TTYHandle;
	
	class Process
	{
		public:
			enum EStatus
			{
				kStarting,
				kRunning,
				kSleeping,
				kStopped,
				kTerminated,
				kZombie
			};
			
			enum
			{
				kNoProcessPID   = 0,
				kRootProcessPID = 1
			};
		
		private:
			pid_t itsPPID;
			pid_t itsPID;
			pid_t itsPGID;
			pid_t itsSID;
			
			UInt32 itsPendingSignals;
			UInt32 itsPreviousSignals;
			
			std::string itsName;
			
			FSTreePtr itsCWD;
			
			TTYHandle* itsControllingTerminal;
			
			FileDescriptorMap itsFileDescriptors;
			
			EStatus itsStatus;
			
			int itsResult;
			
			std::map< int, sig_t > itsSignalMap;
		
		private:
			FSSpec itsProgramFile;
			BinaryImage itsFragmentImage;
			BinaryImage itsOldFragmentImage;
			NN::Owned< CFragConnectionID > itsFragmentConnection;
			NN::Owned< CFragConnectionID > itsOldFragmentConnection;
			
			std::auto_ptr< Sh::StringArray > itsArgvStorage;
			std::auto_ptr< Sh::VarArray    > itsEnvironStorage;
			std::auto_ptr< Thread > itsThread;
			
			ErrnoDataPtr   itsErrnoData;
			EnvironDataPtr itsEnvironData;
			
			std::string itsLastEnv;
		
		public:
			struct RootProcess {};
			
			Process( RootProcess );
			Process( pid_t ppid );
			
			~Process();
			
			pid_t ParentProcessID() const  { return itsPPID; }
			pid_t ProcessID()       const  { return itsPID;  }
			
			pid_t GetPPID() const  { return itsPPID; }
			pid_t GetPID()  const  { return itsPID;  }
			pid_t GetPGID() const  { return itsPGID; }
			pid_t GetSID()  const  { return itsSID;  }
			
			EStatus Status()      const  { return itsStatus; }
			int Result()          const  { return itsResult; }
			std::string ProgramName()  const  { return itsName;   }
			
			FSSpec ProgramFile() const  { return itsProgramFile; }
			
			TTYHandle* ControllingTerminal() const  { return itsControllingTerminal; }
			
			FSTreePtr CurrentWorkingDirectory() const  { return itsCWD; }
			
			FSTreePtr GetCWD() const  { return itsCWD; }
			
			FileDescriptorMap& FileDescriptors()  { return itsFileDescriptors; }
			
			int const* ErrnoData()  const { return itsErrnoData; }
			int      * ErrnoData()        { return itsErrnoData; }
			
			char** const* EnvironData()  const { return itsEnvironData; }
			char**      * EnvironData()        { return itsEnvironData; }
			
			char* GetEnv( const char* name );
			void SetEnv( const char* name, const char* value, bool overwrite );
			void PutEnv( const char* string );
			void UnsetEnv( const char* name );
			void ClearEnv();
			
			void SetPGID( pid_t pgid )  { itsPGID = pgid; }
			void SetSID ( pid_t sid  )  { itsSID  = sid;  }
			
			void Status( EStatus status )  { itsStatus = status; }
			void Result( int     result )  { itsResult = result; }
			
			void ProgramName( const std::string& name )  { itsName = name; }
			
			void SetControllingTerminal( TTYHandle* terminal )  { itsControllingTerminal = terminal; }
			
			void ChangeDirectory( const FSTreePtr& newCWD );
			
			void Orphan();
			sig_t SetSignalAction( int signal, sig_t signalAction );
			void Raise( int signal );
			void Terminate();
			void Terminate( int result );
		
		public:
			bool Forked() const  { return itsThread.get() == NULL; }
			
			NN::Owned< N::ThreadID > Exec( const FSSpec&       progFile,
			                               const char* const   argv[],
			                               const char* const*  envp );
			
			int TryExec( const FSSpec& progFile,
			             const char* const argv[],
			             const char* const* envp );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
			
			bool HandlePendingSignals();
	};
	
	struct NoSuchProcess  {};
	
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
			
			bool Exists( pid_t pid ) const  { return itsProcesses.count( pid ) > 0; }
			
			Process& operator[]( pid_t pid );
			
			pid_t NewProcess( Process* process );
			
			int RemoveProcess( iterator it );
			int RemoveProcess( pid_t pid );
			
			void KillAll();
			
			void Reap();
			
			void SendSignalToProcessesControlledByTerminal( int sig, TTYHandle* terminal );
			
			ProcessMap const& Map() const  { return itsProcesses; }
			ProcessMap      & Map()        { return itsProcesses; }
			
			const_iterator begin() const  { return itsProcesses.begin(); }
			const_iterator end  () const  { return itsProcesses.end  (); }
			
			iterator begin()  { return itsProcesses.begin(); }
			iterator end  ()  { return itsProcesses.end  (); }
	};
	
	extern GenieProcessTable gProcessTable;
	
}

#endif

