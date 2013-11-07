/*	========
 *	times.cc
 *	========
 */

// relix
#include "relix/syscall/times.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::times;
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( times );
	
	#pragma force_active reset
	
}

