/*
	quickdraw.hh
	------------
*/

#ifndef QUICKDRAW_HH
#define QUICKDRAW_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif


/*
	CopyBits() is completely unavailable in 64-bit builds.
	In Mac OS X 10.7+, its declaration is missing from Quickdraw.h.
*/

#if __LP64__  ||  MAC_OS_X_VERSION_10_7
	#if CONFIG_QUICKDRAW
		#warning Overriding enabled CONFIG_QUICKDRAW
	#endif
	#undef CONFIG_QUICKDRAW
	#define CONFIG_QUICKDRAW  0
#endif

/*
	CGDisplayGetDrawingContext() wasn't added until Mac OS X 10.3,
	so use CopyBits() in 10.2.
*/

#ifndef MAC_OS_X_VERSION_10_3
	#if defined( CONFIG_QUICKDRAW )  &&  ! CONFIG_QUICKDRAW
		#warning Overriding disabled CONFIG_QUICKDRAW
	#endif
	#undef CONFIG_QUICKDRAW
	#define CONFIG_QUICKDRAW  1
#endif

/*
	Using CopyBits() instead of CGImage improves performance considerably
	in 10.5 on x86, and somewhat in 10.4 on PPC, so prefer it by default.
*/

#ifndef CONFIG_QUICKDRAW
#define CONFIG_QUICKDRAW  1
#endif

#endif
