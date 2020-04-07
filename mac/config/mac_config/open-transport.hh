/*
	open-transport.hh
	-----------------
*/

#ifndef MACCONFIG_OPENTRANSPORT_HH
#define MACCONFIG_OPENTRANSPORT_HH

#ifdef __clang__
#include <Availability.h>
#endif

// mac-config
#include "mac_config/system.hh"


#ifdef __MAC_10_8
// Open Transport headers are unavailable in the 10.8 SDK and later.
#define CONFIG_OPEN_TRANSPORT 0
#endif

#ifndef CONFIG_OPEN_TRANSPORT
/*
	Open Transport is available in System 7.1 via an extension.
	
	TODO:
	However, Open Transport is no longer available in OS X 10.9.
*/
#define CONFIG_OPEN_TRANSPORT  CONFIG_SYSTEM_7_1_FEATURED
#endif

#endif
