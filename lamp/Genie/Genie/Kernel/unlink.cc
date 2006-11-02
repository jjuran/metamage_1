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
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static int unlink( const char* pathname )
	{
		NN::RegisterExceptionConversion< P7::Errno, N::OSStatus >();
		
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
		catch ( const N::OSStatus& err )
		{
			P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
			
			return CurrentProcess().SetErrno( errnum );
		}
		catch ( ... )
		{
			return CurrentProcess().SetErrno( EINVAL );
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( unlink );
	
}

