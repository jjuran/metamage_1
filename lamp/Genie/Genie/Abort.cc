/*	========
 *	Abort.cc
 *	========
 */

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// Standard C
#include <signal.h>
#include <stdlib.h>

// Genie
#include "Genie/Faults.hh"


#ifdef __LAMP__

void abort()
{
	try
	{
		Genie::DeliverFatalSignal( SIGABRT );
	}
	catch ( ... )
	{
	}
	
	::ExitToShell();
}
	
#endif

