/*
	main.cc
	-------
*/

// Standard C
#include <stdio.h>

// cthread
#include "cthread/parameter_block.hh"

// cthread-custom
#include "cthread-custom.hh"


using namespace cthread::custom;


static int counter = 0;

static
void print( const char* s )
{
	printf( "%d: %s\n", counter, s );
}

static
void* thread_entry( void* arg, const void*, const void* )
{
	int x = *(int*) arg;
	
	printf( "stack space: %lu\n", current_thread_stack_space() );
	
	while ( counter < x )
	{
		++counter;
		yield_to_any_thread();
	}
	
	print( "done" );
	
	return NULL;
}

int main( int argc, char** argv )
{
	print( "begin" );
	
	int x = 5;
	
	parameter_block pb = { &thread_entry, &x };
	
	cthread::thread_id thread = create_thread( pb, 16 * 1024 );
	
	print( "created" );
	
	for ( int i = 0;  i < 7;  ++i )
	{
		yield_to_any_thread();
		
		print( "yielded" );
	}
	
	destroy_thread( thread );
	
	print( "destroyed" );
	
	return 0;
}
