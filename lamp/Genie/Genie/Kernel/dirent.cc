/*	=========
 *	dirent.cc
 *	=========
 */

// POSIX
#include "dirent.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


#undef dirfd


namespace Genie
{
	
	static int getdents( unsigned fd, struct dirent* dirp, unsigned int count )
	{
		SystemCallFrame frame( "getdents" );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			if ( count < sizeof (dirent) )
			{
				return frame.SetErrno( EINVAL );
			}
			
			return dir.ReadDir( *dirp );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( getdents );
	
	#pragma force_active reset
	
}

