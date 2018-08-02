/*
	OvalRgn.cc
	----------
*/

#include "OvalRgn.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


static
void XorRgnRect( RgnHandle  rgn,
                 RgnHandle  tmp,
                 short      left,
                 short      top,
                 short      right,
                 short      bottom )
{
	SetRectRgn( tmp, left, top, right, bottom );
	
	XorRgn( tmp, rgn, rgn );
}

static
void CircularOvalRgn( RgnHandle rgn, short width, short height )
{
	const Fixed c_squared = 0x00010000;
	
	static RgnHandle tmp = NewRgn();
	
	short x = width / 2;
	short y0 = 0;
	
	const Fixed width_reciprocal  = FixRatio( 1, width  );
	const Fixed height_reciprocal = FixRatio( 1, height );
	
	while ( y0 < x )
	{
		--x;
		
		const short y1 = y0 + 1;
		
		const Fixed a = (width  - x  * 2 + 1) * width_reciprocal;
		const Fixed b = (height - y0 * 2 - 1) * height_reciprocal;
		
		const Fixed a_squared = FixMul( a, a );
		const Fixed b_squared = FixMul( b, b );
		
		if ( a_squared + b_squared > c_squared  ||  x <= y1 )
		{
			if ( x == y1  &&  a_squared + b_squared < c_squared )
			{
				--x;
			}
			
			XorRgnRect( rgn, tmp, x,          y0, width - x,          y1 );
			XorRgnRect( rgn, tmp, x, height - y1, width - x, height - y0 );
			
			if ( x == y0 )
			{
				++x;
			}
			
			XorRgnRect( rgn, tmp,          y0, x,          y1, width - x );
			XorRgnRect( rgn, tmp, height - y1, x, height - y0, width - x );
			
			y0 = y1;
		}
	}
	
	XorRgnRect( rgn, tmp, y0, y0, height - y0, height - y0 );
	
	SetEmptyRgn( tmp );
}

static
void EllipticOvalRgn( RgnHandle rgn, short width, short height )
{
	// TODO
}

void OvalRgn( RgnHandle rgn, short width, short height )
{
	(width == height ? CircularOvalRgn : EllipticOvalRgn)( rgn, width, height );
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
