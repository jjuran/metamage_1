/*
	pthreads.cc
	-----------
*/

// POSIX
#include <pthread.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 3;


using tap::ok_if;


static int output;

static void* entry( void* arg )
{
	const char* s = (const char*) arg;
	
	write( output, s, strlen( s ) );
	
	sleep( 2 );
	
	write( output, "!", 1 );
	
	errno = 123;
	
	return (void*) "\n";
}

static void hello_world()
{
	int fds[ 2 ];
	
	CHECK( pipe( fds ) );
	
	output = fds[ 1 ];
	
	pthread_t thread;
	
	const char* arg = "Hello ";
	
	int ok = pthread_create( &thread, NULL, &entry, (void*) arg );
	
	sleep( 1 );
	
	write( output, "world", 5 );
	
	sleep( 2 );
	
	void* result;
	
	pthread_join( thread, &result );
	
	ok_if( errno != 123, "per-thread errno # TODO" );
	
	const char* s = (const char*) result;
	
	write( output, s, strlen( s ) );
	
	char buffer[ 16 ];
	
	ssize_t n_read = CHECK( read( fds[ 0 ], buffer, sizeof buffer ) );
	
	ok_if( n_read == sizeof "Hello world!\n" - 1 );
	
	ok_if( memcmp( buffer, "Hello world!\n", n_read ) == 0 );
}

int main( int argc, char** argv )
{
	tap::start( "pthreads", n_tests );
	
	hello_world();
	
	return 0;
}

