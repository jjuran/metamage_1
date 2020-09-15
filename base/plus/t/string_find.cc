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


static const plus::string::size_type npos = plus::string::npos;


static void find()
{
	plus::string s = "This is a test.";
	
	EXPECT( s.find( 'X'  ) == npos );
	EXPECT( s.find( '\0' ) == npos );
	
	EXPECT( s.find( ' '       ) == 4    );
	EXPECT( s.find( ' ',  4   ) == 4    );
	EXPECT( s.find( ' ',  5   ) == 7    );
	EXPECT( s.find( ' ',  9   ) == 9    );
	EXPECT( s.find( ' ', 10   ) == npos );
	EXPECT( s.find( ' ', 20   ) == npos );
	EXPECT( s.find( ' ', npos ) == npos );
	
	plus::string is = "is";
	
	EXPECT( s.find( is     ) == 2    );
	EXPECT( s.find( is,  2 ) == 2    );
	EXPECT( s.find( is,  3 ) == 5    );
	EXPECT( s.find( is,  5 ) == 5    );
	EXPECT( s.find( is,  6 ) == npos );
	EXPECT( s.find( is, 20 ) == npos );
	
	EXPECT( s.find( "is" ) == 2 );
	
	plus::string dot_null( STR_LEN( ".\0" ) );
	
	EXPECT( s.find( dot_null ) == npos );
	
	EXPECT( s.find( ""     ) ==  0   );
	EXPECT( s.find( "", 15 ) == 15   );
	EXPECT( s.find( "", 16 ) == npos );
	
	EXPECT( dot_null.find( '\0'    ) == 1    );
	EXPECT( dot_null.find( '\0', 2 ) == npos );
}

static void rfind()
{
	plus::string s = "This is a test.";
	
	EXPECT( s.rfind( 'X'  ) == npos );
	EXPECT( s.rfind( '\0' ) == npos );
	
	EXPECT( s.rfind( ' '       ) == 9    );
	EXPECT( s.rfind( ' ', npos ) == 9    );
	EXPECT( s.rfind( ' ', 20   ) == 9    );
	EXPECT( s.rfind( ' ', 10   ) == 9    );
	EXPECT( s.rfind( ' ',  9   ) == 9    );
	EXPECT( s.rfind( ' ',  8   ) == 7    );
	EXPECT( s.rfind( ' ',  4   ) == 4    );
	EXPECT( s.rfind( ' ',  3   ) == npos );
	
	plus::string is = "is";
	
	EXPECT( s.rfind( is       ) == 5    );
	EXPECT( s.rfind( is, npos ) == 5    );
	EXPECT( s.rfind( is, 20   ) == 5    );
	EXPECT( s.rfind( is,  6   ) == 5    );
	EXPECT( s.rfind( is,  5   ) == 5    );
	EXPECT( s.rfind( is,  4   ) == 2    );
	EXPECT( s.rfind( is,  2   ) == 2    );
	EXPECT( s.rfind( is,  1   ) == npos );
	
	EXPECT( s.rfind( "is" ) == 5 );
	
	plus::string dot_null( STR_LEN( ".\0" ) );
	
	EXPECT( s.rfind( dot_null ) == npos );
	
	EXPECT( s.rfind( ""    ) == 15 );
	EXPECT( s.rfind( "", 0 ) == 0  );
	
	EXPECT( dot_null.rfind( '\0' ) == 1 );
}

static void find_first_of()
{
	plus::string s = "This is a test.";
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	EXPECT( s.find_first_of( ""     ) == npos );
	EXPECT( s.find_first_of( "xyz"  ) == npos );
	EXPECT( s.find_first_of( x_null ) == npos );
	
	EXPECT( s.find_first_of( "test"       ) ==  3   );
	EXPECT( s.find_first_of( "test",  3   ) ==  3   );
	EXPECT( s.find_first_of( "test",  4   ) ==  6   );
	EXPECT( s.find_first_of( "test", 13   ) == 13   );
	EXPECT( s.find_first_of( "test", 14   ) == npos );
	EXPECT( s.find_first_of( "test", 20   ) == npos );
	EXPECT( s.find_first_of( "test", npos ) == npos );
	
	EXPECT( x_null.find_first_of( '\0'    ) == 1    );
	EXPECT( x_null.find_first_of( '\0', 2 ) == npos );
}

static void find_last_of()
{
	plus::string s = "This is a test.";
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	EXPECT( s.find_last_of( ""     ) == npos );
	EXPECT( s.find_last_of( "xyz"  ) == npos );
	EXPECT( s.find_last_of( x_null ) == npos );
	
	EXPECT( s.find_last_of( "is a"       ) == 12   );
	EXPECT( s.find_last_of( "is a", npos ) == 12   );
	EXPECT( s.find_last_of( "is a", 20   ) == 12   );
	EXPECT( s.find_last_of( "is a", 12   ) == 12   );
	EXPECT( s.find_last_of( "is a", 11   ) ==  9   );
	EXPECT( s.find_last_of( "is a",  8   ) ==  8   );
	EXPECT( s.find_last_of( "is a",  2   ) ==  2   );
	EXPECT( s.find_last_of( "is a",  1   ) == npos );
	
	EXPECT( x_null.find_last_of( '\0' ) == 1 );
}

static void find_first_not_of()
{
	plus::string s = "This is a test.";
	
	EXPECT( s.find_first_not_of( s ) == npos );
	
	EXPECT( s.find_first_not_of( "xyz"        ) ==  0   );
	EXPECT( s.find_first_not_of( "This"       ) ==  4   );
	EXPECT( s.find_first_not_of( "This "      ) ==  8   );
	EXPECT( s.find_first_not_of( "This ", 8   ) ==  8   );
	EXPECT( s.find_first_not_of( "This ", 9   ) == 10   );
	EXPECT( s.find_first_not_of( ".set", 10   ) == npos );
	EXPECT( s.find_first_not_of( "xyz",  15   ) == npos );
	EXPECT( s.find_first_not_of( "xyz",  20   ) == npos );
	EXPECT( s.find_first_not_of( "xyz",  npos ) == npos );
	
	EXPECT( s.find_first_not_of( ""     ) ==  0   );
	EXPECT( s.find_first_not_of( "", 14 ) == 14   );
	EXPECT( s.find_first_not_of( "", 15 ) == npos );
	
	plus::string x_null( STR_LEN( "x\0" ) );
	
	EXPECT( x_null.find_first_not_of( '\0'    ) == 0    );
	EXPECT( x_null.find_first_not_of( 'x'     ) == 1    );
	EXPECT( x_null.find_first_not_of( '\0', 1 ) == npos );
	EXPECT( x_null.find_first_not_of( 'x',  2 ) == npos );
}

static void find_last_not_of()
{
	plus::string s = "This is a test.";
	
	EXPECT( s.find_last_not_of( s ) == npos );
	
	EXPECT( s.find_last_not_of( "xyz"          ) == 14   );
	EXPECT( s.find_last_not_of( "a set."       ) ==  5   );
	EXPECT( s.find_last_not_of( "a set.", npos ) ==  5   );
	EXPECT( s.find_last_not_of( "a set.", 20   ) ==  5   );
	EXPECT( s.find_last_not_of( "a set.",  5   ) ==  5   );
	EXPECT( s.find_last_not_of( "a set.",  4   ) ==  2   );
	EXPECT( s.find_last_not_of( "This",    4   ) ==  4   );
	EXPECT( s.find_last_not_of( "This",    3   ) == npos );

	plus::string x_null( STR_LEN( "x\0" ) );
	
	//fprintf( stderr, "%d\n", x_null.find_last_not_of( '\0' ) );
	
	EXPECT( x_null.find_last_not_of( '\0' ) == 0 );
	EXPECT( x_null.find_last_not_of( 'x'  ) == 1 );
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
