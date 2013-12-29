/*	=======
 *	stat.cc
 *	=======
 */

// relix-kernel
#include "relix/syscall/faccessat.hh"
#include "relix/syscall/fchmod.hh"
#include "relix/syscall/fchmodat.hh"
#include "relix/syscall/fstat.hh"
#include "relix/syscall/fstatat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::faccessat;
	using relix::fchmod;
	using relix::fchmodat;
	using relix::fstat;
	using relix::fstatat;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( faccessat );
	REGISTER_SYSTEM_CALL( fchmodat  );
	REGISTER_SYSTEM_CALL( fchmod    );
	REGISTER_SYSTEM_CALL( fstatat   );
	REGISTER_SYSTEM_CALL( fstat     );
	
	#pragma force_active reset
	
}

