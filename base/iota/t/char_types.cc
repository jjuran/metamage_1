/*
	char_types.cc
	-------------
*/

// Standard C
#include <ctype.h>

// iota
#include "iota/char_types.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 256 * 15;

// type, preposition, function, char
#define CTYPE( t, p, f, c )  (t) p##f( c )
#define ITYPE( t, p, f, c )  (t) iota::p##_##f( c )

#define MATCH( t, p, f, c )  (CTYPE( t, p, f, c ) == ITYPE( t, p, f, c ))

static void char_types()
{
	for ( int i = 0;  i < 256;  ++i )
	{
	#ifdef __MSL__
		EXPECT( true );
	#else
		EXPECT( MATCH( bool, is, ascii, i ) );
	#endif
		EXPECT( MATCH( bool, is, cntrl, i ) );
	#ifdef __MSL__
		EXPECT( true );
	#else
		EXPECT( MATCH( bool, is, blank, i ) );
	#endif
		EXPECT( MATCH( bool, is, space, i ) );
		EXPECT( MATCH( bool, is, print, i ) );
		EXPECT( MATCH( bool, is, graph, i ) );
		EXPECT( MATCH( bool, is, upper, i ) );
		EXPECT( MATCH( bool, is, lower, i ) );
		EXPECT( MATCH( bool, is, alpha, i ) );
		EXPECT( MATCH( bool, is, digit, i ) );
		EXPECT( MATCH( bool, is, xdigit, i ) );
		EXPECT( MATCH( bool, is, alnum, i ) );
		EXPECT( MATCH( bool, is, punct, i ) );
		EXPECT( MATCH( char, to, upper, i ) );
		EXPECT( MATCH( char, to, lower, i ) );
	}
}

int main( int argc, char** argv )
{
	tap::start( "char_types", n_tests );
	
	char_types();
	
	return 0;
}
