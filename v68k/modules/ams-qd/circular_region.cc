/*
	circular_region.cc
	------------------
*/

#include "circular_region.hh"

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

RgnHandle circular_region( short diameter, short width, short height )
{
	const Fixed c_squared = 0x00010000;
	
	static RgnHandle tmp = NewRgn();
	
	short x = diameter / 2u;
	short y0 = 0;
	
	const Fixed diameter_reciprocal = FixRatio( 1, diameter );
	
	short* scratchpad = (short*) alloca( 2 + diameter / 2u );
	
	short* p = scratchpad;
	
	while ( y0 < x )
	{
		--x;
		
		const short y1 = y0 + 1;
		
		const Fixed a = (diameter - x  * 2 + 1) * diameter_reciprocal;
		const Fixed b = (diameter - y0 * 2 - 1) * diameter_reciprocal;
		
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
	
	Size rgnSize = sizeof (MacRegion) + diameter * 2 * 6;  // close enough
	
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
	
	return tmp;
}
