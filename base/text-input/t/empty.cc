/*
	t/empty.cc
	----------
	
	Copyright 2009, Joshua Juran
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 16 * 3;


using tap::ok_if;
using tap::ok_if_null;


static void empty( const char* newline, std::size_t length )
{
	text_input::feed feed;
	
	ok_if( feed.buffer_empty() );
	
	ok_if( feed.empty() );
	
	ok_if( !feed.has_complete_line() );
	
	feed.accept_input( newline, length );
	
	ok_if( feed.buffer_empty(), "buffer empty after single blank line" );
	
	ok_if( !feed.empty(), "not empty after single blank line" );
	
	ok_if( feed.has_complete_line(), "has complete line after single blank line" );
	
	feed.accept_input( newline, length );
	
	ok_if( !feed.buffer_empty(), "buffer not empty after second blank line" );
	
	const plus::string& one = feed.get_line_ref();
	
	ok_if( one == "\n", "first line is \\n" );
	
	ok_if( feed.buffer_empty(), "buffer empty after reading once" );
	
	ok_if( !feed.empty() );
	
	ok_if( feed.has_complete_line() );
	
	const plus::string& two = feed.get_line_ref();
	
	ok_if( two == "\n" );
	
	ok_if( feed.buffer_empty() );
	
	ok_if( feed.empty() );
	
	ok_if( !feed.has_complete_line() );
	
	const plus::string* three = feed.get_line();
	
	ok_if_null( three );
}

int main( int argc, const char *const *argv )
{
	tap::start( "empty", n_tests );
	
	empty( STR_LEN(      "\n" ) );
	empty( STR_LEN( "\r"      ) );
	empty( STR_LEN( "\r" "\n" ) );
	
	return 0;
}

