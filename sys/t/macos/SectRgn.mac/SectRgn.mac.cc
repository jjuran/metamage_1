/*
	SectRgn.mac.cc
	--------------
*/

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 4;


#define EXPECT_RECTS( a, b )  EXPECT_CMP( &a, 8, &b, 8 )


QDGlobals qd;

GrafPtr WMgrPort : 0x09DE;

static void init()
{
	InitGraf( &qd.thePort );
	InitWindows();
}

static void menu_bottom()
{
	Rect menuBottom = { 19, 0, 20, 512 };
	
	RgnHandle rgn = NewRgn();
	RgnHandle tmp = NewRgn();
	
	RectRgn( rgn, &menuBottom );
	
	SectRgn( WMgrPort->clipRgn, rgn, tmp );
	SectRgn( WMgrPort->clipRgn, rgn, rgn );
	
	EXPECT_RECTS( tmp[0]->rgnBBox, menuBottom );
	EXPECT_RECTS( rgn[0]->rgnBBox, menuBottom );
	
	menuBottom.left += 50;
	RectRgn( rgn, &menuBottom );
	SectRgn( WMgrPort->clipRgn, rgn, rgn );
	
	EXPECT_RECTS( rgn[0]->rgnBBox, menuBottom );
	
	menuBottom.right -= 50;
	RectRgn( rgn, &menuBottom );
	SectRgn( WMgrPort->clipRgn, rgn, rgn );
	
	EXPECT_RECTS( rgn[0]->rgnBBox, menuBottom );
	
	DisposeRgn( tmp );
	DisposeRgn( rgn );
}


int main( int argc, char** argv )
{
	tap::start( "SectRgn.mac", n_tests );
	
	init();
	
	menu_bottom();
	
	return 0;
}
