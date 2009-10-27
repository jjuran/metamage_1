/*
	t/newlines.cc
	-------------
	
	Copyright 2009, Joshua Juran
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 11;


using tap::ok_if;


static void newlines()
{
	text_input::feed feed;
	
	feed.accept_input( STR_LEN( "foo\n\n"
	                            "bar\r\r"
	                            "baz\r\n"
	                            "qux\n\r"
	                            "zee" ) );
	
	ok_if( feed.get_line_ref() == "foo\n" );
	ok_if( feed.get_line_ref() ==    "\n" );
	ok_if( feed.get_line_ref() == "bar\n" );
	ok_if( feed.get_line_ref() ==    "\n" );
	ok_if( feed.get_line_ref() == "baz\n" );
	ok_if( feed.get_line_ref() == "qux\n" );
	ok_if( feed.get_line_ref() ==    "\n" );
	ok_if( feed.get_line_ref() == ""      );
	ok_if( feed.get_line_ref() == ""      );
	
	ok_if( feed.get_fragment_ref() == "zee" );
	ok_if( feed.get_fragment_ref() == ""    );
}

int main( int argc, const char *const *argv )
{
	tap::start( "newlines", n_tests );
	
	newlines();
	
	return 0;
}

