/*
	adb.hh
	------
*/

#ifndef MACCONFIG_ADB_HH
#define MACCONFIG_ADB_HH

// mac-config
#include "mac_config/system.hh"


/*
	ADB is supported even prior to System 6 (which is the earliest we track).
	The ADB API is no longer available in Carbon.
*/

#ifndef CONFIG_ADB
#define CONFIG_ADB  (! TARGET_API_MAC_CARBON)
#endif

/*
	Take ADB for granted if we're on PPC or if System 7.6 or later is required.
	System 7.5.5 is the last version that runs on a Mac Plus.
*/

#ifndef CONFIG_ADB_GRANTED
	#ifdef __POWERPC__
		#define CONFIG_ADB_GRANTED  1
	#else
		#define CONFIG_ADB_GRANTED  CONFIG_SYSTEM_7_6_REQUIRED
	#endif
#endif  // #ifndef CONFIG_ADB_GRANTED

#endif
