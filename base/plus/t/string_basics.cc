/*
	t/string_basics.cc
	------------------
*/

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 18 + 15 + 10;


using tap::ok_if;


static void empty()
{
	plus::string empty;
	
	ok_if( empty.empty() );
	
	ok_if( empty.size  () == 0 );
	ok_if( empty.length() == 0 );
	
	ok_if( empty.capacity() == 15 );
	
	ok_if( empty.data () == (void*) &empty );
	ok_if( empty.c_str() == (void*) &empty );
	ok_if( empty.begin() == (void*) &empty );
	ok_if( empty.end  () == (void*) &empty );
	
	ok_if( *empty.c_str() == '\0' );
	
	ok_if( empty == "" );
	
	plus::string also_empty( empty );
	
	ok_if( also_empty.size() == 0 );
	
	ok_if( also_empty.data() == (void*) &also_empty );
	
	ok_if( *also_empty.c_str() == '\0' );
	
	ok_if( also_empty == empty );
	
	also_empty.assign( "" );
	
	ok_if( also_empty.size() == 0 );
	
	ok_if( also_empty.data() == (void*) &also_empty );
	
	ok_if( *also_empty.c_str() == '\0' );
	
	ok_if( also_empty == empty );
}

static void small()
{
	plus::string one = "1";
	
	ok_if( one.size() == 1 );
	
	ok_if( one.data() == (void*) &one );
	
	ok_if( &one.data()[0] == &one[0] );
	
	ok_if( one[0] == '1' );
	
	ok_if( one.c_str()[1] == '\0' );
	
	ok_if( one == "1" );
	
	plus::string fifteen = "0123456789abcde";
	
	ok_if( fifteen.size    () == 15 );
	ok_if( fifteen.capacity() == 15 );
	
	ok_if( fifteen.data() == (void*) &fifteen );
	
	ok_if( &fifteen.data()[0] == &fifteen[0] );
	
	ok_if( fifteen[ 0] == '0' );
	ok_if( fifteen[ 1] == '1' );
	ok_if( fifteen[14] == 'e' );
	
	ok_if( fifteen.c_str()[15] == '\0' );
	
	ok_if( fifteen == "0123456789abcde" );
}

static void large()
{
	plus::string sixteen = "0123456789abcdef";
	
	ok_if( sixteen.size    () == 16 );
	ok_if( sixteen.capacity() >= 16 );
	
	ok_if( sixteen.data() != (void*) &sixteen );
	
	ok_if( &sixteen.data()[0] == &sixteen[0] );
	
	ok_if( sixteen[ 0] == '0' );
	ok_if( sixteen[ 1] == '1' );
	ok_if( sixteen[14] == 'e' );
	ok_if( sixteen[15] == 'f' );
	
	ok_if( sixteen.c_str()[16] == '\0' );
	
	ok_if( sixteen == "0123456789abcdef" );
	
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_basics", n_tests );
	
	empty();
	
	small();
	
	large();
	
	return 0;
}

