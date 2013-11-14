/*	========
 *	ioctl.cc
 *	========
 */

// relix-kernel
#include "relix/syscall/ioctl.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::ioctl;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( ioctl );
	
	#pragma force_active reset
	
}

