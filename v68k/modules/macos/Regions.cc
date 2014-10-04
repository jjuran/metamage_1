/*
	Regions.cc
	----------
*/

#include "Regions.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <string.h>

// quickdraw
#include "qd/xor_region.hh"

// macos
#include "Rect-utils.hh"
#include "Region-ops.hh"


using quickdraw::xor_region;


static const Rect emptyRect = { 0, 0, 0, 0 };


pascal MacRegion** NewRgn_patch()
{
	RgnHandle h = (RgnHandle) NewHandleClear( sizeof (MacRegion) );
	
	if ( h != NULL )
	{
		h[0]->rgnSize = sizeof (MacRegion);
	}
	
	return h;
}

pascal void DisposeRgn_patch( MacRegion** rgn )
{
	DisposeHandle( (Handle) rgn );
}

pascal void CopyRgn_patch( MacRegion** src, MacRegion** dst )
{
	if ( src == dst )
	{
		return;
	}
	
	const size_t size = src[0]->rgnSize;
	
	SetHandleSize( (Handle) dst, size );
	
	if ( MemError() != noErr )
	{
		return;
	}
	
	memcpy( *dst, *src, size );
}

static void set_rect_region( MacRegion** rgn, const Rect& r )
{
	MacRegion& region = **rgn;
	
	region.rgnSize = sizeof (MacRegion);
	
	region.rgnBBox = r;
	
	SetHandleSize( (Handle) rgn, sizeof (MacRegion) );
}

pascal void SetEmptyRgn_patch( MacRegion** rgn )
{
	set_rect_region( rgn, emptyRect );
}

pascal void SetRectRgn_patch( MacRegion**  rgn,
                              short        left,
                              short        top,
                              short        right,
                              short        bottom )
{
	if ( top >= bottom  ||  left >= right )
	{
		set_rect_region( rgn, emptyRect );
	}
	else
	{
		const Rect r = { top, left, bottom, right };
		
		set_rect_region( rgn, r );
	}
}

pascal void RectRgn_patch( MacRegion** rgn, const Rect* r )
{
	if ( r->top >= r->bottom  ||  r->left >= r->right )
	{
		r = &emptyRect;
	}
	
	set_rect_region( rgn, *r );
}

pascal void XOrRgn_patch( MacRegion** a, MacRegion** b, MacRegion** dst )
{
	binary_region_op( a, b, dst, &xor_region );
}

pascal unsigned char EmptyRgn_patch( MacRegion** rgn )
{
	/*
		Empty regions should be 0,0 - 0,0, but Postel's Law suggests we
		allow for the caller manipulating the bounding box directly --
		i.e. don't just check for all zero data.
	*/
	
	return empty_rect( rgn[0]->rgnBBox );
}
