/*	=======
 *	wait.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/wait.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	static GenieProcessTable::iterator CheckAnyPID( pid_t ppid )
	{
		bool hasAnyChildren = false;
		
		typedef GenieProcessTable::iterator iterator;
		
		// FIXME:  Replace with find_if
		for ( iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetPPID() == ppid )
			{
				if ( proc.Status() == kProcessTerminated )
				{
					return it;
				}
				
				hasAnyChildren = true;
			}
		}
		
		if ( !hasAnyChildren )
		{
			P7::ThrowErrno( ECHILD );
		}
		
		return gProcessTable.end();
	}
	
	static GenieProcessTable::iterator CheckPID( pid_t ppid, pid_t pid )
	{
		typedef GenieProcessTable::iterator iterator;
		
		iterator found = gProcessTable.Map().find( pid );
		
		if ( found == gProcessTable.end() )
		{
			P7::ThrowErrno( ECHILD );
		}
		
		if ( found->second->GetPPID() != ppid )
		{
			// complain
		}
		
		if ( found->second->Status() != kProcessTerminated )
		{
			found = gProcessTable.end();
		}
		
		return found;
	}
	
	static pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		pid_t ppid = CurrentProcess().GetPID();
		
		try
		{
			while ( true )
			{
				typedef GenieProcessTable::iterator iterator;
				
				iterator found = pid == -1 ? CheckAnyPID( ppid )
				                           : CheckPID( ppid, pid );
				
				if ( found != gProcessTable.end() )
				{
					if ( stat_loc != NULL )
					{
						*stat_loc = found->second->Result();
					}
					
					pid_t foundPID = found->first;
					
					found->second->Status( kProcessReleased );
					
					Yield();
					
					return foundPID;
				}
				
				if ( options & 1 )
				{
					// no hang
					return 0;
				}
				
				Yield();
			}
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		// Not reached
		return -1;
	}
	
	REGISTER_SYSTEM_CALL( waitpid );
	
}

