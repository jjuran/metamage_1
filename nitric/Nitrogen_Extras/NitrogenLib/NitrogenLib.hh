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
	#include <math.h>
	#include <Carbon.h>
	
#endif

#include "NitrogenHeaders.cp"

