/*
	t/string_alloc.cc
	-----------------
*/

// Standard C
#include <string.h>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 3 + 2 + 2 + 5;


static void large_copy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a = digits;
	
	EXPECT( a.data() != digits );
	
	plus::string b = a;
	
	EXPECT( a == b );
	
	EXPECT( a.data() == b.data() );  // equal iff copies are shared
}

static void handoff()
{
	char* buffer = (char*) ::operator new( sizeof "0123456789abcdef" );
	
	memcpy( buffer, "0123456789abcdef", sizeof "0123456789abcdef" );
	
	plus::string a( buffer, sizeof "0123456789abcdef" - 1, plus::delete_basic );
	
	EXPECT( a.data() == buffer );
	
	plus::string b = a;
	
	EXPECT( a.data() != b.data() );
}

static void static_nocopy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a( digits, sizeof "0123456789abcdef" - 1, plus::delete_never );
	
	EXPECT( a.data() == digits );
	
	plus::string b = a;
	
	EXPECT( a.data() == b.data() );
}

static void static_varcopy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a( digits, sizeof "0123456789abcdef" - 1, plus::delete_never );
	
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
