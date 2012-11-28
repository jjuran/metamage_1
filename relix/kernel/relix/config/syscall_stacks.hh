/*
	config/syscall_stacks.hh
	------------------------
*/

#ifndef RELIX_CONFIG_SYSCALLSTACKS_HH
#define RELIX_CONFIG_SYSCALLSTACKS_HH

// relix-kernel
#include "relix/config/mini.hh"


#ifndef CONFIG_SYSCALL_STACKS
#ifdef __MC68K__
#define CONFIG_SYSCALL_STACKS  (!CONFIG_MINI)
#else
#define CONFIG_SYSCALL_STACKS  0
#endif
#endif

#endif

