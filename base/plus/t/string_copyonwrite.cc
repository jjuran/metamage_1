/*
	t/string_copyonwrite.cc
	-----------------------
*/

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 4;


using tap::ok_if;


static void copyonwrite()
{
	plus::var_string a = "0123456789abcdef";
	
	a.reserve( 23 );
	
	plus::var_string b = a;
	
	b += "ghij";  // reallocates due to copy-on-write
	
	ok_if( b.size() == 20 );
	
	ok_if( b.capacity() >= b.size() );
	
	ok_if( b.capacity() == 23 );
	
	ok_if( b == "0123456789abcdefghij" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_copyonwrite", n_tests );
	
	copyonwrite();
	
	return 0;
}

