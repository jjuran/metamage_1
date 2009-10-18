/*	==========
 *	symlink.cc
 *	==========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags )
	{
		SystemCallFrame frame( "linkat" );
		
		FSTreePtr oldFile = ResolvePathAt( olddirfd, oldpath );
		
		// Resolve links?  POSIX says yes, but Linux doesn't.
		
		try
		{
			FSTreePtr newFile = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			struct ::stat location_status;
			
			try
			{
				// Stat the location.  Throws ENOENT if nonexistent.
				newFile->Stat( location_status );
				
				// The new location also exists; bail.
				return frame.SetErrno( EEXIST );
			}
			catch ( const p7::errno_t& errnum )
			{
				if ( errnum != ENOENT )  throw;
			}
			
			// If we got here, newFile is a valid location.
			
			oldFile->HardLink( newFile );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( linkat );
	
	#pragma force_active reset
	
}

