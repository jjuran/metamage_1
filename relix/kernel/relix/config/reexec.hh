/*
	config/reexec.hh
	----------------
*/

#ifndef RELIX_CONFIG_REEXEC_HH
#define RELIX_CONFIG_REEXEC_HH

// relix-kernel
#include "relix/config/mini.hh"


#ifndef CONFIG_REEXEC
#define CONFIG_REEXEC  (! CONFIG_MINI)
#endif

#endif
