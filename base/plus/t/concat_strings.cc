/*
	t/concat_strings.cc
	-------------------
*/

// iota
#include "iota/strings.hh"

// plus
#include "plus/string/concat.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 4;


static void concat()
{
	EXPECT( plus::concat( STR_LEN( ""    ), STR_LEN( ""    ) ) == ""       );
	EXPECT( plus::concat( STR_LEN( "foo" ), STR_LEN( ""    ) ) == "foo"    );
	EXPECT( plus::concat( STR_LEN( ""    ), STR_LEN( "bar" ) ) == "bar"    );
	EXPECT( plus::concat( STR_LEN( "foo" ), STR_LEN( "bar" ) ) == "foobar" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_copy", n_tests );
	
	concat();
	
	return 0;
}
