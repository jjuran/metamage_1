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

// relix-api
#include "relix/api/deliver_fatal_signal.hh"


void abort()
{
	try
	{
		relix::deliver_fatal_signal( SIGABRT );
	}
	catch ( ... )
	{
	}
	
	mac::sys::exit_to_shell();
}
	
#endif
