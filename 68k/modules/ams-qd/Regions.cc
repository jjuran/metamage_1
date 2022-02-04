/*
	Regions.cc
	----------
*/

#include "Regions.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "Polygons.hh"
#include "Rect-utils.hh"
#include "Rects.hh"


#pragma exceptions off


OSErr MemErr : 0x0220;


static const Rect emptyRect = { 0, 0, 0, 0 };


pascal void StdRgn_patch( signed char verb, MacRegion** rgn )
{
	static RgnHandle utility_rgn = (scoped_zone(), NewRgn());
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( verb == kQDGrafVerbFrame )
	{
		RgnHandle frame = utility_rgn;  // clobbered by CopyRgn()
		
		CopyRgn( rgn, frame );
		
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			UnionRgn( rgn, rgnSave, rgnSave );
		}
		
		InsetRgn( frame, port.pnSize.h, port.pnSize.v );
		
		DiffRgn( rgn, frame, frame );
		
		verb = kQDGrafVerbPaint;
		
		rgn = frame;
	}
	
	RgnHandle clipRgn = utility_rgn;  // clobbered by SectRgn()
	
	SectRgn( port.clipRgn, rgn, clipRgn );
	
	RgnHandle saved_clipRgn = port.clipRgn;
	
	port.clipRgn = clipRgn;
	
	const Rect& box = clipRgn[0]->rgnBBox;
	
	StdRect( verb, &box );
	
	port.clipRgn = saved_clipRgn;
}

pascal void FrameRgn_patch( MacRegion** rgn )
{
	StdRgn( kQDGrafVerbFrame, rgn );
}

pascal void PaintRgn_patch( MacRegion** rgn )
{
	StdRgn( kQDGrafVerbPaint, rgn );
}

pascal void EraseRgn_patch( MacRegion** rgn )
{
	StdRgn( kQDGrafVerbErase, rgn );
}

pascal void InverRgn_patch( MacRegion** rgn )
{
	StdRgn( kQDGrafVerbInvert, rgn );
}

pascal void FillRgn_patch( MacRegion** rgn, const Pattern* pattern )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.fillPat = *pattern;
	
	StdRgn( kQDGrafVerbFill, rgn );
}

pascal MacRegion** NewRgn_patch()
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
	GrafPort& port = **get_addrof_thePort();
	
	RgnHandle rgn = NewRgn();
	
	port.rgnSave = (Handle) rgn;
	
	OpenPoly();
}

pascal void CloseRgn_patch( MacRegion** rgn )
{
	GrafPort& port = **get_addrof_thePort();
	
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
	
	if ( MemErr != noErr )
	{
		return;
	}
	
	fast_memcpy( *dst, *src, size );
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

pascal unsigned char RectInRgn_patch( const Rect* r, MacRegion** rgn )
{
	/*
		Inside Macintosh Volume I documents that RectInRgn() sometimes
		returns true when the rectangle merely intersects the region's
		bounding box but not the region itself.  It's not clear if Apple's
		implementation is more than a glorified SectRect() call, but in any
		case the documentation doesn't require it.
	*/
	
	Rect intersection;
	
	return SectRect( r, &rgn[0]->rgnBBox, &intersection );
}

pascal unsigned char EqualRgn_patch( MacRegion** a, MacRegion** b )
{
	if ( a == b )
	{
		return true;
	}
	
	if ( a[0]->rgnSize != b[0]->rgnSize )
	{
		return false;
	}
	
	if ( EmptyRgn( a )  &&  EmptyRgn( b ) )
	{
		return true;
	}
	
	return fast_memcmp( *a, *b, a[0]->rgnSize ) == 0;
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
