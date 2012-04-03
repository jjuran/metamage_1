/*
	t/string_c_str.cc
	-----------------
*/

// plus
#include "plus/string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 7;


using tap::ok_if;


static void truncation()
{
	plus::string twenty = "01234567890123456789";
	
	plus::string sixteen( twenty, 0, 16 );
	
	ok_if( twenty.data() == sixteen.data() );
	
	ok_if( twenty.c_str() == sixteen.data() );
	
	ok_if( sixteen.size() == 16 );
	
	ok_if( sixteen == "0123456789012345" );
	
	ok_if( twenty.data() == sixteen.data() );
	
	ok_if( sixteen.c_str()[16] == '\0' );
	
	ok_if( twenty.data() != sixteen.data() );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_c_str", n_tests );
	
	truncation();
	
	return 0;
}

