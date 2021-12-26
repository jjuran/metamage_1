/*
	t/string_alloc.cc
	-----------------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 3 + 2 + 2 + 5;


#define LARGE_STRING  "0123456789abcdef" "ghijklmnopqrstuv"


static void large_copy()
{
	const char* digits = LARGE_STRING;
	
	plus::string a = digits;
	
	EXPECT( a.data() != digits );
	
	plus::string b = a;
	
	EXPECT( a == b );
	
	EXPECT( a.data() == b.data() );  // equal iff copies are shared
}

static void handoff()
{
	char* buffer = (char*) malloc( sizeof LARGE_STRING );
	
	if ( buffer == NULL )
	{
		abort();
	}
	
	memcpy( buffer, LARGE_STRING, sizeof LARGE_STRING );
	
	plus::string a( buffer, sizeof LARGE_STRING - 1, vxo::delete_free );
	
	EXPECT( a.data() == buffer );
	
	plus::string b = a;
	
	EXPECT( a.data() != b.data() );
}

static void static_nocopy()
{
	const char* digits = LARGE_STRING;
	
	plus::string a( digits, sizeof LARGE_STRING - 1, vxo::delete_never );
	
	EXPECT( a.data() == digits );
	
	plus::string b = a;
	
	EXPECT( a.data() == b.data() );
}

static void static_varcopy()
{
	const char* digits = LARGE_STRING;
	
	plus::string a( digits, sizeof LARGE_STRING - 1, vxo::delete_never );
	
	EXPECT( a.data() == digits );
	
	plus::var_string b = a;
	
	EXPECT( a.data() != b.data() );  // copy now, no COW
	
	plus::var_string c = b;
	
	const char* c_data = c.data();
	
	EXPECT( b.data() != c.data() );
	
	plus::string d = c;
	
	EXPECT( c.data() != d.data() );  // var_strings aren't shared
	
	c.begin();
	
	EXPECT( c_data == c.data() );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_alloc", n_tests );
	
	large_copy();
	
	handoff();
	
	static_nocopy();
	
	static_varcopy();
	
	return 0;
}
