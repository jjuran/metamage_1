/*
	RoundRectRgn.cc
	---------------
*/

#include "RoundRectRgn.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-qd
#include "OvalRgn.hh"
#include "Regions.hh"


static
RgnHandle rectangular_utility_region()
{
	static RgnHandle rgn = NewRgn();
	
	return rgn;
}

static
void XorRgnRect( RgnHandle  rgn,
                 short      left,
                 short      top,
                 short      right,
                 short      bottom )
{
	RgnHandle tmp = rectangular_utility_region();
	
	SetRectRgn( tmp, left, top, right, bottom );
	
	XorRgn( tmp, rgn, rgn );
}

static
void round_rect_corners( RgnHandle rgn, RgnHandle corners )
{
	const Rect* bbox;
	bbox = &rgn[0]->rgnBBox;
	
	const short width  = bbox->right - bbox->left;
	const short height = bbox->bottom - bbox->top;
	
	bbox = &corners[0]->rgnBBox;
	
	const short ovalWidth  = bbox->right - bbox->left;
	const short ovalHeight = bbox->bottom - bbox->top;
	
	const short hRadius = ovalWidth  / 2;
	const short vRadius = ovalHeight / 2;
	
	static RgnHandle tmp = NewRgn();
	
	SetRectRgn( tmp,
	            0,       0,
	            hRadius, vRadius );
	
	SectRgn( corners, tmp, tmp );
	XorRgn( tmp, rgn, rgn );
	
	SetRectRgn( tmp,
	            ovalWidth - hRadius, 0,
	            ovalWidth,           vRadius );
	
	SectRgn( corners, tmp, tmp );
	OffsetRgn( tmp, width - ovalWidth, 0 );
	XorRgn( tmp, rgn, rgn );
	
	SetRectRgn( tmp,
	            0,       ovalHeight - vRadius,
	            hRadius, ovalHeight );
	
	SectRgn( corners, tmp, tmp );
	OffsetRgn( tmp, 0, height - ovalHeight );
	XorRgn( tmp, rgn, rgn );
	
	SetRectRgn( tmp,
	            ovalWidth - hRadius, ovalHeight - vRadius,
	            ovalWidth,           ovalHeight );
	
	SectRgn( corners, tmp, tmp );
	OffsetRgn( tmp, width - ovalWidth, height - ovalHeight );
	XorRgn( tmp, rgn, rgn );
	
	SetEmptyRgn( tmp );
}

static RgnHandle corners = NewRgn_patch();

void RoundRectRgn( MacRegion**  rgn,
                   short        width,
                   short        height,
                   short        ovalWidth,
                   short        ovalHeight )
{
	XorRgnRect( rgn, 0, 0, width, height );
	
	OvalRgn( corners, ovalWidth, ovalHeight );
	
	XorRgnRect( corners, 0, 0, ovalWidth, ovalHeight );
	
	round_rect_corners( rgn, corners );
	
	SetEmptyRgn( corners );
}

void FramedRoundRectRgn( MacRegion**  rgn,
                         short        width,
                         short        height,
                         short        ovalWidth,
                         short        ovalHeight,
                         short        penWidth,
                         short        penHeight )
{
	const short hRadius = ovalWidth  / 2;
	const short vRadius = ovalHeight / 2;
	
	XorRgnRect( rgn, hRadius, 0,                  width - hRadius, penHeight );
	XorRgnRect( rgn, hRadius, height - penHeight, width - hRadius, height    );
	
	XorRgnRect( rgn, 0,                vRadius, penWidth, height - vRadius );
	XorRgnRect( rgn, width - penWidth, vRadius, width,    height - vRadius );
	
	FramedOvalRgn( corners, ovalWidth, ovalHeight, penWidth, penHeight );
	
	round_rect_corners( rgn, corners );
	
	SetEmptyRgn( corners );
}
