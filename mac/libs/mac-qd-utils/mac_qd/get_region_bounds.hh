/*
	get_region_bounds.hh
	--------------------
*/

#ifndef MACQD_GETREGIONBOUNDS_HH
#define MACQD_GETREGIONBOUNDS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"


#if ! __LP64__

namespace mac {
namespace qd  {
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	inline
	const Rect& get_region_bounds_unsafe( RgnHandle rgn )
	{
		/*
			We're dereferencing the handle.  This is okay
			if you don't use the reference after making a
			call that could cause Memory Manager blocks
			to be moved or purged.  Handle with care.
		*/
		
		return rgn[0]->rgnBBox;
	}
	
	inline
	Rect get_region_bounds( RgnHandle rgn )
	{
		return rgn[0]->rgnBBox;
	}
	
#else
	
#if ! __LP64__
	
	inline
	Rect get_region_bounds( RgnHandle rgn )
	{
		Rect bounds;
		
		GetRegionBounds( rgn, &bounds );
		
		return bounds;
	}
	
	inline
	Rect get_region_bounds_unsafe( RgnHandle rgn )
	{
		return get_region_bounds( rgn );
	}
	
#endif
#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
	
}
}

#endif  // #if ! __LP64__

#endif
