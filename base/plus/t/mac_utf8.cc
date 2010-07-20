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


using tap::ok_if;


static void utf8_from_mac()
{
	plus::string empty;
	
	plus::string utf8 = plus::utf8_from_mac( empty );
	
	ok_if( utf8.empty() );
	
	plus::string mac = "long enough to be allocated";
	
	utf8 = plus::utf8_from_mac( mac );
	
	ok_if( mac.data() == utf8.data() );  // should be shared
	
	utf8 = plus::utf8_from_mac( "\xA4" );  // section
	
	ok_if( utf8 == "\xC2\xA7" );
	
	utf8 = plus::utf8_from_mac( "\xA5" );  // bullet
	
	ok_if( utf8 == "\xE2\x80\xA2" );
}

static void mac_from_utf8()
{
	plus::string empty;
	
	plus::string mac = plus::mac_from_utf8( empty );
	
	ok_if( mac.empty() );
	
	plus::string utf8 = "long enough to be allocated";
	
	mac = plus::mac_from_utf8( utf8 );
	
	ok_if( mac.data() == utf8.data() );  // should be shared
	
	mac = plus::mac_from_utf8( "\xC2\xA7" );  // section
	
	ok_if( mac == "\xA4" );
	
	mac = plus::mac_from_utf8( "\xE2\x80\xA2" );  // bullet
	
	ok_if( mac == "\xA5" );
}

int main( int argc, char** argv )
{
	tap::start( "mac_utf8", n_tests );
	
	utf8_from_mac();
	
	mac_from_utf8();
	
	return 0;
}

