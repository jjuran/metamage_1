/*
	t/exceptions.cc
	---------------
*/

// iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 2;


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
	
	EXPECT( buffer_overrun );
	
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
	
	EXPECT( buffer_occupied );
}

int main( int argc, const char *const *argv )
{
	tap::start( "exceptions", n_tests );
	
	buffer_exceptions();
	
	return 0;
}
