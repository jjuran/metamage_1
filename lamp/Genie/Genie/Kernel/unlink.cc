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
	
	DECLARE_MODULE_INIT( Kernel_unlink )
	DEFINE_MODULE_INIT(  Kernel_unlink )
	
	static int unlink( const char* pathname )
	{
		SystemCallFrame frame( "unlink" );
		
		try
		{
			FSTreePtr current = CurrentProcess().GetCWD();
			
			FSTreePtr file = ResolvePathname( pathname, current );
			
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
			return frame.GetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( unlink );
	
}

