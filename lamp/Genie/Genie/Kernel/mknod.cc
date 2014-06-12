/*
	mknod.cc
	--------
*/

// relix-kernel
#include "relix/syscall/mknodat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::mknodat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( mknodat );
	
	#pragma force_active reset
	
}
