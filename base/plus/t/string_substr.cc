/*
	t/string_substr.cc
	------------------
*/

// Standard C++
#include <stdexcept>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 10;


using tap::ok_if;


static void substr()
{
	plus::string empty;
	
	ok_if( empty.substr(      ).empty() );
	ok_if( empty.substr( 0, 0 ).empty() );
	
	plus::string foobar = "foobar";
	
	ok_if( foobar.substr(      ) == foobar );
	ok_if( foobar.substr( 0, 6 ) == foobar );
	ok_if( foobar.substr( 0, 8 ) == foobar );
	
	ok_if( foobar.substr( 2, 3 ) == "oba"  );
	ok_if( foobar.substr( 2, 5 ) == "obar" );
	
	ok_if( foobar.substr( 0, 0 ).empty() );
	ok_if( foobar.substr( 6, 0 ).empty() );
	
	bool exception_thrown = false;
	
	try
	{
		foobar.substr( 7, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_substr", n_tests );
	
	substr();
	
	return 0;
}

