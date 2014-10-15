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


static const unsigned n_tests = 11 + 4 + 4 + 4 + 2;


#define EXPECT_RGN( rgn, mem )  EXPECT_CMP( *(rgn), (rgn)[0]->rgnSize, (mem), sizeof (mem) )

#define EXPECT_RGNS( a, b )  EXPECT_CMP( *(a), (a)[0]->rgnSize, *(b), (b)[0]->rgnSize )


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

static void binary()
{
	RgnHandle a = NewRgn();
	RgnHandle b = NewRgn();
	
	
	SetRectRgn( a,  1, 2,  4, 3 );
	SetRectRgn( b, 16, 5, 64, 8 );
	
	XorRgn( a, b, a );
	
	const short dexter[] =
	{
		44, 2, 1, 8, 64,
		
		2,  1, 4,          0x7FFF,
		3,  1, 4,          0x7FFF,
		5,        16, 64,  0x7FFF,
		8,        16, 64,  0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( a, dexter );
	
	
	SetRectRgn( a, 16, 2, 64, 3 );
	SetRectRgn( b,  1, 5,  4, 8 );
	
	XorRgn( a, b, a );
	
	const short sinister[] =
	{
		44, 2, 1, 8, 64,
		
		2,        16, 64,  0x7FFF,
		3,        16, 64,  0x7FFF,
		5,  1, 4,          0x7FFF,
		8,  1, 4,          0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( a, sinister );
	
	
	SetRectRgn( a, 1, 2, 64, 8 );
	SetRectRgn( b, 4, 3, 16, 5 );
	
	XorRgn( a, b, a );
	
	const short hollow[] =
	{
		44, 2, 1, 8, 64,
		
		2,  1,        64,  0x7FFF,
		3,     4, 16,      0x7FFF,
		5,     4, 16,      0x7FFF,
		8,  1,        64,  0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( a, hollow );
	
	
	SetRectRgn( a, 1, 2, 16, 5 );
	SetRectRgn( b, 4, 3, 64, 8 );
	
	XorRgn( a, b, a );
	
	const short overlap[] =
	{
		44, 2, 1, 8, 64,
		
		2,  1,    16,      0x7FFF,
		3,     4,     64,  0x7FFF,
		5,  1,    16,      0x7FFF,
		8,     4,     64,  0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( a, overlap );
	
	
	DisposeRgn( a );
	DisposeRgn( b );
}

static void cancel()
{
	RgnHandle a = NewRgn();
	RgnHandle b = NewRgn();
	RgnHandle c = NewRgn();
	
	
	SetRectRgn( a, 4, 1, 5, 2 );
	SetRectRgn( b, 4, 1, 6, 3 );
	
	XorRgn( a, b, c );
	
	const short corner[] =
	{
		36, 1, 4, 3, 6,
		
		1,     5, 6,  0x7FFF,
		2,  4, 5,     0x7FFF,
		3,  4,    6,  0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( c, corner );
	
	
	XorRgn( a, c, c );
	
	EXPECT_RGNS( c, b );
	
	
	XorRgn( b, c, c );
	
	EXPECT_RGN( c, empty_region );
	
	
	SetRectRgn( a, 4, 1, 6, 3 );
	SetRectRgn( b, 4, 3, 6, 5 );
	
	XorRgn( a, b, c );
	
	const short blocks[] =
	{
		10, 1, 4, 5, 6
	};
	
	EXPECT_RGN( c, blocks );
	
	
	DisposeRgn( a );
	DisposeRgn( b );
	DisposeRgn( c );
}

static void shifts()
{
	const short centered[] = { 10, -300, -400, 300, 400 };
	
	RgnHandle a = NewRgn();
	RgnHandle b = NewRgn();
	
	SetRectRgn( a, 0, 0, 800, 600 );
	
	OffsetRgn( a, -400, -300 );
	
	EXPECT_RGN( a, centered );
	
	SetRectRgn( b, -2, -2, 2, 2 );
	
	XorRgn( a, b, a );
	
	OffsetRgn( a, 400, 300 );
	
	const short punctured[] =
	{
		44,
		0, 0, 600, 800,
		
		0,    0,           800,  0x7FFF,
		298,     398, 402,       0x7FFF,
		302,     398, 402,       0x7FFF,
		600,  0,           800,  0x7FFF,
		
		0x7FFF
	};
	
	EXPECT_RGN( a, punctured );
	
	DisposeRgn( a );
	DisposeRgn( b );
}


int main( int argc, char** argv )
{
	tap::start( "region-math.mac", n_tests );
	
	empty();
	unary();
	
	binary();
	cancel();
	shifts();
	
	return 0;
}
