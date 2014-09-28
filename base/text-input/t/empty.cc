/*
	t/empty.cc
	----------
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 4 * 3;


static void empty( const char* newline, std::size_t length )
{
	text_input::feed feed;
	
	const plus::string* zero = feed.get_line();
	
	EXPECT_NULL( zero );
	
	feed.accept_input( newline, length );
	
	const plus::string* one = feed.get_line();
	
	EXPECT( one  &&  *one == "\n" );  // first line is "\n"
	
	feed.accept_input( newline, length );
	
	const plus::string* two = feed.get_line();
	
	EXPECT( two  &&  *two == "\n" );
	
	const plus::string* three = feed.get_line();
	
	EXPECT_NULL( three );
}

int main( int argc, const char *const *argv )
{
	tap::start( "empty", n_tests );
	
	empty( STR_LEN(      "\n" ) );
	empty( STR_LEN( "\r"      ) );
	empty( STR_LEN( "\r" "\n" ) );
	
	return 0;
}
