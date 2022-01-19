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
#include "circular_region.hh"


static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

void RoundRectRgn( MacRegion**  rgn,
                   short        width,
                   short        height,
                   short        ovalWidth,
                   short        ovalHeight )
{
	ovalWidth  = min( ovalWidth,  width  );
	ovalHeight = min( ovalHeight, height );
	
	// TODO:  Support assymetric diameters for real
	
	short diameter = ovalWidth < ovalHeight ? ovalWidth : ovalHeight;
	
	RgnHandle tmp = circular_region( diameter, width, height );
	
	XorRgn( tmp, rgn, rgn );
}

static inline
void FramedRoundRectHollowRgn( MacRegion**  rgn,
                               short        width,
                               short        height,
                               short        ovalWidth,
                               short        ovalHeight,
                               short        penWidth,
                               short        penHeight )
{
	if ( (width -= 2 * penWidth) > 0  &&  (height -= 2 * penHeight) > 0 )
	{
		ovalWidth  -= 2 * penWidth;
		ovalHeight -= 2 * penHeight;
		
		RoundRectRgn( rgn, width, height, ovalWidth, ovalHeight );
		
		OffsetRgn( rgn, penWidth, penHeight );
	}
}

void FramedRoundRectRgn( MacRegion**  rgn,
                         short        width,
                         short        height,
                         short        ovalWidth,
                         short        ovalHeight,
                         short        penWidth,
                         short        penHeight )
{
	FramedRoundRectHollowRgn( rgn,
	                          width,
	                          height,
	                          ovalWidth,
	                          ovalHeight,
	                          penWidth,
	                          penHeight );
	
	RoundRectRgn( rgn, width, height, ovalWidth, ovalHeight );
}
