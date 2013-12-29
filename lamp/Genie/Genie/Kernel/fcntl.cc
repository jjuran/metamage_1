/*	========
 *	fcntl.cc
 *	========
 */

// relix-kernel
#include "relix/syscall/fcntl.hh"
#include "relix/syscall/openat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::openat;
	using relix::fcntl;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( openat );
	REGISTER_SYSTEM_CALL( fcntl  );
	
	#pragma force_active reset
	
}

