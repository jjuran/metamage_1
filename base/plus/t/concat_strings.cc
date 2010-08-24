/*
	t/concat_strings.cc
	-------------------
*/

// iota
#include "iota/strings.hh"

// plus
#include "plus/concat.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 4;


using tap::ok_if;


static void concat()
{
	ok_if( plus::concat( STR_LEN( ""    ), STR_LEN( ""    ) ) == ""       );
	ok_if( plus::concat( STR_LEN( "foo" ), STR_LEN( ""    ) ) == "foo"    );
	ok_if( plus::concat( STR_LEN( ""    ), STR_LEN( "bar" ) ) == "bar"    );
	ok_if( plus::concat( STR_LEN( "foo" ), STR_LEN( "bar" ) ) == "foobar" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_copy", n_tests );
	
	concat();
	
	return 0;
}

