/*	========
 *	Abort.cc
 *	========
 */

#ifdef __RELIX__

// Standard C
#include <signal.h>
#include <stdlib.h>

// mac-sys-utils
#include "mac_sys/exit_to_shell.hh"

// Genie
#include "Genie/Faults.hh"


void abort()
{
	try
	{
		Genie::DeliverFatalSignal( SIGABRT );
	}
	catch ( ... )
	{
	}
	
	mac::sys::exit_to_shell();
}
	
#endif

