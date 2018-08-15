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


void RoundRectRgn( MacRegion**  rgn,
                   short        width,
                   short        height,
                   short        ovalWidth,
                   short        ovalHeight )
{
	if ( ovalWidth == ovalHeight )
	{
		RgnHandle tmp = circular_region( ovalWidth, width, height );
		
		XorRgn( tmp, rgn, rgn );
	}
	else
	{
		// TODO
	}
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
