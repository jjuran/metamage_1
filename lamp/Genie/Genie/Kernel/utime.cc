/*	========
 *	utime.cc
 *	========
 */

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static inline bool merely_touch( const timespec* t )
	{
		const long now = UTIME_NOW;
		
		return t == NULL  ||  t[0].tv_nsec == now  &&  t[1].tv_nsec == now;
	}
	
	static int utimensat( int fd, const char* path, const timespec times[2], int flags )
	{
		SystemCallFrame frame( "utimensat" );
		
		try
		{
			FSTreePtr file = path != NULL ? ResolvePathAt( fd, path )
			                              : GetFileHandle( fd )->GetFile();
			
			const bool nofollow = flags & AT_SYMLINK_NOFOLLOW;
			
			if ( !nofollow  &&  path != NULL )
			{
				ResolveLinks_InPlace( file );
			}
			
			if ( merely_touch( times ) )
			{
				file->SetTimes();
			}
			else
			{
				file->SetTimes( times );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( utimensat );
	
	#pragma force_active reset
	
}

