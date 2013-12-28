/*	=========
 *	rename.cc
 *	=========
 */

// relix-kernel
#include "relix/syscall/renameat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::renameat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( renameat );
	
	#pragma force_active reset
	
}

