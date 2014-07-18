/*
	kernel_boundary.cc
	------------------
*/

#include "Genie/Dispatch/kernel_boundary.hh"

// mac-sys-utils
#include "mac_sys/current_thread_stack_space.hh"

// relix-kernel
#include "relix/api/breathe.hh"
#include "relix/api/deliver_fatal_signal.hh"
#include "relix/api/terminate_current_process.hh"
#include "relix/signal/call_signal_handler.hh"
#include "relix/signal/signal.hh"
#include "relix/time/cpu_time_checkpoint.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif


namespace relix
{
	
	long enter_system_call( long syscall_number, long* params )
	{
		enter_system();
		
		const unsigned long space = mac::sys::current_thread_stack_space();
		
		// space will be 0 if we're not on a Thread Manager stack
		
		if ( space != 0  &&  space < 8192 )
		{
			deliver_fatal_signal( SIGSTKFLT );
		}
		
	rebreathe:
		
		try
		{
			breathe( true );
		}
		catch ( const signal& sig )
		{
			if ( sig.number != 0  &&  sig.action.sa_handler == SIG_DFL )
			{
				terminate_current_process( sig.number );
			}
			
			leave_system();
			
			call_signal_handler( sig );
			
			enter_system();
			
			goto rebreathe;
		}
		
		return syscall_number;
	}
	
	bool leave_system_call( int result )
	{
		if ( the_signal.number  &&  the_signal.action.sa_handler == SIG_DFL )
		{
			int signo = the_signal.number;
			
			the_signal.number = 0;
			
			terminate_current_process( signo );
		}
		
		leave_system();
		
		if ( the_signal.number )
		{
			const signal sig = the_signal;
			
			the_signal.number = 0;
			
			call_signal_handler( sig );
			
			return sig.action.sa_flags & SA_RESTART;
		}
		
		return false;
	}
	
}
