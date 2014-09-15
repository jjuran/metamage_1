/*
	rectangle-math.mac.cc
	---------------------
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


static const unsigned n_tests = (8 + 8) + 16 + 8 + 8 + 8;


static inline void expect( bool condition, const char* comment )
{
	if ( condition )
	{
		comment = NULL;
	}
	
	tap::ok_if( condition, comment );
}

#define STR_( x ) #x
#define STR( x ) STR_( x )

#define EXPECT( cond )  expect( cond, "File '" __FILE__ "'; Line " STR(__LINE__) )


static void basics()
{
	Rect r = { 1, 2, 3, 4 };
	
	EXPECT( r.top    == 1 );
	EXPECT( r.left   == 2 );
	EXPECT( r.bottom == 3 );
	EXPECT( r.right  == 4 );
	
	SetRect( &r, 2, 4, 6, 8 );
	
	EXPECT( r.top    == 4 );
	EXPECT( r.left   == 2 );
	EXPECT( r.bottom == 8 );
	EXPECT( r.right  == 6 );
	
	SetRect( &r, 0, 0, 1, 1 );
	
	EXPECT( !EmptyRect( &r ) );
	
	SetRect( &r, -32767, -32767, 32767, 32767 );
	
	EXPECT( !EmptyRect( &r ) );
	
	SetRect( &r, 0, 0, 0, 1 );
	
	EXPECT( EmptyRect( &r ) );
	
	SetRect( &r, 0, 0, 1, 0 );
	
	EXPECT( EmptyRect( &r ) );
	
	Rect zero = { 0, 0, 0, 0 };
	
	EXPECT( EmptyRect( &zero ) );
	
	SetRect( &r, 32767, 32767, -32767, -32767 );
	
	EXPECT( EmptyRect( &r ) );
	
	EXPECT(  EqualRect( &r, &r    ) );
	EXPECT( !EqualRect( &r, &zero ) );
}

static void shifts()
{
	Rect r = { 0, 0, 10, 10 };
	
	InsetRect( &r, 2, 3 );
	
	EXPECT( r.top    == 3 );
	EXPECT( r.left   == 2 );
	EXPECT( r.bottom == 7 );
	EXPECT( r.right  == 8 );
	
	InsetRect( &r, 1, -1 );
	
	EXPECT( r.top    == 2 );
	EXPECT( r.left   == 3 );
	EXPECT( r.bottom == 8 );
	EXPECT( r.right  == 7 );
	
	OffsetRect( &r, 2, 1 );
	
	EXPECT( r.top    == 3 );
	EXPECT( r.left   == 5 );
	EXPECT( r.bottom == 9 );
	EXPECT( r.right  == 9 );
	
	OffsetRect( &r, -1, 0 );
	
	EXPECT( r.top    == 3 );
	EXPECT( r.left   == 4 );
	EXPECT( r.bottom == 9 );
	EXPECT( r.right  == 8 );
}

static void common()
{
	Rect a = { 0, 0, 600, 800 };
	Rect b;
	Rect c;
	
	EXPECT( SectRect( &a, &a, &b ) );
	
	EXPECT( EqualRect( &a, &b ) );
	
	a.top = 20;
	
	EXPECT( SectRect( &a, &b, &c ) );
	
	EXPECT( EqualRect( &a, &c ) );
	
	InsetRect( &a, 4, 4 );
	
	EXPECT( SectRect( &a, &b, &c ) );
	
	EXPECT( EqualRect( &a, &c ) );
	
	b.bottom = 24;
	
	EXPECT( !SectRect( &a, &b, &c ) );
	
	const Rect zero = { 0, 0, 0, 0 };
	
	EXPECT( EqualRect( &c, &zero ) );
}

static void unions()
{
	Rect a = { 0, 0, 0, 0 };
	Rect b = { 9, 9, 9, 9 };
	Rect c;
	
	UnionRect( &a, &b, &c );
	
	EXPECT( c.top    == 0 );
	EXPECT( c.left   == 0 );
	EXPECT( c.bottom == 9 );
	EXPECT( c.right  == 9 );
	
	Point p = { 0, 8 };
	Point q = { 6, 0 };
	
	Pt2Rect( p, q, &c );
	
	EXPECT( c.top    == 0 );
	EXPECT( c.left   == 0 );
	EXPECT( c.bottom == 6 );
	EXPECT( c.right  == 8 );
	
}

static void within()
{
	Rect box = { 0, 0, 600, 800 };
	
	Point topLeft = { 0, 0 };
	
	Point bottomRight = { 600, 800 };
	
	EXPECT(  PtInRect( topLeft,     &box ) );
	EXPECT( !PtInRect( bottomRight, &box ) );
	
	OffsetRect( &box, 1, 0 );
	
	EXPECT( !PtInRect( topLeft,     &box ) );
	EXPECT( !PtInRect( bottomRight, &box ) );
	
	OffsetRect( &box, 0, 1 );
	
	EXPECT( !PtInRect( topLeft,     &box ) );
	EXPECT(  PtInRect( bottomRight, &box ) );
	
	OffsetRect( &box, -1, 0 );
	
	EXPECT( !PtInRect( topLeft,     &box ) );
	EXPECT( !PtInRect( bottomRight, &box ) );
}


int main( int argc, char** argv )
{
	tap::start( "rectangle-math.mac", n_tests );
	
	basics();
	shifts();
	common();
	unions();
	within();
	
	return 0;
}
