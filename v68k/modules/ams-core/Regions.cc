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

// ams-core
#include "QDGlobals.hh"
#include "Rect-utils.hh"
#include "Rects.hh"


static const Rect emptyRect = { 0, 0, 0, 0 };


pascal void StdRgn_patch( signed char verb, MacRegion** rgn )
{
	RgnHandle tmp = NewRgn();
	
	if ( tmp == NULL )
	{
		return;
	}
	
	if ( verb == kQDGrafVerbFrame )
	{
		RgnHandle frame = tmp;
		
		CopyRgn( rgn, frame );
		
		GrafPort& port = **get_addrof_thePort();
		
		InsetRgn( frame, port.pnSize.h, port.pnSize.v );
		
		DiffRgn( rgn, frame, frame );
		
		verb = kQDGrafVerbPaint;
		
		rgn = frame;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	RgnHandle clipRgn = tmp;
	
	SectRgn( port.clipRgn, rgn, clipRgn );
	
	RgnHandle saved_clipRgn = port.clipRgn;
	
	port.clipRgn = clipRgn;
	
	const Rect& box = clipRgn[0]->rgnBBox;
	
	StdRect_patch( verb, &box );
	
	port.clipRgn = saved_clipRgn;
	
	DisposeRgn( tmp );
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
	
	RgnHandle rgn = NewRgn_patch();
	
	port.rgnSave = (Handle) rgn;
	
	HidePen();
}

pascal void CloseRgn_patch( MacRegion** rgn )
{
	ShowPen();
	
	GrafPort& port = **get_addrof_thePort();
	
	CopyRgn_patch( (RgnHandle) port.rgnSave, rgn );
	
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

pascal unsigned char EmptyRgn_patch( MacRegion** rgn )
{
	/*
		Empty regions should be 0,0 - 0,0, but Postel's Law suggests we
		allow for the caller manipulating the bounding box directly --
		i.e. don't just check for all zero data.
	*/
	
	return empty_rect( rgn[0]->rgnBBox );
}
