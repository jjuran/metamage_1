/*	=========
 *	signal.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "signal.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


#if TARGET_RT_MAC_CFM

typedef __sig_handler sig_t;

#endif


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_signal )
	DEFINE_MODULE_INIT(  Kernel_signal )
	
	
	static void send_signal( Process& process, int sig )
	{
		if ( sig != 0 )
		{
			process.Raise( sig );
		}
	}
	
	static int kill_pid( pid_t pid, int sig )
	{
		if ( Process* process = FindProcess( pid ) )
		{
			send_signal( *process, sig );
			
			return 0;
		}
		
		return CurrentProcess().SetErrno( ESRCH );
	}
	
	static int kill_pgid( pid_t pgid, int sig )
	{
		bool killed_any = false;
		
		Process& current = CurrentProcess();
		
		typedef GenieProcessTable::iterator iterator;
		
		for ( iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetLifeStage() == kProcessReleased )
			{
				continue;
			}
			
			bool pgid_matches = pgid == 0 ? &proc          != &current
			                              : proc.GetPGID() == pgid;
			
			if ( pgid_matches )
			{
				send_signal( proc, sig );
				
				killed_any = true;
			}
		}
		
		return killed_any ? 0 : current.SetErrno( ESRCH );
		
	}
	
	static int kill( pid_t pid, int sig )
	{
		Process& current = CurrentProcess();
		
		int result = pid >   0 ? kill_pid ( pid,               sig )
		           : pid ==  0 ? kill_pgid( current.GetPGID(), sig )
		           : pid == -1 ? kill_pgid( 0,                 sig )
		           :             kill_pgid( -pid,              sig );
		
		// In case we signalled ourself
		current.HandlePendingSignals();
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( kill );
	
	static sig_t signal( int sig, sig_t func )
	{
		return CurrentProcess().SetSignalAction( sig, func );
	}
	
	REGISTER_SYSTEM_CALL( signal );
	
}

