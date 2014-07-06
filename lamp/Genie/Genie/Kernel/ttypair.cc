/*
	ttypair.cc
	----------
*/

// relix-kernel
#include "relix/config/pts.hh"
#include "relix/syscall/ttypair.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


using namespace relix;

#pragma force_active on

#if CONFIG_PTS

REGISTER_SYSTEM_CALL( ttypair );

#endif

#pragma force_active reset
