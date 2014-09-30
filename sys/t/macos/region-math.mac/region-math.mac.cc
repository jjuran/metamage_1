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


static const unsigned n_tests = 26 + 10;


static void empty()
{
	RgnHandle a = NewRgn();
	
	EXPECT( a[0]->rgnSize == sizeof (MacRegion) );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 0 );
	EXPECT( a[0]->rgnBBox.left   == 0 );
	EXPECT( a[0]->rgnBBox.right  == 0 );
	EXPECT( a[0]->rgnBBox.bottom == 0 );
	
	SetRectRgn( a, 1, 2, -3, -4 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 0 );
	EXPECT( a[0]->rgnBBox.left   == 0 );
	EXPECT( a[0]->rgnBBox.right  == 0 );
	EXPECT( a[0]->rgnBBox.bottom == 0 );
	
	SetRectRgn( a, 20, 0, 20, 512 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 0 );
	EXPECT( a[0]->rgnBBox.left   == 0 );
	EXPECT( a[0]->rgnBBox.right  == 0 );
	EXPECT( a[0]->rgnBBox.bottom == 0 );
	
	SetRectRgn( a, 1, 0, 10000, 0 );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 0 );
	EXPECT( a[0]->rgnBBox.left   == 0 );
	EXPECT( a[0]->rgnBBox.right  == 0 );
	EXPECT( a[0]->rgnBBox.bottom == 0 );
	
	const Rect r = { 1, 2, -3, -4 };
	
	RectRgn( a, &r );
	
	EXPECT( EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 0 );
	EXPECT( a[0]->rgnBBox.left   == 0 );
	EXPECT( a[0]->rgnBBox.right  == 0 );
	EXPECT( a[0]->rgnBBox.bottom == 0 );
	
	DisposeRgn( a );
}

static void unary()
{
	RgnHandle a = NewRgn();
	
	SetRectRgn( a, 1, 2, 4, 8 );
	
	EXPECT( !EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 2 );
	EXPECT( a[0]->rgnBBox.left   == 1 );
	EXPECT( a[0]->rgnBBox.bottom == 8 );
	EXPECT( a[0]->rgnBBox.right  == 4 );
	
	const Rect r = { 1, 2, 3, 5 };
	
	RectRgn( a, &r );
	
	EXPECT( !EmptyRgn( a ) );
	
	EXPECT( a[0]->rgnBBox.top    == 1 );
	EXPECT( a[0]->rgnBBox.left   == 2 );
	EXPECT( a[0]->rgnBBox.bottom == 3 );
	EXPECT( a[0]->rgnBBox.right  == 5 );
	
	DisposeRgn( a );
}


int main( int argc, char** argv )
{
	tap::start( "region-math.mac", n_tests );
	
	empty();
	unary();
	
	return 0;
}
