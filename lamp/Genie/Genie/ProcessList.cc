/*
	ProcessList.cc
	--------------
*/

#include "Genie/ProcessList.hh"

// Standard C++
#include <map>

// Standard C
#include <signal.h>

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Threads.hh"
#include "Nitrogen/Timer.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class ProcessList
	{
		public:
			typedef std::map< pid_t, boost::intrusive_ptr< Process > >  Map;
			typedef Map::value_type                                     value_type;
			typedef Map::const_iterator                                 const_iterator;
			typedef Map::iterator                                       iterator;
		
		private:
			Map    itsMap;
		
		public:
			ProcessList();
			
			~ProcessList();
			
			const boost::intrusive_ptr< Process >& NewProcess( Process::RootProcess );
			const boost::intrusive_ptr< Process >& NewProcess( Process& parent );
			
			void RemoveProcess( pid_t pid );
			
			Map const& GetMap() const  { return itsMap; }
			Map      & GetMap()        { return itsMap; }
			
			const_iterator begin() const  { return itsMap.begin(); }
			const_iterator end  () const  { return itsMap.end  (); }
			
			iterator begin()  { return itsMap.begin(); }
			iterator end  ()  { return itsMap.end  (); }
	};
	
	static ProcessList theProcessList;
	
	static inline ProcessList& GetProcessList()
	{
		return theProcessList;
	}
	
	const boost::intrusive_ptr< Process >& NewProcess( Process& parent )
	{
		return GetProcessList().NewProcess( parent );
	}
	
	Process& GetInitProcess()
	{
		static const boost::intrusive_ptr< Process >& init = GetProcessList().NewProcess( Process::RootProcess() );
		
		return *init;
	}
	
	
	const size_t max_n_tasks = 1024;
	
	static pid_t global_last_pid = 0;
	
	static pid_t next_pid()
	{
		if ( ++global_last_pid >= max_n_tasks )
		{
			global_last_pid = 1;
		}
		
		pid_t last = global_last_pid - 1;
		
		ProcessList::iterator begin = GetProcessList().GetMap().find( global_last_pid );
		ProcessList::iterator end   = GetProcessList().end();
		
		if ( begin == end )
		{
			return global_last_pid;
		}
		
	rescan:
		
		for ( ProcessList::iterator it = begin;  it != end;  ++it )
		{
			Process& proc = *it->second;
			
			const pid_t pid = proc.GetPID();
			
			++last;
			
			if ( pid != last )
			{
				return global_last_pid = last;
			}
		}
		
		if ( ++last >= max_n_tasks )
		{
			if ( begin !=  GetProcessList().begin() )
			{
				end   = begin;
				begin = GetProcessList().begin();
				
				last = 0;
				
				goto rescan;
			}
			else
			{
				p7::throw_errno( EAGAIN );
			}
		}
		
		return global_last_pid = last;
	}
	
	
	ProcessList::ProcessList()
	{
	}
	
	ProcessList::~ProcessList()
	{
	}
	
	static void* reap_process( void*, pid_t pid, Process& process )
	{
		if ( process.GetLifeStage() == kProcessReleased )
		{
			GetProcessList().RemoveProcess( pid );
		}
		
		return NULL;
	}
	
	namespace
	{
		
		void ReaperThreadEntry()
		{
			while ( true )
			{
				for_each_process( &reap_process );
				
				N::YieldToAnyThread();
			}
		}
		
	}
	
	const boost::intrusive_ptr< Process >& ProcessList::NewProcess( Process::RootProcess )
	{
		static n::owned< N::ThreadID > reaper = N::NewThread< ReaperThreadEntry >( N::kCooperativeThread );
		
		const pid_t pid = next_pid();
		
		boost::intrusive_ptr< Process > process( new Process( Process::RootProcess() ) );
		
		return itsMap[ pid ] = process;
	}
	
	const boost::intrusive_ptr< Process >& ProcessList::NewProcess( Process& parent )
	{
		const pid_t pid = next_pid();
		
		boost::intrusive_ptr< Process > process( new Process( parent, pid ) );
		
		return itsMap[ pid ] = process;
	}
	
	void ProcessList::RemoveProcess( pid_t pid )
	{
		iterator it = itsMap.find( pid );
		
		if ( it == itsMap.end() )
		{
			Mac::SysBeep();
		}
		else
		{
			itsMap.erase( it );
		}
	}
	
	static void* kill_process( void*, pid_t, Process& process )
	{
		process.Raise( SIGKILL );
		
		return NULL;
	}
	
	void kill_all_processes()
	{
		for_each_process( &kill_process );
		
		while ( GetProcessList().GetMap().size() > 1 )
		{
			N::YieldToAnyThread();
		}
	}
	
	
	bool process_exists( pid_t pid )
	{
		return GetProcessList().GetMap().count( pid );
	}
	
	void* for_each_process( void* (*f)( void*, pid_t, Process& ), void* param )
	{
		ProcessList::iterator end = GetProcessList().end();
		
		for ( ProcessList::iterator it = GetProcessList().begin();  it != end;  ++it )
		{
			if ( Process* process = it->second.get() )
			{
				if ( void* result = f( param, it->first, *process ) )
				{
					return result;
				}
			}
		}
		
		return NULL;
	}
	
	void spawn_process( const char* program_path )
	{
		Process& parent = GetInitProcess();
		
		const boost::intrusive_ptr< Process >& external = NewProcess( parent );
		
		char const *const argv[] = { program_path, NULL };
		
		try
		{
			(void) external->Exec( program_path, argv, NULL );
		}
		catch ( ... )
		{
			GetProcessList().RemoveProcess( external->GetPID() );
		}
		
		parent.ResumeAfterFork();
	}
	
}

