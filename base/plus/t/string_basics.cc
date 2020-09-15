/*
	t/string_basics.cc
	------------------
*/

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 19 + 16 + 10;


static void empty()
{
	plus::string empty;
	
	EXPECT( empty.empty() );
	
	EXPECT( empty.size  () == 0 );
	EXPECT( empty.length() == 0 );
	
	EXPECT( empty.capacity() >= 15 );
	EXPECT( empty.capacity() <= 31 );
	
	EXPECT( empty.data () == (void*) &empty );
	EXPECT( empty.c_str() == (void*) &empty );
	EXPECT( empty.begin() == (void*) &empty );
	EXPECT( empty.end  () == (void*) &empty );
	
	EXPECT( *empty.c_str() == '\0' );
	
	EXPECT( empty == "" );
	
	plus::string also_empty( empty );
	
	EXPECT( also_empty.size() == 0 );
	
	EXPECT( also_empty.data() == (void*) &also_empty );
	
	EXPECT( *also_empty.c_str() == '\0' );
	
	EXPECT( also_empty == empty );
	
	also_empty.assign( "" );
	
	EXPECT( also_empty.size() == 0 );
	
	EXPECT( also_empty.data() == (void*) &also_empty );
	
	EXPECT( *also_empty.c_str() == '\0' );
	
	EXPECT( also_empty == empty );
}

static void small()
{
	plus::string one = "1";
	
	EXPECT( one.size() == 1 );
	
	EXPECT( one.data() == (void*) &one );
	
	EXPECT( &one.data()[0] == &one[0] );
	
	EXPECT( one[0] == '1' );
	
	EXPECT( one.c_str()[1] == '\0' );
	
	EXPECT( one == "1" );
	
	plus::string fifteen = "0123456789abcde";
	
	EXPECT( fifteen.size    () == 15 );
	EXPECT( fifteen.capacity() >= 15 );
	EXPECT( fifteen.capacity() <= 31 );
	
	EXPECT( fifteen.data() == (void*) &fifteen );
	
	EXPECT( &fifteen.data()[0] == &fifteen[0] );
	
	EXPECT( fifteen[ 0] == '0' );
	EXPECT( fifteen[ 1] == '1' );
	EXPECT( fifteen[14] == 'e' );
	
	EXPECT( fifteen.c_str()[15] == '\0' );
	
	EXPECT( fifteen == "0123456789abcde" );
}

static void large()
{
	plus::string thirtytwo = "0123456789abcdef" "ghijklmnopqrstuv";
	
	EXPECT( thirtytwo.size    () == 32 );
	EXPECT( thirtytwo.capacity() >= 32 );
	
	EXPECT( thirtytwo.data() != (void*) &thirtytwo );
	
	EXPECT( &thirtytwo.data()[0] == &thirtytwo[0] );
	
	EXPECT( thirtytwo[ 0] == '0' );
	EXPECT( thirtytwo[ 1] == '1' );
	EXPECT( thirtytwo[30] == 'u' );
	EXPECT( thirtytwo[31] == 'v' );
	
	EXPECT( thirtytwo.c_str()[32] == '\0' );
	
	EXPECT( thirtytwo == "0123456789abcdef" "ghijklmnopqrstuv" );
	
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_basics", n_tests );
	
	empty();
	
	small();
	
	large();
	
	return 0;
}
