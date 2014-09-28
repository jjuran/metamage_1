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


static void reserved()
{
	plus::var_string empty;
	
	const char* data = empty.data();
	
	empty.reserve( 10 );
	
	EXPECT( empty.size    () ==  0 );
	EXPECT( empty.capacity() == 15 );
	
	EXPECT( empty.data() == data );
	
	empty.reserve( 16 );
	
	EXPECT( empty.size    () ==  0 );
	EXPECT( empty.capacity() == 19 );
	
	EXPECT( empty.data() != data );
	
	plus::var_string s = "0123456789abcdef";
	
	data = s.data();
	
	EXPECT( s.capacity() == 19 );
	
	s.reserve( 7 );
	
	EXPECT( s.data() == data );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 19 );
	
	s.reserve( 24 );
	
	EXPECT( s.data() != data );
	
	data = s.data();
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 27 );
	
	EXPECT( s == "0123456789abcdef" );
	
	s.reserve( 12 );
	
	EXPECT( s.data() != data );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "0123456789abcdef" );
	
	s.assign( "0123456789abcde" );
	
	EXPECT( s.size    () == 15 );
	EXPECT( s.capacity() == 15 );
	
	EXPECT( s.data() == (void*) &s );
	
	s.reserve( 16 );
	
	EXPECT( s.size    () == 15 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "0123456789abcde" );
	
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
	
	EXPECT( !caught_length_error );
	
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
	
	EXPECT( caught_length_error );
}

static void resized()
{
	plus::var_string s;
	
	s.resize( 5, 'a' );
	
	EXPECT( s.size() == 5 );
	
	EXPECT( s == "aaaaa" );
	
	s.resize( 3 );
	
	EXPECT( s.size() == 3 );
	
	EXPECT( s == "aaa" );
	
	s.resize( 8, 'b' );
	
	EXPECT( s.size() == 8 );
	
	EXPECT( s == "aaabbbbb" );
	
	s.resize( 9 );
	
	EXPECT( s.size() == 9 );
	
	EXPECT( s != "aaabbbbb" );
	
	s.resize( 8 );
	
	s.resize( 16, 'c' );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "aaabbbbb" "cccccccc" );
	
	s.resize( 17, 'd' );
	
	EXPECT( s.size    () == 17 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "aaabbbbb" "cccccccc" "d" );
	
	s.resize( 21, 'e' );
	
	EXPECT( s.size    () == 21 );
	EXPECT( s.capacity() == 35 );
	
	EXPECT( s == "aaabbbbb" "cccccccc" "deeee" );
	
	s.resize( 16 );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 35 );
	
	EXPECT( s == "aaabbbbb" "cccccccc" );
	
	s.resize( 8 );
	
	EXPECT( s.size    () ==  8 );
	EXPECT( s.capacity() == 35 );
	
	EXPECT( s == "aaabbbbb" );
	
	s.resize( 40 );
	
	EXPECT( s.size    () == 40 );
	EXPECT( s.capacity() == 43 );
	
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
	
	EXPECT( !caught_length_error );
	
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
	
	EXPECT( caught_length_error );
}

static void appended()
{
	plus::var_string s = "01234567";
	
	s.append( "89abcdef" );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "01234567" "89abcdef" );
}

static void inserted()
{
	plus::var_string s = "01234567";
	
	const char* half = "89abcdef";
	
	s.insert( s.begin(), half, half + sizeof "89abcdef" - 1 );
	
	EXPECT( s.size    () == 16 );
	EXPECT( s.capacity() == 19 );
	
	EXPECT( s == "89abcdef" "01234567" );
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
