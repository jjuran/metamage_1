/*
	t/string_copy.cc
	----------------
*/

// Standard C++
#include <stdexcept>

// Standard C
#include <string.h>

// iota
#include "iota/strings.hh"

// plus
#include "plus/string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 5;


using tap::ok_if;


static void copy()
{
	char buffer[ 8 ];
	
	memset( buffer, '_', sizeof buffer );
	
	plus::string s = "1234567890";
	
	s.copy( buffer, 4, 3 );
	
	ok_if( memcmp( buffer, STR_LEN( "4567____" ) ) == 0 );
	
	s.copy( buffer, 5, 7 );
	
	ok_if( memcmp( buffer, STR_LEN( "8907____" ) ) == 0 );
	
	bool exception_thrown = false;
	
	try
	{
		s.copy( buffer, 8, 10 );
	}
	catch ( ... )
	{
		exception_thrown = true;
	}
	
	ok_if( memcmp( buffer, STR_LEN( "8907____" ) ) == 0 );
	
	ok_if( !exception_thrown );
	
	exception_thrown = false;
	
	try
	{
		s.copy( buffer, 8, 11 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( exception_thrown );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_copy", n_tests );
	
	copy();
	
	return 0;
}

