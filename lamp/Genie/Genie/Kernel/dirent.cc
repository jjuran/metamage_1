/*	=========
 *	dirent.cc
 *	=========
 */

// POSIX
#include "dirent.h"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


#undef dirfd


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_dirent )
	DEFINE_MODULE_INIT( Kernel_dirent )
	
	namespace N = Nitrogen;
	
	
	static int getdents( unsigned fd, struct dirent* dirp, unsigned int count )
	{
		SystemCallFrame frame( "getdents" );
		
		Breathe();
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			if ( count < sizeof (dirent) )
			{
				return frame.SetErrno( EINVAL );
			}
			
			if ( const dirent* entry = dir.ReadDir() )
			{
				*dirp = *entry;
			}
			else
			{
				return 0;
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return sizeof (dirent);
	}
	
	REGISTER_SYSTEM_CALL( getdents );
	
}

