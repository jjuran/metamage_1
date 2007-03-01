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
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	static int rmdir( const char* pathname )
	{
		try
		{
			FSTreePtr current = CurrentProcess().GetCWD();
			
			FSTreePtr dir = ResolvePathname( pathname, current );
			
			struct ::stat sb;
			
			dir->Stat( sb );
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( !isDir )
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
			dir->Delete();
		}
		catch ( const N::OSStatus& err )
		{
			// Unfortunately, fBsyErr can mean different things.
			// Here we assume it means the directory is not empty.
			
			if ( err == fBsyErr )
			{
				return CurrentProcess().SetErrno( ENOTEMPTY );
			}
			
			return GetErrnoFromExceptionInSystemCall();
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( rmdir );
	
}

