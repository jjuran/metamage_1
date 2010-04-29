/*
	t/string_find.cc
	----------------
*/

// Standard C++
#include <stdexcept>

// Standard C
//#include <stdio.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// plus
#include "plus/string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 22 + 23 + 12 + 12 + 17 + 11;


using tap::ok_if;


static const plus::string::size_type npos = plus::string::npos;


static void find()
{
	plus::string s = "This is a test.";
	
	ok_if( s.find( 'X'  ) == npos );
	ok_if( s.find( '\0' ) == npos );
	
	ok_if( s.find( ' '       ) == 4    );
	ok_if( s.find( ' ',  4   ) == 4    );
	ok_if( s.find( ' ',  5   ) == 7    );
	ok_if( s.find( ' ',  9   ) == 9    );
	ok_if( s.find( ' ', 10   ) == npos );
	ok_if( s.find( ' ', 20   ) == npos );
	ok_if( s.find( ' ', npos ) == npos );
	
	plus::string is = "is";
	
	ok_if( s.find( is     ) == 2    );
	ok_if( s.find( is,  2 ) == 2    );
	ok_if( s.find( is,  3 ) == 5    );
	ok_if( s.find( is,  5 ) == 5    );
	ok_if( s.find( is,  6 ) == npos );
	ok_if( s.find( is, 20 ) == npos );
	
	ok_if( s.find( "is" ) == 2 );
	
	plus::string dot_null( STR_LEN( ".\0" ) );
	
	ok_if( s.find( dot_null ) == npos );
	
	ok_if( s.find( ""     ) ==  0   );
	ok_if( s.find( "", 15 ) == 15   );
	ok_if( s.find( "", 16 ) == npos );
	
	ok_if( dot_null.find( '\0'    ) == 1    );
	ok_if( dot_null.find( '\0', 2 ) == npos );
}

static void rfind()
{
	plus::string s = "This is a test.";
	
	ok_if( s.rfind( 'X'  ) == npos );
	ok_if( s.rfind( '\0' ) == npos );
	
	ok_if( s.rfind( ' '       ) == 9    );
	ok_if( s.rfind( ' ', npos ) == 9    );
	ok_if( s.rfind( ' ', 20   ) == 9    );
	ok_if( s.rfind( ' ', 10   ) == 9    );
	ok_if( s.rfind( ' ',  9   ) == 9    );
	ok_if( s.rfind( ' ',  8   ) == 7    );
	ok_if( s.rfind( ' ',  4   ) == 4    );
	ok_if( s.rfind( ' ',  3   ) == npos );
	
	plus::string is = "is";
	
	ok_if( s.rfind( is       ) == 5    );
	ok_if( s.rfind( is, npos ) == 5    );
	ok_if( s.rfind( is, 20   ) == 5    );
	ok_if( s.rfind( is,  6   ) == 5    );
	ok_if( s.rfind( is,  5   ) == 5    );
	ok_if( s.rfind( is,  4   ) == 2    );
	ok_if( s.rfind( is,  2   ) == 2    );
	ok_if( s.rfind( is,  1   ) == npos );
	
	ok_if( s.rfind( "is" ) == 5 );
	
	plus::string dot_null( STR_LEN( ".\0" ) );
	
	ok_if( s.rfind( dot_null ) == npos );
	
	ok_if( s.rfind( ""    ) == 15 );
	ok_if( s.rfind( "", 0 ) == 0  );
	
	ok_if( dot_null.rfind( '\0' ) == 1 );
}

static void find_first_of()
{
	plus::string s = "This is a test.";
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	ok_if( s.find_first_of( ""     ) == npos );
	ok_if( s.find_first_of( "xyz"  ) == npos );
	ok_if( s.find_first_of( x_null ) == npos );
	
	ok_if( s.find_first_of( "test"       ) ==  3   );
	ok_if( s.find_first_of( "test",  3   ) ==  3   );
	ok_if( s.find_first_of( "test",  4   ) ==  6   );
	ok_if( s.find_first_of( "test", 13   ) == 13   );
	ok_if( s.find_first_of( "test", 14   ) == npos );
	ok_if( s.find_first_of( "test", 20   ) == npos );
	ok_if( s.find_first_of( "test", npos ) == npos );
	
	ok_if( x_null.find_first_of( '\0'    ) == 1    );
	ok_if( x_null.find_first_of( '\0', 2 ) == npos );
}

static void find_last_of()
{
	plus::string s = "This is a test.";
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	ok_if( s.find_last_of( ""     ) == npos );
	ok_if( s.find_last_of( "xyz"  ) == npos );
	ok_if( s.find_last_of( x_null ) == npos );
	
	ok_if( s.find_last_of( "is a"       ) == 12   );
	ok_if( s.find_last_of( "is a", npos ) == 12   );
	ok_if( s.find_last_of( "is a", 20   ) == 12   );
	ok_if( s.find_last_of( "is a", 12   ) == 12   );
	ok_if( s.find_last_of( "is a", 11   ) ==  9   );
	ok_if( s.find_last_of( "is a",  8   ) ==  8   );
	ok_if( s.find_last_of( "is a",  2   ) ==  2   );
	ok_if( s.find_last_of( "is a",  1   ) == npos );
	
	ok_if( x_null.find_last_of( '\0' ) == 1 );
}

static void find_first_not_of()
{
	plus::string s = "This is a test.";
	
	ok_if( s.find_first_not_of( s ) == npos );
	
	ok_if( s.find_first_not_of( "xyz"        ) ==  0   );
	ok_if( s.find_first_not_of( "This"       ) ==  4   );
	ok_if( s.find_first_not_of( "This "      ) ==  8   );
	ok_if( s.find_first_not_of( "This ", 8   ) ==  8   );
	ok_if( s.find_first_not_of( "This ", 9   ) == 10   );
	ok_if( s.find_first_not_of( ".set", 10   ) == npos );
	ok_if( s.find_first_not_of( "xyz",  15   ) == npos );
	ok_if( s.find_first_not_of( "xyz",  20   ) == npos );
	ok_if( s.find_first_not_of( "xyz",  npos ) == npos );
	
	ok_if( s.find_first_not_of( ""     ) ==  0   );
	ok_if( s.find_first_not_of( "", 14 ) == 14   );
	ok_if( s.find_first_not_of( "", 15 ) == npos );
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	ok_if( x_null.find_first_not_of( '\0'    ) == 0    );
	ok_if( x_null.find_first_not_of( 'x'     ) == 1    );
	ok_if( x_null.find_first_not_of( '\0', 1 ) == npos );
	ok_if( x_null.find_first_not_of( 'x',  2 ) == npos );
}

static void find_last_not_of()
{
	plus::string s = "This is a test.";
	
	ok_if( s.find_last_not_of( s ) == npos );
	
	ok_if( s.find_last_not_of( "xyz"          ) == 14   );
	ok_if( s.find_last_not_of( "a set."       ) ==  5   );
	ok_if( s.find_last_not_of( "a set.", npos ) ==  5   );
	ok_if( s.find_last_not_of( "a set.", 20   ) ==  5   );
	ok_if( s.find_last_not_of( "a set.",  5   ) ==  5   );
	ok_if( s.find_last_not_of( "a set.",  4   ) ==  2   );
	ok_if( s.find_last_not_of( "This",    4   ) ==  4   );
	ok_if( s.find_last_not_of( "This",    3   ) == npos );

	plus::string x_null( STR_LEN( "x\0" ) );
	
	//fprintf( stderr, "%d\n", x_null.find_last_not_of( '\0' ) );
	
	ok_if( x_null.find_last_not_of( '\0' ) == 0 );
	ok_if( x_null.find_last_not_of( 'x'  ) == 1 );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_find", n_tests );
	
	find();
	
	rfind();
	
	find_first_of();
	
	find_last_of();
	
	find_first_not_of();
	
	find_last_not_of();
	
	return 0;
}

