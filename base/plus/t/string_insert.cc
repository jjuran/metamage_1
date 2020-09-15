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


static void insert()
{
	plus::var_string test;
	
	test.insert( 0, "" );
	
	EXPECT( test == "" );
	
	bool exception_thrown = false;
	
	try
	{
		test.insert( 1, "" );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	EXPECT( exception_thrown );
	
	test.insert( 0, "bar" );
	
	EXPECT( test == "bar" );
	
	test.insert( 3, "baz" );
	
	EXPECT( test == "barbaz" );
	
	test.insert( 0, "foo" );
	
	EXPECT( test == "foobarbaz" );
	
	test.insert( test.begin() + 6, ' ' );
	
	EXPECT( test == "foobar baz" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_insert", n_tests );
	
	insert();
	
	return 0;
}
