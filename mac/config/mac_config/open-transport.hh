/*
	open-transport.hh
	-----------------
*/

#ifndef MACCONFIG_OPENTRANSPORT_HH
#define MACCONFIG_OPENTRANSPORT_HH

/*
	In Mac OS X, Open Transport headers are deprecated in 10.4
	and removed entirely in 10.8.  If present, they're covered
	by the CoreServices.h umbrella header.
	
	If we're building for System 6 only, then CONFIG_OPEN_TRANSPORT
	will be 0, but CONFIG_OPEN_TRANSPORT_HEADERS will be 1, because
	we can still include the headers and use inline functions.
*/

// Mac OS X
#ifdef __APPLE__
	#include <CoreServices/CoreServices.h>
	
	#ifdef __OPENTRANSPORTPROVIDERS__
		#define CONFIG_OPEN_TRANSPORT 1
	#else
		#define CONFIG_OPEN_TRANSPORT 0
	#endif
	#define CONFIG_OPEN_TRANSPORT_HEADERS CONFIG_OPEN_TRANSPORT
#else
	#define CONFIG_OPEN_TRANSPORT_HEADERS 1
#endif

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
