/*	========
 *	utime.cc
 *	========
 */

// relix-kernel
#include "relix/syscall/utimensat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::utimensat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( utimensat );
	
	#pragma force_active reset
	
}

