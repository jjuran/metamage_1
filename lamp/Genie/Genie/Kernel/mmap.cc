/*	=======
 *	mmap.cc
 *	=======
 */

// relix-kernel
#include "relix/syscall/mmap.hh"
#include "relix/syscall/msync.hh"
#include "relix/syscall/munmap.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::_relix_mmap;
	using relix::msync;
	using relix::munmap;
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _relix_mmap );
	REGISTER_SYSTEM_CALL( munmap      );
	REGISTER_SYSTEM_CALL( msync       );
	
	#pragma force_active reset
	
}
