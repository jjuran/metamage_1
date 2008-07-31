/*	========
 *	Abort.cc
 *	========
 */

// Standard C
#include <signal.h>
#include <stdlib.h>

// Iota
#include "iota/strings.hh"

// Nitrogen
#include "Nitrogen/Threads.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemConsole.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static void Abort()
	{
		Process& current = CurrentProcess();
		
		if ( current.GetSchedule() != kProcessUnscheduled )
		{
			current.ResetSignalAction( SIGABRT );
			
			current.Raise( SIGABRT );
			
			current.HandlePendingSignals();
			
			// Probably not reached
			WriteToSystemConsole( STR_LEN( "Genie: abort(): process failed to terminate, stopping its thread\n" ) );
		}
		
		N::SetThreadState( N::GetCurrentThread(), N::kStoppedThreadState );
	}
	
}

void abort()
{
	try
	{
		Genie::Abort();
	}
	catch ( ... )
	{
	}
	
	::ExitToShell();
}
	
