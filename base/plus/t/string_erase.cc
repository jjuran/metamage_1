/*
	t/string_erase.cc
	-----------------
*/

// Standard C++
#include <stdexcept>

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 7 + 4;


using tap::ok_if;


static void substring()
{
	plus::string hex_digits = "0123456789abcdef";
	
	plus::var_string a = hex_digits;
	
	const char* a_data = a.begin();  // copy on write
	
	a.erase();
	
	ok_if( a.empty() );
	
	ok_if( a.data() == a_data );
	
	a = hex_digits;
	
	a.erase( 13, 20 );
	
	ok_if( a == "0123456789abc" );
	
	a.erase( 0, 3 );
	
	ok_if( a == "3456789abc" );
	
	a.erase( 4, 3 );
	
	ok_if( a == "3456abc" );
	
	a.erase( 7, 0 );
	
	ok_if( a == "3456abc" );
	
	bool exception_thrown = false;
	
	try
	{
		a.erase( 8, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
}

static void iter_range()
{
	plus::string hex_digits = "0123456789abcdef";
	
	plus::var_string a = hex_digits;
	
	char* begin = a.begin();  // copy on write
	
	a.erase( begin + 13, a.end() );
	
	ok_if( a == "0123456789abc" );
	
	a.erase( begin, begin + 3 );
	
	ok_if( a == "3456789abc" );
	
	a.erase( begin + 4, a.end() - 3 );
	
	ok_if( a == "3456abc" );
	
	a.erase( begin + 7, a.end() );
	
	ok_if( a == "3456abc" );
	
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_erase", n_tests );
	
	substring();
	
	iter_range();
	
	return 0;
}

