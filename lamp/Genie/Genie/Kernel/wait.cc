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
			
			if ( proc.ParentProcessID() == ppid )
			{
				if ( proc.Status() == Process::kTerminated )
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
		
		if ( found->second->ParentProcessID() != ppid )
		{
			// complain
		}
		
		if ( found->second->Status() != Process::kTerminated )
		{
			found = gProcessTable.end();
		}
		
		return found;
	}
	
	static pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		int ppid = CurrentProcess().ProcessID();
		
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
					
					int foundPID = found->first;
					
					(void)gProcessTable.RemoveProcess( found );
					
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
		catch ( P7::Errno& err )
		{
			CurrentProcess().SetErrno( err );
		}
		
		return -1;
	}
	
	REGISTER_SYSTEM_CALL( waitpid );
	
}

