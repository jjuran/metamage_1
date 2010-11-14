/*	=========
 *	dirent.cc
 *	=========
 */

// POSIX
#include "dirent.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/SystemCallRegistry.hh"


#undef dirfd


namespace Genie
{
	
	static int getdents( unsigned fd, struct dirent* dirp, unsigned int count )
	{
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			if ( count < sizeof (dirent) )
			{
				return set_errno( EINVAL );
			}
			
			return dir.ReadDir( *dirp );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( getdents );
	
	#pragma force_active reset
	
}

