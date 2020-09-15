/*
	open-transport.hh
	-----------------
*/

#ifndef MACCONFIG_OPENTRANSPORT_HH
#define MACCONFIG_OPENTRANSPORT_HH

// mac-config
#include "mac_config/system.hh"


#ifndef CONFIG_OPEN_TRANSPORT
/*
	Open Transport is available in System 7.1 via an extension.
	
	TODO:
	However, Open Transport is no longer available in OS X 10.9.
*/
#define CONFIG_OPEN_TRANSPORT  CONFIG_SYSTEM_7_1_FEATURED
#endif

#endif
