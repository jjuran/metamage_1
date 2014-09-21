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
	plus::string fifty = "01234567890123456789012345678901234567890123456789";
	
	plus::string forty( fifty, 0, 40 );
	
	EXPECT( fifty.data() == forty.data() );
	
	EXPECT( fifty.c_str() == forty.data() );
	
	EXPECT( forty.size() == 40 );
	
	EXPECT( forty == "0123456789012345678901234567890123456789" );
	
	EXPECT( fifty.data() == forty.data() );
	
	EXPECT( forty.c_str()[40] == '\0' );
	
	EXPECT( fifty.data() != forty.data() );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_c_str", n_tests );
	
	truncation();
	
	return 0;
}
