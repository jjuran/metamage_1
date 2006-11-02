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
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static void RegisterMacToUnixErrorConversions()
	{
		NN::RegisterExceptionConversion< P7::Errno, N::OSStatus >();
	}
	
	static int rmdir( const char* pathname )
	{
		RegisterMacToUnixErrorConversions();
		
		try
		{
			FSTreePtr current = CurrentProcess().CurrentWorkingDirectory();
			
			FSTreePtr dir = ResolvePathname( pathname, current );
			
			struct ::stat sb;
			
			dir->Stat( sb );
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( !isDir )
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
			dir->Delete();
			
			return 0;
		}
		catch ( const N::OSStatus& err )
		{
			P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
			
			// Unfortunately, fBsyErr can mean different things.
			// Here we assume it means the directory is not empty.
			if ( errnum == EBUSY )
			{
				errnum = ENOTEMPTY;
			}
			
			return CurrentProcess().SetErrno( errnum );
		}
		catch ( ... )
		{
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( rmdir );
	
}

