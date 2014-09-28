/*
	t/newlines.cc
	-------------
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 13;


static void newlines()
{
	text_input::feed feed;
	
	feed.accept_input( STR_LEN( "foo\n\n"
	                            "bar\r\r"
	                            "baz\r\n"
	                            "qux\n\r"
	                            "zee" ) );
	
	EXPECT( *feed.get_line() == "foo\n" );
	EXPECT( *feed.get_line() ==    "\n" );
	EXPECT( *feed.get_line() == "bar\n" );
	EXPECT( *feed.get_line() ==    "\n" );
	EXPECT( *feed.get_line() == "baz\n" );
	EXPECT( *feed.get_line() == "qux\n" );
	EXPECT( *feed.get_line() ==    "\n" );
	
	EXPECT( feed.get_line() == NULL );
	EXPECT( feed.get_line() == NULL );
	
	EXPECT( feed.get_fragment_ref() == "zee" );
	EXPECT( feed.get_fragment_ref() == ""    );
	
	feed.accept_input( STR_LEN( "zig" ) );
	
	const plus::string* fragment = feed.get_fragment();
	
	EXPECT( fragment != NULL  &&  *fragment == "zig" );
	
	EXPECT( feed.get_fragment() == NULL );
}

int main( int argc, const char *const *argv )
{
	tap::start( "newlines", n_tests );
	
	newlines();
	
	return 0;
}
