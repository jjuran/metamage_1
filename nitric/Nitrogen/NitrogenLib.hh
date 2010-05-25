//	NitrogenLib.hh

#include <cmath>

#if defined(__MWERKS__) && defined(__MACH__)
	
	/*
	 *	Apple Framework Headers
	 */
	
	#ifndef __NOEXTENSIONS__
		#define __NOEXTENSIONS__
	#endif
	
	#ifndef __CF_USE_FRAMEWORK_INCLUDES__
		#define __CF_USE_FRAMEWORK_INCLUDES__
	#endif
	
#endif

#ifdef __MACH__
	
	#define MAC_OS_X_VERSION_MIN_REQUIRED  MAC_OS_X_VERSION_10_2
	
	#include <Carbon/Carbon.h>
	
#else
	
	// MSL from CW Pro 6 has a math/fp interaction bug.
	// If you include cmath, fp.h, then math.h, it breaks.
	// We need to include cmath before fp.h, so we include math.h before fp.h.
	// Carbon.h includes fp.h.
	
	#include <math.h>
	
	#include <ConditionalMacros.h>
	
	#if !TARGET_API_MAC_CARBON
		
		// Carbonate provides:
		// * Implementations of Carbon-only routines
		// * Implementations of Carbon accessors as functions for 68K
		// * Implementations of Carbon accessors as inlines
		// * Implementations of Carbon-renamed routines for CFM-68K
		
		#include "Carbonate/Carbonate.hh"
		
	#endif
	
	#include <Carbon.h>
	
#endif

