/*	========
 *	rmdir.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static int rmdir( const char* pathname )
	{
		SystemCallFrame frame( "rmdir" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr dir = ResolvePathname( pathname, current );
			
			// Do not resolve links
			
			struct ::stat sb;
			
			dir->Stat( sb );
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( !isDir )
			{
				return frame.SetErrno( ENOTDIR );
			}
			
			dir->Delete();
		}
		catch ( const N::OSStatus& err )
		{
			// Unfortunately, fBsyErr can mean different things.
			// Here we assume it means the directory is not empty.
			
			if ( err == fBsyErr )
			{
				return frame.SetErrno( ENOTEMPTY );
			}
			
			return frame.SetErrnoFromException();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( rmdir );
	
	#pragma force_active reset
	
}

