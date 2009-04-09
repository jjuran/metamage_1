/*	========
 *	utime.cc
 *	========
 */

// Standard C++
#include <algorithm>

// POSIX
#include "utime.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Genie
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int futimesat_k( int dirfd, const char* path, const timeval* access,
	                                                     const timeval* mod,
	                                                     const timeval* backup,
	                                                     const timeval* creat )
	{
		SystemCallFrame frame( "futimesat_k" );
		
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			ResolveLinks_InPlace( file );
			
			if ( access || mod || backup || creat )
			{
				file->SetTimes( access, mod, backup, creat );
			}
			else
			{
				file->SetTimes();
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( futimesat_k );
	
	#pragma force_active reset
	
}

