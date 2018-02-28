/*
	apple-events.hh
	---------------
*/

#ifndef MACCONFIG_APPLEEVENTS_HH
#define MACCONFIG_APPLEEVENTS_HH

// mac-config
#include "mac_config/system.hh"


#ifndef CONFIG_APPLE_EVENTS
#define CONFIG_APPLE_EVENTS  CONFIG_SYSTEM_7_FEATURED
#endif

#ifndef CONFIG_APPLE_EVENTS_GRANTED
#define CONFIG_APPLE_EVENTS_GRANTED  CONFIG_SYSTEM_7_REQUIRED
#endif

#endif
