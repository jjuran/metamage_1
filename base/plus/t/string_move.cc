/*
	t/string_move.cc
	----------------
*/

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 3;


static void move_to_var_string()
{
	plus::string foo = "0123456789abcdef" "ghijklmnopqrstuv";
	plus::string bar = foo;
	
	EXPECT( foo.data() == bar.data() );
	
	plus::var_string baz = bar.move();
	
	EXPECT( foo.data() != bar.data() );
	EXPECT( foo.data() != baz.data() );  // move to var_string gets tainted
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_move", n_tests );
	
	move_to_var_string();
	
	return 0;
}
