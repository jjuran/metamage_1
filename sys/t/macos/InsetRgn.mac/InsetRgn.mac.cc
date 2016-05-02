/*
	InsetRgn.mac.cc
	---------------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2;


#define EXPECT_RGN( rgn, mem )  EXPECT_CMP( *(rgn), (rgn)[0]->rgnSize, (mem), sizeof (mem) )


QDGlobals qd;

static
void dot()
{
	RgnHandle dot = NewRgn();
	
	SetRectRgn( dot, 80, 80, 81, 81 );
	
	InsetRgn( dot, -1, -1 );
	
	const short outset[] = { 10, 79, 79, 82, 82 };
	
	EXPECT_RGN( dot, outset );
	
	RgnHandle mid = NewRgn();
	
	SetRectRgn( mid, 79, 80, 82, 81 );
	XorRgn( dot, mid, dot );
	
	SetRectRgn( mid, 80, 79, 81, 82 );
	XorRgn( dot, mid, dot );
	
	InsetRgn( dot, 1, 1 );
	
	const short empty[] = { 10, 0, 0, 0, 0 };
	
	EXPECT_RGN( dot, empty );
	
	DisposeRgn( mid );
	DisposeRgn( dot );
}


int main( int argc, char** argv )
{
	tap::start( "InsetRgn.mac", n_tests );
	
	dot();
	
	return 0;
}
