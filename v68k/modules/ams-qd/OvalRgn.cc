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

// Standard C
#include <stdlib.h>

// quickdraw
#include "qd/convex_region_generator.hh"


using quickdraw::convex_region_generator;


static
short* generate_circle( short width, short height, const short* p, short* r )
{
	convex_region_generator generator( r );
	
	short y = 0;
	short x = *p++;
	
	generator.start( y, x, width - x );
	
	while ( x > ++y )
	{
		x = *p++;
		
		generator.expand( y, x, width - x );
	}
	
	if ( x < y )
	{
		--p;
	}
	
	while ( --x >= 0 )
	{
		y = *--p;
		
		generator.expand( y, x, width - x );
	}
	
	x = 0;
	y = *p++;
	
	while ( ++x < y )
	{
		generator.contract( height - y, x, width - x );
		
		y = *p++;
	}
	
	if ( x > y )
	{
		--p;
	}
	
	while ( y > 0 )
	{
		x = *--p;
		
		generator.contract( height - y, x, width - x );
		
		--y;
	}
	
	return generator.finish( height );
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
	
	short* scratchpad = (short*) alloca( 2 + height / 2 );
	
	short* p = scratchpad;
	
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
			
			*p++ = x;
			
			y0 = y1;
		}
	}
	
	Size rgnSize = sizeof (MacRegion) + height * 2 * 6;  // close enough
	
	if ( GetHandleSize( (Handle) tmp ) < rgnSize )
	{
		SetHandleSize( (Handle) tmp, rgnSize );
	}
	
	Rect& bbox = tmp[0]->rgnBBox;
	
	bbox.top  = 0;
	bbox.left = 0;
	bbox.bottom = height;
	bbox.right  = width;
	
	short* begin = (short*) (*tmp + 1);
	short* end   = generate_circle( width, height, scratchpad, begin );
	
	tmp[0]->rgnSize = sizeof (MacRegion) + (end - begin) * sizeof (short);
	
	XorRgn( tmp, rgn, rgn );
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
