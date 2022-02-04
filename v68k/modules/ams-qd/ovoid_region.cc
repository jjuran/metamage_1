/*
	ovoid_region.cc
	---------------
*/

#include "ovoid_region.hh"

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>

// log-of-war
#include "logofwar/report.hh"

// quickdraw
#include "qd/convex_region_generator.hh"

// ams-common
#include "scoped_zone.hh"


#pragma exceptions off


using quickdraw::convex_region_generator;


static inline
size_t byte_distance( const void* begin, const void* end )
{
	return (const char*) end - (const char*) begin;
}

static inline
Fixed FixSqr( Fixed x )
{
	return FixMul( x, x );
}

RgnHandle ovoid_region( short hdiam, short vdiam, short width, short height )
{
	const Fixed c_squared = 0x00010000;
	
	const short max_diam = hdiam < vdiam ? vdiam : hdiam;
	
	static RgnHandle tmp = (scoped_zone(), NewRgn());
	
	tmp[0]->rgnSize = sizeof (MacRegion);
	
	Rect& bbox = tmp[0]->rgnBBox;
	
	bbox.top  = 0;
	bbox.left = 0;
	bbox.bottom = height;
	bbox.right  = width;
	
	Size rgnSize = sizeof (MacRegion) + max_diam * 2 * 6;  // close enough
	
	if ( GetHandleSize( (Handle) tmp ) < rgnSize )
	{
		SetHandleSize( (Handle) tmp, rgnSize );
	}
	
	short* begin = (short*) (*tmp + 1);
	
	convex_region_generator generator( begin );
	
	short x = hdiam / 2u;
	short y = 0;
	
	const Fixed hdiam_reciprocal = FixRatio( 1, hdiam );
	const Fixed vdiam_reciprocal = FixRatio( 1, vdiam );
	
	const short mid = (vdiam - 1) / 2u;
	
	Fixed a_squared = 0;
	Fixed b_squared = FixSqr( (vdiam - y * 2 - 1) * vdiam_reciprocal );
	
	Fixed c_squared_minus_b_squared = c_squared - b_squared;
	
	do
	{
		a_squared = FixSqr( (hdiam - --x * 2 + 1) * hdiam_reciprocal );
	}
	while ( a_squared < c_squared_minus_b_squared );
	
	generator.start( y, x, width - x );
	
	while ( x > 0  &&  ++y <= mid )
	{
		Fixed c_squared_minus_a_squared = c_squared - a_squared;
		
		do
		{
			b_squared = FixSqr( (vdiam - y * 2 - 1) * vdiam_reciprocal );
		}
		while ( b_squared > c_squared_minus_a_squared  &&  ++y < mid );
		
		if ( y >= mid )
		{
			break;
		}
		
		c_squared_minus_b_squared = c_squared - b_squared;
		
		do
		{
			a_squared = FixSqr( (hdiam - --x * 2 + 1) * hdiam_reciprocal );
		}
		while ( x > 0  &&  a_squared < c_squared_minus_b_squared );
		
		generator.expand( y, x, width - x );
	}
	
	if ( x > 0  &&  y <= mid )
	{
		generator.expand( y, 0, width );
	}
	
	short* end = generator.finish_reflected( height, begin );
	
	tmp[0]->rgnSize = sizeof (MacRegion) + byte_distance( begin, end );
	
	return tmp;
}
