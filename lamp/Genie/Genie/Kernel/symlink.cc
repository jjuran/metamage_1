/*	==========
 *	symlink.cc
 *	==========
 */

// relix-kernel
#include "relix/syscall/readlinkat.hh"
#include "relix/syscall/symlinkat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::symlinkat;
	using relix::_readlinkat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( symlinkat   );
	REGISTER_SYSTEM_CALL( _readlinkat );
	
	#pragma force_active reset
	
}

