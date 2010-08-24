/*
	t/string_insert.cc
	------------------
*/

// Standard C++
#include <stdexcept>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 6;


using tap::ok_if;


static void insert()
{
	plus::var_string test;
	
	test.insert( 0, "" );
	
	ok_if( test == "" );
	
	bool exception_thrown = false;
	
	try
	{
		test.insert( 1, "" );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
	
	test.insert( 0, "bar" );
	
	ok_if( test == "bar" );
	
	test.insert( 3, "baz" );
	
	ok_if( test == "barbaz" );
	
	test.insert( 0, "foo" );
	
	ok_if( test == "foobarbaz" );
	
	test.insert( test.begin() + 6, ' ' );
	
	ok_if( test == "foobar baz" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_insert", n_tests );
	
	insert();
	
	return 0;
}

