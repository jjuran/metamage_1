/*
	t/string_capacity.cc
	--------------------
*/

// Standard C++
#include <stdexcept>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 26 + 27 + 3 + 3;


using tap::ok_if;


static void reserved()
{
	plus::var_string empty;
	
	const char* data = empty.data();
	
	empty.reserve( 10 );
	
	ok_if( empty.size    () ==  0 );
	ok_if( empty.capacity() == 15 );
	
	ok_if( empty.data() == data );
	
	empty.reserve( 16 );
	
	ok_if( empty.size    () ==  0 );
	ok_if( empty.capacity() == 19 );
	
	ok_if( empty.data() != data );
	
	plus::var_string s = "0123456789abcdef";
	
	data = s.data();
	
	ok_if( s.capacity() == 19 );
	
	s.reserve( 7 );
	
	ok_if( s.data() == data );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 19 );
	
	s.reserve( 24 );
	
	ok_if( s.data() != data );
	
	data = s.data();
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 27 );
	
	ok_if( s == "0123456789abcdef" );
	
	s.reserve( 12 );
	
	ok_if( s.data() != data );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "0123456789abcdef" );
	
	s.assign( "0123456789abcde" );
	
	ok_if( s.size    () == 15 );
	ok_if( s.capacity() == 15 );
	
	ok_if( s.data() == (void*) &s );
	
	s.reserve( 16 );
	
	ok_if( s.size    () == 15 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "0123456789abcde" );
	
	bool caught_length_error = false;
	
	try
	{
	#ifdef __MACOS__
		
		s.reserve( s.max_size() );
		
	#endif
	}
	catch ( const std::length_error& )
	{
		caught_length_error = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( !caught_length_error );
	
	caught_length_error = false;
	
	try
	{
		s.reserve( s.max_size() + 1 );
	}
	catch ( const std::length_error& )
	{
		caught_length_error = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( caught_length_error );
}

static void resized()
{
	plus::var_string s;
	
	s.resize( 5, 'a' );
	
	ok_if( s.size() == 5 );
	
	ok_if( s == "aaaaa" );
	
	s.resize( 3 );
	
	ok_if( s.size() == 3 );
	
	ok_if( s == "aaa" );
	
	s.resize( 8, 'b' );
	
	ok_if( s.size() == 8 );
	
	ok_if( s == "aaabbbbb" );
	
	s.resize( 9 );
	
	ok_if( s.size() == 9 );
	
	ok_if( s != "aaabbbbb" );
	
	s.resize( 8 );
	
	s.resize( 16, 'c' );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "aaabbbbb" "cccccccc" );
	
	s.resize( 17, 'd' );
	
	ok_if( s.size    () == 17 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "aaabbbbb" "cccccccc" "d" );
	
	s.resize( 21, 'e' );
	
	ok_if( s.size    () == 21 );
	ok_if( s.capacity() == 35 );
	
	ok_if( s == "aaabbbbb" "cccccccc" "deeee" );
	
	s.resize( 16 );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 35 );
	
	ok_if( s == "aaabbbbb" "cccccccc" );
	
	s.resize( 8 );
	
	ok_if( s.size    () ==  8 );
	ok_if( s.capacity() == 35 );
	
	ok_if( s == "aaabbbbb" );
	
	s.resize( 40 );
	
	ok_if( s.size    () == 40 );
	ok_if( s.capacity() == 43 );
	
	bool caught_length_error = false;
	
	try
	{
	#ifdef __MACOS__
		
		s.resize( s.max_size() );
		
	#endif
	}
	catch ( const std::length_error& )
	{
		caught_length_error = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( !caught_length_error );
	
	caught_length_error = false;
	
	try
	{
		s.resize( s.max_size() + 1 );
	}
	catch ( const std::length_error& )
	{
		caught_length_error = true;
	}
	catch ( ... )
	{
	}
	
	ok_if( caught_length_error );
}

static void appended()
{
	plus::var_string s = "01234567";
	
	s.append( "89abcdef" );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "01234567" "89abcdef" );
}

static void inserted()
{
	plus::var_string s = "01234567";
	
	char* half = "89abcdef";
	
	s.insert( s.begin(), half, half + sizeof "89abcdef" - 1 );
	
	ok_if( s.size    () == 16 );
	ok_if( s.capacity() == 19 );
	
	ok_if( s == "89abcdef" "01234567" );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_capacity", n_tests );
	
	reserved();
	
	resized();
	
	appended();
	
	inserted();
	
	return 0;
}

