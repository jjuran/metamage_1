/*	===========
 *	copyfile.cc
 *	===========
 */

// relix-kernel
#include "relix/syscall/copyfileat.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace {

using relix::copyfileat;


#pragma force_active on

REGISTER_SYSTEM_CALL( copyfileat );

#pragma force_active reset

}  // namespace

