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
	
	static int kill( pid_t pid, int sig )
	{
		GenieProcessTable::ProcessMap::const_iterator found = gProcessTable.Map().find( pid );
		
		if ( found == gProcessTable.end() )
		{
			return CurrentProcess().SetErrno( ESRCH );
		}
		
		if ( sig != 0 )
		{
			Process& process = *found->second;
			
			process.Raise( sig );
			
			if ( CurrentProcess().GetPID() == pid )
			{
				// If we sent ourselves the signal, handle it now
				process.HandlePendingSignals();
			}
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( kill );
	
	static sig_t signal( int sig, sig_t func )
	{
		return CurrentProcess().SetSignalAction( sig, func );
	}
	
	REGISTER_SYSTEM_CALL( signal );
	
}

