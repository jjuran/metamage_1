/*	=========
 *	unlink.cc
 *	=========
 */

// relix-kernel
#include "relix/syscall/unlinkat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::unlinkat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( unlinkat );
	
	#pragma force_active reset
	
}

