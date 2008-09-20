/*	=========
 *	unlink.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int unlink( const char* pathname )
	{
		SystemCallFrame frame( "unlink" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( pathname, current );
			
			// Do not resolve links -- delete the symlink
			
			struct ::stat sb;
			
			file->Stat( sb );
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( isDir )
			{
				return frame.SetErrno( EISDIR );
			}
			
			file->Delete();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( unlink );
	
	#pragma force_active reset
	
}

