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


using tap::ok_if;


static void substr()
{
	plus::string empty;
	
	ok_if( empty.substr(      ).empty() );
	ok_if( empty.substr( 0, 0 ).empty() );
	
	plus::string foobar = "foobar";
	
	ok_if( foobar.substr(      ) == foobar );
	ok_if( foobar.substr( 0, 6 ) == foobar );
	ok_if( foobar.substr( 0, 8 ) == foobar );
	
	ok_if( foobar.substr( 2, 3 ) == "oba"  );
	ok_if( foobar.substr( 2, 5 ) == "obar" );
	
	ok_if( foobar.substr( 0, 0 ).empty() );
	ok_if( foobar.substr( 6, 0 ).empty() );
	
	bool exception_thrown = false;
	
	try
	{
		foobar.substr( 7, 0 );
	}
	catch ( const std::out_of_range& )
	{
		exception_thrown = true;
	}
	
	ok_if( exception_thrown );
}

static void small_substring()
{
	plus::string source = "This is a test...";
	
	
	plus::string sub = source.substr( 1, 15 );
	
	ok_if( sub == "his is a test.." );
	
	const char* data = sub.data();
	
	ok_if( data != source.data() );  // small string buffer
	
	ok_if( sub.c_str() == data );  // no reallocation
}

static void prefix()
{
	plus::string source = "This is definitely a test...";
	
	
	plus::string sub1 = source.substr( 0, 22 );
	
	ok_if( sub1 == "This is definitely a t" );
	
	const char* data = sub1.data();
	
	ok_if( data == source.data() );  // shared buffer
	
	ok_if( sub1.end()[0] == 'e' );  // This is the last (full) stop
	
	const char* c_str = sub1.c_str();
	
	ok_if( c_str != data );  // reallocation required
	
	ok_if( c_str == sub1.c_str() );  // ... but only once
	
	ok_if( sub1.end()[0] == '\0' );
	
	ok_if( sub1 == "This is definitely a t" );
	
	sub1 = source.substr( 0, 22 );
	
	plus::string sub2 = sub1.substr( 0, 16 );
	
	ok_if( sub2 == "This is definite" );
	
	plus::var_string var = sub1;
	
	var[2] = 'u';
	
	ok_if( sub1 == "This is definitely a t" );
	
	ok_if( sub2 == "This is definite" );
}

static void suffix()
{
	plus::string source = "This is definitely a test...";
	
	
	plus::string sub1 = source.substr( 5 );
	
	ok_if( sub1 == "is definitely a test..." );
	
	const char* data = sub1.data();
	
	ok_if( data - 5 == source.data() );  // shared buffer
	
	ok_if( sub1.c_str() == data );  // no reallocation
	
	plus::string sub2 = sub1.substr( 3 );
	
	ok_if( sub2 == "definitely a test..." );
	
	ok_if( sub1.data() + 3 == sub2.data() );
	
	plus::var_string var = sub1;
	
	var[3] = 'D';
	
	ok_if( sub1 == "is definitely a test..." );
	
	ok_if( sub2 == "definitely a test..." );
}

static void reset()
{
	plus::string source = "This is definitely a test...";
	
	plus::string sub = source.substr( 0, 25 );
	
	ok_if( sub == "This is definitely a test" );
	
	source.reset();
	
	ok_if( sub == "This is definitely a test" );
	
	const char* c_str = sub.c_str();
	
	ok_if( sub.end()[0] == '\0' );
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

