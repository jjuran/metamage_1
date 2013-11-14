/*
	unshare.cc
	----------
*/

// relix-kernel
#include "relix/syscall/unshare.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


using relix::unshare;


#pragma force_active on

REGISTER_SYSTEM_CALL( unshare );

#pragma force_active reset

