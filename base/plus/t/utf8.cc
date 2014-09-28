/*
	utf8.cc
	-------
*/

// Standard C
#include <string.h>

// chars
#include "encoding/utf8.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 19;


static bool utf8_is_convertible( const char* begin )
{
	using chars::unichar_t;
	
	size_t n = strlen( begin );
	
	const char* end = begin + n;
	
	const unichar_t uc = chars::get_next_code_point_from_utf8( begin, end );
	
	return uc != unichar_t( -1 );
}

static void convertibility()
{
	EXPECT( utf8_is_convertible( "\x7F" ) );
	
	EXPECT( utf8_is_convertible( "\xC2\x80" ) );
	
	EXPECT( utf8_is_convertible( "\xE2\x80\x80" ) );
	
	EXPECT( utf8_is_convertible( "\xF2\x80\x80\x80" ) );
	
	// overlong encodings
	
	EXPECT( !utf8_is_convertible( "\xC0\x80" ) );
	
	EXPECT( !utf8_is_convertible( "\xE0\x80\x80" ) );
	
	EXPECT( !utf8_is_convertible( "\xF0\x80\x80\x80" ) );
	
	// leading continuation byte
	
	EXPECT( !utf8_is_convertible( "\x80" ) );
	
	// missing continuation bytes
	
	EXPECT( !utf8_is_convertible( "\xC2"     ) );
	EXPECT( !utf8_is_convertible( "\xC2\x00" ) );
	
	EXPECT( !utf8_is_convertible( "\xE2"         ) );
	EXPECT( !utf8_is_convertible( "\xE2\x00"     ) );
	EXPECT( !utf8_is_convertible( "\xE2\x80\x00" ) );
	
	EXPECT( !utf8_is_convertible( "\xF2"             ) );
	EXPECT( !utf8_is_convertible( "\xF2\x00"         ) );
	EXPECT( !utf8_is_convertible( "\xF2\x80\x00"     ) );
	EXPECT( !utf8_is_convertible( "\xF2\x80\x80\x00" ) );
	
	// surrogates
	
	EXPECT( !utf8_is_convertible( "\xED\xA0\x80" ) );  // 0xD800
	EXPECT( !utf8_is_convertible( "\xED\xB0\x80" ) );  // 0xDC00
	
}

int main( int argc, char** argv )
{
	tap::start( "utf8", n_tests );
	
	convertibility();
	
	return 0;
}
