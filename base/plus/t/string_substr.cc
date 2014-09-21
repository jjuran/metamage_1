/*
	t/string_substr.cc
	------------------
*/

// Standard C++
#include <stdexcept>

// plus
#include "plus/var_string.hh"

// tap-out
#include "tap/test.hh"


static const unsigned n_tests = 10 + 3 + 10 + 7 + 3;


static void substr()
{
	plus::string empty;
	
	EXPECT( empty.substr(      ).empty() );
	EXPECT( empty.substr( 0, 0 ).empty() );
	
	plus::string foobar = "foobar";
	
	EXPECT( foobar.substr(      ) == foobar );
	EXPECT( foobar.substr( 0, 6 ) == foobar );
	EXPECT( foobar.substr( 0, 8 ) == foobar );
	
	EXPECT( foobar.substr( 2, 3 ) == "oba"  );
	EXPECT( foobar.substr( 2, 5 ) == "obar" );
	
	EXPECT( foobar.substr( 0, 0 ).empty() );
	EXPECT( foobar.substr( 6, 0 ).empty() );
	
	bool exception_thrown = false;
	
	try
	{
		foobar.substr( 7, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	EXPECT( exception_thrown );
}

static void small_substring()
{
	plus::string source = "This is a test...";
	
	
	plus::string sub = source.substr( 1, 15 );
	
	EXPECT( sub == "his is a test.." );
	
	const char* data = sub.data();
	
	EXPECT( data != source.data() );  // small string buffer
	
	EXPECT( sub.c_str() == data );  // no reallocation
}

static void prefix()
{
	plus::string source = "This is absolutely, positively, a test...";
	
	
	plus::string sub1 = source.substr( 0, 35 );
	
	EXPECT( sub1 == "This is absolutely, positively, a t" );
	
	const char* data = sub1.data();
	
	EXPECT( data == source.data() );  // shared buffer
	
	EXPECT( sub1.end()[0] == 'e' );  // This is the last (full) stop
	
	const char* c_str = sub1.c_str();
	
	EXPECT( c_str != data );  // reallocation required
	
	EXPECT( c_str == sub1.c_str() );  // ... but only once
	
	EXPECT( sub1.end()[0] == '\0' );
	
	EXPECT( sub1 == "This is absolutely, positively, a t" );
	
	sub1 = source.substr( 0, 35 );
	
	plus::string sub2 = sub1.substr( 0, 32 );
	
	EXPECT( sub2 == "This is absolutely, positively, " );
	
	plus::var_string var = sub1;
	
	var[2] = 'u';
	
	EXPECT( sub1 == "This is absolutely, positively, a t" );
	
	EXPECT( sub2 == "This is absolutely, positively, " );
}

static void suffix()
{
	plus::string source = "This is absolutely, positively, a test...";
	
	
	plus::string sub1 = source.substr( 5 );
	
	EXPECT( sub1 == "is absolutely, positively, a test..." );
	
	const char* data = sub1.data();
	
	EXPECT( data - 5 == source.data() );  // shared buffer
	
	EXPECT( sub1.c_str() == data );  // no reallocation
	
	plus::string sub2 = sub1.substr( 3 );
	
	EXPECT( sub2 == "absolutely, positively, a test..." );
	
	EXPECT( sub1.data() + 3 == sub2.data() );
	
	plus::var_string var = sub1;
	
	var[3] = 'A';
	
	EXPECT( sub1 == "is absolutely, positively, a test..." );
	
	EXPECT( sub2 == "absolutely, positively, a test..." );
}

static void reset()
{
	plus::string source = "This is absolutely, positively, a test...";
	
	plus::string sub = source.substr( 0, 38 );
	
	EXPECT( sub == "This is absolutely, positively, a test" );
	
	source.reset();
	
	EXPECT( sub == "This is absolutely, positively, a test" );
	
	const char* c_str = sub.c_str();
	
	EXPECT( sub.end()[0] == '\0' );
}

int main( int argc, const char *const *argv )
{
	tap::start( "string_substr", n_tests );
	
	substr();
	
	small_substring();
	
	prefix();
	
	suffix();
	
	reset();
	
	return 0;
}
