/*
	t/string_basics.cc
	------------------
*/

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 18 + 15 + 10;


static void empty()
{
	plus::string empty;
	
	EXPECT( empty.empty() );
	
	EXPECT( empty.size  () == 0 );
	EXPECT( empty.length() == 0 );
	
	EXPECT( empty.capacity() == 15 );
	
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
	EXPECT( fifteen.capacity() == 15 );
	
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
	plus::string sixteen = "0123456789abcdef";
	
	EXPECT( sixteen.size    () == 16 );
	EXPECT( sixteen.capacity() >= 16 );
	
	EXPECT( sixteen.data() != (void*) &sixteen );
	
	EXPECT( &sixteen.data()[0] == &sixteen[0] );
	
	EXPECT( sixteen[ 0] == '0' );
	EXPECT( sixteen[ 1] == '1' );
	EXPECT( sixteen[14] == 'e' );
	EXPECT( sixteen[15] == 'f' );
	
	EXPECT( sixteen.c_str()[16] == '\0' );
	
	EXPECT( sixteen == "0123456789abcdef" );
	
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_basics", n_tests );
	
	empty();
	
	small();
	
	large();
	
	return 0;
}
