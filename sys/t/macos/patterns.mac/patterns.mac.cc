/*
	patterns.mac.cc
	---------------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>

// iota
#include "iota/bits.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 1 + 1 + 1;


#define _ 0
#define X 1

#define EXPECT_ICONS( a, b )  EXPECT_CMP( a, icon_bytes, b, icon_bytes )


struct icon
{
	uint32_t rows[ 32 ];
};

static const icon ltGray_32[] =
{
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
	BIG32( X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_ ),
	BIG32( _,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_ ),
};

static inline int rowBytes_from_bitwidth( int bits )
{
	return (bits + 15) / 16 * 2;
}

const int icon_width  = 32;
const int icon_height = 32;

const int icon_px = icon_width * icon_height;  // 1024
const int icon_bytes = icon_px / 8;  // 128;

const int icon_rowBytes = rowBytes_from_bitwidth( icon_width );  // 4

QDGlobals qd;

static GrafPtr testing_grafPort;

static void init()
{
	InitGraf( &qd.thePort );
	
	testing_grafPort = (GrafPtr) malloc( sizeof (GrafPort) );
	
	OpenPort( testing_grafPort );
	
	PortSize( 32, 32 );
	
	char* mem = (char*) malloc( icon_bytes );
	
	BitMap bitmap = { mem, icon_rowBytes, { 0, 0, icon_height, icon_width } };
	
	SetPortBits( &bitmap );
}

static void term()
{
	SetPort( NULL );
	
	if ( testing_grafPort != NULL )
	{
		free( testing_grafPort->portBits.baseAddr );
		
		ClosePort( testing_grafPort );
		
		free( testing_grafPort );
	}
}

static void backpat( const char* base, const Rect& bounds )
{
	Rect rect = bounds;
	
	BackPat( &qd.ltGray );
	
	EraseRect( &rect );
	
	EXPECT_ICONS( base, ltGray_32 );
}

static void penpat( const char* base, const Rect& bounds )
{
	Rect rect = bounds;
	
	PaintRect( &rect );
	
	PenPat( &qd.ltGray );
	
	PaintRect( &rect );
	
	EXPECT_ICONS( base, ltGray_32 );
}

static void penmode( const char* base, const Rect& bounds )
{
	Rect rect = bounds;
	
	// Clear previous data (which is correct) in case PaintRect() does nothing.
	FillRect( &rect, &qd.white );
	
	PenPat( &qd.dkGray );
	
	PenMode( notPatCopy );
	
	PaintRect( &rect );
	
	EXPECT_ICONS( base, ltGray_32 );
}


int main( int argc, char** argv )
{
	tap::start( "patterns.mac", n_tests );
	
	init();
	
	const char* base   = testing_grafPort->portBits.baseAddr;
	const Rect& bounds = testing_grafPort->portBits.bounds;
	
	backpat( base, bounds );
	penpat ( base, bounds );
	penmode( base, bounds );
	
	term();
	
	return 0;
}
