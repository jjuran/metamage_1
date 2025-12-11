/*
	Region-make.cc
	--------------
*/

#include "Region-make.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"

// ams-qd
#include "Polygons.hh"
#include "Rects.hh"


#pragma exceptions off


OSErr MemErr : 0x0220;


static const Rect emptyRect = { 0, 0, 0, 0 };


pascal RgnHandle NewRgn_patch()
{
	RgnHandle h = (RgnHandle) NewHandleClear( sizeof (MacRegion) );
	
	if ( h != NULL )
	{
		h[0]->rgnSize = sizeof (MacRegion);
	}
	
	return h;
}

pascal void OpenRgn_patch()
{
	GrafPort& port = *get_thePort();
	
	RgnHandle rgn = NewRgn();
	
	port.rgnSave = (Handle) rgn;
	
	OpenPoly();
}

pascal void CloseRgn_patch( RgnHandle rgn )
{
	GrafPort& port = *get_thePort();
	
	PolyHandle poly = (PolyHandle) port.polySave;
	
	ClosePoly();
	
	SetEmptyRgn( rgn );
	
	const short n_pts = (poly[0]->polySize - sizeof (Polygon)) / 4;
	
	if ( n_pts >= 3 )
	{
		PolyRgn( rgn, poly );
	}
	
	KillPoly( poly );
	
	UnionRgn( (RgnHandle) port.rgnSave, rgn, rgn );
	
	DisposeHandle( port.rgnSave );
	
	port.rgnSave = NULL;
}

pascal void DisposeRgn_patch( RgnHandle rgn )
{
	DisposeHandle( (Handle) rgn );
}

pascal void CopyRgn_patch( RgnHandle src, RgnHandle dst )
{
	if ( src == dst )
	{
		return;
	}
	
	const size_t size = src[0]->rgnSize;
	
	SetHandleSize( (Handle) dst, size );
	
	if ( MemErr != noErr )
	{
		return;
	}
	
	fast_memcpy( *dst, *src, size );
}

static void set_rect_region( RgnHandle rgn, const Rect& r )
{
	MacRegion& region = **rgn;
	
	region.rgnSize = sizeof (MacRegion);
	
	region.rgnBBox = r;
	
	SetHandleSize( (Handle) rgn, sizeof (MacRegion) );
}

pascal void SetEmptyRgn_patch( RgnHandle rgn )
{
	set_rect_region( rgn, emptyRect );
}

pascal void SetRectRgn_patch( RgnHandle  rgn,
                              short      left,
                              short      top,
                              short      right,
                              short      bottom )
{
	const Rect r = { top, left, bottom, right };
	
	RectRgn( rgn, &r );
}

pascal void RectRgn_patch( RgnHandle rgn, const Rect* r )
{
	if ( r->top >= r->bottom  ||  r->left >= r->right )
	{
		r = &emptyRect;
	}
	
	set_rect_region( rgn, *r );
}
