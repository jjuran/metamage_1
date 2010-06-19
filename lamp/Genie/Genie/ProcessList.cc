/*
	ProcessList.cc
	--------------
*/

#include "Genie/ProcessList.hh"

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
	
	
	ProcessList& GetProcessList()
	{
		static ProcessList theProcessList;
		
		return theProcessList;
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
	
	static void ReapProcesses()
	{
		for ( ProcessList::iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
		{
			Process& proc = *it->second;
			
			pid_t pid = proc.GetPID();
			
			if ( proc.GetLifeStage() == kProcessReleased )
			{
				GetProcessList().RemoveProcess( pid );
			}
		}
	}
	
	namespace
	{
		
		void ReaperThreadEntry()
		{
			while ( true )
			{
				ReapProcesses();
				
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
	
	void ProcessList::KillAll()
	{
		for ( Map::iterator it = itsMap.begin();  it != itsMap.end();  ++it )
		{
			Process& proc = *it->second;
			
			proc.Raise( SIGKILL );
		}
		
		while ( itsMap.size() > 1 )
		{
			N::YieldToAnyThread();
		}
	}
	
	
}

