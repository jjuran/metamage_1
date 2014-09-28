/*
	mac_utf8.cc
	-----------
*/

// plus
#include "plus/mac_utf8.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 4 + 4;


static void utf8_from_mac()
{
	plus::string empty;
	
	plus::string utf8 = plus::utf8_from_mac( empty );
	
	EXPECT( utf8.empty() );
	
	plus::string mac = "long enough to be allocated, even on 64-bit";
	
	utf8 = plus::utf8_from_mac( mac );
	
	EXPECT( mac.data() == utf8.data() );  // should be shared
	
	utf8 = plus::utf8_from_mac( "\xA4" );  // section
	
	EXPECT( utf8 == "\xC2\xA7" );
	
	utf8 = plus::utf8_from_mac( "\xA5" );  // bullet
	
	EXPECT( utf8 == "\xE2\x80\xA2" );
}

static void mac_from_utf8()
{
	plus::string empty;
	
	plus::string mac = plus::mac_from_utf8( empty );
	
	EXPECT( mac.empty() );
	
	plus::string utf8 = "long enough to be allocated, even on 64-bit";
	
	mac = plus::mac_from_utf8( utf8 );
	
	EXPECT( mac.data() == utf8.data() );  // should be shared
	
	mac = plus::mac_from_utf8( "\xC2\xA7" );  // section
	
	EXPECT( mac == "\xA4" );
	
	mac = plus::mac_from_utf8( "\xE2\x80\xA2" );  // bullet
	
	EXPECT( mac == "\xA5" );
}

int main( int argc, char** argv )
{
	tap::start( "mac_utf8", n_tests );
	
	utf8_from_mac();
	
	mac_from_utf8();
	
	return 0;
}

