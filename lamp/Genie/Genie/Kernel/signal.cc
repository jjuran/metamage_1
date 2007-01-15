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
	
	static int kill( pid_t pid, int sig )
	{
		if ( !gProcessTable.Exists( pid ) )
		{
			return CurrentProcess().SetErrno( ESRCH );
		}
		
		if ( sig != 0 )
		{
			gProcessTable[ pid ].Raise( sig );
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

