/*
	t/string_c_str.cc
	-----------------
*/

// plus
#include "plus/string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 7;


static void truncation()
{
	plus::string twenty = "01234567890123456789";
	
	plus::string sixteen( twenty, 0, 16 );
	
	EXPECT( twenty.data() == sixteen.data() );
	
	EXPECT( twenty.c_str() == sixteen.data() );
	
	EXPECT( sixteen.size() == 16 );
	
	EXPECT( sixteen == "0123456789012345" );
	
	EXPECT( twenty.data() == sixteen.data() );
	
	EXPECT( sixteen.c_str()[16] == '\0' );
	
	EXPECT( twenty.data() != sixteen.data() );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_c_str", n_tests );
	
	truncation();
	
	return 0;
}
