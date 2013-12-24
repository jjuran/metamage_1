/*	=======
 *	sync.cc
 *	=======
 */

// POSIX
#include "unistd.h"

// relix-kernel
#include "relix/syscall/fsync.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::fsync;
	
	static int fdatasync( int fd )
	{
		return fsync( fd );
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( fdatasync );
	REGISTER_SYSTEM_CALL( fsync     );
	REGISTER_SYSTEM_CALL( sync      );
	
	#pragma force_active reset
	
}

