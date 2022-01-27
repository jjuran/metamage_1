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


static
void CircularOvalRgn( RgnHandle rgn, short width, short height )
{
	RgnHandle tmp = circular_region( width );
	
	XorRgn( tmp, rgn, rgn );
}

static
void EllipticOvalRgn( RgnHandle rgn, short width, short height )
{
	RgnHandle tmp = ovoid_region( width, height );
	
	XorRgn( tmp, rgn, rgn );
}

void OvalRgn( RgnHandle rgn, short width, short height )
{
	// TODO:  Support assymetric diameters for real
	
	if ( width != height )
	{
		EllipticOvalRgn( rgn, width, height );
	}
	else
	{
		CircularOvalRgn( rgn, width, height );
	}
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
