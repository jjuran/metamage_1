/*
	PtInRgn.mac.cc
	--------------
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


static const unsigned n_tests = 18;


static inline
Point mkpt( short h, short v )
{
	const Point pt = { v, h };
	
	return pt;
}

static
bool PtInRgn( short h, short v, RgnHandle rgn )
{
	return PtInRgn( mkpt( h, v ), rgn );
}

static void pt_in_rgn()
{
	Rect bounds = { 57, 156, 207, 356 };
	
	RgnHandle window = NewRgn();
	RgnHandle shadow = NewRgn();
	
	EXPECT( ! PtInRgn( 0, 0, window ) );
	
	RectRgn( window, &bounds );
	OffsetRect( &bounds, 1, 1 );
	RectRgn( shadow, &bounds );
	
	EXPECT( ! PtInRgn( 0, 0, window ) );
	
	EXPECT( ! PtInRgn( 156, 56, window ) );
	EXPECT( ! PtInRgn( 155, 57, window ) );
	EXPECT(   PtInRgn( 156, 57, window ) );
	
	EXPECT(   PtInRgn( 355, 206, window ) );
	EXPECT( ! PtInRgn( 356, 206, window ) );
	EXPECT( ! PtInRgn( 355, 207, window ) );  // FAIL
	
	UnionRgn( window, shadow, window );
	
	EXPECT( ! PtInRgn( 0, 0, window ) );
	
	EXPECT( ! PtInRgn( 156, 56, window ) );
	EXPECT( ! PtInRgn( 155, 57, window ) );
	EXPECT(   PtInRgn( 156, 57, window ) );
	
	EXPECT(   PtInRgn( 355, 206, window ) );
	EXPECT(   PtInRgn( 356, 206, window ) );
	EXPECT(   PtInRgn( 355, 207, window ) );
	
	EXPECT(   PtInRgn( 356, 207, window ) );
	EXPECT( ! PtInRgn( 357, 207, window ) );
	EXPECT( ! PtInRgn( 356, 208, window ) );  // FAIL
	
	DisposeRgn( shadow );
	DisposeRgn( window );
}


int main( int argc, char** argv )
{
	tap::start( "PtInRgn.mac", n_tests );
	
	pt_in_rgn();
	
	return 0;
}
