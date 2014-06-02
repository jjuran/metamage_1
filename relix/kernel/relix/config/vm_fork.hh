/*
	config/vm_fork.hh
	-----------------
*/

#ifndef RELIX_CONFIG_VMFORK_HH
#define RELIX_CONFIG_VMFORK_HH

// relix-kernel
#include "relix/config/mini.hh"


#ifndef CONFIG_VM_FORK
#define CONFIG_VM_FORK  (! CONFIG_MINI)
#endif

#endif
