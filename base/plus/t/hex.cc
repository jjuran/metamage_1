/*
	hex.cc
	------
*/

// vxo-string
#include "vxo-string/lib/unhex.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"

/*
	Granted, it's a little weird that we're testing code from another
	library.  However, vxo::unhex() replaced plus::unhex(), and it's
	better to keep a working test than to delete it.  At some point it
	can get moved to vxo-tests (when that exists), but for now it's here.
*/

using vxo::unhex;

static const unsigned n_tests = 6 + 21 + 24;


#define EXPECT_EQ_HEX( a, b )  EXPECT_CMP( a.data(), a.size(), b.data(), b.size() )

#define UNHEX( s )  string_cast( unhex( plus::string( s ) ) )

#define UNHEX_ALIGN( s, a )  string_cast( unhex( plus::string( s ), a ) )

#define STRING( s )  ::plus::string( STR_LEN( s ) )


static inline
plus::string string_cast( const vxo::Box& box )
{
	return *(const plus::string*) &box;
}

static void zero()
{
	EXPECT_EQ_HEX( UNHEX( "" ), plus::string() );
	EXPECT_EQ_HEX( UNHEX( "0" ), plus::string( 1, '\0' ) );
	EXPECT_EQ_HEX( UNHEX( "00" ), plus::string( 1, '\0' ) );
	EXPECT_EQ_HEX( UNHEX( "000" ), plus::string( 2, '\0' ) );
	EXPECT_EQ_HEX( UNHEX( "0000" ), plus::string( 2, '\0' ) );
	EXPECT_EQ_HEX( UNHEX( "00000" ), plus::string( 3, '\0' ) );
}

static void digits()
{
	EXPECT_EQ_HEX( UNHEX( "1" ), STRING( "\x01" ) );
	EXPECT_EQ_HEX( UNHEX( "2" ), STRING( "\x02" ) );
	EXPECT_EQ_HEX( UNHEX( "3" ), STRING( "\x03" ) );
	EXPECT_EQ_HEX( UNHEX( "4" ), STRING( "\x04" ) );
	EXPECT_EQ_HEX( UNHEX( "5" ), STRING( "\x05" ) );
	EXPECT_EQ_HEX( UNHEX( "6" ), STRING( "\x06" ) );
	EXPECT_EQ_HEX( UNHEX( "7" ), STRING( "\x07" ) );
	EXPECT_EQ_HEX( UNHEX( "8" ), STRING( "\x08" ) );
	EXPECT_EQ_HEX( UNHEX( "9" ), STRING( "\x09" ) );
	
	EXPECT_EQ_HEX( UNHEX( "A" ), STRING( "\x0a" ) );
	EXPECT_EQ_HEX( UNHEX( "B" ), STRING( "\x0b" ) );
	EXPECT_EQ_HEX( UNHEX( "C" ), STRING( "\x0c" ) );
	EXPECT_EQ_HEX( UNHEX( "D" ), STRING( "\x0d" ) );
	EXPECT_EQ_HEX( UNHEX( "E" ), STRING( "\x0e" ) );
	EXPECT_EQ_HEX( UNHEX( "F" ), STRING( "\x0f" ) );
	
	EXPECT_EQ_HEX( UNHEX( "a" ), STRING( "\x0a" ) );
	EXPECT_EQ_HEX( UNHEX( "b" ), STRING( "\x0b" ) );
	EXPECT_EQ_HEX( UNHEX( "c" ), STRING( "\x0c" ) );
	EXPECT_EQ_HEX( UNHEX( "d" ), STRING( "\x0d" ) );
	EXPECT_EQ_HEX( UNHEX( "e" ), STRING( "\x0e" ) );
	EXPECT_EQ_HEX( UNHEX( "f" ), STRING( "\x0f" ) );
}

static void alignment()
{
	EXPECT_EQ_HEX( UNHEX_ALIGN( "87654321", 1 ), STRING( "\x87\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN( "87654321", 2 ), STRING( "\x87\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN( "87654321", 4 ), STRING( "\x87\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(  "7654321", 1 ), STRING( "\x07\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(  "7654321", 2 ), STRING( "\x07\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(  "7654321", 4 ), STRING( "\x07\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(   "654321", 1 ), STRING(     "\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(   "654321", 2 ), STRING( "\x00\x65\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(   "654321", 4 ), STRING( "\x00\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(    "54321", 1 ), STRING(     "\x05\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(    "54321", 2 ), STRING( "\x00\x05\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(    "54321", 4 ), STRING( "\x00\x05\x43\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(     "4321", 1 ), STRING(         "\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(     "4321", 2 ), STRING(         "\x43\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(     "4321", 4 ), STRING( "\x00\x00\x43\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(      "321", 1 ), STRING(         "\x03\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(      "321", 2 ), STRING(         "\x03\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(      "321", 4 ), STRING( "\x00\x00\x03\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(       "21", 1 ), STRING(             "\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(       "21", 2 ), STRING(         "\x00\x21" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(       "21", 4 ), STRING( "\x00\x00\x00\x21" ) );
	
	EXPECT_EQ_HEX( UNHEX_ALIGN(        "1", 1 ), STRING(             "\x01" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(        "1", 2 ), STRING(         "\x00\x01" ) );
	EXPECT_EQ_HEX( UNHEX_ALIGN(        "1", 4 ), STRING( "\x00\x00\x00\x01" ) );
}

int main( int argc, char** argv )
{
	tap::start( "hex", n_tests );
	
	zero();
	
	digits();
	
	alignment();
	
	return 0;
}
