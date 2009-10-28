/*
	t/exceptions.cc
	---------------
	
	Copyright 2009, Joshua Juran
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 2;


using tap::ok_if;


static void buffer_exceptions()
{
	text_input::feed feed;
	
	bool buffer_overrun = false;
	
	try
	{
		char buffer[ text_input::feed::buffer_length + 1 ] = { 0 };
		
		feed.accept_input( buffer, sizeof buffer );
	}
	catch ( const text_input::feed::buffer_overrun& )
	{
		buffer_overrun = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( buffer_overrun );
	
	feed.accept_input( STR_LEN( "\n" ) );
	
	bool buffer_occupied = false;
	
	try
	{
		feed.accept_input( STR_LEN( "\n" ) );
	}
	catch ( const text_input::feed::buffer_occupied& )
	{
		buffer_occupied = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( buffer_occupied );
}

int main( int argc, const char *const *argv )
{
	tap::start( "exceptions", n_tests );
	
	buffer_exceptions();
	
	return 0;
}

