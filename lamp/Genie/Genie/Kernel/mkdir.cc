/*	========
 *	mkdir.cc
 *	========
 */

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int mkdir( const char* pathname, mode_t mode )
	{
		SystemCallFrame frame( "mkdir" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr location = ResolvePathname( pathname, current );
			
			// Do not resolve links
			
			location->CreateDirectory( mode );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( mkdir );
	
	#pragma force_active reset
	
}

