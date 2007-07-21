/*	========
 *	Abort.cc
 *	========
 */

// Standard C
#include <signal.h>
#include <stdlib.h>

// Nitrogen
#include "Nitrogen/Threads.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace N = Nitrogen;


void abort()
{
	try
	{
		Genie::Process& current = Genie::CurrentProcess();
		
		if ( current.GetSchedule() != Genie::kProcessUnscheduled )
		{
			current.SetSignalAction( SIGABRT, SIG_DFL );
			
			current.Raise( SIGABRT );
			
			current.HandlePendingSignals();
			
			// Probably not reached
		}
		
		N::SetThreadState( N::GetCurrentThread(), N::kStoppedThreadState );
	}
	catch ( ... )
	{
	}
	
	::ExitToShell();
}
	
