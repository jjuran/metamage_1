/*	=======
 *	sync.cc
 *	=======
 */

// POSIX
#include "unistd.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	static int fdatasync( int fd )
	{
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( false );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	static int fsync( int fd )
	{
		try
		{
			GetFileHandleWithCast< StreamHandle >( fd ).Synchronize( true );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( fdatasync );
	REGISTER_SYSTEM_CALL( fsync     );
	REGISTER_SYSTEM_CALL( sync      );
	
	#pragma force_active reset
	
}

