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

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/Shared.h"

// Nitrogen Extras / Templates
#include "Templates/DataPointer.h"

// Nitrogen Extras / Utilities
#include "Utilities/Threads.h"

// Silver
#include "ProcTypes.hh"

// Io
#include "Io/Handle.hh"

// ShellShock
#include "ShellShock/StringArray.hh"

// Genie
#include "Genie/FileDescriptor.hh"
#include "Genie/Terminal.hh"


namespace Genie
{
	
	class Process;
	
}

namespace Nitrogen
{
	
	template <>
	struct OwnedDefaults< Genie::Process* >
	{
		typedef DisposeWithDelete Disposer;
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
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
	
	typedef int*           ErrnoDataPtr;
	typedef char* const**  EnvironDataPtr;
	
	
	struct FragmentImage {};
	
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
			int myPPID;
			int myPID;
			int fPGID;
			int fSID;
			std::string myName;
			N::FSDirSpec myCWD;
			CharacterDevice* fControllingTerminal;
			FileDescriptorMap myFileDescriptors;
			EStatus myStatus;
			int myResult;
			std::map< int, sig_t > signalMap;
		
		private:
			NX::DataPtr< FragmentImage > fFragmentImage;
			NX::DataPtr< FragmentImage > fOldFragmentImage;
			N::Owned< CFragConnectionID > fFragmentConnection;
			N::Owned< CFragConnectionID > fOldFragmentConnection;
			ErrnoDataPtr fErrno;
			EnvironDataPtr fEnviron;
			
			std::auto_ptr< Sh::StringArray > argvStorage;
			std::auto_ptr< Sh::StringArray > environStorage;
			std::auto_ptr< Thread > thread;
		
		public:
			Process( int ppid );
			
			~Process()  {}
			
			int ParentProcessID() const  { return myPPID;   }
			int ProcessID()       const  { return myPID;    }
			int GetPGID()         const  { return fPGID;    }
			int GetSID()          const  { return fSID;     }
			EStatus Status()      const  { return myStatus; }
			int Result()          const  { return myResult; }
			std::string ProgramName()  const  { return myName;   }
			
			CharacterDevice* ControllingTerminal() const  { return fControllingTerminal; }
			
			N::FSDirSpec CurrentDirectory() const  { return myCWD; }
			
			FileDescriptorMap& FileDescriptors()  { return myFileDescriptors; }
			
			void SetPGID( int pgid )  { fPGID = pgid; }
			void SetSID ( int sid  )  { fSID  = sid;  }
			
			void Status( EStatus status )  { myStatus = status; }
			void Result( int     result )  { myResult = result; }
			
			void ProgramName( const std::string& name )  { myName = name; }
			
			void SetControllingTerminal( CharacterDevice* terminal )  { fControllingTerminal = terminal; }
			
			//long ChangeDirectory( const N::FSDirSpec& dir );
			int ChangeDirectory( const char* pathname );
			
			void Orphan();
			sig_t SetSignalAction( int signal, sig_t signalAction );
			void Raise( int signal );
			void Terminate();
			void Terminate( int result );
		
		public:
			bool Forked() const  { return thread.get() == NULL; }
			
			int Exec( const FSSpec& progFile,
			          const char* const argv[],
			          const char* const* envp );
			
			int TryExec( const FSSpec& progFile,
			             const char* const argv[],
			             const char* const* envp );
			
			void InitThread();
			
			int SetErrno( int errorNumber );
			
			void Stop();
			void Continue();
	};
	
	typedef Process GenieProcess;
	
	struct NoSuchProcess  {};
	
	class GenieProcessTable
	{
		public:
			typedef std::map< int, N::Shared< Process* > > ProcessMap;
			typedef ProcessMap::const_iterator const_iterator;
			typedef ProcessMap::iterator iterator;
		
		public:
			GenieProcessTable();
			~GenieProcessTable()
			{
				//Assert_( myProcesses.size() == 1 );
			}
			
			bool Exists( int pid ) const  { return myProcesses.count( pid ) > 0; }
			
			Process& operator[]( int pid );
			
			int NewProcess( Process* process );
			
			int RemoveProcess( iterator it );
			int RemoveProcess( int pid );
			
			void KillAll();
			
			void Reap();
			
			void SendSignalToProcessesControlledByTerminal( int sig, CharacterDevice* terminal );
			
			ProcessMap const& Map() const  { return myProcesses; }
			ProcessMap      & Map()        { return myProcesses; }
			
			const_iterator begin() const  { return myProcesses.begin(); }
			const_iterator end  () const  { return myProcesses.end  (); }
			
			iterator begin()  { return myProcesses.begin(); }
			iterator end  ()  { return myProcesses.end  (); }
		
		private:
			ProcessMap myProcesses;
			int myNextPID;
	};
	
	extern GenieProcessTable gProcessTable;
	
}

#endif

