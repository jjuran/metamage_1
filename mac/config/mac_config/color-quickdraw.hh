/*
	color-quickdraw.hh
	------------------
*/

#ifndef MACCONFIG_COLORQUICKDRAW_HH
#define MACCONFIG_COLORQUICKDRAW_HH

// mac-config
#include "mac_config/system.hh"


/*
	Color is supported as of System 4.1, which added support for the Mac II.
*/

#ifndef CONFIG_COLOR_QUICKDRAW
#define CONFIG_COLOR_QUICKDRAW  CONFIG_SYSTEM_4_1_FEATURED
#endif

/*
	Take Color QuickDraw for granted if we're not on a 68000, or if System 7.6
	or later is required.  All 68020+ (and PPC and x86) Macs support color.
	System 7.5.5 is the last version that runs on monochrome-only machines.
*/

#ifndef CONFIG_COLOR_QUICKDRAW_GRANTED
	#if defined( __MC68K__ )  &&  !__MC68020__
		#define CONFIG_COLOR_QUICKDRAW_GRANTED  CONFIG_SYSTEM_7_6_REQUIRED
	#else
		#define CONFIG_COLOR_QUICKDRAW_GRANTED  1
	#endif
#endif

#endif
