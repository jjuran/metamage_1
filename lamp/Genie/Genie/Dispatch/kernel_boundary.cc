/*
	kernel_boundary.cc
	------------------
*/

#include "Genie/Dispatch/kernel_boundary.hh"

// Nitrogen
#ifndef NITROGEN_THREADS_HH
#include "Nitrogen/Threads.hh"
#endif

// Genie
#include "Genie/Faults.hh"
#include "Genie/Process.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	extern class Process* gCurrentProcess;
	
	void enter_system_call( long syscall_number, long* params )
	{
		gCurrentProcess->EnterSystemCall();
		
		const size_t space = N::ThreadCurrentStackSpace( N::GetCurrentThread() );
		
		// space will be 0 if we're not on a Thread Manager stack
		
		if ( space != 0  &&  space < 8192 )
		{
			DeliverFatalSignal( SIGSTKFLT );
		}
		
		Breathe();
	}
	
	bool leave_system_call( int result )
	{
		gCurrentProcess->LeaveSystemCall();
		
		return false;
	}
	
}

