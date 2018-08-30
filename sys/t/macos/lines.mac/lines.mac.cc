/*
	lines.mac.cc
	------------
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


static const unsigned n_tests = 5 + 5;


#define _ 0
#define X 1

#define EXPECT_ICONS( a, b )  EXPECT_CMP( a, icon_bytes, b, icon_bytes )


static inline
int rowBytes_from_bitwidth( int bits )
{
	return (bits + 15) / 16 * 2;
}

typedef uint16_t icon_row;

const int icon_width  = 16;
const int icon_height = 16;

const int icon_px = icon_width * icon_height;  // 256
const int icon_bytes = icon_px / 8;  // 32;

const int icon_rowBytes = rowBytes_from_bitwidth( icon_width );  // 2


struct icon
{
	icon_row rows[ icon_height ];
};

static const icon line_1[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,X,X,X,X,X,X,X,X,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_2[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,X,X,X,X,X,X,X,X,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_3[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,X,X,X,X,X,X,X,X,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,X,_,X,_,X,_,X,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_1_1[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_n2_n2[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,X,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_3_n3[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,X,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,X,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

static const icon line_n4_4[] =
{
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,X,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,X,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,X,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
	BIG16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),
};

QDGlobals qd;

static GrafPtr testing_grafPort;

static
void init()
{
	InitGraf( &qd.thePort );
	
	testing_grafPort = (GrafPtr) malloc( sizeof (GrafPort) );
	
	OpenPort( testing_grafPort );
	
	PortSize( icon_width, icon_height );
	
	char* mem = (char*) malloc( icon_bytes );
	
	BitMap bitmap = { mem, icon_rowBytes, { 0, 0, icon_height, icon_width } };
	
	SetPortBits( &bitmap );
}

static
void term()
{
	SetPort( NULL );
	
	if ( testing_grafPort != NULL )
	{
		free( testing_grafPort->portBits.baseAddr );
		
		ClosePort( testing_grafPort );
		
		free( testing_grafPort );
	}
}

static
void nondiagonal( const char* base, const Rect& bounds )
{
	Rect rect = bounds;
	
	EraseRect( &rect );
	
	MoveTo( 9, 9 );  // prove that MoveTo() isn't actually Move()
	MoveTo( 2, 3 );
	LineTo( 9, 3 );
	
	EXPECT_ICONS( base, line_1 );
	
	ShowPen();
	Move( -2, -1 );
	Line( 0, 6 );
	
	EXPECT_ICONS( base, line_2 );
	
	HidePen();
	PenPat( &qd.gray );
	Line( 8, 0 );
	
	EXPECT_ICONS( base, line_3 );
	
	HidePen();
	Line( 0, 10 );
	
	EXPECT_ICONS( base, line_3 );
	
	EXPECT( qd.thePort->pnLoc.v == 18 );
	
	ShowPen();
	PenPat( &qd.black );
}

static
void diagonal_line( const char*  base,
                    const Rect&  bounds,
                    short        dh,
                    short        dv,
                    const icon*  bits )
{
	EraseRect( &bounds );
	
	MoveTo( 4, 4 );
	Line( dh, dv );
	
	EXPECT_ICONS( base, bits );
}

static
void diagonal( const char* base, const Rect& bounds )
{
	Rect rect = bounds;
	
	diagonal_line( base, bounds,  1,  1, line_1_1   );
	diagonal_line( base, bounds, -2, -2, line_n2_n2 );
	diagonal_line( base, bounds,  3, -3, line_3_n3  );
	diagonal_line( base, bounds, -4,  4, line_n4_4  );
	diagonal_line( base, bounds, -5,  5, line_n4_4  );  // clipped
}


int main( int argc, char** argv )
{
	tap::start( "lines.mac", n_tests );
	
	init();
	
	const char* base   = testing_grafPort->portBits.baseAddr;
	const Rect& bounds = testing_grafPort->portBits.bounds;
	
	nondiagonal( base, bounds );
	diagonal   ( base, bounds );
	
	term();
	
	return 0;
}
