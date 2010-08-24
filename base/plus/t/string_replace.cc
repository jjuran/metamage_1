/*
	t/string_replace.cc
	-------------------
*/

// Standard C++
#include <stdexcept>
#include <string>

// Standard C
#include <string.h>

// iota
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 10 + 7 + 2 + 3;


using tap::ok_if;


static const plus::string empty;


static void string()
{
	plus::var_string test = "0123456789abcdef";
	
	const plus::string::size_type zero = 0;
	
	test.replace( zero, zero, empty );
	
	ok_if( test == "0123456789abcdef" );
	
	test.replace( 16, 0, empty );
	
	ok_if( test == "0123456789abcdef" );
	
	test.replace( 0, 2, empty );
	
	ok_if( test == "23456789abcdef" );
	
	test.replace( 13, 2, empty );
	
	ok_if( test == "23456789abcde" );
	
	test.replace( 7, 2, empty );
	
	ok_if( test == "2345678bcde" );
	
	const plus::string xyz = "xyz";
	
	test.replace( 3, 3, xyz );
	
	ok_if( test == "234xyz8bcde" );
	
	test.replace( 1, 1, xyz );
	
	ok_if( test == "2xyz4xyz8bcde" );
	
	test.replace( 13, 0, xyz );
	
	ok_if( test == "2xyz4xyz8bcdexyz" );
	
	test.replace( 2, 13, xyz );
	
	ok_if( test == "2xxyzz" );
	
	bool exception_thrown = false;
	
	try
	{
		test.replace( 7, 0, empty );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
}

static void substring()
{
	plus::var_string test = "abcdefghijklmnop";
	
	const plus::string digits = "0123456789";
	
	bool exception_thrown = false;
	
	try
	{
		test.replace( 17, 0, empty, 0, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
	
	exception_thrown = false;
	
	try
	{
		test.replace( 0, 0, empty, 1, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
	
	test.replace( 0, 0, empty, 0, 0 );
	
	ok_if( test == "abcdefghijklmnop" );
	
	test.replace( 0, 1, digits, 0, 1 );
	
	ok_if( test == "0bcdefghijklmnop" );
	
	test.replace( 15, 1, digits, 9, 1 );
	
	ok_if( test == "0bcdefghijklmno9" );
	
	test.replace( 3, 4, digits, 2, 2 );
	
	ok_if( test == "0bc23hijklmno9" );
	
	test.replace( 7, 2, digits, 5, 5 );
	
	ok_if( test == "0bc23hi56789lmno9" );
}

static void fill()
{
	plus::var_string test = "abc123xyz";
	
	bool exception_thrown = false;
	
	try
	{
		test.replace( 10, 0, 0, '\0' );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
	
	test.replace( 3, 3, 5, '.' );
	
	ok_if( test == "abc.....xyz" );
}

static void range()
{
	plus::var_string test = "0123456789";
	
	test.replace( test.begin() + 2, test.end() - 2, STR_LEN( "foo" ) );
	
	ok_if( test == "01foo89" );
	
	test.replace( test.end() - 1, test.end(), STR_LEN( "bar" ) );
	
	ok_if( test == "01foo8bar" );
	
	test.replace( test.end(), test.end(), 3, '.' );
	
	ok_if( test == "01foo8bar..." );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_replace", n_tests );
	
	string();
	
	substring();
	
	fill();
	
	range();
	
	return 0;
}

