/*
	bitmap-to-region.mac.cc
	-----------------------
*/

// Mac OS
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


static const unsigned n_tests = 2 + 4 + 8 + 2;


#define EXPECT_RGN( rgn, mem )  EXPECT_CMP( *(rgn), (rgn)[0]->rgnSize, (mem), sizeof (mem) )


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

static void empty( const BitMap& bitmap )
{
	EraseRect( &bitmap.bounds );
	
	RgnHandle r = NewRgn();
	
	OSErr err = BitMapToRegion( r, &bitmap );
	
	EXPECT( err == noErr );
	
	const short empty_rgn[] = { 10, 0, 0, 0, 0 };
	
	EXPECT_RGN( r, empty_rgn );
}

static void rectangular( const BitMap& bitmap )
{
	Rect rect = bitmap.bounds;
	
	RgnHandle r = NewRgn();
	
	
	PaintRect( &rect );
	
	OSErr err = BitMapToRegion( r, &bitmap );
	
	EXPECT( err == noErr );
	
	MacRegion region[] = { { 10, { 0, 0, 32, 32 } } };
	
	EXPECT_RGN( r, region );
	
	
	EraseRect( &bitmap.bounds );
	InsetRect( &region->rgnBBox, 3, 5 );
	
	PaintRect( &region->rgnBBox );
	
	err = BitMapToRegion( r, &bitmap );
	
	EXPECT( err == noErr );
	
	EXPECT_RGN( r, region );
	
	DisposeRgn( r );
}

#define ONE_TO_30  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
#define GRAY_AA  0, ONE_TO_30, 31
#define GRAY_55  ONE_TO_30, 31, 32

static const short complex_rgn[] =
{
	2188,
	0, 0, 31, 32,
	
	0, GRAY_AA, 0x7FFF,
	1, GRAY_AA, 0x7FFF,
	2, GRAY_55, 0x7FFF,
	3, GRAY_55, 0x7FFF,
	4, GRAY_AA, 0x7FFF,
	5, GRAY_AA, 0x7FFF,
	6, GRAY_55, 0x7FFF,
	7, GRAY_55, 0x7FFF,
	8, GRAY_AA, 0x7FFF,
	9, GRAY_AA, 0x7FFF,
	10, GRAY_55, 0x7FFF,
	11, GRAY_55, 0x7FFF,
	12, GRAY_AA, 0x7FFF,
	13, GRAY_AA, 0x7FFF,
	14, GRAY_55, 0x7FFF,
	15, GRAY_55, 0x7FFF,
	16, GRAY_AA, 0x7FFF,
	17, GRAY_AA, 0x7FFF,
	18, GRAY_55, 0x7FFF,
	19, GRAY_55, 0x7FFF,
	20, GRAY_AA, 0x7FFF,
	21, GRAY_AA, 0x7FFF,
	22, GRAY_55, 0x7FFF,
	23, GRAY_55, 0x7FFF,
	24, GRAY_AA, 0x7FFF,
	25, GRAY_AA, 0x7FFF,
	26, GRAY_55, 0x7FFF,
	27, GRAY_55, 0x7FFF,
	28, GRAY_AA, 0x7FFF,
	29, GRAY_AA, 0x7FFF,
	30, GRAY_55, 0x7FFF,
	31, GRAY_55, 0x7FFF,
	
	0x7FFF
};

static const short more_complex_rgn[] =
{
	2196,
	0, 0, 32, 32,
	
	0, GRAY_AA, 0x7FFF,
	1, GRAY_AA, 0x7FFF,
	2, GRAY_55, 0x7FFF,
	3, GRAY_55, 0x7FFF,
	4, GRAY_AA, 0x7FFF,
	5, GRAY_AA, 0x7FFF,
	6, GRAY_55, 0x7FFF,
	7, GRAY_55, 0x7FFF,
	8, GRAY_AA, 0x7FFF,
	9, GRAY_AA, 0x7FFF,
	10, GRAY_55, 0x7FFF,
	11, GRAY_55, 0x7FFF,
	12, GRAY_AA, 0x7FFF,
	13, GRAY_AA, 0x7FFF,
	14, GRAY_55, 0x7FFF,
	15, GRAY_55, 0x7FFF,
	16, GRAY_AA, 0x7FFF,
	17, GRAY_AA, 0x7FFF,
	18, GRAY_55, 0x7FFF,
	19, GRAY_55, 0x7FFF,
	20, GRAY_AA, 0x7FFF,
	21, GRAY_AA, 0x7FFF,
	22, GRAY_55, 0x7FFF,
	23, GRAY_55, 0x7FFF,
	24, GRAY_AA, 0x7FFF,
	25, GRAY_AA, 0x7FFF,
	26, GRAY_55, 0x7FFF,
	27, GRAY_55, 0x7FFF,
	28, GRAY_AA, 0x7FFF,
	29, GRAY_AA, 0x7FFF,
	30, GRAY_55, 0x7FFF,
	31, 1 - GRAY_55, 0x7FFF,
	32, 0, 1, 0x7FFF,
	
	0x7FFF
};

static const short clipped_rgn[] =
{
	2188 - 64,
	0, 0, 31, 31,
	
	0, GRAY_AA, 0x7FFF,
	1, GRAY_AA, 0x7FFF,
	2, ONE_TO_30, 0x7FFF,
	3, ONE_TO_30, 0x7FFF,
	4, GRAY_AA, 0x7FFF,
	5, GRAY_AA, 0x7FFF,
	6, ONE_TO_30, 0x7FFF,
	7, ONE_TO_30, 0x7FFF,
	8, GRAY_AA, 0x7FFF,
	9, GRAY_AA, 0x7FFF,
	10, ONE_TO_30, 0x7FFF,
	11, ONE_TO_30, 0x7FFF,
	12, GRAY_AA, 0x7FFF,
	13, GRAY_AA, 0x7FFF,
	14, ONE_TO_30, 0x7FFF,
	15, ONE_TO_30, 0x7FFF,
	16, GRAY_AA, 0x7FFF,
	17, GRAY_AA, 0x7FFF,
	18, ONE_TO_30, 0x7FFF,
	19, ONE_TO_30, 0x7FFF,
	20, GRAY_AA, 0x7FFF,
	21, GRAY_AA, 0x7FFF,
	22, ONE_TO_30, 0x7FFF,
	23, ONE_TO_30, 0x7FFF,
	24, GRAY_AA, 0x7FFF,
	25, GRAY_AA, 0x7FFF,
	26, ONE_TO_30, 0x7FFF,
	27, ONE_TO_30, 0x7FFF,
	28, GRAY_AA, 0x7FFF,
	29, GRAY_AA, 0x7FFF,
	30, ONE_TO_30, 0x7FFF,
	31, ONE_TO_30, 0x7FFF,
	
	0x7FFF
};

static void complex( const BitMap& bitmap )
{
	Rect rect = bitmap.bounds;
	
	RgnHandle r = NewRgn();
	
	const uint32_t pattern[] = { 0xAA005500, 0xAA005500 };
	
	FillRect( &rect, (const Pattern*) pattern );
	
	
	OSErr err = BitMapToRegion( r, &bitmap );
	
	EXPECT( err == noErr );
	EXPECT_RGN( r, complex_rgn );
	
	
	MoveTo( 0, 31 );
	LineTo( 0, 31 );
	
	err = BitMapToRegion( r, &bitmap );
	
	EXPECT( err == noErr );
	EXPECT_RGN( r, more_complex_rgn );
	
	
	BitMap alt_bitmap = bitmap;
	
	alt_bitmap.bounds.bottom -= 1;
	
	err = BitMapToRegion( r, &alt_bitmap );
	
	EXPECT( err == noErr );
	EXPECT_RGN( r, complex_rgn );
	
	
	alt_bitmap.bounds.right -= 1;
	
	err = BitMapToRegion( r, &alt_bitmap );
	
	EXPECT( err == noErr );
	EXPECT_RGN( r, clipped_rgn );
	
	
	DisposeRgn( r );
}

#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(                            \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define PACK9( _8, _7, _6, _5, _4, _3, _2, _1, _0 )  \
	PACK16( _8, _7, _6, _5, _4, _3, _2, _1, _0, 0, 0, 0, 0, 0, 0, 0 )

#define _ 0
#define X 1

static const uint16_t splat_bits[] =
{
	PACK9( _,_,_,_,X,_,_,_,_ ),
	PACK9( _,X,_,_,X,_,_,X,_ ),
	PACK9( _,_,X,_,X,_,X,_,_ ),
	PACK9( _,_,_,_,_,_,_,_,_ ),
	PACK9( X,X,X,_,_,_,X,X,X ),
	PACK9( _,_,_,_,_,_,_,_,_ ),
	PACK9( _,_,X,_,X,_,X,_,_ ),
	PACK9( _,X,_,_,X,_,_,X,_ ),
	PACK9( _,_,_,_,X,_,_,_,_ ),
};

static BitMap splat_bitmap =
{
	(Ptr) splat_bits,
	2,
	{ 0, 0, 9, 9 },
};

static void splat()
{
	const short splat_rgn[] =
	{
		66 * 2,
		0, 0, 9, 9,
		
		0,             4, 5,             0x7FFF,
		1,    1, 2,             7, 8,    0x7FFF,
		2,    1,    3,       6,    8,    0x7FFF,
		3,       2, 3, 4, 5, 6, 7,       0x7FFF,
		4, 0,       3,       6,       9, 0x7FFF,
		5, 0,       3,       6,       9, 0x7FFF,
		6,       2, 3, 4, 5, 6, 7,       0x7FFF,
		7,    1,    3,       6,    8,    0x7FFF,
		8,    1, 2,             7, 8,    0x7FFF,
		9,             4, 5,             0x7FFF,
		
		0x7FFF
	};
	
	RgnHandle r = NewRgn();
	
	OSErr err = BitMapToRegion( r, &splat_bitmap );
	
	EXPECT( err == noErr );
	EXPECT_RGN( r, splat_rgn );
}


int main( int argc, char** argv )
{
	tap::start( "bitmap-to-region.mac", n_tests );
	
	init();
	
	empty      ( testing_grafPort->portBits );
	rectangular( testing_grafPort->portBits );
	complex    ( testing_grafPort->portBits );
	
	splat();
	
	term();
	
	return 0;
}
