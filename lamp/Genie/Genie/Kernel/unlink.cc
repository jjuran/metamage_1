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
#include "Genie/Yield.hh"


namespace Genie
{
	
	static int unlink( const char* pathname )
	{
		try
		{
			FSTreePtr current = CurrentProcess().CurrentWorkingDirectory();
			
			FSTreePtr file = ResolvePathname( pathname, current );
			
			struct ::stat sb;
			
			file->Stat( sb );
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( isDir )
			{
				return CurrentProcess().SetErrno( EISDIR );
			}
			
			file->Delete();
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( unlink );
	
}

