/*	========
 *	mkdir.cc
 *	========
 */

// relix-kernel
#include "relix/syscall/mkdirat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::mkdirat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( mkdirat );
	
	#pragma force_active reset
	
}

