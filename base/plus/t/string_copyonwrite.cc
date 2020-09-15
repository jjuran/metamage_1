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


static void copyonwrite()
{
	plus::var_string a = "0123456789abcdef";
	
	a.reserve( 23 );
	
	plus::var_string b = a;
	
	b += "ghij";  // reallocates due to copy-on-write
	
	EXPECT( b.size() == 20 );
	
	EXPECT( b.capacity() >= b.size() );
	
	/*
		We don't copy on write anymore, so b's initial capacity will be 19,
		and after expansion might be 36.
	*/
	//EXPECT( b.capacity() == 23 );
	
	EXPECT( b == "0123456789abcdefghij" );
	
	plus::var_string c = "it's the end of the world as we know it";
	
	char* c_data = c.begin();
	
	plus::string foo = c;  // must not be shared
	
	*c_data = 'I';
	
	EXPECT( foo[0] == 'i' );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_copyonwrite", n_tests );
	
	copyonwrite();
	
	return 0;
}
