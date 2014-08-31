/*
	Regions.cc
	----------
*/

#include "Regions.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


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

pascal void SetEmptyRgn_patch( MacRegion** rgn )
{
	MacRegion& region = **rgn;
	
	region.rgnSize = sizeof (MacRegion);
	
	region.rgnBBox.top    = 0;
	region.rgnBBox.left   = 0;
	region.rgnBBox.bottom = 0;
	region.rgnBBox.right  = 0;
	
	SetHandleSize( (Handle) rgn, sizeof (MacRegion) );
}

pascal void SetRectRgn_patch( MacRegion**  rgn,
                              short        left,
                              short        top,
                              short        right,
                              short        bottom )
{
	MacRegion& region = **rgn;
	
	region.rgnSize = sizeof (MacRegion);
	
	region.rgnBBox.top    = top;
	region.rgnBBox.left   = left;
	region.rgnBBox.bottom = bottom;
	region.rgnBBox.right  = right;
	
	SetHandleSize( (Handle) rgn, sizeof (MacRegion) );
}

pascal void RectRgn_patch( MacRegion** rgn, const Rect* r )
{
	MacRegion& region = **rgn;
	
	region.rgnSize = sizeof (MacRegion);
	
	region.rgnBBox = *r;
	
	SetHandleSize( (Handle) rgn, sizeof (MacRegion) );
}
