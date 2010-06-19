/*
	ProcessList.cc
	--------------
*/

#include "Genie/ProcessList.hh"

// Standard C
#include <signal.h>

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Threads.hh"
#include "Nitrogen/Timer.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
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
	
	
	ProcessList::ProcessList() : itsLastPID( 1 )
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
		
		pid_t pid = 1;
		
		boost::intrusive_ptr< Process > process( new Process( Process::RootProcess() ) );
		
		return itsMap[ pid ] = process;
	}
	
	const boost::intrusive_ptr< Process >& ProcessList::NewProcess( Process& parent )
	{
		pid_t pid = ++itsLastPID;
		
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

