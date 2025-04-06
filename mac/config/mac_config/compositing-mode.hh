/*
	compositing-mode.hh
	-------------------
*/

#ifndef MACCONFIG_COMPOSITINGMODE_HH
#define MACCONFIG_COMPOSITINGMODE_HH

// Mac OS X
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif


/*
	Compositing mode exists as of Mac OS X 10.2 "Jaguar".
	However, there are usability issues with it in 10.2,
	wo we won't enable it until Mac OS X 10.3 "Panther".
*/

#ifdef MAC_OS_X_VERSION_10_2
#define CONFIG_COMPOSITING_MODE_EXISTS  1
#else
#define CONFIG_COMPOSITING_MODE_EXISTS  0
#endif

#ifndef CONFIG_COMPOSITING
#ifdef MAC_OS_X_VERSION_10_3  // not 10.2
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif
#endif

#endif
