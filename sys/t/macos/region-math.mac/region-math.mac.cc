/*
	region-math.mac.cc
	------------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 11 + 4;


#define EXPECT_RGN( rgn, mem )  EXPECT_CMP( *(rgn), (rgn)[0]->rgnSize, (mem), sizeof (mem) )


static short empty_region[] = { sizeof (MacRegion), 0, 0, 0, 0 };


struct rect_region
{
	short data[ 5 ];
	
	rect_region( short top, short left, short bottom, short right )
	{
		short* p = data;
		
		*p++ = sizeof (MacRegion);
		
		*p++ = top;
		*p++ = left;
		*p++ = bottom;
		*p++ = right;
	}
	
	operator const short*()  { return data; }
};

static void empty()
{
	RgnHandle a = NewRgn();
	
	EXPECT( a[0]->rgnSize == sizeof (MacRegion) );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT_RGN( a, empty_region );
	
	SetRectRgn( a, 1, 2, -3, -4 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT_RGN( a, empty_region );
	
	SetRectRgn( a, 20, 0, 20, 512 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT_RGN( a, empty_region );
	
	SetRectRgn( a, 1, 0, 10000, 0 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT_RGN( a, empty_region );
	
	const Rect r = { 1, 2, -3, -4 };
	
	RectRgn( a, &r );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT_RGN( a, empty_region );
	
	DisposeRgn( a );
}

static void unary()
{
	RgnHandle a = NewRgn();
	
	SetRectRgn( a, 1, 2, 4, 8 );
	
	EXPECT( !EmptyRgn( a ) );
	
	EXPECT_RGN( a, rect_region( 2, 1, 8, 4 ) );
	
	const Rect r = { 1, 2, 3, 5 };
	
	RectRgn( a, &r );
	
	EXPECT( !EmptyRgn( a ) );
	
	EXPECT_RGN( a, rect_region( 1, 2, 3, 5 ) );
	
	DisposeRgn( a );
}


int main( int argc, char** argv )
{
	tap::start( "region-math.mac", n_tests );
	
	empty();
	unary();
	
	return 0;
}
