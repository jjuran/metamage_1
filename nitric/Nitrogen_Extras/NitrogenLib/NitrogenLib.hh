//	NitrogenLib.hh

#include <cmath>

#ifdef	__MACH__
	
	/*
	 *	Apple Framework Headers
	 */
	
	#ifndef __NOEXTENSIONS__
		#define __NOEXTENSIONS__
	#endif
	
	#ifndef __CF_USE_FRAMEWORK_INCLUDES__
		#define __CF_USE_FRAMEWORK_INCLUDES__
	#endif
	
	#include <Carbon/Carbon.h>
	
#else
	
	// MSL from CW Pro 6 has a math/fp interaction bug.
	// If you include cmath, fp.h, then math.h, it breaks.
	// We need to include cmath before fp.h, so we include math.h before fp.h.
	// Carbon.h includes fp.h.
	
	#include <math.h>
	
	// OpenTransport.h will define some useful Unix error constants for us,
	// but only if we define OTUNIXERRORS.  Since we precompile, this is our
	// only shot to do it.
	
	#define OTUNIXERRORS 1
	
	#include <Carbon.h>
	
#endif

#include "NitrogenHeaders.cp"

