/*
	config/fork.cc
	--------------
*/

// POSIX
#include <unistd.h>

// relix-kernel
#include "relix/config/fork.hh"
#include "relix/syscall/registry.hh"


#if CONFIG_FORK

#pragma force_active on

REGISTER_SYSTEM_CALL( fork );

#pragma force_active reset

#endif
