/*
	pen-state.mac.cc
	----------------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 11;


#define EXPECT_PENSTATE( a, b )  EXPECT_CMP( &a, sizeof (PenState), &b, sizeof (PenState) )


QDGlobals qd;

static GrafPtr testing_grafPort;

static void init()
{
	InitGraf( &qd.thePort );
	
	testing_grafPort = (GrafPtr) malloc( sizeof (GrafPort) );
	
	OpenPort( testing_grafPort );
}

static void term()
{
	SetPort( NULL );
	
	if ( testing_grafPort != NULL )
	{
		ClosePort( testing_grafPort );
		
		free( testing_grafPort );
	}
}

static const PenState default_penState =
{
	{ 0, 0 },  // pnLoc
	{ 1, 1 },  // pnSize
	patCopy,   // pnMode
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // pnPat
};

static void get_set()
{
	PenState penState;
	
	GetPenState( &penState );
	EXPECT_PENSTATE( penState, default_penState );
	
	penState.pnLoc.v = 2;
	penState.pnLoc.h = 3;
	penState.pnSize.v = 4;
	penState.pnSize.h = 5;
	
	penState.pnMode = patBic;
	penState.pnPat  = qd.ltGray;
	
	SetPenState( &penState );
	EXPECT_PENSTATE( qd.thePort->pnLoc, penState );
	
	penState = default_penState;
	
	GetPenState( &penState );
	EXPECT_PENSTATE( penState, qd.thePort->pnLoc );
	
	Point pnLoc;
	
	GetPen( &pnLoc );
	EXPECT( pnLoc.v == 2  &&  pnLoc.h == 3 );
	
	PenSize( 7, 6 );
	EXPECT( qd.thePort->pnSize.v == 6  &&  qd.thePort->pnSize.h == 7 );
	
	PenMode( patXor );
	EXPECT( qd.thePort->pnMode == patXor );
	
	PenPat( &qd.dkGray );
	EXPECT_CMP( &qd.thePort->pnPat, sizeof (Pattern), &qd.dkGray, sizeof (Pattern) );
	
	PenNormal();
	
	GetPenState( &penState );
	EXPECT( penState.pnLoc.v == 2  &&  penState.pnLoc.h == 3 );
	
	penState.pnLoc = default_penState.pnLoc;
	EXPECT_PENSTATE( penState, default_penState );
	
	MoveTo( -9, -8 );
	EXPECT( qd.thePort->pnLoc.v == -8  &&  qd.thePort->pnLoc.h == -9 );
	
	Move( 1, 2 );
	EXPECT( qd.thePort->pnLoc.v == -6  &&  qd.thePort->pnLoc.h == -8 );
}


int main( int argc, char** argv )
{
	tap::start( "pen-state.mac", n_tests );
	
	init();
	
	get_set();
	
	term();
	
	return 0;
}
