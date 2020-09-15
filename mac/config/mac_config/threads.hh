/*
	threads.hh
	----------
*/

#ifndef MACCONFIG_THREADS_HH
#define MACCONFIG_THREADS_HH

// mac-config
#include "mac_config/system.hh"


#ifndef CONFIG_THREAD_MANAGER
/*
	Thread Manager is available in System 7 (pre-7.5) via an extension.
*/
#define CONFIG_THREAD_MANAGER  CONFIG_SYSTEM_7_FEATURED
#endif

#ifndef CONFIG_THREAD_MANAGER_GRANTED
/*
	Thread Manager is built into System 7.5 and later.
*/
#define CONFIG_THREAD_MANAGER_GRANTED  CONFIG_SYSTEM_7_5_REQUIRED
#endif

#endif
