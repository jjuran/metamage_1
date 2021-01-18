/*
	kernel_boundary.cc
	------------------
*/

#include "relix/glue/kernel_boundary.hh"

// relix-kernel
#include "relix/api/breathe.hh"
#include "relix/api/check_stack_space.hh"
#include "relix/api/terminate_current_process.hh"
#include "relix/signal/call_signal_handler.hh"
#include "relix/signal/signal.hh"
#include "relix/time/cpu_time_checkpoint.hh"


namespace relix
{
	
	long enter_system_call( long syscall_number )
	{
		enter_system();
		
		check_stack_space();
		
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
	
	bool leave_system_call()
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
