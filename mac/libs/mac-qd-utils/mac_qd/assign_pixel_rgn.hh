/*
	assign_pixel_rgn.hh
	-------------------
*/

#ifndef MACQD_ASSIGNPIXELRGN_HH
#define MACQD_ASSIGNPIXELRGN_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#if TARGET_API_MAC_CARBON  ||  OPAQUE_TOOLBOX_STRUCTS
/*
	TARGET_API_MAC_CARBON is always defined by the build environment.
	If OPAQUE_TOOLBOX_STRUCTS is left undefined, it defaults to false for
	non-Carbon.  The only way to have OPAQUE_TOOLBOX_STRUCTS without Carbon
	is to define it in the build environment.  So it's fine that we check it
	before including any Mac OS headers.
*/
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif  // #if TARGET_API_MAC_CARBON  ||  OPAQUE_TOOLBOX_STRUCTS

// mac-types
#include "mac_types/Point.hh"
#include "mac_types/Rect.hh"


struct MacRegion;


namespace mac {
namespace qd  {
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	// Toolbox structs are opaque -- nothing we can do but call SetRectRgn().
	
	inline
	void assign_pixel_rgn( RgnHandle rgn, Point pt )
	{
		SetRectRgn( rgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
	}
	
#else
	
	struct Region
	{
		short             rgnSize;
		mac::types::Rect  rgnBBox;
	};
	
	typedef Region** RgnHandle;
	
	#ifdef __MC68K__
	const uint32_t OneOne : 0x0A02;
	#else
	const uint32_t OneOne = 0x00010001;
	#endif
	
	inline
	void assign_pixel_rgn( RgnHandle rgn, mac::types::Point pt )
	{
		uint32_t corner = (uint32_t&) pt;
		
		long* p = (long*) &rgn[0]->rgnBBox;
		
		*p++ = corner;
		*p   = corner + OneOne;
	}
	
	inline
	void assign_pixel_rgn( ::MacRegion** rgn, ::Point pt )
	{
		assign_pixel_rgn( (RgnHandle) rgn, (mac::types::Point&) pt );
	}
	
#endif
	
}
}

#endif
