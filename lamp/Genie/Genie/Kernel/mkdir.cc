/*	========
 *	mkdir.cc
 *	========
 */

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int mkdirat( int dirfd, const char* path, mode_t mode )
	{
		SystemCallFrame frame( "mkdir" );
		
		try
		{
			FSTreePtr location = ResolvePathAt( dirfd, path );
			
			// Do not resolve links
			
			location->CreateDirectory( mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( mkdirat );
	
	#pragma force_active reset
	
}

