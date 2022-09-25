/*
	OvalRgn.cc
	----------
*/

#include "OvalRgn.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-qd
#include "circular_region.hh"
#include "ovoid_region.hh"
#include "RoundRectRgn.hh"


void OvalRgn( RgnHandle rgn, short width, short height )
{
	RgnHandle tmp = width == height ? circular_region( width )
	                                : ovoid_region( width, height );
	
	XorRgn( tmp, rgn, rgn );
}

static inline
void FramedOvalHollowRgn( MacRegion**  rgn,
                          short        width,
                          short        height,
                          short        penWidth,
                          short        penHeight )
{
	if ( (width -= 2 * penWidth) > 0  &&  (height -= 2 * penHeight) > 0 )
	{
		OvalRgn( rgn, width, height );
		
		OffsetRgn( rgn, penWidth, penHeight );
	}
}

void FramedOvalRgn( MacRegion**  rgn,
                    short        width,
                    short        height,
                    short        penWidth,
                    short        penHeight )
{
	FramedOvalHollowRgn( rgn, width, height, penWidth, penHeight );
	
	OvalRgn( rgn, width, height );
}
