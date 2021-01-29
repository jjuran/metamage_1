/*
	config/fork.hh
	--------------
*/

#ifndef RELIX_CONFIG_FORK_HH
#define RELIX_CONFIG_FORK_HH

// relix-kernel
#include "relix/config/mini.hh"


#ifndef CONFIG_FORK
#define CONFIG_FORK  (!CONFIG_MINI)
#endif

#endif
