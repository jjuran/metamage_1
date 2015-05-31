/*
	hex.cc
	------
*/

// plus
#include "plus/hexadecimal.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 6 + 21 + 24;


#define EXPECT_EQ_HEX( a, b )  EXPECT_CMP( a.data(), a.size(), b.data(), b.size() )

#define STRING( s )  ::plus::string( STR_LEN( s ) )


static void zero()
{
	EXPECT_EQ_HEX( plus::unhex( "" ), plus::string() );
	EXPECT_EQ_HEX( plus::unhex( "0" ), plus::string( 1, '\0' ) );
	EXPECT_EQ_HEX( plus::unhex( "00" ), plus::string( 1, '\0' ) );
	EXPECT_EQ_HEX( plus::unhex( "000" ), plus::string( 2, '\0' ) );
	EXPECT_EQ_HEX( plus::unhex( "0000" ), plus::string( 2, '\0' ) );
	EXPECT_EQ_HEX( plus::unhex( "00000" ), plus::string( 3, '\0' ) );
}

static void digits()
{
	EXPECT_EQ_HEX( plus::unhex( "1" ), STRING( "\x01" ) );
	EXPECT_EQ_HEX( plus::unhex( "2" ), STRING( "\x02" ) );
	EXPECT_EQ_HEX( plus::unhex( "3" ), STRING( "\x03" ) );
	EXPECT_EQ_HEX( plus::unhex( "4" ), STRING( "\x04" ) );
	EXPECT_EQ_HEX( plus::unhex( "5" ), STRING( "\x05" ) );
	EXPECT_EQ_HEX( plus::unhex( "6" ), STRING( "\x06" ) );
	EXPECT_EQ_HEX( plus::unhex( "7" ), STRING( "\x07" ) );
	EXPECT_EQ_HEX( plus::unhex( "8" ), STRING( "\x08" ) );
	EXPECT_EQ_HEX( plus::unhex( "9" ), STRING( "\x09" ) );
	
	EXPECT_EQ_HEX( plus::unhex( "A" ), STRING( "\x0a" ) );
	EXPECT_EQ_HEX( plus::unhex( "B" ), STRING( "\x0b" ) );
	EXPECT_EQ_HEX( plus::unhex( "C" ), STRING( "\x0c" ) );
	EXPECT_EQ_HEX( plus::unhex( "D" ), STRING( "\x0d" ) );
	EXPECT_EQ_HEX( plus::unhex( "E" ), STRING( "\x0e" ) );
	EXPECT_EQ_HEX( plus::unhex( "F" ), STRING( "\x0f" ) );
	
	EXPECT_EQ_HEX( plus::unhex( "a" ), STRING( "\x0a" ) );
	EXPECT_EQ_HEX( plus::unhex( "b" ), STRING( "\x0b" ) );
	EXPECT_EQ_HEX( plus::unhex( "c" ), STRING( "\x0c" ) );
	EXPECT_EQ_HEX( plus::unhex( "d" ), STRING( "\x0d" ) );
	EXPECT_EQ_HEX( plus::unhex( "e" ), STRING( "\x0e" ) );
	EXPECT_EQ_HEX( plus::unhex( "f" ), STRING( "\x0f" ) );
}

static void alignment()
{
	EXPECT_EQ_HEX( plus::unhex( "87654321", 1 ), STRING( "\x87\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex( "87654321", 2 ), STRING( "\x87\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex( "87654321", 4 ), STRING( "\x87\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(  "7654321", 1 ), STRING( "\x07\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(  "7654321", 2 ), STRING( "\x07\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(  "7654321", 4 ), STRING( "\x07\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(   "654321", 1 ), STRING(     "\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(   "654321", 2 ), STRING( "\x00\x65\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(   "654321", 4 ), STRING( "\x00\x65\x43\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(    "54321", 1 ), STRING(     "\x05\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(    "54321", 2 ), STRING( "\x00\x05\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(    "54321", 4 ), STRING( "\x00\x05\x43\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(     "4321", 1 ), STRING(         "\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(     "4321", 2 ), STRING(         "\x43\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(     "4321", 4 ), STRING( "\x00\x00\x43\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(      "321", 1 ), STRING(         "\x03\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(      "321", 2 ), STRING(         "\x03\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(      "321", 4 ), STRING( "\x00\x00\x03\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(       "21", 1 ), STRING(             "\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(       "21", 2 ), STRING(         "\x00\x21" ) );
	EXPECT_EQ_HEX( plus::unhex(       "21", 4 ), STRING( "\x00\x00\x00\x21" ) );
	
	EXPECT_EQ_HEX( plus::unhex(        "1", 1 ), STRING(             "\x01" ) );
	EXPECT_EQ_HEX( plus::unhex(        "1", 2 ), STRING(         "\x00\x01" ) );
	EXPECT_EQ_HEX( plus::unhex(        "1", 4 ), STRING( "\x00\x00\x00\x01" ) );
}

int main( int argc, char** argv )
{
	tap::start( "hex", n_tests );
	
	zero();
	
	digits();
	
	alignment();
	
	return 0;
}
