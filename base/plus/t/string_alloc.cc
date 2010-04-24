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


static const unsigned n_tests = 3 + 2 + 2 + 2 + 5;


using tap::ok_if;


static void large_copy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a = digits;
	
	ok_if( a.data() != digits );
	
	plus::string b = a;
	
	ok_if( a == b );
	
	ok_if( a.data() == b.data() );  // equal iff copies are shared
}

static void handoff()
{
	char* buffer = (char*) ::operator new( sizeof "0123456789abcdef" );
	
	memcpy( buffer, "0123456789abcdef", sizeof "0123456789abcdef" );
	
	plus::string a( buffer, sizeof "0123456789abcdef" - 1, plus::delete_basic );
	
	ok_if( a.data() == buffer );
	
	plus::string b = a;
	
	ok_if( a.data() != b.data() );
}

static void stack_copy()
{
	const char digits[] = "0123456789abcdef";
	
	plus::string a( digits, sizeof "0123456789abcdef" - 1, plus::delete_none );
	
	ok_if( a.data() == digits );
	
	plus::string b = a;
	
	ok_if( a.data() != b.data() );
}

static void static_nocopy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a( digits, sizeof "0123456789abcdef" - 1, plus::delete_never );
	
	ok_if( a.data() == digits );
	
	plus::string b = a;
	
	ok_if( a.data() == b.data() );
}

static void static_varcopy()
{
	const char* digits = "0123456789abcdef";
	
	plus::string a( digits, sizeof "0123456789abcdef" - 1, plus::delete_never );
	
	ok_if( a.data() == digits );
	
	plus::var_string b = a;
	
	ok_if( a.data() == b.data() );
	
	plus::var_string c = b;
	
	const char* c_data = c.data();
	
	ok_if( b.data() == c.data() );
	
	plus::string d = c;
	
	ok_if( c.data() == d.data() );
	
	c.begin();
	
	ok_if( c_data != c.data() );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_alloc", n_tests );
	
	large_copy();
	
	handoff();
	
	stack_copy();
	
	static_nocopy();
	
	static_varcopy();
	
	return 0;
}

