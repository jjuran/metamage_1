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
#include "Genie/caught_signal.hh"
#include "Genie/Faults.hh"
#include "Genie/Process.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	extern class Process* gCurrentProcess;
	
	
	static void call_signal_handler( const caught_signal& signal )
	{
		const sigset_t signo_mask = 1 << signal.signo - 1;
		
		sigset_t signal_mask = signal.action.sa_mask;
		
		if ( !(signal.action.sa_flags & (SA_NODEFER | SA_RESETHAND)) )
		{
			signal_mask |= signo_mask;
		}
		
		gCurrentProcess->ClearPendingSignalSet( signo_mask );
		
		const sigset_t blocked_signals = gCurrentProcess->GetBlockedSignals();
		
		gCurrentProcess->BlockSignals( signal_mask );
		
		signal.action.sa_handler( signal.signo );
		
		gCurrentProcess->SetBlockedSignals( blocked_signals );
	}
	
	void enter_system_call( long syscall_number, long* params )
	{
		gCurrentProcess->EnterSystemCall();
		
		const size_t space = N::ThreadCurrentStackSpace( N::GetCurrentThread() );
		
		// space will be 0 if we're not on a Thread Manager stack
		
		if ( space != 0  &&  space < 8192 )
		{
			DeliverFatalSignal( SIGSTKFLT );
		}
		
	rebreathe:
		
		try
		{
			Breathe();
		}
		catch ( const caught_signal& signal )
		{
			gCurrentProcess->LeaveSystemCall();
			
			call_signal_handler( signal );
			
			gCurrentProcess->EnterSystemCall();
			
			goto rebreathe;
		}
	}
	
	bool leave_system_call( int result )
	{
		gCurrentProcess->LeaveSystemCall();
		
		return false;
	}
	
}

